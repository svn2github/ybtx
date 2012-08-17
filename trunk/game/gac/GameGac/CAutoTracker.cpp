#include "stdafx.h"
#include "CAutoTracker.h"
#include "CCharacterDirector.h"
#include "CEntityClientManager.h"
#include "CoreCommon.h"
#include "CFighterDirector.h"
#include "CCoreSceneClient.h"
#include "CPixelPath.h"
#include "IPathFinding.h"
#include "CRelationMgrClient.h"

CAutoTracker::CAutoTracker()
: m_uAttackerID(0)
, m_uTargetID(0)
, m_pHandler(NULL)
, m_bIsAutoTracking(false)
, m_bIsNormalAttackTracker(false)
, m_posDest(0.0f, 0.0f)
{
}

CAutoTracker::~CAutoTracker()
{
	if(m_pHandler)
	{
		if(m_pHandler->CreatedInCPP())
		{
			SafeDelete(m_pHandler);
		}
		else
		{
			AdvDelRef(m_pHandler);
		}
	}
	if(IsAutoTracking())
	{
		CEntityClient* pAttacker = CEntityClientManager::GetEntityByID(m_uAttackerID);
		pAttacker->UnRegistDistortedTick(this);
	}
}

uint32 CAutoTracker::GetTickTime()
{
	static uint32 ms_uTickTime = 500;
	return ms_uTickTime;
}

float CAutoTracker::GetDistAdjustment()
{
	static float ms_fDistAdjustment = 1.5f;
	return ms_fDistAdjustment;
}

bool CAutoTracker::Init(uint32 uAttackerID, uint32 uTargetID, float fReachGridDist, IAutoTrackerHandler* pHandler, bool bIsNormalAttackTracker)
{
	//if(fReachGridDist < GetDistAdjustment())
	//{
	//	fReachGridDist = GetDistAdjustment() + 0.5f;
	//}

	CEntityClient* pAttacker = CEntityClientManager::GetEntityByID(uAttackerID);
	CCharacterDirector* pCharAttacker = class_cast<CCharacterDirector*>(pAttacker);
	if(!pCharAttacker || pCharAttacker->CppGetCtrlState(eFCS_ForbitAutoTrack))
		return false;

	CEntityClient* pTarget = CEntityClientManager::GetEntityByID(uTargetID);
	if (pTarget->GetGameEntityType() == eGET_CharacterFollower)
	{
		CCharacterFollower* pCharTarget = class_cast<CCharacterFollower*>(pTarget);
		CRelationMgrClient& relationMgr = CRelationMgrClient::Inst();
		if (relationMgr.GetRelationType(pAttacker->GetFighter(), pCharTarget->GetFighter()) == eRT_DuelTargetWrong)
		{
			pCharAttacker->OnMagicCondFail(eDSR_DuelTargetWrong);
			return false;
		}
	}

	m_uAttackerID = uAttackerID;
	m_uTargetID = uTargetID;
	m_fReachGridDist = fReachGridDist;
	SetHandler(pHandler);
	m_bIsAutoTracking = true;
	m_bIsNormalAttackTracker = bIsNormalAttackTracker;
	m_posDest = CFPos(0.0f, 0.0f);
	//cout << "CAutoTracker::Init" << endl;
	pAttacker->RegistDistortedTick(this, GetTickTime());
	this->OnTick();
	return true;
}

void CAutoTracker::OnTick()
{
	CEntityClient* pAttacker = CEntityClientManager::GetEntityByID(m_uAttackerID);
	CEntityClient* pTarget = CEntityClientManager::GetEntityByID(m_uTargetID);
	
	if(!pAttacker || !pTarget)
	{
		pAttacker->UnRegistDistortedTick(this);
		CCharacterDirector* pCharAttacker = class_cast<CCharacterDirector*>(pAttacker);
		pCharAttacker->StopMoving();
		Cancel();
		return;
	}

	if(!m_pHandler)
	{
		GenErr("CAutoTracker没有设置handler");
	}

	CFPos posAttacker = pAttacker->GetPixelPos();
	CFPos posTarget = pTarget->GetPixelPos();
	if(posAttacker == posTarget)
	{
		m_pHandler->OnReached(pAttacker, pTarget);
		return;
	}

	bool bBypassBarrier = HasBarrierInLine(pAttacker, pTarget) && IsNormalAttackTracker();
	if(m_pHandler->OnCheckStopCond(pAttacker, pTarget) && IsInScope(pAttacker, pTarget) && !bBypassBarrier)
	{
		//pAttacker->StopMoving();

		//CCharacterFollower* pCharAttacker = class_cast<CCharacterFollower*>(pAttacker);
		//pCharAttacker->DoActionState(eAS_Idle);

		m_pHandler->OnReached(pAttacker, pTarget);
		//CAppClient::Inst()->UnRegisterTick(this);
		//cout << "autotrack onreached" << endl;
		return;
	}
	else
	{
		Track(pAttacker, pTarget, bBypassBarrier);
	}
}

bool CAutoTracker::IsInScope(CEntityClient* pAttacker, CEntityClient* pTarget)
{
	Ast(pAttacker);
	Ast(pTarget);
	//CFPos posFrom, posTo;

	//posFrom = pAttacker->GetCoreObj()->GetPixelPos();
	//posTo = pTarget->GetCoreObj()->GetPixelPos();
	//float uX = posFrom.x - posTo.x;
	//float uY = posFrom.y - posTo.y;

	//float fDist = sqrt( uX*uX + uY*uY ) / eGridSpanForObj; 
	float fDist = pAttacker->GetEntityDistInGrid(pTarget);
	//cout << "距离 " << uDist << " ReachGridDist " << m_fReachGridDist << endl;
	//return fDist <= (m_fReachGridDist + 1.0f);

	return fDist <= m_fReachGridDist;
}

bool CAutoTracker::HasBarrierInLine(CEntityClient* pAttacker, CEntityClient* pTarget)
{
	Ast(pAttacker);
	Ast(pTarget);

	CFPos posFrom, posTo;
	posFrom = pAttacker->GetPixelPos();
	posTo = pTarget->GetPixelPos();

	CPixelPath* pPath = pAttacker->GetScene()->CreatePath(posFrom, posTo, eFPT_Line, eBT_MidBarrier);
	if(pPath == NULL)
		return true;
	SafeRelease(pPath);

	return false;
}

void CAutoTracker::SetHandler(IAutoTrackerHandler* pHandler)
{
	if(pHandler)
		AdvAddRef(pHandler);
	if(m_pHandler)
	{
		if(m_pHandler->CreatedInCPP())
		{
			SafeDelete(m_pHandler);
		}
		else
		{
			AdvDelRef(m_pHandler);
		}
	}
	m_pHandler = pHandler;
}
IAutoTrackerHandler* CAutoTracker::GetHandler()const
{
	return m_pHandler;
}

uint32 CAutoTracker::GetMaxStepInAStar()
{
	return 15;
}

void CAutoTracker::Track(CEntityClient* pAttacker, CEntityClient* pTarget, bool bBypassingBarrier)
{
	Ast(pAttacker);
	if(pTarget == NULL)
	{
		//cout << "追踪目标不存在" << endl;
		return ;
	}

	CCharacterDirector* pCharAttacker = class_cast<CCharacterDirector*>(pAttacker);
	Ast(pCharAttacker);
	
	CFighterDirector* pFighter = class_cast<CFighterDirector*>(pCharAttacker->GetFighter());
	if(pFighter->GetCtrlState(eFCS_ForbitMove))
	{
		//cout << "攻击者不能移动" << endl;
		return ;
	}

	//跟踪目标
	//通知客户端向目标移动
	//float fReachGridDist = float(m_pNormalAttackCfg->GetMaxAttackDis()->GetDblValue(pFighter)); //之所以乘以0.7是为了解决自动追踪停止的地点有可能还未进入普攻范围的问题
	CFPos SrcPos = pCharAttacker->GetPixelPos();							//起点

	if(!pCharAttacker->IsMoving()) 
	{
		pCharAttacker->CreateAccelerator();
	}

	float fAttackerMoveSpeed = pCharAttacker->GetMaxSpeed();
	//cout << "追踪速度 " << fAttackerMoveSpeed << endl;
	//Ast(fAttackerMoveSpeed - 0.0f > 0.00001f);
	//EMoveToResult eMoveRet = m_pAttacker->GetHandler()->CoreObjMoveTo(m_pTarget->GetHandler()->GetPixelPos(), m_fAttackerMoveSpeed, eFPT_AStar,eBT_NoBarrier,fReachGridDist);
	// 不直接拿目标follower对象上面的坐标，拿的是目标follower对象上面保存的服务端对象的位置
	
	CFPos TargetPos = pTarget->GetPixelPos();
	if (m_posDest == TargetPos)		// 目标未动，没必要再MOVETO一次
		return;

	EMoveToResult eMoveRet;
	if(bBypassingBarrier)
	{
		CFPos posDest;
		CPixelPath* pPath = pAttacker->GetScene()->CreatePath(SrcPos, TargetPos, eFPT_HypoAStarOpti, eBT_LowBarrier, GetMaxStepInAStar());
		if(pPath == NULL)
		{
			Cancel();
			return ;
		}
		const PathList& deqWaypoint = pPath->GetGridPath()->GetPath();
		PathList::const_iterator it = deqWaypoint.begin();
		for(;;it++)
		{
			if(it == deqWaypoint.end())
			{
				Cancel();
				return;
			}
			posDest.x = GetPixelOfQuarGrid(it->x);
			posDest.y = GetPixelOfQuarGrid(it->y);
			CPixelPath* pPixelPath = pAttacker->GetScene()->CreatePath(posDest, TargetPos, eFPT_Line, eBT_LowBarrier);
			if(pPixelPath)
			{
				SafeRelease(pPixelPath);
				break;
			}

		}
		SafeRelease(pPath);
		eMoveRet = pCharAttacker->MoveToInAStarPath(posDest, eFPT_HypoAStarOpti, eBT_LowBarrier, 0, GetMaxStepInAStar());
	}
	else
	{
		float fAttackerBottomSize = pCharAttacker->GetBottomSize();
		float fTargetBottomSize = pTarget->GetBottomSize();
		float fReachGridDist = max(0, m_fReachGridDist - GetDistAdjustment() + fAttackerBottomSize + fTargetBottomSize);
		//cout << "autotrack:moveTo targetpos = " << TargetPos.x / eGridSpanForObj << "," << TargetPos.y / eGridSpanForObj << " fReachGridDist = " << fReachGridDist << endl;
		eMoveRet = pCharAttacker->MoveToInAStarPath(TargetPos, eFPT_HypoAStarOpti,eBT_LowBarrier, fReachGridDist/**0.8f*/, GetMaxStepInAStar());
	}

	if(eMoveRet == eMTR_Success)
	{
		m_posDest = TargetPos;
	}
	else
	{
		Cancel();
	}
}

bool CAutoTracker::IsNormalAttackTracker()
{
	return m_bIsNormalAttackTracker;
}

void CAutoTracker::Cancel()
{
	//cout << "CAutoTracker::Cancel" << endl;
	m_bIsAutoTracking = false;
	CEntityClient* pAttacker = CEntityClientManager::GetEntityByID(m_uAttackerID);
	pAttacker->UnRegistDistortedTick(this);
	if(m_pHandler)
	{
		CEntityClient* pAttacker = CEntityClientManager::GetEntityByID(m_uAttackerID);
		CEntityClient* pTarget = CEntityClientManager::GetEntityByID(m_uTargetID);

		m_pHandler->OnCancel(pAttacker, pTarget);
	}
}

void IAutoTrackerHandler::OnReached(CEntityClient* pAttacker, CEntityClient* pTarget)
{ 
	CALL_CLASS_CALLBACK_2(pAttacker, pTarget)	
		GenErr("未实现接口IAutoTrackerHandler::OnReached");
}

void IAutoTrackerHandler::OnCancel(CEntityClient* pAttacker, CEntityClient* pTarget)
{ 
	CALL_CLASS_CALLBACK_2(pAttacker, pTarget)	
		//GenErr("未实现接口IAutoTrackerHandler::OnCancel");
}

bool IAutoTrackerHandler::OnCheckStopCond(CEntityClient* pAttacker, CEntityClient* pTarget)
{
	CALL_CLASS_CALLBACK_2_RET(bool, pAttacker, pTarget)	
		GenErr("未实现接口IAutoTrackerHandler::OnCheckStopCond"); return false; 
}
