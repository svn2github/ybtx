#include "stdafx.h"
#include "CDirMoveState.h"
#include "CMetaSceneServer.h"
#include "TMetaScene.h"
#include "CFighterDictator.h"
#include "CCharacterDictator.h"
#include "CDir.h"
#include "CoreCommon.h"
#include "CCoreSceneServer.h"
#include "CAllStateMgr.h"
#include "CSpecialStateMgr.h"
#include "CSkillInstServer.h"
#include "COtherStateMgr.inl"
#include "CMoveServerManager.h"
#include "TSqrAllocator.inl"

CDirMoveStateServer::CDirMoveStateServer(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller, CSpecialStateMgrServer* pMgr, 
	const CSpecialStateCfgServerSharedPtr& pCfg, int32 iTime, int32 iRemainTime)
: CSpecialStateBase(pSkillInst, pInstaller, pMgr, pCfg, iTime, iRemainTime)
,m_pMoveMgr(NULL)
{
	CFighterDictator* pOwner = m_pMgr->GetOwner();
	CEntityServer* pChar = pOwner->GetEntity();
	if(!pChar)
	{
		GenErr("方向移动状态的拥有者必须为人物\n");
		return;
	}
	pOwner->CppSetCtrlState(eFCS_ForbitMove, true);
	pOwner->CppSetCtrlState(eFCS_AllowChangeDirection, true);
	MoveByChangeDir();
}

CDirMoveStateServer::~CDirMoveStateServer()
{
	CFighterDictator* pOwner = m_pMgr->GetOwner();
	CEntityServer* pChar = pOwner->GetEntity();
	if(!pChar)
	{
		GenErr("方向移动状态的拥有者必须为人物\n");
		return;
	}
	pChar->StopMoving();
	pOwner->CppSetCtrlState(eFCS_ForbitMove, false);
	pOwner->CppSetCtrlState(eFCS_AllowChangeDirection, false);
}

void CDirMoveStateServer::RegisterEvent()
{
	GetAllMgr()->GetSpecialStateMgrServer()->RegisterEvent(this, eSET_DirectionOrMoveSpeedChange);
}

pair<bool, bool> CDirMoveStateServer::MessageEvent(uint32 uEventID, CGenericTarget * pTrigger)
{
	switch(uEventID)
	{
	case eSET_DirectionOrMoveSpeedChange:
		MoveByChangeDir();
		break;
	default:
		{
			stringstream sExp;
			sExp << "CDirMoveStateServer::MessageEvent()有奇怪的触发事件" << uEventID << "进来了\n";
			GenErr("CDirMoveStateServer::MessageEvent()有奇怪的触发事件进来了", sExp.str());
			return make_pair(false, false);
		}
	}
	return make_pair(false, true);
}

void CDirMoveStateServer::MoveByChangeDir()
{
	CFighterDictator* pOwner = GetOwner();
	if(!pOwner)
		return;
	CCharacterDictator* pOwnerChar = pOwner->GetCharacter();
	if(pOwnerChar)
		return;

	CDir dir = pOwnerChar->GetMoveDir();

	CFPos pos = pOwnerChar->GetPixelPos();
	CVector2f pVec2f;
	dir.Get(pVec2f);
	pVec2f *= float(eGridSpanForObj);

	CCoreSceneServer* pScene = pOwner->GetScene();
	CMetaSceneServer* pMeta = pScene->GetMetaScene();

	float dx, dy, dx1, dy1;

	const float conBorder = 10.0f;

	if(pVec2f.x >= 0)
	{
		dx1 = pMeta->GetWidthInGrid() * eGridSpanForObj - pos.x - conBorder;
	}
	else
	{
		dx1 = -pos.x + conBorder;
	}

	if(pVec2f.y >= 0)
	{
		dy1 = pMeta->GetHeightInGrid() * eGridSpanForObj - pos.y - conBorder;
	}
	else
	{
		dy1 = -pos.y + conBorder;
	}

	if(pVec2f.y != 0.0f)
	{
		double fdx2 = (double)dy1 * pVec2f.x / pVec2f.y;
		dx = fabs(double(dx1)) < fabs(fdx2) ? dx1 : (float)fdx2;
	}
	else
	{
		dx = dx1;
	}

	if(pVec2f.x != 0.0f)
	{
		double fdy2 = (double)dx1 * pVec2f.y / pVec2f.x;
		dy = fabs(double(dy1)) < fabs(fdy2) ? dy1 : (float)fdy2;
	}
	else
	{
		dy = dy1;
	}

	pos += CFPos(dx, dy);

	float fSpeed = pOwner->m_RunSpeed.Get(pOwner);

	if (m_pMoveMgr && pOwnerChar->IsValid(m_pMoveMgr))
	{
		m_pMoveMgr->MoveTo(pos, fSpeed, eFPT_HypoLine, eBT_LowBarrier);
	}
	else
	{
		pOwnerChar->MoveToInLinePath(pos, fSpeed, eFPT_HypoLine, eBT_LowBarrier);
		m_pMoveMgr = pOwnerChar->GetMoveManager();	
	}
}

