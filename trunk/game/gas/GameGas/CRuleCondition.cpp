#include "stdafx.h"
#include "CRuleCondition.h"
#include "CCharacterDictator.h"
#include "CFighterDictator.h"
#include "CSkillRuleBaseData.h"
#include "CSkillRuleMgr.h"
#include "Random.h"

CHpGreaterCond::CHpGreaterCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
	:m_pSkillRule(pSkillRule)
	,m_eRCKType(pRuleData->m_eRCKType)
{
	float fAllHealth = GetCharacter()->CppGetMaxHP();
	m_fVergeHpValue = fAllHealth*(pRuleData->m_fOneVal)/100;

}

bool CHpGreaterCond::CheckCondition()
{
	float fNowHealth = GetCharacter()->CppGetHP();
	if(fNowHealth>m_fVergeHpValue)
		return true;
	else
		return false;
}

bool CHpGreaterCond::AutoCheck()
{
	float fCurHealth = GetSkillRule()->GetSkillRuleMgr()->GetCurrentHp();
	float fPreHealth = GetSkillRule()->GetSkillRuleMgr()->GetFrontHp();
	if(fCurHealth>m_fVergeHpValue && fPreHealth< m_fVergeHpValue)
		return true;
	else
		return false;
}

void CHpGreaterCond::CreateEventHandler()
{
	GetSkillRule()->AddEventHandler(eNpcEvent_OnHpChanged,this);
}

CHpLesserCond::CHpLesserCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
	:m_pSkillRule(pSkillRule)
	,m_eRCKType(pRuleData->m_eRCKType)
{
	float fAllHealth = GetCharacter()->CppGetMaxHP();
	m_fVergeHpValue = fAllHealth*(pRuleData->m_fOneVal)/100;
}

bool CHpLesserCond::CheckCondition()
{
	float fNowHealth = GetCharacter()->CppGetHP();
	if(fNowHealth<m_fVergeHpValue)
		return true;
	else
		return false;
}

bool CHpLesserCond::AutoCheck()
{
	float fCurHealth = GetSkillRule()->GetSkillRuleMgr()->GetCurrentHp();
	float fPreHealth = GetSkillRule()->GetSkillRuleMgr()->GetFrontHp();
	if(fCurHealth<m_fVergeHpValue && fPreHealth> m_fVergeHpValue)
		return true;
	else
		return false;
}

void CHpLesserCond::CreateEventHandler()
{
	GetSkillRule()->AddEventHandler(eNpcEvent_OnHpChanged,this);
}

CMpGreaterCond::CMpGreaterCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
	:m_pSkillRule(pSkillRule)
	,m_eRCKType(pRuleData->m_eRCKType)
{
	float fAllMana =GetCharacter()->CppGetMaxMP();
	m_fVergeMpValue = fAllMana*(pRuleData->m_fOneVal)/100;
}

bool CMpGreaterCond::CheckCondition()
{
	float fNowMana = GetCharacter()->CppGetMP();
	if(fNowMana>m_fVergeMpValue)
		return true;
	else
		return false;
}

bool CMpGreaterCond::AutoCheck()
{
	float fCurMana = GetSkillRule()->GetSkillRuleMgr()->GetCurrentMp();
	float fPreMana = GetSkillRule()->GetSkillRuleMgr()->GetFrontMp();
	if(fCurMana>m_fVergeMpValue && fPreMana< m_fVergeMpValue)
		return true;
	else
		return false;
}

void CMpGreaterCond::CreateEventHandler()
{
	GetSkillRule()->AddEventHandler(eNpcEvent_OnMpChanged,this);
}

CMpLesserCond::CMpLesserCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
	:m_pSkillRule(pSkillRule)
	,m_eRCKType(pRuleData->m_eRCKType)
{
	float fAllMana =GetCharacter()->CppGetMaxMP();
	m_fVergeMpValue = fAllMana*(pRuleData->m_fOneVal)/100;
}

bool CMpLesserCond::CheckCondition()
{
	float fNowMana = GetCharacter()->CppGetMP();
	if(fNowMana<m_fVergeMpValue)
		return true;
	else
		return false;
}

bool CMpLesserCond::AutoCheck()
{
	float fCurMana = GetSkillRule()->GetSkillRuleMgr()->GetCurrentMp();
	float fPreMana = GetSkillRule()->GetSkillRuleMgr()->GetFrontMp();
	if(fCurMana<m_fVergeMpValue && fPreMana> m_fVergeMpValue)
		return true;
	else
		return false;
}

void CMpLesserCond::CreateEventHandler()
{
	GetSkillRule()->AddEventHandler(eNpcEvent_OnMpChanged,this);
}

CEnterBattleCond::CEnterBattleCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
	:m_pSkillRule(pSkillRule)
	,m_eRCKType(pRuleData->m_eRCKType)
{
	
}

bool CEnterBattleCond::CheckCondition()
{
	return false;
}

bool CEnterBattleCond::AutoCheck()
{
	return true;
}

void CEnterBattleCond::CreateEventHandler()
{
	GetSkillRule()->AddEventHandler(eNpcEvent_OnEnterBattleState,this);
}

CBeHurtValueGreater::CBeHurtValueGreater(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
	:m_pSkillRule(pSkillRule)
	,m_eRCKType(pRuleData->m_eRCKType)
{
	m_iBeHurtValue = static_cast<int32>(pRuleData->m_fOneVal);
}

bool CBeHurtValueGreater::CheckCondition()
{
	return false;
}

bool CBeHurtValueGreater::AutoCheck()
{
	CCharacterDictator* pCharacter = GetCharacter();
	Ast(pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	Ast(pFighter);
	CSkillRuleMgr* pRuleMgr = GetSkillRule()->GetSkillRuleMgr();
	Ast (pRuleMgr);
	int32 iBeHurtHp = (-1) * pRuleMgr->GetHpChange();
	if(iBeHurtHp>m_iBeHurtValue)
		return true;
	else
		return false;
}

void CBeHurtValueGreater::CreateEventHandler()
{
	GetSkillRule()->AddEventHandler(eNpcEvent_OnBehurt,this);
}

CRandomRate::CRandomRate(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
	:m_pSkillRule(pSkillRule)
	,m_eRCKType(pRuleData->m_eRCKType)
{
	m_iRandNum = (int32)pRuleData->m_fOneVal;
}

bool CRandomRate::CheckCondition()
{
	return Random::Rand(0, 100) <= m_iRandNum;
}

bool CRandomRate::AutoCheck()
{
	return false;
}

void CBattleTimerExcess::CBattleTimeTick::OnTick()
{
	CSkillRule* pSkillRule = m_pBattleTimer->GetSkillRule();
	if(eRCK_Begin == m_pBattleTimer->m_eRCKType)
	{
		bool bResult = pSkillRule->CheckContinueCond();
		if(bResult)
			pSkillRule->OnSkillCondSatify();
	}
	else
	{
		pSkillRule->UnInstallContinueCond();
	}
	pSkillRule->GetCharacter()->UnRegistDistortedTick(this);
}

CBattleTimerExcess::CBattleTimerExcess(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
	:m_pSkillRule(pSkillRule)
	,m_eRCKType(pRuleData->m_eRCKType)
{
	m_uBattleTime = (uint32)(pRuleData->m_fOneVal*1000);
	m_pTick = NULL;
}

bool CBattleTimerExcess::CheckCondition()
{
	return false;
}

bool CBattleTimerExcess::AutoCheck()
{
	if (!m_pTick)
		m_pTick = new CBattleTimeTick(this);
	GetCharacter()->RegistDistortedTick(m_pTick, (uint32)m_uBattleTime);
	return false;
}

void CBattleTimerExcess::CreateEventHandler()
{
	GetSkillRule()->AddEventHandler(eNpcEvent_OnEnterBattleState,this);
}

void CBattleTimerExcess::ReInstallCond()
{
	uint64 uBattleTime = (uint64)m_uBattleTime*1000;
	uint64 uStartBattleTime = GetSkillRule()->GetSkillRuleMgr()->GetStartBattleTime();
	uint64 uNowTime = GetCharacter()->GetDistortedFrameTime();
	if(uNowTime - uStartBattleTime > uBattleTime)
		return;
	uint32 uRemainTime = static_cast<uint32>(uBattleTime - uNowTime + uStartBattleTime);
	if (!m_pTick)
		m_pTick = new CBattleTimeTick(this);
	GetCharacter()->RegistDistortedTick(m_pTick, uRemainTime);
}

void CBattleTimerExcess::UnInstallCond()
{
	if(m_pTick)
		GetCharacter()->UnRegistDistortedTick(m_pTick);
}

CBattleTimerExcess::~CBattleTimerExcess()
{
	if (m_pTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pTick);
		delete m_pTick;
		m_pTick = NULL;
	}
}

CDeathCond::CDeathCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
	:m_pSkillRule(pSkillRule)
	,m_eRCKType(pRuleData->m_eRCKType)
{
	pSkillRule->SetSkillRuleKind(eSRK_Dead);
}

bool CDeathCond::CheckCondition()
{
	return false;
}

bool CDeathCond::AutoCheck()
{
	return true;
}

void CDeathCond::CreateEventHandler()
{
}

void CTimerCond::CTimerTick::OnTick()
{
	CSkillRule* pSkillRule = m_pTimerCond->GetSkillRule();
	bool bResult = pSkillRule->CheckContinueCond(m_pTimerCond);
	if(bResult)
		pSkillRule->OnSkillCondSatify(false);
}

CTimerCond::CTimerCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
	:m_pSkillRule(pSkillRule)
	,m_eRCKType(pRuleData->m_eRCKType)
{
	m_uTime = (uint32)(pRuleData->m_fOneVal*1000);
	m_pTick = NULL;
}

bool CTimerCond::CheckCondition()
{
	return false;
}

bool CTimerCond::AutoCheck()
{
	return false;
}

void CTimerCond::ReInstallCond()
{
	if(!m_pTick)
	{
		m_pTick = new CTimerTick(this);
		GetCharacter()->RegistDistortedTick(m_pTick, (uint32)m_uTime);
	}
}

void CTimerCond::UnInstallCond()
{
	if (m_pTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pTick);
		delete m_pTick;
		m_pTick = NULL;
	}
}

CTimerCond::~CTimerCond()
{
	UnInstallCond();
}

void CRangeTimerCond::CRangeTimerTick::OnTick()
{
	CSkillRule* pSkillRule = m_pRangeTimerCond->GetSkillRule();
	pSkillRule->GetCharacter()->UnRegistDistortedTick(this);
	bool bResult = pSkillRule->CheckContinueCond(m_pRangeTimerCond);
	if(bResult)
		pSkillRule->OnSkillCondSatify();
	uint32 uTime = Random::Rand((uint32)m_pRangeTimerCond->m_uMinTime, (uint32)m_pRangeTimerCond->m_uMaxTime);
	pSkillRule->GetCharacter()->RegistDistortedTick(this,uTime);
}

CRangeTimerCond::CRangeTimerCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
	:m_pSkillRule(pSkillRule)
	,m_eRCKType(pRuleData->m_eRCKType)
{
	m_uMinTime = (uint32)(pRuleData->m_fOneVal*1000);
	m_uMaxTime = (uint32)(pRuleData->m_fTwoVal*1000);
	m_pTick = NULL;
}

bool CRangeTimerCond::CheckCondition()
{
	return false;
}

bool CRangeTimerCond::AutoCheck()
{
	return false;
}

void CRangeTimerCond::ReInstallCond()
{
	if(!m_pTick)
	{
		m_pTick = new CRangeTimerTick(this);
		uint32 uTime = Random::Rand((uint32)m_uMinTime, (uint32)m_uMaxTime);
		GetCharacter()->RegistDistortedTick(m_pTick,uTime);
	}
}

void CRangeTimerCond::UnInstallCond()
{
	if (m_pTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pTick);
		delete m_pTick;
		m_pTick = NULL;
	}
}

CRangeTimerCond::~CRangeTimerCond()
{
	UnInstallCond();
}

CEnterPhaseCond::CEnterPhaseCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
:m_pSkillRule(pSkillRule)
,m_eRCKType(pRuleData->m_eRCKType)
{

}

bool CEnterPhaseCond::CheckCondition()
{
	return false;
}

bool CEnterPhaseCond::AutoCheck()
{
	return true;
}

void CEnterPhaseCond::CreateEventHandler()
{
	GetSkillRule()->AddEventHandler(eNpcEvent_OnEnterBattleState,this);
	GetSkillRule()->AddEventHandler(eNpcEvent_OnEnterPhase,this);
}

void CPhaseTimerExcess::CPhaseTimerTick::OnTick()
{
	CSkillRule* pSkillRule = m_pPhaseTimerCond->GetSkillRule();
	if (eRCK_Begin == m_pPhaseTimerCond->m_eRCKType)
	{
		bool bResult = pSkillRule->CheckContinueCond();
		if(bResult)
			pSkillRule->OnSkillCondSatify();
	} 
	else
	{
		pSkillRule->UnInstallContinueCond();
	}
	pSkillRule->GetCharacter()->UnRegistDistortedTick(this);
}

CPhaseTimerExcess::CPhaseTimerExcess(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
	:m_pSkillRule(pSkillRule)
	,m_eRCKType(pRuleData->m_eRCKType)
{
	m_uTime = (uint32)(pRuleData->m_fOneVal*1000);
	m_pTick = NULL;
}

bool CPhaseTimerExcess::AutoCheck()
{
	if (!m_pTick)
		m_pTick = new CPhaseTimerTick(this);
	GetCharacter()->RegistDistortedTick(m_pTick, (uint32)m_uTime);
	return false;
}

bool CPhaseTimerExcess::CheckCondition()
{
	return false;
}

void CPhaseTimerExcess::UnInstallCond()
{
	if(m_pTick)
		GetCharacter()->UnRegistDistortedTick(m_pTick);
}

void CPhaseTimerExcess::CreateEventHandler()
{
	GetSkillRule()->AddEventHandler(eNpcEvent_OnEnterBattleState,this);
	GetSkillRule()->AddEventHandler(eNpcEvent_OnEnterPhase,this);
}

CPhaseTimerExcess::~CPhaseTimerExcess()
{
	if (m_pTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pTick);
		delete m_pTick;
		m_pTick = NULL;
	}
}

CEnterWanderCond::CEnterWanderCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
:m_pSkillRule(pSkillRule)
,m_eRCKType(pRuleData->m_eRCKType)
{
	pSkillRule->SetSkillRuleKind(eSRK_Wander);
}

bool CEnterWanderCond::CheckCondition()
{
	return false;
}

bool CEnterWanderCond::AutoCheck()
{
	return true;
}

void CEnterWanderCond::CreateEventHandler()
{
	GetSkillRule()->AddEventHandler(eNpcEvent_OnEnterWander,this);
}

void CWanderTimerExcess::CWanderTimerTick::OnTick()
{
	CSkillRule* pSkillRule = m_pWanderTimerCond->GetSkillRule();
	if (eRCK_Begin == m_pWanderTimerCond->m_eRCKType)
	{
		bool bResult = pSkillRule->CheckContinueCond();
		if(bResult)
			pSkillRule->OnSkillCondSatify();
	} 
	else
	{
		pSkillRule->UnInstallContinueCond();
	}
	pSkillRule->GetCharacter()->UnRegistDistortedTick(this);
}

CWanderTimerExcess::CWanderTimerExcess(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
:m_pSkillRule(pSkillRule)
,m_eRCKType(pRuleData->m_eRCKType)
{
	m_uTime = (uint32)(pRuleData->m_fOneVal*1000);
	m_pTick = NULL;
	pSkillRule->SetSkillRuleKind(eSRK_Wander);
}

bool CWanderTimerExcess::AutoCheck()
{
	if (!m_pTick)
		m_pTick = new CWanderTimerTick(this);
	GetSkillRule()->GetCharacter()->RegistDistortedTick(m_pTick, (uint32)m_uTime);
	return false;
}

bool CWanderTimerExcess::CheckCondition()
{
	return false;
}

void CWanderTimerExcess::UnInstallCond()
{
	if(m_pTick)
		GetCharacter()->UnRegistDistortedTick(m_pTick);
}

void CWanderTimerExcess::CreateEventHandler()
{
	GetSkillRule()->AddEventHandler(eNpcEvent_OnEnterWander,this);
}

CWanderTimerExcess::~CWanderTimerExcess()
{
	if (m_pTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pTick);
		delete m_pTick;
		m_pTick = NULL;
	}
}

CEnemyDead::CEnemyDead(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
:m_pSkillRule(pSkillRule)
,m_eRCKType(pRuleData->m_eRCKType)
{

}

bool CEnemyDead::AutoCheck()
{
	return true;
}

bool CEnemyDead::CheckCondition()
{
	return false;
}

void CEnemyDead::CreateEventHandler()
{
	GetSkillRule()->AddEventHandler(eNpcEvent_OnEnemyDead,this);
}

CHpFirstGreaterCond::CHpFirstGreaterCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
:CHpGreaterCond(pSkillRule,pRuleData)
,m_bInEffect(true)
{

}

bool CHpFirstGreaterCond::AutoCheck()
{
	if(m_bInEffect)
	{
		if (CHpGreaterCond::AutoCheck())
		{
			m_bInEffect = false;
			return true;
		} 
		else
		{
			return false;
		}
	}
	return false;
}

CHpFirstLesserCond::CHpFirstLesserCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
:CHpLesserCond(pSkillRule,pRuleData)
,m_bInEffect(true)
{

}

bool CHpFirstLesserCond::AutoCheck()
{
	if(m_bInEffect)
	{
		if (CHpLesserCond::AutoCheck())
		{
			m_bInEffect = false;
			return true;
		} 
		else
		{
			return false;
		}
	}
	return false;
}

CMpFirstGreaterCond::CMpFirstGreaterCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
:CMpGreaterCond(pSkillRule,pRuleData)
,m_bInEffect(true)
{

}

bool CMpFirstGreaterCond::AutoCheck()
{
	if(m_bInEffect)
	{
		if (CMpGreaterCond::AutoCheck())
		{
			m_bInEffect = false;
			return true;
		} 
		else
		{
			return false;
		}
	}
	return false;
}

CMpFirstLesserCond::CMpFirstLesserCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData)
:CMpLesserCond(pSkillRule,pRuleData)
,m_bInEffect(true)
{

}

bool CMpFirstLesserCond::AutoCheck()
{
	if(m_bInEffect)
	{
		if (CMpLesserCond::AutoCheck())
		{
			m_bInEffect = false;
			return true;
		} 
		else
		{
			return false;
		}
	}
	return false;
}

