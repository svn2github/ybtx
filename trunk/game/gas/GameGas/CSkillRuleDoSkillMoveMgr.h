#pragma once

#include "CNpcAIMallocObject.h"
#include "CDistortedTick.h"
#include "PatternCOR.h"
#include "CSkillRuleDef.h"
#include "CPos.h"

class CSkillRule;
class CCharacterDictator;
class CFighterDictator;

class CSkillRuleDoSkillMoveMgr
	: public CNpcAIMallocObject 
	, public CPtCORHandler
	, CDistortedTick
{
public:
	CSkillRuleDoSkillMoveMgr(CSkillRule* pSkillRule, CFPos targetPos, EDoSkillType eType);
	~CSkillRuleDoSkillMoveMgr();

	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	TPtRefee<CSkillRuleDoSkillMoveMgr, CSkillRule>& GetRefsBySkillRule() {return m_pRefeeBySkillRule;}
private:
	void OnTick();
	void CreateDelayTickToDoSkill();
private:
	CSkillRule*			m_pSkillRule;
	CFighterDictator*	m_pFighter;
	bool				m_bDoSkill;
	EDoSkillType		m_eDoSkillType;
	uint32				m_uMoveId;
	TPtRefee<CSkillRuleDoSkillMoveMgr, CSkillRule>				m_pRefeeBySkillRule;
};
