 #pragma once
#include "IPipeHandler.h"
#include "CAddress.h"
#include "CTick.h"

namespace sqr
{
	class IPipeModule;
	class IPipeReactor;
	class IPipe;
	enum EPipeDisconnectReason;

	class CListenPipeHandler;

	class CApexConnHandler
		: public IPipeHandler
		, public CTick
	{
		friend class CConnServer;
		template <typename T> friend class TConnection;
	public:
		CApexConnHandler();
		~CApexConnHandler(void);
		void StartApexProxy(const CAddress& addr);
		void StopApexProxy(void);
		void Send(const void* pData,uint32 uSize);
		
		void OnTick();
	private:
		void OnConnected();
		void OnConnectFailed(EPipeConnFailedReason eReason);
		void OnDisconnected(EPipeDisconnectReason eReason,bool bGracefully);
		void OnDataReceived();
		void OnDataSent(uint32 uSentSize);

		IPipe*	m_Pipe;
		CAddress m_pAddress;
	};
};
