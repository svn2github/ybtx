#include "stdafx.h"
#include "CCoreObjTracer.h"
#include "CSyncCoreObjectMediator.h"
#include "CSyncTimeSystemServer.h"
#include "ErrLogHelper.h"
#include "CSyncCoreSceneServer.h"
#include "CSyncMetaSceneServer.h"
#include "CoreCommon.h"
#include "IDistortedTime.h"
#include "SyncCoreObjEvent.h"
#include "TSqrAllocator.inl"


CCoreObjTracer::CCoreObjTracer(CSyncCoreObjectServer* pCoreObj)
:m_pCoreObj(pCoreObj)
,m_fSpeed(0.0f)
,m_eFindPathType(eFPT_Line)
,m_eBarrierType(eBT_HighBarrier)
,m_fReachDist(0.0f)
,m_uCurTickFreq(uint32(-1))
,m_uNextTickFreq(uint32(-1))
{
	m_Target.SetHolder(this);
}

void CCoreObjTracer::OnTick()
{
	EMoveToResult ret = Move();
	switch(ret)
	{
	case eMTR_Success:
		AdjustTickFreq();
		break;
	case eMTR_SuperPosition:
		m_pCoreObj->EndTracing();
		break;
	default:
		m_pCoreObj->StopMoving();
		m_pCoreObj->StopTracing();
		break;
	}
}

void CCoreObjTracer::OnCOREvent(CPtCORSubject* pSubject,uint32 uEvent,void* pArg)
{
	switch(uEvent)
	{
	case eSCOE_SpeedChanged:
		//cout << m_pCoreObj->GetGlobalID() << " trace " << GetTarget()->GetGlobalID() << " " << static_cast<CSyncCoreObjectServer*>(pSubject)->GetGlobalID() << endl;
		if(GetTarget())
			AdjustTickFreq();
		break;
	default:
		break;
	}
}

float CCoreObjTracer::GetSelfSpeed()const
{
	return m_pCoreObj->GetSpeed();
}

float CCoreObjTracer::GetTargetSpeed()const
{
	//target有可能是mediator,所以通过TCoreObj::GetSpeed拿到的速度不对
	CSyncCoreObjectServer* pTarget = GetTarget();
	CSyncCoreObjectMediator* pTargetMediator = pTarget->CastToObjMediator();
	if(pTargetMediator && pTargetMediator->IsFollowing())
	{
		return pTargetMediator->GetFollowingSpeed();
	}
		
	return GetTarget()->GetSpeed();
}

void CCoreObjTracer::AdjustTickFreq()
{
	Ast(GetTarget());

	if(m_uNextTickFreq != uint32(-1))
	{
		m_uCurTickFreq = m_uNextTickFreq;
		m_uNextTickFreq = uint32(-1);
		m_pCoreObj->GetDistortedTime()->RegistDistortedTick( this, m_uCurTickFreq );
		return;
	}

	CFPos posSelf = m_pCoreObj->GetPixelPos();
	float fDist = posSelf.Dist(m_fTargetCurRealPos) / eGridSpan;
	uint32 uTickFreq = 0;
	float fTargetSpeed = GetTargetSpeed();
	float fSelfSpeed = GetSelfSpeed();
	float fSpeedParam = max(fTargetSpeed, fSelfSpeed);	//为了保证trace的灵敏度,取两者的速度最大值

	float fThreshold1 = fSpeedParam * 0.5f;
	float fThreshold2 = fSpeedParam * 1.5f;
	float fThreshold3 = fSpeedParam * 3.0f;
	float fThreshold4 = fSpeedParam * 6.0f;
	float fThreshold5 = fSpeedParam * 12.0f;
	if(fDist < fThreshold1)
	{
		uTickFreq = 75;
	}
	else if( fDist >= fThreshold1 && fDist < fThreshold2)
	{
		uTickFreq = 150;
	}
	else if( fDist >= fThreshold2 && fDist < fThreshold3)
	{
		uTickFreq = 500;
	}
	else if( fDist >= fThreshold3 && fDist < fThreshold4)
	{
		uTickFreq = 1000;
	}
	else if( fDist >= fThreshold4 && fDist < fThreshold5)
	{
		uTickFreq = 5000;
	}
	else if( fDist >= fThreshold5)
	{
		uTickFreq = 10000;
	}

	uint32 uLeftTime = 0;
	if(Registered())
	{
		uLeftTime = GetLeftTime();
	}
	//cout << "uLeftTime " << uLeftTime << endl;
	uint32 uCurTickFreq;
	if(uLeftTime != 0)
	{
		uCurTickFreq = min(uLeftTime, uTickFreq);
		if(uCurTickFreq == uLeftTime)
		{
			m_uNextTickFreq = uTickFreq;
		}
	}
	else
	{
		uCurTickFreq = uTickFreq;
	}

	if(m_uCurTickFreq == uCurTickFreq)
		return;

	m_uCurTickFreq = uCurTickFreq;
	//cout << m_pCoreObj->GetGlobalID() << " TraceTick " << m_uCurTickFreq << " 1 " << fThreshold1 << " 2 " << fThreshold2 << " 3 " << fThreshold3 << " Dist " << fDist << endl;
	m_pCoreObj->GetDistortedTime()->RegistDistortedTick( this, m_uCurTickFreq );
}

CCoreObjTracer::~CCoreObjTracer()
{
	Stop();
}

ETraceResult CCoreObjTracer::Trace(CSyncCoreObjectServer* pTarget, float fSpeed, EFindPathType eFindPathType, EBarrierType eBarrierType, float fReachDist)
{
	m_eFindPathType = eFindPathType;
	m_eBarrierType = eBarrierType;
	m_fReachDist = fReachDist;
	if(GetTarget())
	{
		GetTarget()->Detach(this, eSCOE_SpeedChanged);
		//cout << GetTarget()->GetGlobalID() << " detach " << m_pCoreObj->GetGlobalID() << " 's tracer " << endl;
	}
	m_Target.Attach(pTarget->m_RefsByTracer);

	if(Move(fSpeed) != eMTR_Success)
	{
		m_Target.Detach();
		return eTR_MoveFailed;
	}

	AdjustTickFreq();
	m_pCoreObj->Attach(this, eSCOE_SpeedChanged);
	GetTarget()->Attach(this, eSCOE_SpeedChanged);

	//cout << m_pCoreObj->GetGlobalID() << " attach " << m_pCoreObj->GetGlobalID() << " 's tracer " << endl;
	//cout << GetTarget()->GetGlobalID() << " attach " << m_pCoreObj->GetGlobalID() << " 's tracer " << endl;
	return eTR_Success;
}

void CCoreObjTracer::Stop()
{
	//cout << m_pCoreObj->GetGlobalID() << " detach " << m_pCoreObj->GetGlobalID() << " 's tracer " << endl;
	m_pCoreObj->Detach(this, eSCOE_SpeedChanged);
	if(GetTarget())
	{
		GetTarget()->Detach(this, eSCOE_SpeedChanged);
		//cout << GetTarget()->GetGlobalID() << " detach " << m_pCoreObj->GetGlobalID() << " 's tracer " << endl;
	}
	m_Target.Detach();
	m_pCoreObj->GetDistortedTime()->UnregistDistortedTick(this);
	m_uCurTickFreq = uint32(-1);
	m_uNextTickFreq = uint32(-1);
}

EMoveToResult CCoreObjTracer::Move(float fSpeed)
{
	SQR_TRY
	{
		CSyncCoreObjectServer* pTarget = GetTarget();
		if(!pTarget)
			return eMTR_NotAllowed;

		if(pTarget->GetScene()!=m_pCoreObj->GetScene())
			return eMTR_FindPathFailed;

		pTarget->GetRealCurPos(m_fTargetCurRealPos);
		CPos selfPos, targetPos;
		m_pCoreObj->GetGridPos(selfPos);
		pTarget->GetGridPos(targetPos);
		int32 iCurStep = abs(selfPos.x - targetPos.x) + abs(selfPos.y - targetPos.y);
		int32 iMaxStep = iCurStep > 10 ? iCurStep : 10;
		//cout << "trace move " << m_fTargetCurRealPos.x << "," << m_fTargetCurRealPos.y << " " << m_nMaxStep << endl;
		return m_pCoreObj->MoveToLimit(m_fTargetCurRealPos, fSpeed == 0 ? GetSelfSpeed() : fSpeed, m_eFindPathType, m_eBarrierType, m_fReachDist, 0, NULL, iMaxStep);
	}
	SQR_CATCH(exp)
	{
		ostringstream strm;	
		strm << "scene " << m_pCoreObj->GetScene()->GetMetaScene()->GetSceneName() 
			<< " srcpos " << m_pCoreObj->GetPixelPos().x << "," << m_pCoreObj->GetPixelPos().y
			<< " despos " << m_fTargetCurRealPos.x << "," << m_fTargetCurRealPos.y
			<< " speed " << m_fSpeed 
			<< " findpathtype " << m_eFindPathType
			<< " barriertype " << m_eBarrierType
			<< " reachdist " << m_fReachDist << endl;
		exp.AppendMsg(strm.str().c_str());
		LogExp(exp);
	}
	SQR_TRY_END;
	return eMTR_FindPathFailed;
}

CSyncCoreObjectServer* CCoreObjTracer::GetTarget()const
{
	return m_Target.Get();
}

