#pragma once

#include "IScriptConnMgrHandler.h"
#include "CConnServer.h"

class IScriptConnMgrServerHandler
	: public IScriptConnMgrHandler<CConnServer>
{
public:
	virtual void OnAccepted(CConnServer* pConn) 
	{ 
		CALL_CLASS_CALLBACK_1(pConn) 
	};
	
	virtual void OnBeginServiceFailed(const char* szErrMsg)
	{
		CALL_CLASS_CALLBACK_1(szErrMsg)
	}

	virtual void OnServiceEnded()
	{
		CALL_CLASS_CALLBACK();
	}

	virtual void OnBeginServiceSucceeded()
	{
		CALL_CLASS_CALLBACK()
	}

	virtual void OnCheat(CConnServer* pConn)
	{ 
		CALL_CLASS_CALLBACK_1(pConn) 
	}
};
