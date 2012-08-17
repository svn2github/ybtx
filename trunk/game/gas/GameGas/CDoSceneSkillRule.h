#pragma once
#include "CSkillRule.h"

class CDoSceneSkillRule
	:public CSkillRule
{
	typedef list<string, TNpcAIDataAllocator<string> > SkillNameLst;
public:
	CDoSceneSkillRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData);
	~CDoSceneSkillRule();

	void CheckSkill();
	virtual EDoSkillType CheckSkillDistance() { return EDoSkillType_DirectDo;} //不检测距离直接释放
	virtual bool DoRuleSkill();
	virtual void DoNoRuleSkill();
private:
	string			m_strSceneName;
	CFPos			m_PixelPos;
	bool			m_bDoSkill;
};
