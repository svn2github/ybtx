#pragma once
#include "stdafx.h"
#include "CClientCharacterMsgDispatcher.h"

template<typename CmdClass>
void CClientCharacterMsgDispatcher::RegisterCharacterCommand()
{
	SetHandler<CmdClass>(&CClientCharacterMsgDispatcher::OnCharacterCommand);
}
