#include "stdafx.h"
#include "CNpcAlertMgr.h"
#include "CCharacterDictator.h"
#include "CSyncVariantServer.h"
#include "TSqrAllocator.inl"
#include "CCharacterMediator.h"
#include "NpcInfoMgr.h"
#include "CNpcAlertMember.h"

template class TPtRefee<CNpcAlertMgr, CNpcAI>;

CNpcAlertMgr::CNpcAlertMgr(CNpcAI* pNpcAI, uint32 uTargetID, bool bNormal, uint32 uAlertTime)
:m_pAI(pNpcAI)
,m_bNormal(bNormal)
{
	m_RefsByNpcAI.SetHolder(this);
	pNpcAI->SetNpcAlertMgrHolder(this);
	GetOwner()->StopMoving();
	if (bNormal)
	{
		CCharacterDictator* pTarget = CCharacterDictator::GetCharacterByID(uTargetID);
		uint32 uTargetEnemyNumber = pTarget->GetChaserTargetNum();
		m_uAlertTime = GetOwner()->GetAlertTime(uTargetEnemyNumber);
	}
	else
	{
		m_uAlertTime = uAlertTime;
	}
	m_bAlertEnd = false;
	AddAlertTarget(uTargetID);
	GetOwner()->m_bAlertState = true;
}

CNpcAlertMgr::~CNpcAlertMgr()
{
	GetOwnerCharacter()->UnRegistDistortedTick(this);
	GetOwner()->m_bAlertState = false;

	AlertListIter iter = m_lAlertList.begin();
	for (; iter != m_lAlertList.end(); iter++)
	{
		delete *iter;
	}
	m_lAlertList.clear();
	ClearNpcAlertEffect();
	UpDateAlertTarget();

	GetOwner()->SetNpcAlertMgrHolder(NULL);
}

void CNpcAlertMgr::OnTick()
{
	GetOwnerCharacter()->UnRegistDistortedTick(this);
	UpDateAlertTarget();
	if (!m_bAlertEnd)
	{
		GetOwnerCharacter()->RegistDistortedTick(this, (uint32)(m_uAlertTime*1000*0.25));
		m_bAlertEnd = true;
		OnEnterAlertState();
	}
	else
	{
		OnCanceled(true);
	}	
}

void CNpcAlertMgr::AddAlertTarget(uint32 uTargetID)
{
	if (!GetOwner()->TargetCanBeAlert(uTargetID))
		return;
	if (IsInAlertList(uTargetID))
		return;
	CNpcAlertMember* pMember = new CNpcAlertMember(this, uTargetID);
	if (m_lAlertList.size() == 0)
	{
		m_lAlertList.push_back(pMember);
		DoNpcAlertEffect(m_uAlertTime);
		GetOwnerCharacter()->RegistDistortedTick(this, (uint32)(m_uAlertTime*1000*0.75));
	}
	else
		m_lAlertList.push_back(pMember);
	UpDateAlertTarget();
}

void CNpcAlertMgr::OnCanceled(bool bSuccess)
{
	bool bAlertSuccess = false;
	AlertListIter iter = m_lAlertList.begin();
	if (bSuccess)
	{
		for (; iter != m_lAlertList.end(); iter++)
		{
			CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID((*iter)->GetAlertTargetID());
			if (pCharacter && GetOwner()->CanFight( pCharacter))
			{
				bAlertSuccess = true;
				RegisterDelayAttackTick((*iter)->GetAlertTargetID());
				DoNpcAlertEndEffect();
				break;
			}
		}
	}
	if (!bAlertSuccess)
	{
		CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnCanMove);
		GetOwner()->RevertNpcBirthDir();
	}
	delete this;
}

void CNpcAlertMgr::RemoveAlertTarget(uint32 uTargetID)
{
	if (!IsInAlertList(uTargetID))
		return;
	AlertListIter iter = m_lAlertList.begin();
	if (uTargetID == (*iter)->GetAlertTargetID())
	{
		delete (*iter);
		m_lAlertList.pop_front();
		ClearNpcAlertEffect();
		m_bAlertEnd = false;
		if (m_lAlertList.size() != 0)
		{
			GetOwnerCharacter()->UnRegistDistortedTick(this);
			DoNpcAlertEffect(m_uAlertTime);
			GetOwnerCharacter()->RegistDistortedTick(this, (uint32)(m_uAlertTime*1000*0.75));
		}
	}
	else
	{
		AlertListIter iter_end = m_lAlertList.end();
		for (; iter != iter_end;iter++)
		{
			if ((*iter)->GetAlertTargetID() == uTargetID)
			{
				delete *iter;
				m_lAlertList.erase(iter);
				break;
			}
		}
	}
	
	UpDateAlertTarget();
	if (m_lAlertList.size() == 0)
		OnCanceled(false);
}

CNpcAI*	CNpcAlertMgr::GetOwner()
{
	return m_pAI;
}

CCharacterDictator* CNpcAlertMgr::GetOwnerCharacter()
{
	Ast (m_pAI);
	return m_pAI->GetCharacter();
}

bool CNpcAlertMgr::IsInAlertList(uint32 uTargetID)
{
	AlertMapIter iter = m_mAlertMap.find(uTargetID);
	return iter != m_mAlertMap.end();
}

void CNpcAlertMgr::OnEnterAlertState()
{
	GetOwnerCharacter()->GetAlertActionVariant()->SetNumber((bool)true);
}

void CNpcAlertMgr::OnLeaveAlertState()
{
	GetOwnerCharacter()->GetAlertActionVariant()->SetNumber((bool) false);
}

void CNpcAlertMgr::UpDateAlertTarget()
{
	if (m_lAlertList.size() == 0)
	{
		GetOwnerCharacter()->GetAlertTargetIDVariant()->SetNumber((uint32)0);
		OnLeaveAlertState();
	}
	else
	{
		AlertListIter iter = m_lAlertList.begin();
		GetOwnerCharacter()->GetAlertTargetIDVariant()->SetNumber((uint32)(*iter)->GetAlertTargetID());
	}
}

void CNpcAlertMgr::RegisterDelayAttackTick(uint32 uTargetID)
{
	GetOwner()->m_pNpcAlertDelayAttackTick = new CNpcAI::CNpcAlertDelayAttackTick(GetOwner(), uTargetID);
	GetOwnerCharacter()->RegistDistortedTick(GetOwner()->m_pNpcAlertDelayAttackTick, 0);
}

CCharacterDictator* CNpcAlertMgr::GetAlertTarget()
{
	AlertListIter iter = m_lAlertList.begin();
	if (iter == m_lAlertList.end())
		return NULL;
	CCharacterDictator* pTarget = CCharacterDictator::GetCharacterByID((*iter)->GetAlertTargetID());
	return pTarget;
}

void CNpcAlertMgr::DoNpcAlertEffect(uint32 uAlertTime)
{
	CCharacterDictator* pTarget = GetAlertTarget();
	if (pTarget && pTarget->CastToCharacterMediator())
		pTarget->CastToCharacterMediator()->DoAlertPhaseEffect(GetOwner()->GetCharacter(), uAlertTime);
}

void CNpcAlertMgr::DoNpcAlertEndEffect()
{
	CCharacterDictator* pTarget = GetAlertTarget();
	if (pTarget && pTarget->CastToCharacterMediator())
		pTarget->CastToCharacterMediator()->DoAlertEndEffect(GetOwner()->GetCharacter());
}

void CNpcAlertMgr::ClearNpcAlertEffect()
{
	CCharacterDictator* pOwnCharacter = GetOwner()->GetCharacter();
	if (pOwnCharacter)
		pOwnCharacter->ClearNpcAlertEffect();
}

void CNpcAlertMgr::AddAlertTargetToMap(uint32 uAlertTargetID, CNpcAlertMember* pMember)
{
	pair<AlertMapIter, bool> pairMap = m_mAlertMap.insert(make_pair(uAlertTargetID, pMember));
	Ast (pairMap.second);
}

void CNpcAlertMgr::DelAlertTargetToMap(uint32 uAlertTargetID, CNpcAlertMember* pMember)
{
	AlertMapIter iter = m_mAlertMap.find(uAlertTargetID);
	Ast (iter != m_mAlertMap.end());
	m_mAlertMap.erase(iter);
}

