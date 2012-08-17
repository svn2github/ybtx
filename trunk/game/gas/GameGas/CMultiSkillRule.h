#pragma once
#include "CSkillRule.h"
#include "TNpcAIAllocator.h"

class CMultiSkillRule 
	: public CSkillRule
{
public:
	CMultiSkillRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData);
	~CMultiSkillRule();
	virtual void OnSkillCondSatify(bool bRepeat = true);
private:
	typedef list<string, TNpcAIDataAllocator<string> > SkillNameLst;
	SkillNameLst m_lstSkillName;
};

