#pragma once
#include "CNetworkMallocObject.h"

class CPipeEventObject
	:public CNetworkMallocObject
{
private:
	WSAEVENT	m_Event;
public:
	void Create(const SOCKET& Socket);
	void Destroy();

	const WSAEVENT& Get()const;
};
