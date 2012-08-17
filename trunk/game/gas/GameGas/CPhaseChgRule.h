#pragma once
#include "CSkillRule.h"

class CPhaseChangeRule : public CSkillRule
{
public:
	CPhaseChangeRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData);
	virtual void OnSkillCondSatify(bool bRepeat = true);
	virtual bool DoRuleSkill();
};

