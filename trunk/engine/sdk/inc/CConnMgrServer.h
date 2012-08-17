#pragma once

#include "CDynamicObject.h"
#include "EBeginServiceResultEnum.h"
#include "CConnMallocObject.h"

namespace sqr
{
	class ISend;
	class IConnMgrServerHandler;
	class CLogOwner;
	struct CAddress;

	class CTraitsServer;

	template<typename Traits>
	class TApp;

	class CConnMgrServer
		:public virtual CDynamicObject
		,public CConnMallocObject
	{
		friend class TApp<CTraitsServer>;
		friend class CCoreBeginServiceResult;
		friend class CCoreEndServiceResult;
		friend class CCoreOnNewConnAcceptedResult;
		friend class CCoreOnDisconnectResult;
		friend class CConnServer;
	public:
		CConnMgrServer();
		~CConnMgrServer(void);

		static CConnMgrServer* Inst();
		
		void BeginService(const CAddress& addr,uint32 uSendBufferSize,uint32 uRecvBufferSize);
		void EndService(void);

		uint32 GetConnNum() const;

		void SetHandler(IConnMgrServerHandler* pHandler);
		IConnMgrServerHandler* GetHandler()const;

	private:
		static CConnMgrServer*& IntInst();

		//当服务器出现异常给所有连接发送错误消息
		static void SendErrMsgToConn(const char* szError, CLogOwner* pOwner);

		uint32 Send(const void* pData,uint32 uSize);
		void OnBeginServiceSucceeded();
		void OnBeginServiceFailed(const char* szErrMsg);
		void OnServiceEnded();

		IConnMgrServerHandler* m_pHandler;
		
		EServiceState m_eServiceState;

		bool	m_bDetectedDeadLoopEnabled;

		size_t	m_uConnNum;
	};
}

