#pragma once
#include "CPipeState_Busy.h"

class CPipeState_Listening
	:public CPipeState_Busy
#ifdef _WIN32
	,public CStateWithEvent
#endif
{
public:
	static int CreateBindedSocket(SOCKET& Socket,const CAddress& Address);

	CPipeState_Listening(CSyncPipe* pPipe,SOCKET Socket);
	~CPipeState_Listening();

	CPipeState* GetPipeState()const;

	void ProcessEvent(bool bRead,bool bWrite,bool bError);

	bool Listen(const CAddress& Address);
	bool Connect(const CAddress& Address);
	void ShutDown(bool IfCallback);
	void Register();
	void Unregister();
};
