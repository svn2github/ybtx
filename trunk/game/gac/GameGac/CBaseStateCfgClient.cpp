#include "stdafx.h"
#include "LoadSkillCommon.h"
#include "LoadSkillCommon.inl"
#include "CBaseStateCfgClient.h"
#include "CTableFile.h"
#include "FightDef.h"

class CMagicStateClient;
class CTriggerStateClient;
class CDamageChangeStateClient;
class CSpecialStateClient;

uint32 CBaseStateCfgClient<CMagicStateClient>::GetStateTypeFloor()
{
	return eSIC_MagicState;
}

uint32 CBaseStateCfgClient<CTriggerStateClient>::GetStateTypeFloor()
{
	return eSIC_TriggerState;
}

uint32 CBaseStateCfgClient<CDamageChangeStateClient>::GetStateTypeFloor()
{
	return eSIC_DamageChangeState;
}

uint32 CBaseStateCfgClient<CSpecialStateClient>::GetStateTypeFloor()
{
	return eSIC_SpecialState;
}

