#pragma once
#include "CServerMsgDispatcher.h"

class CServerFighterMsgDispatcher
	: public CServerMsgDispatcher< CServerFighterMsgDispatcher >
{
public:
	static void RegisterMsgHandler();

private:
	template<typename CmdClass>static void RegisterFighterCommand();
	template<typename CmdClass>void OnFighterCommand(const CmdClass* pCmd, void* pParam);
};
