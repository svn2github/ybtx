#pragma once
#include "CPipeState_Busy.h"


class CPipeState_Connecting
	:public CPipeState_Busy
#ifdef _WIN32
	,public CStateWithEvent
#endif
{
private:
	CAddress	m_RemoteAddress;
public:
	CPipeState_Connecting(CSyncPipe* pPipe,const CAddress& Address,uint32& uResult);
	~CPipeState_Connecting();

	CPipeState* GetPipeState()const;

	void DoTimeOut();
	
	bool SwitchToConnected();

	void ProcessEvent(bool bRead,bool bWrite,bool bError);

	bool Listen(const CAddress& Address);
	bool Connect(const CAddress& Address);
	void ShutDown(bool IfCallback);
	void Register();
	void Unregister();

	bool IsConnecting()const;
};
