#pragma once
#include "PatternCOR.h"
#include "CDistortedTick.h"

class CFighterDictator;
class CSkillInstServer;

class CDelayMagicOpTick
	:public CPtCORHandler
	,public CDistortedTick
{
protected:
	CDelayMagicOpTick(CSkillInstServer* pSkillInst, CFighterDictator* pFrom);
	virtual ~CDelayMagicOpTick();

	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	void AttachEvent();
	void DetachEvent();

protected:
	CFighterDictator* m_pFrom;
	CSkillInstServer* m_pSkillInst;
};
