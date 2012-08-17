#pragma once
#include "TConnectionMgr.h"
#include "IPipeHandler.h"
#include "CSyncTraitsServer.h"
#include "ISend.h"
#include "CTick.h"
#include "CLog.h"
#include "CAddress.h"
#include "EBeginServiceResultEnum.h"

namespace sqr
{
	class IPipeModule;
	class IPipeReactor;
	class IPipe;
	class IMulticast;
	class CListenPipeHandler;
	class CLogOwner;
	class CClientGuidLogTick;

	class CSynConnMgrServer
		: public TConnectionMgr<CSyncTraitsServer>
		, private CTick
		, public IPipeHandler
		, public ISend
	{
		friend class CSynConnServer;
		friend class CConnMgrServer;
		template <typename T> friend class TConnection;
		friend class CCoreSendMsgJob;
		friend class CCoreSendErrMsgToAllConnJob;
		friend class CCoreSendErrMsgToConnJob;
		friend class CCoreDoShutDownConnJob;
		template<typename Traits> friend class TApp;
		friend class CClientGuidLogTick;

	public:
		CSynConnMgrServer();
		~CSynConnMgrServer(void);

		EBeginServiceResult BeginService(const CAddress& addr,uint32 uSendBufferSize,uint32 uRecvBufferSize);

		//return false表示原本就没有在listen    关闭侦听端口，并且把所有已经建立好的连接关闭
		bool EndService(void);

		ISend* GetIS()const;

		uint32 GetConnNum() const;

		void SetOffLineLog(const char* msg);

	private:
		//当服务器出现异常给所有连接发送错误消息
		static void SendErrMsgToConn(const char* szError, CLogOwner* pOwner);
		static void SendErrMsgToConn(const char* szError, uint32 uLen, CLogOwner* pOwner);

		uint32 Send(const void* pData,uint32 uSize);
		void MulticastSendCmd(IMulticast*pMulticast, const void* pData, uint32 uSize);

		void OnConnected();
		void OnConnectFailed(EPipeConnFailedReason eReason);
		void OnDisconnected(EPipeDisconnectReason eReason,bool bGracefully);
		void OnAccepted(IPipe* pPipe);
		void OnDataReceived();
		void OnDataSent(uint32 uSentSize);
		void OnListenFailed();
		void OnListenSuccessed();
		void OnClientGuidSet(CSynConnServer* pConn);
	
		void OnTick();

		void CreateClientGuidLog();
		void LogClientGuidInfo(const char* szMsg);
		void CheckClientGuidLog();

		IPipe*							m_pListenPipe;
		IMulticast*						m_pMulticast;
		CLog*							m_pOffLineLog;
		typedef basic_string<char, std::char_traits<char>, TConnAllocator<char> >	String_t;
		String_t	m_szAddress;
		uint32		m_uPort;

		CLog*							m_pClientGuidLog;
		CClientGuidLogTick*				m_pClientGuidLogTick;

		uint32							m_uGuidLogFileNum;

		typedef	std::list<CSynConnServer*,TConnAllocator<CSynConnServer*> >		TestingConn_t;
		TestingConn_t	m_lstTestingConn;	//绑定testing client

	};
}

