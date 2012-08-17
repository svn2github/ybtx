#pragma once
#include "stdafx.h"
#include "CClientApexMsgDispatcher.h"

template<typename CmdClass>
void CClientApexMsgDispatcher::RegisterApexCommand()
{
	SetHandler<CmdClass>(&CClientApexMsgDispatcher::OnApexCommand);
}
