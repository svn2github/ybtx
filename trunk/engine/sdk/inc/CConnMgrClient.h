#pragma once
#include "CTick.h"
#include "TConnectionMgr.h"
#include "CTraitsClient.h"
#include "CAddress.h"

namespace sqr
{
	class IPipeModule;
	class IPipe;

	class IConnMgrClientHandler;
	class CConnClient;
	class CLog;

	template <typename Traits>
	class TConnection;

	class CConnMgrClient
		:public TConnectionMgr<CTraitsClient>
	{
		friend class CConnClient;
	public:
		CConnMgrClient();
		~CConnMgrClient(void);

		CConnClient*	Connect(const CAddress& addr);
	private:
		typedef TConnectionMgr<CTraitsClient>	Parent_t;
		friend class TConnectionMgr<CTraitsClient>;

		void WriteRecvTrafficLog(const char* pLog);
		void SetOffLineLog(const char* msg);
		CLog*	m_pRecvTrafficLog;
		CLog*	m_pOffLineLog;
	};
}

;
