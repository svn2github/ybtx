#include "stdafx.h"
#include "CMedObjFollowState.h"
#include "CSyncCoreObjectMediator.inl"
#include "CoreObjectCommon.h"
#include "PrtlGac2GasObjectDef.h"
#include "PrtlGas2GacObjectDef.h"
#include "CPixelPath.h"
#include "CSyncCoreSceneServer.h"
#include "TimeHelper.h"
#include "BaseHelper.h"
#include "CoreCommon.h"
#include "ErrLogHelper.h"
#include "ExpHelper.h"
#include "BaseMath.h"
#include <iomanip>
#include "CSynConnServer.h"
//#include "CSyncTimeSystemServer.h"
#include "IDistortedTime.h"

CMedObjFollowState::CMedObjFollowState(CSyncCoreObjectMediator* pObject,const CGac2GasOC_Move_Mediator_To* pCmd)
:m_pCoreObj(pObject)
,m_fSpeed(pCmd->fSpeed)
,m_fMovedDist(0)
,m_iCheatScore(0)
,m_fBeginDist(0)
,m_fDirBeginDist(0)
//,m_bDirStepped(false)
//,m_uMoveSuspendedTime(uint64(-1))//,m_fLastMovedDist(0)
{
	SQR_TRY
	{
		//RegistDirStepCheckTick(EstimateDirStepFreq());

		CFPos EndPixelPos=pCmd->EndPixelPos;
		
		m_BeginPixelPos=pObject->GetPixelPos();
		m_EndPixelPos=EndPixelPos;

		if(m_BeginPixelPos != pCmd->PixelPos)
		{
			ostringstream strm;
			strm << pObject->GetGlobalID() << " " << setprecision(12) << m_BeginPixelPos.x << "," << m_BeginPixelPos.y << " != " << pCmd->PixelPos.x << "," << pCmd->PixelPos.y << endl;
#ifdef LOG_COREOBJ_MOVE
			pObject->LogMsg(strm.str());
#endif
			GenErr(" MedObj DirObj pos don't match ", strm.str());
		}

		m_eFindPathType=static_cast<EFindPathType>(pCmd->usbFindPathType);
		m_eBarrierType=static_cast<EBarrierType>(pCmd->usbBarrierType);

		m_fEndDist = pCmd->fEndDist;

		int16* pData = (int16*)(pCmd + 1);
		m_pPath = pObject->GetScene()->CreatePath( pData, pCmd->udbPathLen,m_BeginPixelPos, m_EndPixelPos);
		//m_uLastStepTimePoint = GetProcessTime();

		//m_tcpLastPosChanged.SetCheckPoint();
		//m_tcpThisPath.SetCheckPoint();
		m_uThisPathExpiredTime = m_pCoreObj->GetConnection()->GetConnTime();
		m_uDirThisPathExpiredTime = m_pCoreObj->GetConnection()->GetClientTime();

		if(m_pPath->GetPathPixelLength() < m_fEndDist)
		{
			ostringstream strm;
			strm << setprecision(12) << m_pPath->GetPathPixelLength() << " < " << m_fEndDist;
#ifdef LOG_COREOBJ_MOVE
			pObject->LogMsg(strm.str());
#endif
			GenErr("CMedObjFollowState::SetNewPath err ", strm.str());
		}

		//CSyncTimeSystemServer::Inst()->Register(this, eCSAF_TickOnce);
	}
	SQR_CATCH(exp)
	{
		CSynConnServer* pConn = pObject->GetConnection();
		if (pConn)
			LogExp(exp, pConn);
		else
			LogExp(exp);
	}
	SQR_TRY_END;
}

CMedObjFollowState::~CMedObjFollowState()
{
	//CSyncTimeSystemServer::Inst()->UnRegister(this);
	SafeRelease(m_pPath);
}

const CFPos& CMedObjFollowState::GetBeginPixelPos()const
{
	return m_BeginPixelPos;
}

const CFPos& CMedObjFollowState::GetEndPixelPos()const
{
	return m_EndPixelPos;
}

EFindPathType CMedObjFollowState::GetFindPathType()const
{
	return m_eFindPathType;
}

EBarrierType CMedObjFollowState::GetBarrierType()const
{
	return m_eBarrierType;
}

float CMedObjFollowState::GetSpeed()const
{
	return m_fSpeed;
}

bool CMedObjFollowState::SetSpeed(const CGac2GasOC_Change_Mediator_Move_Speed* pCmd)
{
	if(m_pCoreObj->m_fDirKnownMaxSpeed < pCmd->fSpeed)
	{
		//ostringstream strm;
		//strm << m_pCoreObj->m_fDirKnownMaxSpeed << "<" << pCmd->fSpeed;
		//GenErr("invalid dir speed", strm.str());
		return false;
	}

	const uint64 uCurTime = m_pCoreObj->GetConnection()->GetConnTime();
	const uint64 uElapsedTime =  uCurTime - m_uThisPathExpiredTime;
	const float fExpectedMoveDist = sqr::CalMoveStep(m_fSpeed, uint32(uElapsedTime));
	m_fBeginDist += fExpectedMoveDist;
	m_uThisPathExpiredTime = uCurTime;

	const uint64 uCurClientTime = m_pCoreObj->GetConnection()->GetClientTime();
	const uint64 uElapsedClientTime =  uCurClientTime - m_uDirThisPathExpiredTime;
	m_fDirBeginDist += sqr::CalMoveStep(m_fSpeed, uint32(uElapsedClientTime));
	m_uDirThisPathExpiredTime = uCurClientTime;

	//cout << "setspeed " << pCmd->fSpeed << " dist " << m_fBeginDist << "(" << m_fSpeed << "," << uElapsedTime << ")" << endl;
	m_fSpeed=pCmd->fSpeed;

	return true;
	//RegistDirStepCheckTick(EstimateDirStepFreq());
}

float CMedObjFollowState::GetMovedDist()const
{
	return m_fMovedDist;
}

float CMedObjFollowState::GetEndDist()const
{
	return m_fEndDist;
}


void CMedObjFollowState::Step(float fNewMovedDist)
{
	m_fMovedDist = fNewMovedDist;

	//if(!m_bDirStepped)
	//	ResumeFollowerMoving();

	//m_bDirStepped = true;
}

void CMedObjFollowState::GetRealCurPos(CFPos& PixelPos)
{
	VerNe( m_pPath->GetPixelPosOfPath(PixelPos,GetRealMovedDist()),uint16(-1) );
}

float CMedObjFollowState::CalcDirMovedDist()const
{
	uint64 uCurTime = m_pCoreObj->GetConnection()->GetClientTime();
	const uint32 uElapsedTime = uint32(uCurTime - m_uDirThisPathExpiredTime);
	float fDist = m_fDirBeginDist + sqr::CalMoveStep( m_fSpeed, uElapsedTime );
	fDist = limit2( fDist, GetMovedDist(), GetEndDist() );
	float fMaxMovedDist = GetEndDist();
	if(fDist > fMaxMovedDist)
		fDist = fMaxMovedDist; 
	return fDist;
}

float CMedObjFollowState::GetRealMovedDist()const
{
	uint64 uCurTime = m_pCoreObj->GetConnection()->GetConnTime();
	const uint32 uElapsedTime = uint32(uCurTime - m_uThisPathExpiredTime);
	const float fRealDist = m_fBeginDist + sqr::CalMoveStep( m_fSpeed, uElapsedTime );
	float fIdealMovedDist = limit2( fRealDist, GetMovedDist(), GetEndDist() );
	float fMaxMovedDist = GetEndDist();
	if(fIdealMovedDist > fMaxMovedDist)
		fIdealMovedDist  = fMaxMovedDist; 
	return fIdealMovedDist;
}

void CMedObjFollowState::SetNewPath(const CGac2GasOC_Change_Mediator_Move_Target* pCmd)
{
	SQR_TRY
	{

		m_BeginPixelPos=m_pCoreObj->GetPixelPos();

		m_EndPixelPos=pCmd->EndPixelPos;
		m_fSpeed=pCmd->fSpeed;
		m_fMovedDist=0;
		m_fEndDist=pCmd->fEndDist;

		Ast(m_pPath != NULL);
		m_eFindPathType=static_cast<EFindPathType>(pCmd->usbFindPathType);
		m_eBarrierType=static_cast<EBarrierType>(pCmd->usbBarrierType);
		m_pPath->Release();

		int16* pData = (int16*)(pCmd + 1);
		m_pPath = m_pCoreObj->GetScene()->CreatePath(pData, pCmd->udbPathLen, m_BeginPixelPos, m_EndPixelPos);

		m_fBeginDist = 0;
		m_uThisPathExpiredTime = m_pCoreObj->GetConnection()->GetConnTime();
		m_fDirBeginDist = 0;
		m_uDirThisPathExpiredTime = m_pCoreObj->GetConnection()->GetClientTime();
		//m_tcpThisPath.SetCheckPoint();
		//m_fLastMovedDist = 0;
		//m_tcpLastPosChanged.SetCheckPoint();

		//RegistDirStepCheckTick(EstimateDirStepFreq());

		if(m_pPath->GetPathPixelLength() < m_fEndDist )
		{
			ostringstream strm;
			strm << setprecision(12) << m_pPath->GetPathPixelLength() << " < " << m_fEndDist << endl;

#ifdef DIRECTOR_SEND_MOVED_DIST
			strm << pCmd->fMovedDist;
#else
			float fMedMovedDist;
			m_pCoreObj->CalcRealMovedDist(fMedMovedDist);
			strm << fMedMovedDist;
#endif
			strm << " " << pCmd->BeginPixelPos.x << "," << pCmd->BeginPixelPos.y << " " << m_BeginPixelPos.x << "," << m_BeginPixelPos.y << endl;

#ifdef LOG_COREOBJ_MOVE
			m_pCoreObj->LogMsg(strm.str());
#endif
			//CFPos endPos;
			//m_pPath->GetEndPixelPos(endPos);
			//cout << m_EndPixelPos.x << "," << m_EndPixelPos.y << " " << endPos.x << "," << endPos.y << endl;
			//m_pPath->PrintPath();
			GenErr("CMedObjFollowState::SetNewPath err ", strm.str());
		}
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
}

int32 CMedObjFollowState::GetCheatScore()const
{
	return m_iCheatScore;
}

const CPixelPath* CMedObjFollowState::GetPath()const
{
	return m_pPath;
}

uint16 CMedObjFollowState::GetPixelPosOfPath(CFPos& PixelPos,float fDist)const
{
	return m_pPath->GetPixelPosOfPath(PixelPos, fDist);
}

//void CMedObjFollowState::OnTick()
//{
//	if(!m_bDirStepped)
//	{
//		SuspendFollowerMoving();
//		return;
//	}
//
//	m_bDirStepped = false;
//}

//void CMedObjFollowState::SuspendFollowerMoving()
//{
//	if(m_uMoveSuspendedTime != uint64(-1))
//		return;
//
//	CGas2GacOC_Suspend_Follower_Moving cmd;
//	cmd.uqbGlobalID = m_pCoreObj->GetGlobalID();
//	cmd.fMovedDist = GetMovedDist();
//
//	m_pCoreObj->ViewMulticastSendCmd(&cmd);
//;
//	m_uMoveSuspendedTime = m_pCoreObj->GetDistortedTime()->GetDistortedFrameTime();
//}

//const float g_fDirStepFreqEstimatedRate = 1.2f;

//void CMedObjFollowState::ResumeFollowerMoving()
//{
//	if(!Registered())
//		GenErr("DirStepCheckTick is not registed");
//
//	if(m_uMoveSuspendedTime == uint64(-1))
//		return;
//
//	CGas2GacOC_Resume_Follower_Moving cmd;
//	cmd.uqbGlobalID = m_pCoreObj->GetGlobalID();
//	cmd.uqbSuspendedTime = uint32(m_pCoreObj->GetDistortedTime()->GetDistortedFrameTime() - m_uMoveSuspendedTime + GetInterval() * (g_fDirStepFreqEstimatedRate-1));	// 加上检测时人为放宽的误差
//
//	//cout << "resume " << cmd.uqbSuspendedTime << endl;
//	m_pCoreObj->ViewMulticastSendCmd(&cmd);
//	m_uMoveSuspendedTime = uint64(-1);
//}

//uint32 CMedObjFollowState::EstimateDirStepFreq()
//{
//	//cout << g_fDirStepMaxSentMovedDist << g_uDirTellMedStepMaxInterval << endl;
//	if(m_fSpeed <= 0)
//		GenErr("m_fSpeed <= 0");
//
//	uint32 uInterval = min(g_uDirTellMedStepMaxInterval, uint32(g_fDirStepMaxSentMovedDist / (m_fSpeed * eGridSpan) * 1000));
//	//cout << m_pCoreObj->GetGlobalID() << "Dir step freq " << uInterval << " speed " << m_fSpeed << endl;
//	return uint32(uInterval * g_fDirStepFreqEstimatedRate);
//}
//
//void CMedObjFollowState::RegistDirStepCheckTick(uint32 uInterval)
//{
//	m_pCoreObj->GetDistortedTime()->RegistDistortedTick(this, uInterval);
//	m_bDirStepped = false;
//	ResumeFollowerMoving();
//}

