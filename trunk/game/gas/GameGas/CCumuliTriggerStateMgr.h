#pragma once
#include "CCumuliTriggerState.h"
#include "CTriggerableStateMgr.h"

class CCumuliTriggerStateMgrServer
	: public TTriggerableStateMgrServer<CCumuliTriggerStateCfgServer, CCumuliTriggerStateServer>
{
public:
	CCumuliTriggerStateMgrServer(CFighterDictator* pOwner)
		: TTriggerableStateMgrServer<CCumuliTriggerStateCfgServer, CCumuliTriggerStateServer>(pOwner)
	{

	}
};
