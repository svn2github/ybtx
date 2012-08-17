#include "stdafx.h"
#include "CNpcEnmityMember.h"
#include "CCharacterDictator.h"
#include "CEnmityMemberCheckTick.h"
#include "BaseHelper.h"
#include "CNpcChaosCoolDownTick.h"
#include "CBattleStateManager.h"

CNpcEnmityMember::CNpcEnmityMember(CCharacterDictator* pEntitySelf, CCharacterDictator* pEntityTarget, CEnmityMgr* pEnmityMgr, 
										bool bHaveHurtEffect, float fLockEnemyRangeInPixel)
:m_uTargetEntityID(pEntityTarget->GetEntityID())
,m_pEnmityMgr(pEnmityMgr)
,m_pEntitySelf(pEntitySelf)
,m_pChangeEnemyTick(NULL)
,m_pChaosCoolDownTick(NULL)
,m_bHaveAttackOnce(false)
,m_bHaveHurtEffect(bHaveHurtEffect)
{
	//群组成员不需要watch每个仇恨对象，只由虚拟Npc一个人watch就行了，虚拟Npc会通知其成员作相关操作
	if (!m_pEntitySelf->GetNpcAIHandler()->CastToMemberAI())
		m_pEntitySelf->Watch(this, pEntityTarget, fLockEnemyRangeInPixel);
	
	CCharacterDictator* pEnemy = CCharacterDictator::GetCharacterByID(m_uTargetEntityID);
	Ast (pEnemy);
	CFighterDictator* pEnemyFighter = pEnemy->GetFighter();
	Ast (pEnemyFighter);
	if (!m_bHaveHurtEffect)
		pEnemyFighter->Attach(this, eCE_Attack);

	EnmityMgrDoAttach();
	NotifyEnemyEnterBattle();
	m_pChangeEnemyTick = new CEnmityMemberCheckTick(m_pEnmityMgr, this);
	m_pEnmityMgr->AddEnmityMember(m_uTargetEntityID, this);
}

CNpcEnmityMember::~CNpcEnmityMember()
{
	Ast (m_pEnmityMgr);

	if (!m_pEntitySelf->GetNpcAIHandler()->CastToMemberAI())
		m_pEntitySelf->StopWatching(this);

	CCharacterDictator* pEnemy = CCharacterDictator::GetCharacterByID(m_uTargetEntityID);
	if (pEnemy)
	{
		CFighterDictator* pEnemyFighter = pEnemy->GetFighter();
		pEnemyFighter->Detach(this, eCE_Attack);
	}

	EnmityMgrDoDetach();
	NotifyEnemyLeaveBattle();
	CFighterDictator* pFighter = m_pEntitySelf->GetFighter();
	Ast (pFighter);
	pFighter->Detach(this, eCE_BeAttacked);
	pFighter->Detach(this, eCE_Attack);
	if (m_pChangeEnemyTick)
	{
		m_pEntitySelf->UnRegistDistortedTick(m_pChangeEnemyTick);
		SafeDelete(m_pChangeEnemyTick);
	}
	if (m_pChaosCoolDownTick)
	{
		m_pEntitySelf->UnRegistDistortedTick(m_pChaosCoolDownTick);
		SafeDelete(m_pChaosCoolDownTick);
	}
	m_pEnmityMgr->DelEnmityMember(m_uTargetEntityID, this);
}

uint32 CNpcEnmityMember::GetEnemyEntityID()
{
	return m_uTargetEntityID;
}

void CNpcEnmityMember::ReMoveSelfFromEnmityList()
{
	CCharacterDictator* pEnemy = CCharacterDictator::GetCharacterByID(m_uTargetEntityID);
	if (pEnemy && m_pEnmityMgr)
		m_pEnmityMgr->RemoveFarawayEnemy(pEnemy);
}

void CNpcEnmityMember::OnWatched(EWatchResult eResult)
{
	if (eResult == eWR_NotInScope)
		ReMoveSelfFromEnmityList();
}

void CNpcEnmityMember::OnObserveeEntered()
{
}

void CNpcEnmityMember::OnObserveeLeft()
{
	ReMoveSelfFromEnmityList();
}

void CNpcEnmityMember::AttachAttackMsg()
{
	CFighterDictator* pFighter = m_pEntitySelf->GetFighter();
	if (m_pChangeEnemyTick->Registered())
		m_pEntitySelf->UnRegistDistortedTick(m_pChangeEnemyTick);
	m_pEntitySelf->RegistDistortedTick(m_pChangeEnemyTick, 8000);
	if (pFighter)
	{
		pFighter->Attach(this, eCE_BeAttacked);
		pFighter->Attach(this, eCE_Attack);
	}
}

void CNpcEnmityMember::DetachAttackMsg()
{
	CFighterDictator* pFighter = m_pEntitySelf->GetFighter();
	m_pEntitySelf->UnRegistDistortedTick(m_pChangeEnemyTick);
	m_bHaveAttackOnce = false;
	if (m_pChaosCoolDownTick)
	{
		m_pEntitySelf->UnRegistDistortedTick(m_pChaosCoolDownTick);
		SafeDelete(m_pChaosCoolDownTick);
	}
	if (pFighter)
	{
		pFighter->Detach(this, eCE_BeAttacked);
		pFighter->Detach(this, eCE_Attack);
	}
}

void CNpcEnmityMember::UpDateCheckTick()
{
	if (m_pChangeEnemyTick)
	{
		m_pEntitySelf->UnRegistDistortedTick(m_pChangeEnemyTick);
		m_pEntitySelf->RegistDistortedTick(m_pChangeEnemyTick, 8000);
	}
}

void CNpcEnmityMember::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	CCharacterDictator* pEnemy = CCharacterDictator::GetCharacterByID(m_uTargetEntityID);
	Ast (pEnemy);
	CFighterDictator* pEnemyFighter = pEnemy->GetFighter();
	Ast (pEnemyFighter);
	CFighterDictator* pFighterSelf = m_pEntitySelf->GetFighter();
	Ast (pFighterSelf);

	if (pSubject == pEnemyFighter)			//敌人消息
	{
		CFighterDictator* pArgFighter = reinterpret_cast<CFighterDictator*>(pArg);
		Ast (pArgFighter);
		if (uEvent == eCE_Attack && pArgFighter == pFighterSelf)
		{
			m_bHaveHurtEffect = true;
			pEnemyFighter->Detach(this, eCE_Attack);
		}
	}
	else									//Npc自己消息
	{
		switch(uEvent)
		{
		case eCE_BeAttacked:
			{
				CFighterDictator* pFighter = reinterpret_cast<CFighterDictator*>(pArg);
				if (pFighter && pFighter == pEnemyFighter)
				{
					UpDateCheckTick();
				}
			}
			break;

		case eCE_Attack:
			{
				m_bHaveAttackOnce = true;
				pFighterSelf->Detach(this, eCE_Attack);
				UpDateCheckTick();
			}
			break;

		default:
			break;
		}
	}
}

void CNpcEnmityMember::CreateChaosTick(uint32 uChaosTime)
{	
	if (!m_pChaosCoolDownTick)
		m_pChaosCoolDownTick = new CNpcChaosCoolDownTick(this);
	if (m_pChaosCoolDownTick->Registered())
		m_pEntitySelf->UnRegistDistortedTick(m_pChaosCoolDownTick);
	m_pEntitySelf->RegistDistortedTick(m_pChaosCoolDownTick, uChaosTime);
}

bool CNpcEnmityMember::CanChangeEnemy()
{
	//已经攻击过一次，并且不存在嘲讽冷却tick，此时才能换仇恨
	if (m_bHaveAttackOnce && !m_pChaosCoolDownTick) 
		return true;
	return false;
}

bool CNpcEnmityMember::BeHaveMadeHurtEffect() const
{
	return m_bHaveHurtEffect;
}

void CNpcEnmityMember::EnmityMgrDoAttach()
{
	Ast (m_pEntitySelf);
	CFighterDictator* pOwnerFighter = m_pEntitySelf->GetFighter();
	Ast (pOwnerFighter);
	CCharacterDictator* pEnemy = CCharacterDictator::GetCharacterByID(m_uTargetEntityID);
	if (pEnemy)
	{
		CFighterDictator* pEnemyFighter = pEnemy->GetFighter();
		//pOwnerFighter->Attach(m_pEnmityMgr, eCE_ChangeMapBegin);
		pOwnerFighter->Attach(m_pEnmityMgr, eCE_BattleRelationChange);
		pEnemyFighter->Attach(m_pEnmityMgr, eCE_Die);
		pEnemyFighter->Attach(m_pEnmityMgr, eCE_Offline);
		pEnemyFighter->Attach(m_pEnmityMgr, eCE_ChangeMapBegin);
		pEnemyFighter->Attach(m_pEnmityMgr, eCE_EntryEnmity);
		pEnemyFighter->Attach(m_pEnmityMgr, eCE_BattleRelationChange);
		pEnemyFighter->Attach(m_pEnmityMgr, eCE_DebaseEnmity);
	}

}

void CNpcEnmityMember::EnmityMgrDoDetach()
{
	Ast (m_pEntitySelf);
	CFighterDictator* pOwnerFighter = m_pEntitySelf->GetFighter();
	Ast (pOwnerFighter);
	CCharacterDictator* pEnemy = CCharacterDictator::GetCharacterByID(m_uTargetEntityID);
	if (pEnemy)
	{
		CFighterDictator* pEnemyFighter = pEnemy->GetFighter();
		Ast (pEnemyFighter);
		//pOwnerFighter->Detach(m_pEnmityMgr, eCE_ChangeMapBegin);
		pOwnerFighter->Detach(m_pEnmityMgr, eCE_BattleRelationChange);
		pEnemyFighter->Detach(m_pEnmityMgr, eCE_Die);
		pEnemyFighter->Detach(m_pEnmityMgr, eCE_Offline);
		pEnemyFighter->Detach(m_pEnmityMgr, eCE_ChangeMapBegin);
		pEnemyFighter->Detach(m_pEnmityMgr, eCE_EntryEnmity);
		pEnemyFighter->Detach(m_pEnmityMgr, eCE_BattleRelationChange);
		pEnemyFighter->Detach(m_pEnmityMgr, eCE_DebaseEnmity);
	}
}

void CNpcEnmityMember::NotifyEnemyEnterBattle()
{
	CCharacterDictator* pEnemy = CCharacterDictator::GetCharacterByID(m_uTargetEntityID);
	if(pEnemy && !pEnemy->GetNpcAIHandler())
	{
		CFighterDictator* pEnemyFighter = pEnemy->GetFighter();
		Ast(pEnemyFighter);
		CBattleStateManager* pEnemyBattleMgr = pEnemyFighter->GetBattleStateMgr();
		Ast(pEnemyBattleMgr);
		pEnemyBattleMgr->AddBattleRefByNpc();		
	}
}

void CNpcEnmityMember::NotifyEnemyLeaveBattle()
{
	CCharacterDictator* pEnemy = CCharacterDictator::GetCharacterByID(m_uTargetEntityID);
	if (pEnemy && !pEnemy->GetNpcAIHandler())
	{
		CFighterDictator* pEnemyFighter = pEnemy->GetFighter();
		Ast(pEnemyFighter);
		CBattleStateManager* pEnemyBattleMgr = pEnemyFighter->GetBattleStateMgr();
		Ast(pEnemyBattleMgr);
		pEnemyBattleMgr->DelBattleRefByNpc();	
	}	
}



