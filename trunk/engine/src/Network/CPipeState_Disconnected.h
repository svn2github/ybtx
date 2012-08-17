#pragma once
#include "CPipeState.h"


class CPipeState_Disconnected
	:public CPipeState
{
public:
	CPipeState_Disconnected(CSyncPipe* pPipe);
	~CPipeState_Disconnected();

	bool Listen(const CAddress& Address);
	bool Connect(const CAddress& Address);
	void ShutDown(bool IfCallback);
	void Register();
	void Unregister();
};
