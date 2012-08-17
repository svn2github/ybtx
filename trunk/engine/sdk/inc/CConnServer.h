#pragma once
//$id$
//对象系统使用的连接对象,负责对象系统相关的网络消息的派发以及组装网络协议并发送到客户端

#include "CDynamicObject.h"
#include "ISend.h"
#include "CLogOwner.h"
#include "CAddress.h"
#include "TConnAllocator.h"

namespace sqr
{
	class CCoreObjectMediator;

	class CConnServer
		:public CLogOwner
		,public ISend
		,public CConnMallocObject
	{
		typedef vector<CConnServer*,TConnAllocator<CConnServer*> > IDVecConnServer;
	public:
		friend class CCoreDispatchShellMsgResult;
		friend class CCoreSetTestingConnResult;

		CConnServer(uint32 uID, char* pAddress, uint32 uPort);
		~CConnServer(void);
		static CConnServer* GetConnServer(uint32 uID);
		/*IPipe* GetPipe()const;*/

		uint32 Send(const void* pData,uint32 uSize);

		void ShutDown(const char* szMsg);
		void LogOnOffLineMsg(const char* msg);
		uint32 GetGlobalID(){return m_uID;};
		CCoreObjectMediator* GetObjectMediator();
		void SetMediator(CCoreObjectMediator* pMediator);

		void* GetRecvData()const;
		size_t GetRecvDataSize()const;
		void PopRecvData(size_t stSize);
		bool IsShuttingDown()const;
		bool GetRemoteAddress(CAddress& Address)const;

		void SendErrMsg(const char* szError);

		//设置key value，并且给sync conn发送信息设置
		void SetConnValue(const char* szKey, const char* szValue);

		//设置延迟
		void SetLatency(uint32 uLatency);
		uint32 GetLatency()const;

	private:
		static IDVecConnServer& GetIDVecConnServer();
		void CacheRecvData(char* pData,size_t stSize);
		void StoreRecvData();

	private:
		uint32					m_uID;		
		bool					m_bShutDownCalled;
		uint32					m_uLatency;
		bool					m_bTestingConn;

		CCoreObjectMediator*	m_pMedObj;

		static char*			ms_pCurRecvBuffer;			//保存最近的一次网络数据的buffer起始地址，用于减少网络数据复制。
		static size_t			ms_uCurRecvBufferSize;
		basic_string<char,std::char_traits<char>,TConnAllocator<char> >				m_strRecvBuffer;			// 接收缓冲区
		CAddress				m_Address;

	};
}
