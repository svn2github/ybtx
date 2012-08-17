#include "stdafx.h"
#include "LoadSkillCommon.h"
#include "CTplStateCfgClient.inl"
#include "CTxtTableFile.h"
#include "FightDef.h"
#include "CMagicStateClient.h"
#include "COtherStateClient.h"

bool CBaseStateCfgClient::LoadConfig()
{
	CTplStateCfgClient<CMagicStateClient>::MapTplStateCfgByName::iterator it1 = CTplStateCfgClient<CMagicStateClient>::m_mapCfgByName.begin();
	for(; it1 != CTplStateCfgClient<CMagicStateClient>::m_mapCfgByName.end(); ++it1)
	{
		CfgChk::InsertMap(m_mapGlobalCfgByName, (CBaseStateCfg*)(it1->second->get()));
		CfgChk::InsertMap(m_mapGlobalCfgById, (CBaseStateCfg*)(it1->second->get()));
	}

	CTplStateCfgClient<CTriggerStateClient>::MapTplStateCfgByName::iterator it2 = CTplStateCfgClient<CTriggerStateClient>::m_mapCfgByName.begin();
	for(; it2 != CTplStateCfgClient<CTriggerStateClient>::m_mapCfgByName.end(); ++it2)
	{
		CfgChk::InsertMap(m_mapGlobalCfgByName, (CBaseStateCfg*)(it2->second->get()));
		CfgChk::InsertMap(m_mapGlobalCfgById, (CBaseStateCfg*)(it2->second->get()));
	}

	CTplStateCfgClient<CDamageChangeStateClient>::MapTplStateCfgByName::iterator it3 = CTplStateCfgClient<CDamageChangeStateClient>::m_mapCfgByName.begin();
	for(; it3 != CTplStateCfgClient<CDamageChangeStateClient>::m_mapCfgByName.end(); ++it3)
	{
		CfgChk::InsertMap(m_mapGlobalCfgByName, (CBaseStateCfg*)(it3->second->get()));
		CfgChk::InsertMap(m_mapGlobalCfgById, (CBaseStateCfg*)(it3->second->get()));
	}

	CTplStateCfgClient<CCumuliTriggerStateClient>::MapTplStateCfgByName::iterator it5 = CTplStateCfgClient<CCumuliTriggerStateClient>::m_mapCfgByName.begin();
	for(; it5 != CTplStateCfgClient<CCumuliTriggerStateClient>::m_mapCfgByName.end(); ++it5)
	{
		CfgChk::InsertMap(m_mapGlobalCfgByName, (CBaseStateCfg*)(it5->second->get()));
		CfgChk::InsertMap(m_mapGlobalCfgById, (CBaseStateCfg*)(it5->second->get()));
	}

	CTplStateCfgClient<CSpecialStateClient>::MapTplStateCfgByName::iterator it4 = CTplStateCfgClient<CSpecialStateClient>::m_mapCfgByName.begin();
	for(; it4 != CTplStateCfgClient<CSpecialStateClient>::m_mapCfgByName.end(); ++it4)
	{
		CfgChk::InsertMap(m_mapGlobalCfgByName, (CBaseStateCfg*)(it4->second->get()));
		CfgChk::InsertMap(m_mapGlobalCfgById, (CBaseStateCfg*)(it4->second->get()));
	}

	return true;
}

void CBaseStateCfgClient::UnloadConfig()
{
	m_mapGlobalCfgByName.clear();
	m_mapGlobalCfgById.clear();
}

CBaseStateCfgClient::CBaseStateCfgClient(EStateGlobalType type)
: CBaseStateCfg(type)
{
}

CTplStateCfgClient<CMagicStateClient>::CTplStateCfgClient()
: CBaseStateCfgClient(eSGT_MagicState)
{

}

CTplStateCfgClient<CTriggerStateClient>::CTplStateCfgClient()
: CBaseStateCfgClient(eSGT_TriggerState)
{

}

CTplStateCfgClient<CDamageChangeStateClient>::CTplStateCfgClient()
: CBaseStateCfgClient(eSGT_DamageChangeState)
{

}

CTplStateCfgClient<CCumuliTriggerStateClient>::CTplStateCfgClient()
: CBaseStateCfgClient(eSGT_TriggerState)
{

}

CTplStateCfgClient<CSpecialStateClient>::CTplStateCfgClient()
: CBaseStateCfgClient(eSGT_SpecialState)
{

}

EStateGlobalType CTplStateCfgClient<CMagicStateClient>::GetStateType()
{
	return eSGT_MagicState;
}

EStateGlobalType CTplStateCfgClient<CTriggerStateClient>::GetStateType()
{
	return eSGT_TriggerState;
}

EStateGlobalType CTplStateCfgClient<CDamageChangeStateClient>::GetStateType()
{
	return eSGT_DamageChangeState;
}

EStateGlobalType CTplStateCfgClient<CCumuliTriggerStateClient>::GetStateType()
{
	return eSGT_CumuliTriggerState;
}

EStateGlobalType CTplStateCfgClient<CSpecialStateClient>::GetStateType()
{
	return eSGT_SpecialState;
}



uint32 CTplStateCfgClient<CMagicStateClient>::GetStateTypeFloor()
{
	return eSIC_MagicState;
}

uint32 CTplStateCfgClient<CTriggerStateClient>::GetStateTypeFloor()
{
	return eSIC_TriggerState;
}

uint32 CTplStateCfgClient<CDamageChangeStateClient>::GetStateTypeFloor()
{
	return eSIC_DamageChangeState;
}

uint32 CTplStateCfgClient<CCumuliTriggerStateClient>::GetStateTypeFloor()
{
	return eSIC_CumuliTriggerState;
}

uint32 CTplStateCfgClient<CSpecialStateClient>::GetStateTypeFloor()
{
	return eSIC_SpecialState;
}

