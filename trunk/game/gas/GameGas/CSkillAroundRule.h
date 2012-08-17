#pragma once
#include "CSkillRule.h"
#include "TNpcAIAllocator.h"

class CSkillAroundRule 
	: public CSkillRule
{
typedef list<string, TNpcAIAllocator<string> > ListSkill;
typedef ListSkill::const_iterator ListSkillIter;
public:
	CSkillAroundRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData);
	~CSkillAroundRule();
	virtual void OnSkillCondSatify(bool bRepeat = true);
	virtual void DoNoRuleSkill();
private:
	void CheckIsRuleSkill(CSkillRuleData* pSkillRuleData);
	ListSkill m_ListSkill;
};

