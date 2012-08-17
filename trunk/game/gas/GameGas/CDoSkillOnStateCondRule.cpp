#include "stdafx.h"
#include "CDoSkillOnStateCondRule.h"
#include "CCharacterDictator.h"
#include "CSkillRuleBaseData.h"

CDoSkillOnStateCondRule::CDoSkillOnStateCondRule(CSkillRuleMgr *pRuleMgr, CSkillRuleData *pSkillRuleData)
:CSkillRule(pRuleMgr, pSkillRuleData)
{
	SkillNameLst& lstSkillArg = pSkillRuleData->m_lstMultiArgs;
	SkillNameLst::const_iterator iter = lstSkillArg.begin();
	m_strStateName		= (*iter);
}

CDoSkillOnStateCondRule::~CDoSkillOnStateCondRule()
{

}

void CDoSkillOnStateCondRule::DoNoRuleSkill()
{
	Ast (GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	if ( pCharacter->ExistState(m_strStateName.c_str()) )
	{
		CSkillRule::DoNoRuleSkill();
	}
}

bool CDoSkillOnStateCondRule::DoRuleSkill()
{
	Ast (GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	if ( pCharacter->ExistState(m_strStateName.c_str()) )
	{
		return CSkillRule::DoRuleSkill();
	}
	else
	{
		GetOwner()->OnSkillSuccessEnd();
		return true;
	}
}

