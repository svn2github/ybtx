#pragma once
#include "CallBackX.h"
#include "CConnMsgMallocObject.h"

template <typename Conn>
class IScriptConnMgrHandler : public virtual CDynamicObject, public CConnMsgMallocObject
{
public:
	virtual void OnDataReceived(Conn *pConn) 
	{ 
		CALL_CLASS_CALLBACK_1(pConn) 
	}

	virtual void OnDisconnect(Conn* pConn)
	{ 
		CALL_CLASS_CALLBACK_1(pConn) 
	}

	virtual void OnError(Conn* pConn,const TCHAR* szError) 
	{ 
		CALL_CLASS_CALLBACK_2(pConn, szError)
	}
};



