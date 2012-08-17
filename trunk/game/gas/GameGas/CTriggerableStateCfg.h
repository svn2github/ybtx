#pragma once
#include "CBaseStateCfgServer.h"

enum ETargetChange { eTC_Trigger, eTC_Installer, eTC_TargetOfTrigger, eTC_Self, eTC_TargetOfSelf, eTC_SelfCorpse, eTc_End };

class CTriggerableStateServer;
class CCfgCalc;
class CMagicEffServerSharedPtr;

class CTriggerableStateCfgServer
	: public CBaseStateCfgServer
	//暂时不需要做成虚基类
{
	friend class CTriggerableStateServer;
	typedef map<string, ETargetChange, less<string>, 
		TConfigAllocator<pair<string, ETargetChange > > > MapTargetChange;

public:
	CTriggerableStateCfgServer(EStateGlobalType type);
	CTriggerableStateCfgServer(const CTriggerableStateCfgServer& cfg);
	virtual ~CTriggerableStateCfgServer();

	const CCfgCalc*			GetInitialValue()			{return m_calcInitialValue;}
	const CCfgCalc*			GetProbability()			{return m_calcProbability;}
	const uint32			GetTriggerEvent()			{return m_uTriggerEvent;}
	//virtual const CMagicEffServerSharedPtr& GetCancelableMagicEff()const=0;
protected:
	static MapTargetChange	m_mapTargetChange;

	CCfgCalc*				m_calcInitialValue;			//作用次数初始值表达式
	CCfgCalc*				m_calcProbability;			//作用几率表达式
	uint32					m_uTriggerEvent;			//触发事件


};


