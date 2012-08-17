#pragma once
#include "PatternCOR.h"
#include "CDistortedTick.h"
#include "FightDef.h"
#include "PatternRef.h"
#include "TCastingProcessAllocator.h"

//施法流程配置类

class CCastingProcessCfgServer;
class CCastingProcessCfgServerSharedPtr;
class CSkillInstServer;
class CNormalSkillServer;
class CNormalSkillServerSharedPtr;
class CFighterDictator;
class CGenericTarget;
class CMagicEffDataMgrServer;

class CCastingProcess
	:public CDistortedTick
	,public CPtCORHandler
	,public CCastingProcessMallocObject
{
private:
	~CCastingProcess();
	uint32						m_uCount;		
	CCastingProcessCfgServerSharedPtr*	m_pCfg;
	CSkillInstServer*			m_pSkillInst;
	uint64						m_uStartTime;
	float						m_fCastTime;
	float						m_fChannelTime;
	//float						m_fExpiredTime;
	ECastingProcessType			m_eCastType;
	uint32						m_uBeAttackedCount;   //最多能被打断的次数
	TPtRefer< CCastingProcess, CFighterDictator >	m_From;
	CGenericTarget*				m_pTo;
	CNormalSkillServerSharedPtr*			m_pNormalSkill;
	CMagicEffDataMgrServer*		m_pSelfCancelEffData;
	CMagicEffDataMgrServer*		m_pObjCancelEffData;
	bool						m_bNoStop;
	bool						m_bIsFinish;
	uint64						m_uTcpExpiredTime;	//流程执行的总时间

public:
	CCastingProcess( const CCastingProcessCfgServerSharedPtr& pCfg, CSkillInstServer* pInst, CFighterDictator* pFrom, CGenericTarget* pTo,const CNormalSkillServerSharedPtr& pSkill);
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	CFighterDictator*				GetFighter() const		{return m_From.Get();}
	void SetFighter(CFighterDictator* pFighter);
	void RegistEvent();
	void UnRegistEvent();
	void InitCastTime();
	void StartCasting();
	void OnBeAttacked(CFighterDictator* pAttacker);
	void CancelCasting();
	void OnBeCancel(CFighterDictator* pAttacker = NULL);
	void OnTick();
	void SyncCasting(CFighterDictator* pFighter);
	bool GetNoStop()			{return m_bNoStop;}
	CCastingProcessCfgServer*	GetCastingProcessCfg()const;
	void DoEffect();
	bool CanBeInterrupted()const;
	CNormalSkillServer* GetNormalSkill();
	bool IsSingType();
};
