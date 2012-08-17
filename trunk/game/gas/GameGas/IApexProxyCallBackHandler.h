#pragma once
#include "CallBackX.h"
#include "CConnServer.h"

class IApexProxyCallBackHandler
	: public virtual CDynamicObject
{
public:
	IApexProxyCallBackHandler(){};
	~IApexProxyCallBackHandler(){};
	virtual void OnNoticeBeKilledUser(CConnServer* pConn)
	{ 
		CALL_CLASS_CALLBACK_1(pConn) 
	}
};
