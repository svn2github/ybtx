#include "stdafx.h"
#include "CNpcAI.h"
#include "CoreCommon.h"
#include "CCharacterDictator.h"
#include "CSceneCommon.h"
#include "CFighterDictator.h"
#include "Random.h"
#include "BaseHelper.h"
#include "CCoreSceneServer.h"
#include "CCoreObjectCalculator.h"
#include "CCoreObjectDictator.h"
#include "ICharacterDictatorCallbackHandler.h"
#include "CPixelPath.h"
#include "IPathFinding.h"

int32 CNpcAI::GetMaxStep(const CFPos& curPos, const CFPos& targetPos)
{
	//计算步长
	int32 iCurStep = (int32) (abs(curPos.x - targetPos.x) + abs(curPos.y - targetPos.y))/eGridSpanForObj;
	int32 iMaxStep = iCurStep > 5 ? iCurStep : 5 ;
	return iMaxStep;
}

void CNpcAI::TheaterMoveToGird(const CPos& pos, bool bWalkState)
{
	m_bTheaterMove = true;
	CNpcAI::MoveToGrid(pos, bWalkState,0);
}

bool CNpcAI::MoveToGrid(const CPos& pos, bool bWalkState, float fReachDist)
{
	CFPos PixelPos(GetPixelOfGrid(pos.x), GetPixelOfGrid(pos.y));
	return MoveToPixel(PixelPos, bWalkState, fReachDist);
}

bool CNpcAI::MoveToPixel(const CFPos& pos, bool bWalkState, float fReachDist)
{
	CCharacterDictator* pCharacterDictator = GetCharacter();
	Ast(pCharacterDictator);
	CFighterDictator* pFighterDictator = pCharacterDictator->GetFighter();
	Ast(pFighterDictator);
	float fSpeed= bWalkState ?pFighterDictator->m_WalkSpeed.Get(this->GetCharacter()->GetFighter()) : 
		pFighterDictator->m_RunSpeed.Get(this->GetCharacter()->GetFighter());

	return MoveToPixel(pos,fSpeed,bWalkState,fReachDist);
}


bool CNpcAI::MoveToPixelInLinePath(const CFPos& pos, bool bWalkState)
{
	CCharacterDictator* pCharacterDictator = GetCharacter();
	Ast(pCharacterDictator);
	CFighterDictator* pFighterDictator = pCharacterDictator->GetFighter();
	Ast(pFighterDictator);
	float fSpeed= bWalkState ?pFighterDictator->m_WalkSpeed.Get(this->GetCharacter()->GetFighter()) : 
		pFighterDictator->m_RunSpeed.Get(this->GetCharacter()->GetFighter());

	return MoveToPixelInLinePath(pos,fSpeed,bWalkState);
}

bool CNpcAI::MoveToPixelInLinePath(const CFPos& pos, float fSpeed,bool bWalkState)
{
	CCharacterDictator* pCharacterDictator = GetCharacter();
	Ast(pCharacterDictator);
	if (pCharacterDictator->CppGetCtrlState(eFCS_ForbitMove) || !pCharacterDictator->CppIsLive())
	{
		StopMoving();
		return false;
	}

	if (pCharacterDictator->CppGetCtrlState(eFCS_InWalkState) != bWalkState)
		pCharacterDictator->CppSetCtrlState(eFCS_InWalkState, bWalkState);

	CFighterDictator* pFighterDictator = pCharacterDictator->GetFighter();
	Ast(pFighterDictator);

	CFPos CurPixelPos(pCharacterDictator->GetPixelPos());
	if (CurPixelPos == pos)
	{
		OnMoveEnded();
		return false;
	}
	pCharacterDictator->SetAndSyncActionDirByPos(pos);

	EMoveToResult eMoveResult1 = pCharacterDictator->MoveToInLinePath( pos, fSpeed, eFPT_LineIgnoreEnd, eBT_HighBarrier); 
	if (eMoveResult1 == eMTR_SuperPosition || eMoveResult1 == eMTR_FindPathFailed || eMoveResult1 == eMTR_NotAllowed)
	{
		pCharacterDictator->StopMoving();
		pCharacterDictator->SetPixelPos(pos);
		pFighterDictator->CancelAttack();
		OnMoveEnded();
		CNpcEventMsg::Create(this,eNpcEvent_OnEnemyLost, eNECI_Four);
		return false;
	}
	return true;
}

bool CNpcAI::MoveToPixel(const CFPos& pos, float fSpeed,bool bWalkState, float fReachDist)
{
	//CheckNpcMoveState(pos);
	CCharacterDictator* pCharacterDictator = GetCharacter();
	Ast(pCharacterDictator);
	if (pCharacterDictator->CppGetCtrlState(eFCS_ForbitMove) || !pCharacterDictator->CppIsLive())
	{
		StopMoving();
		return false;
	}

	if (pCharacterDictator->CppGetCtrlState(eFCS_InWalkState) != bWalkState)
		pCharacterDictator->CppSetCtrlState(eFCS_InWalkState, bWalkState);

	CFighterDictator* pFighterDictator = pCharacterDictator->GetFighter();
	Ast(pFighterDictator);

	CFPos CurPixelPos(pCharacterDictator->GetPixelPos());
	if (CurPixelPos == pos)
	{
		OnMoveEnded();
		return false;
	}
	pCharacterDictator->SetAndSyncActionDirByPos(pos);

	int32 iMaxStep = GetMaxStep(CurPixelPos, pos);
	EMoveToResult eMoveResult = pCharacterDictator->MoveToInAStarPath( pos, fSpeed, eFPT_AStarIgnoreEnd, eBT_LowBarrier, fReachDist, iMaxStep); 
	if (eMoveResult == eMTR_SuperPosition || eMoveResult == eMTR_FindPathFailed || eMoveResult == eMTR_NotAllowed)
	{
		EMoveToResult eMoveResult1 = pCharacterDictator->MoveToInLinePath( pos, fSpeed, eFPT_LineIgnoreEnd, eBT_HighBarrier); 
		if (eMoveResult1 == eMTR_SuperPosition || eMoveResult1 == eMTR_FindPathFailed || eMoveResult1 == eMTR_NotAllowed)
		{
			pCharacterDictator->StopMoving();
			pCharacterDictator->SetPixelPos(pos);
			pFighterDictator->CancelAttack();
			OnMoveEnded();
			CNpcEventMsg::Create(this,eNpcEvent_OnEnemyLost, eNECI_Five);
			return false;
		}
	}
	return true;
}

void CNpcAI::MoveToDest(uint32 uX,uint32 uY)
{
	m_DestPos.x = (float)(uX*eGridSpanForObj);
	m_DestPos.y = (float)(uY*eGridSpanForObj);
	CNpcEventMsg::Create(this,eNpcEvent_OnMoveToDest);
}

void CNpcAI::MoveToDest(float uX,float uY)
{
	m_DestPos.x = uX;
	m_DestPos.y = uY;
	CNpcEventMsg::Create(this,eNpcEvent_OnMoveToDest);
}

void CNpcAI::MoveToDestEnded()
{
	if (m_uCurrentPoint == 0)
		return;

	MapMovePath::iterator iter = m_mapMovePath.find(m_uCurrentPoint);
	if (iter != m_mapMovePath.end())
	{
		CFPos* pDesPos = (*iter).second;
		CFPos curPos;
		GetCharacter()->GetPixelPos(curPos);
		uint32 fDis = (uint32)pDesPos->Dist(curPos);
		if (fDis > eGridSpanForObj)
		{
			OnMoveToPathPoint(m_uCurrentPoint);
			return;
		}
	}

	switch(m_eMoveType)
	{
	case ENpcMove_Cycle:			//循环巡逻
		{
			if (++m_uCurrentPoint > m_mapMovePath.size())
				m_uCurrentPoint = 1;
		}
		break;
	case ENpcMove_Trip:				//往返巡逻
		{
			if (m_uCurrentPoint == m_mapMovePath.size())
			{
				//Ast (m_bMoveDir);
				m_bMoveDir = false;
			}
			if (m_uCurrentPoint == 1)
			{
				//Ast (!m_bMoveDir);
				m_bMoveDir = true;
			}
			m_bMoveDir ? m_uCurrentPoint++ : m_uCurrentPoint--;
		}
		break;
	case ENpcMove_SingleLine:		//单线巡逻
	case ENpcMove_Single2Random:
		{
			if (m_uCurrentPoint == m_mapMovePath.size())
			{
				if (m_pPatrolTick)
				{
					GetCharacter()->UnRegistDistortedTick(m_pPatrolTick);
					SafeDelete(m_pPatrolTick);
				}
				if (m_pSpecialActionTick)
				{
					GetCharacter()->UnRegistDistortedTick(m_pSpecialActionTick);
					SafeDelete(m_pSpecialActionTick);
				}
				NpcSinglePatrolStateEnd();
				return;
			}
			else
			{
				m_uCurrentPoint++;
			}
		}
		break;
	default: 
		break;
	}

	uint32 uPatrolPointStayTime = GetPatrolPointStayTime();
	if(m_eRealNpcType != ENpcType_Dummy)
	{
		if (GetCharacter()->ExistSpecialActionAni() && (uint32)Random::Rand(0,100) < m_uSpecialActionProbability)
		{
			uint32 uActionFrame = GetCharacter()->GetSpecialActionAniCnt();
			if (!m_pPatrolTick)
				m_pPatrolTick = new PatrolTick(this);
			GetCharacter()->RegistDistortedTick(m_pPatrolTick, 2000 + uPatrolPointStayTime + uActionFrame*33);

			if (!m_pSpecialActionTick)
				m_pSpecialActionTick = new SpecialActionTick(this);
			GetCharacter()->RegistDistortedTick(m_pSpecialActionTick, uPatrolPointStayTime); 
		}
		else
		{
			if (!m_pPatrolTick)
				m_pPatrolTick = new PatrolTick(this);
			GetCharacter()->RegistDistortedTick(m_pPatrolTick, uPatrolPointStayTime);
		}
	}
	else
		OnMoveToPathPoint(m_uCurrentPoint);
}

void CNpcAI::StopMoving()
{
	if(GetCharacter()->CppGetCtrlState(eFCS_ForbitMove))
		return;
	GetCharacter()->StopMoving();
}

void CNpcAI::TraceEnemy(CCharacterDictator* pTarget,EBarrierType eBarrierType, float fDist)
{
	CCharacterDictator* pOwnCharacter = GetCharacter();
	Ast(pOwnCharacter);
	if (pTarget)
	{
		bool bWalkState = false;
		if (pOwnCharacter->CppGetCtrlState(eFCS_InWalkState) != bWalkState)
			pOwnCharacter->CppSetCtrlState(eFCS_InWalkState, bWalkState);
		CFighterDictator* pFighter = pOwnCharacter->GetFighter();
		Ast(pFighter);
		float fSpeed;
		fSpeed = bWalkState ?pFighter->m_WalkSpeed.Get(pFighter) : 
			pFighter->m_RunSpeed.Get(pFighter);
		if (eBT_LowBarrier == eBarrierType)
			pOwnCharacter->Trace(pTarget,fSpeed,eFPT_AStarIgnoreEnd,eBT_LowBarrier,fDist);
		else
			pOwnCharacter->Trace(pTarget,fSpeed,eFPT_LineIgnoreEnd,eBT_HighBarrier,fDist);
	}
}

void CNpcAI::StopTraceEnemy()
{
	CCharacterDictator* pCharacter = GetCharacter();
	Ast (pCharacter);
	pCharacter->StopTracing();
}


void CNpcAI::OnWatched(EWatchResult eResult)
{
	if (eResult == eWR_NotInScope)
	{
		CNpcEventMsg::Create(this,eNpcEvent_OnEnemyLost, eNECI_Six);
		CNpcEventMsg::Create(this, eNpcEvent_OnTargetOutWatchDis);
	}
}

void CNpcAI::OnObserveeEntered()
{
}

void CNpcAI::OnObserveeLeft()
{
	CNpcEventMsg::Create(this,eNpcEvent_OnEnemyLost, eNECI_Seven);
	CNpcEventMsg::Create(this, eNpcEvent_OnTargetOutWatchDis);
}

void CNpcAI::BeginChaseOriginTrace()
{
	CCharacterDictator* pOwnCharacter = GetCharacter();
	Ast(pOwnCharacter);
	CCoreObjectCalculator* pOriginObj = pOwnCharacter->GetScene()->CreateObjectCalculator(pOwnCharacter->GetPixelPos(),NULL);
	m_uOriginObjID = pOriginObj->GetGlobalID();
	pOriginObj->SetPixelPos(m_ChaseBeginPos);
	float fChaseDst = static_cast<float>(GetNpcChaseEnemyDis()*eGridSpanForObj);
	pOwnCharacter->GetCoreObj()->Watch(this,pOriginObj,fChaseDst);
}

void CNpcAI::EndChaseOriginTrace()
{
	CCharacterDictator* pOwnCharacter = GetCharacter();
	Ast(pOwnCharacter);
	CCoreObjectServer* pOriginObj = CCoreObjectServer::GetObjectServer(m_uOriginObjID);
	if (pOriginObj)
	{
		m_uOriginObjID = 0;
		pOwnCharacter->GetScene()->DestroyObject(pOriginObj);
	}
	pOwnCharacter->StopWatching(this);
}

void CNpcAI::OnTraceEnded(ETraceEndedType eType)
{
	if (eTET_Failed == eType || eTET_Stopped == eType)
		CNpcEventMsg::Create(this,eNpcEvent_OnTraceFailed);
	else if(eTET_Traced == eType)
		CNpcEventMsg::Create(this,eNpcEvent_OnTraceSuccess);
}

void CNpcAI::OnMoveBegan()
{
}

void CNpcAI::OnMoveEnded(uint32 uMoveID)
{
	CNpcEventMsg::Create(this,eNpcEvent_OnMoveEnded,eNECI_Zero,reinterpret_cast<void*>(uMoveID));
	CCharacterDictator* pCharacterDictator = GetCharacter();
	Ast(pCharacterDictator);
	if (m_bTheaterMove)		//只有剧场中调用移动的Npc才需要回调
	{
		ICharacterDictatorCallbackHandler* pICharacterDictatorCallbackHandler = pCharacterDictator->GetCallbackHandler();
		Ast(pICharacterDictatorCallbackHandler);
		pICharacterDictatorCallbackHandler->MoveEndMessage(pCharacterDictator);
	}
}

void CNpcAI::OnCreateMovePath(uint8 uPoint, float x, float y)
{
	CFPos *pPos = new CFPos((float)(x*eGridSpanForObj), (float)(y*eGridSpanForObj));
	m_mapMovePath.insert(make_pair(uPoint, pPos));
}

void CNpcAI::OnMoveToPathPoint(uint8 point)
{
	m_uCurrentPoint = point;
	if(m_mapMovePath.size() !=0)
		MoveToDest(m_mapMovePath[m_uCurrentPoint]->x, m_mapMovePath[m_uCurrentPoint]->y);
}

bool CNpcAI::MoveToTargetPos(const CFPos& targetPos, bool bWalkState)
{
	CCharacterDictator* pCharacter = GetCharacter();
	Ast (pCharacter);
	CFighterDictator* pFighterDictator = pCharacter->GetFighter();
	Ast(pFighterDictator);
	//计算步长
	CFPos CurPixelPos(pCharacter->GetPixelPos());
	int32 iMaxStep = GetMaxStep(CurPixelPos, targetPos);
	CPixelPath* pPath = pFighterDictator->GetScene()->CreatePath(CurPixelPos, targetPos, eFPT_AStarIgnoreEnd, eBT_LowBarrier, iMaxStep);
	if (pPath)
	{
		const PathList& deqWayPoint = pPath->GetGridPath()->GetPath();
		uint32 uSize = deqWayPoint.size();
		if (uSize > 2)
		{
			vector<CFPos> vecPath;
			for(uint32 i = 0; i<uSize-1; i++)
			{
				CFPos pPos;
				pPos.x = GetPixelOfQuarGrid(deqWayPoint[i].x);
				pPos.y = GetPixelOfQuarGrid(deqWayPoint[i].y);
				vecPath.push_back(pPos);
			}
			if (vecPath.size() > 0)
			{
				float fSpeed;
				fSpeed = bWalkState ?pFighterDictator->m_WalkSpeed.Get(this->GetCharacter()->GetFighter()) : 
					pFighterDictator->m_RunSpeed.Get(this->GetCharacter()->GetFighter());
				EMoveToResult eMoveToResult = pCharacter->MoveToInCustomPath(vecPath, fSpeed, 0);
				if (eMoveToResult == eMTR_FindPathFailed || eMoveToResult == eMTR_SuperPosition || eMoveToResult == eMTR_NotAllowed)
				{
					ostringstream strm1;
					strm1<<"名为【"<<m_pBaseData->GetName()<<"】的Npc MoveToTargetPos 结果为: "<< eMoveToResult << endl;
					LogErr("CNpcAI::MoveToTargetPos失败", strm1.str().c_str());
					pCharacter->SetOnDead();
				}
				SafeRelease(pPath);
				return true;
			}
		}
		SafeRelease(pPath);
	}
	MoveToPixelInLinePath(targetPos, bWalkState);
	return true;
}
