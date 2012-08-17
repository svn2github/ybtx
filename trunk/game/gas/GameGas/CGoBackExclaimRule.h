#pragma once
#include "CSkillRule.h"

class CGoBackExclaimRule : public CSkillRule
{
public:
	CGoBackExclaimRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData);
	virtual void OnSkillCondSatify(bool bRepeat = true);
	virtual bool DoRuleSkill();
	virtual EDoSkillType CheckSkillDistance();
private:
	typedef list<string, TNpcAIDataAllocator<string> > SkillNameLst;
	SkillNameLst m_lstSkillArgs;
	float		m_fSize;						//ºô¾È·¶Î§
	uint32		m_uNum;							//ºô¾ÈµÄÊýÄ¿
};

