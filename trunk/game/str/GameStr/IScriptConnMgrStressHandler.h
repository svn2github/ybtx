#pragma once

#include "IScriptConnMgrHandler.h"
#include "CConnStress.h"

class IScriptConnMgrStressHandler
	: public IScriptConnMgrHandler<CConnStress>
{
public:
	virtual void OnConnected(CConnStress* pConn) { CALL_CLASS_CALLBACK_1(pConn) };
	virtual void OnConnectFailed(CConnStress* pConn) { CALL_CLASS_CALLBACK_1(pConn) };
};
