#pragma once
#include "CServerMsgDispatcher.h"

class CServerCharacterMsgDispatcher
	: public CServerMsgDispatcher< CServerCharacterMsgDispatcher >
{
public:
	static void RegisterMsgHandler();

private:
	template<typename CmdClass>static void RegisterCharacterCommand();
	template<typename CmdClass>void OnCharacterCommand(const CmdClass* pCmd, void* pParam);
};
