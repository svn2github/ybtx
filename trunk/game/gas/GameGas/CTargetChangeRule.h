#pragma once
#include "CSkillRule.h"

class CTargetChangeRule : public CSkillRule
{
public:
	CTargetChangeRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData);
	virtual void OnSkillCondSatify(bool bRepeat = true);
	virtual bool DoRuleSkill();
private:
	void RandomChangeTarget();
};

