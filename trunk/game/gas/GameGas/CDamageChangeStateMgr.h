#pragma once
#include "CDamageChangeState.h"
#include "CTriggerableStateMgr.h"

class CDamageChangeStateMgrServer
	: public TTriggerableStateMgrServer<CDamageChangeStateCfgServer, CDamageChangeStateServer>
{
public:
	CDamageChangeStateMgrServer(CFighterDictator* pOwner)
		: TTriggerableStateMgrServer<CDamageChangeStateCfgServer, CDamageChangeStateServer>(pOwner)
	{

	}
};
