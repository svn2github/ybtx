#pragma once

#include "IScriptConnMgrHandler.h"
#include "CConnClient.h"

class IScriptConnMgrClientHandler
	: public IScriptConnMgrHandler<CConnClient>
{
public:
	virtual void OnConnected(CConnClient* pConn) { CALL_CLASS_CALLBACK_1(pConn) };
	virtual void OnConnectFailed(CConnClient* pConn,EConnConnectFailedReason eReason) { CALL_CLASS_CALLBACK_2(pConn,eReason) };
};