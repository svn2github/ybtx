#include "stdafx.h"
#include "CServerApexProxyMsgDispatcher.h"
#include "PtrlGac2GasCPPDef.h"
#include "CBaseMsgDispatcher.inl"
#include "CConnServer.h"
#include "TMsgDispatcher.inl"
#include "TMsgDispatcher_inl.inl"
#include "TMsgHandler.inl"
#include "CApexProxy.h"

void CServerApexProxyMsgDispatcher::RegisterMsgHandler()
{
	SetHandlerNumber(eGac2GasCPP_ApexProxy_End, -eGac2GasCPP_ApexProxy_Begin);
	RegisterApexProxyCommand<CGac2GasCPP_ApexProxyMsg>();
}

template<typename CmdClass>
void CServerApexProxyMsgDispatcher::RegisterApexProxyCommand()
{
	SetHandler<CmdClass>(&CServerApexProxyMsgDispatcher::OnApexProxyCommand);
}

template<typename CmdClass>
void CServerApexProxyMsgDispatcher::OnApexProxyCommand(const CmdClass* pCmd, void* pParam)
{
	(const_cast<CApexProxy*>(CApexProxy::GetInst()))->OnApexProxyCommand(pCmd, GetConnection());
}
