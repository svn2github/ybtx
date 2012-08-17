#pragma once
#include "CServerMsgDispatcher.h"

class CServerApexProxyMsgDispatcher
	: public CServerMsgDispatcher< CServerApexProxyMsgDispatcher >
{
public:
	static void RegisterMsgHandler();
private:
	template<typename CmdClass>static void RegisterApexProxyCommand();
	template<typename CmdClass>void OnApexProxyCommand(const CmdClass* pCmd, void* pParam);
};
