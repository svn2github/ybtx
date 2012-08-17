#pragma once
#include "CSkillRule.h"

class CNotNpcTargetChangeRule : public CSkillRule
{
public:
	CNotNpcTargetChangeRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData);
	virtual void OnSkillCondSatify(bool bRepeat = true);
	virtual bool DoRuleSkill();
private:
	void RandomChangeTarget();
};

