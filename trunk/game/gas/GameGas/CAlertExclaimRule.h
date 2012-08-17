#pragma once

#include "CSkillRule.h"

class CSkillRuleMgr;

class CAlertExclaimRule
	: public CSkillRule
{
public:
	CAlertExclaimRule(CSkillRuleMgr* pRuleMgr, CSkillRuleData* pSkillRuleData);
	~CAlertExclaimRule();

	virtual void OnSkillCondSatify(bool bRepeat = true);
	virtual void DoNoRuleSkill();
private:
	void NotifyEnterAlert();
private:
	typedef list<string, TNpcAIDataAllocator<string> > SkillNameLst;
	SkillNameLst m_lstSkillArgs;
	float		m_fSize;						//ºô¾È·¶Î§
	uint32		m_uNum;							//ºô¾ÈµÄÊýÄ¿
	uint32		m_uAlertTime;
};
