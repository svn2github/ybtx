#pragma once
#include "CDistortedTick.h"
#include "PatternCOR.h"
#include "IObjPosObserverHandler.h"
#include "TFighterAllocator.h"

class CFighterDictator;
class CSkillInstServer;
class CGenericTarget;
class CMagicEffServerSharedPtr;
struct SkillInfo;

class CAniKeyFrameMgrServer : public CDistortedTick ,public CPtCORHandler, public IWatchHandler, public CFighterMallocObject
{
public:
	CAniKeyFrameMgrServer(CFighterDictator* pFrom);
	~CAniKeyFrameMgrServer();
	void RegistSkill(const CMagicEffServerSharedPtr& MagicEff,CSkillInstServer* pInst,CGenericTarget* pTo);
private:
	virtual void OnObserveeLeft();
	virtual void OnTick();
	void OnCOREvent(CPtCORSubject* pSubject,uint32 uEvent,void* pArg);
	void UnregistEvent();
	void DoEffect();
	void FailEffect();
	void ResumeAttack(const CSkillInstServer* pInst);
private:
	CFighterDictator* m_pFrom;
	SkillInfo*	m_pSkillInfo;
};
