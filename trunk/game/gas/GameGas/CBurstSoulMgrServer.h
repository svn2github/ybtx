#pragma once
#include "CFighterMediator.h"
#include "PatternRef.h"
#include "CTick.h"
#include "TFighterAllocator.h"

class CSkillInstServer;
class CBurstSoulCfgServerSharedPtr;
class CBurstSoulServer;

class CBurstSoulMgrServer
	:public CFighterMallocObject
	,public CPtCORHandler
{
public:
	typedef vector<CBurstSoulServer*,TFighterAllocator<CBurstSoulServer*> > VecBurstSoulServer;
	CBurstSoulMgrServer(CFighterDictator* pFighter);
	~CBurstSoulMgrServer(void);

	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	uint32 DoBurstSoulExtraEff(CSkillInstServer* pInst, CFighterDictator* pTo, EBurstSoulType eBurstSoulType);
	uint32 DoDoubleBurstPrize(CSkillInstServer* pInst, CFighterDictator* pTo, uint32 uCount);
	void NotifyBurstSoulArrived(EBurstSoulType eBurstSoulType);
	CFighterDictator*	GetFighter(){return m_pFighter;}
	void				AddBurstSoulTimes(uint32 uTimes);
	void				ConsumeBurstSoulTimes(uint32 uTimes);
	void				OnBurstSoulTimesChanged();
	uint32				GetBurstSoulTimes();
	uint32				GetContinuousBurstTimes()	{return m_uContinuousBurstTimes;}
	void				AddContinuousBurstTimes(int32 uChangeTimes);
	uint32				GetNeedBurstTimesByLevel(uint32 uLevel);
	uint32				GetMaxBurstTimes();
private:
	CFighterDictator*		 m_pFighter;
	VecBurstSoulServer		 m_vecBurstSoulServer;
	uint32					 m_uBurstSoulTimes;
	uint32					 m_uContinuousBurstTimes;
	uint32					 m_uMaxBurstSoulTimes;
};

class CBurstSoulServer
	:public CFighterMallocObject
{
public:
	CSkillInstServer*				m_pSkillInst;
	CBurstSoulCfgServerSharedPtr*	m_pCfg;
	CBurstSoulServer(CSkillInstServer* pInst, const CBurstSoulCfgServerSharedPtr& pCfg);
	~CBurstSoulServer();
};

