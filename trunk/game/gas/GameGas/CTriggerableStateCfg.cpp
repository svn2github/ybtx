#include "stdafx.h"
#include "CTriggerableStateCfg.h"
#include "CCfgCalc.inl"
#include "BaseHelper.h"

CTriggerableStateCfgServer::MapTargetChange CTriggerableStateCfgServer::m_mapTargetChange;

CTriggerableStateCfgServer::CTriggerableStateCfgServer(EStateGlobalType type)
: CBaseStateCfgServer(type)
{
}

CTriggerableStateCfgServer::CTriggerableStateCfgServer(const CTriggerableStateCfgServer& cfg)
: CBaseStateCfgServer(cfg)
,m_calcInitialValue(new CCfgCalc(*(cfg.m_calcInitialValue)))
,m_calcProbability(new CCfgCalc(*(cfg.m_calcProbability)))
,m_uTriggerEvent(cfg.m_uTriggerEvent)
{
}

CTriggerableStateCfgServer::~CTriggerableStateCfgServer()
{
	SafeDelete(m_calcInitialValue);
	SafeDelete(m_calcProbability);
}


