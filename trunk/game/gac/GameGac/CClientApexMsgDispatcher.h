#pragma once
#include "CClientMsgDispatcher.h"

class CClientApexMsgDispatcher;

class ClientApexTraits
{
public:
	typedef NoneAction Character;
	typedef NoneAction Fighter;
	typedef NoneAction Actor;
	typedef CClientApexMsgDispatcher Apex;
};

class CClientApexMsgDispatcher
	: public CClientMsgDispatcher< CClientApexMsgDispatcher, ClientApexTraits >
{
	friend class TComMsgDispatcher<CClientApexMsgDispatcher, ClientApexTraits, CConnClient>;
private:
	template<typename CmdClass>
	static void RegisterApexCommand();

	void OnApexCommand(const CGas2GacCpp_ApexProxyMsg *pCmd, void* pParam);
};
