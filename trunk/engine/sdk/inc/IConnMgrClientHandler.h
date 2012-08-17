#pragma once
#include "TConnMgrHandler.h"
#include "CTraitsClient.h"

namespace sqr
{
	class CConnClient;

	enum EConnConnectFailedReason
	{
		eCCFR_UNREACHABLE,
		eCCFR_REFUSED,
		eCCFR_INVALIDADDR,
		eCCFR_OSERROR
	};

	class IConnMgrClientHandler
		:public TConnMgrHandler<CTraitsClient>
	{
	public:
		virtual void OnConnected(CConnClient* pConn) {};
		virtual void OnConnectFailed(CConnClient* pConn,EConnConnectFailedReason eReason) {};
	};
}
