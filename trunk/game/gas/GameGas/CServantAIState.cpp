/*
*===============================================================================================
*		
*				Author: xuyuxiang
*		
*		   Description: 召唤兽AI需要加强，所以把召唤兽状态定义函数单独拆出来	
*
*				  备注：因为召唤兽主角和召唤兽自己都是一部删除引擎对象，所以可能出现主角指针为空的情况，此时应直接返回		
*
*===============================================================================================
*/


#include "stdafx.h"
#include "CServantAI.h"
#include "CCharacterDictator.h"
#include "NpcInfoMgr.h"

//-------------------------- Servant状态函数定义 -------------------------

CServantAI* CNpcAI::CNpcStateServantBase::GetOwner() const
{
	return class_cast<CServantAI*>(CNpcStateBase::GetOwner());
}

/************************************************************************/
//			状态：ServantDefenseState          
//			描述：召唤兽被动防御状态，不战斗跟随主人，战斗则切入普攻状态机
//		适用类型：召唤兽(ENpcType_Summon),军械兽(ENpcType_OrdnanceMonster)
/************************************************************************/
DEFINE_NPCSTATE_METHOD(ServantDefenseState, EnterState)
{
	Ast (GetOwner());
	if (!GetOwner()->BeInCommandMoveState())
		GetOwner()->SetFollowState(true);
	CCharacterDictator* pServant = GetOwner()->GetCharacter();
	if(NULL == pServant)
		return pEvent;
	CCharacterDictator* pMaster = pServant->GetMaster();
	if (NULL == pMaster)
		return pEvent;
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ServantDefenseState, OnEvent)
{
	Ast (GetOwner());
	CCharacterDictator* pServant = GetOwner()->GetCharacter();
	Ast(pServant);
	CCharacterDictator* pMaster = pServant->GetMaster();
	if (!pMaster)
		return pEvent;

	switch (pEvent->GetID())
	{
	case eNpcEvent_OnMoveEnded :
		{
			pServant->SetAndSyncActionDir(pMaster->GetActionDir());
		}
		break;
	default:
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ServantDefenseState, LeaveState)
{
	GetOwner()->SetFollowState(false);
}
END_DEFINE_NPCSTATE_METHOD

/************************************************************************/
//			状态： ServantActiveState
//			描述： 召唤兽主动状态，有敌人出现则进入战斗，否则跟随主人
//		适用类型： 怪物卡(ENpcType_MonsterCard),分身(ENpcType_Shadow)
/************************************************************************/
DEFINE_NPCSTATE_METHOD(ServantActiveState, EnterState)
{
	Ast (GetOwner());
	if (!GetOwner()->BeInCommandMoveState())
		GetOwner()->SetFollowState(true);
	CCharacterDictator* pServant = GetOwner()->GetCharacter();
	Ast(pServant);
	CCharacterDictator* pMaster = pServant->GetMaster();
	if (!pMaster)
		return pEvent;
	CCharacterDictator* pEnemy = GetOwner()->GetNearestVisionEnemy();
	if (!pEnemy)
		return pEvent;
	CEnmityMgr* pEnmityMgr = GetOwner()->GetEnmityMgr();
	Ast(pEnmityMgr);
	if (pEnmityMgr->AddEnemy(pEnemy, false))
	{
		uint32 uEnemyID = pEnemy->GetEntityID();
		CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnServantDoAttack, eNECI_Zero, reinterpret_cast<void*>(uEnemyID));
	}	
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ServantActiveState, OnEvent)
{
	Ast (GetOwner());
	CCharacterDictator* pServant = GetOwner()->GetCharacter();
	Ast(pServant);
	CCharacterDictator* pMaster = pServant->GetMaster();
	if (NULL == pMaster)
		return pEvent;

	switch (pEvent->GetID())
	{
		case eNpcEvent_OnMoveEnded :
			{
				pServant->SetAndSyncActionDir(pMaster->GetActionDir());
			}
			break;

		case eNpcEvent_OnCharacterInSight :
		{
			CCharacterDictator* pEnemy = CCharacterDictator::GetCharacterByID((size_t)pEvent->GetTag());
			if (NULL == pEnemy)
				return pEvent;
			INpcAIHandler* pHandler = pEnemy->GetNpcAIHandler();
			CNpcAI* pNpcAI = pHandler ? pHandler->CastToNpcAI() : NULL;
			if (pNpcAI && !pNpcAI->GetSynToClient())
				return pEvent;
			if (GetOwner()->CanFight(pEnemy) && GetOwner()->IsInLockEnemyRange(pEnemy->GetFighter()))
			{
				CEnmityMgr* pEnmityMgr = GetOwner()->GetEnmityMgr();
				Ast(pEnmityMgr);
				if (pEnmityMgr->AddEnemy(pEnemy, false))
				{
					uint32 uEnemyID = pEnemy->GetEntityID();
					CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnServantDoAttack, eNECI_Zero, reinterpret_cast<void*>(uEnemyID));
				}
			}
		}	
			break;
		default:
			break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ServantActiveState, LeaveState)
{
	GetOwner()->SetFollowState(false);
}
END_DEFINE_NPCSTATE_METHOD

/************************************************************************/
//			状态： ServantPassivityState
//			描述： 召唤兽被动状态，之跟随主人，不参与战斗
//		适用类型： 宠物(ENpcType_Pet),未展开的移动炮台(ENpcType_Cannon)
/************************************************************************/
DEFINE_NPCSTATE_METHOD(ServantPassivityState, EnterState)
{
	Ast (GetOwner());
	if (!GetOwner()->BeInCommandMoveState())
		GetOwner()->SetFollowState(true);
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ServantPassivityState, OnEvent)
{
	Ast (GetOwner());
	CCharacterDictator* pServant = GetOwner()->GetCharacter();
	Ast(pServant);
	CCharacterDictator* pMaster = pServant->GetMaster();
	if (NULL == pMaster)
		return pEvent;

	switch (pEvent->GetID())
	{
		case eNpcEvent_OnMoveEnded :
			{
				pServant->SetAndSyncActionDir(pMaster->GetActionDir());
			}
		default:
			break;
	}	
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ServantPassivityState, LeaveState)
{
	GetOwner()->SetFollowState(false);
}
END_DEFINE_NPCSTATE_METHOD

