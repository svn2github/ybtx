#include "stdafx.h"
#include "CSynConnMgrServer.h"
#include "IPipeReactor.h"
#include "CSynConnServer.h"
#include "CSyncTimeSystemServer.h"
#include "CSyncAppServer.h"
#include "TimeHelper.h"
#include "TConnectionMgr.inl"
#include "TMsgDispatcher_inl.inl"
#include "TMsgHandler.inl"
#include "IMulticast.h"
#include "ErrLogHelper.h"
#include "PrtlGas2GacGenericDef.h"
#include "CConnServer.h"
#include "CSynConnMgrServer.h"
#include "CConnMgrServer.h"
#include "ISynConnMgrServerHandler.h"
#include "TSqrAllocator.inl"
#include "CCoreConnServerResult.h"
#include "CCoreServerResult.inl"
#include "ErrLogHelper.h"
#include "ThreadHelper.h"

namespace sqr 
{
	class CClientGuidLogTick : public CTick
	{
	public:
		CClientGuidLogTick(CSynConnMgrServer* pConnMgr);
		~CClientGuidLogTick();

		void OnTick();
	private:
		CSynConnMgrServer*	m_pConnMgr;
	};
}

CClientGuidLogTick::CClientGuidLogTick(CSynConnMgrServer* pConnMgr)
: m_pConnMgr(pConnMgr)
{

}

CClientGuidLogTick::~CClientGuidLogTick()
{

}

void CClientGuidLogTick::OnTick()
{
	m_pConnMgr->CheckClientGuidLog();
}

template class TConnectionMgr<CSyncTraitsServer>;

CSynConnMgrServer::CSynConnMgrServer()
: m_pListenPipe(NULL)
,m_pMulticast(NULL)
,m_szAddress("")
,m_uPort(0)
,m_pClientGuidLog(NULL)
,m_pClientGuidLogTick(NULL)
,m_uGuidLogFileNum(0)
{
	CSynConnServer::InitSynConnMap();

	char buf[256];
	sprintf(buf, "Conn_%d.log", GetCurPID());
	m_pOffLineLog = CLog::CreateLog(buf);
}

CSynConnMgrServer::~CSynConnMgrServer(void)
{
	if (m_pClientGuidLog)
	{
		m_pClientGuidLog->Release();
	}
	m_pOffLineLog->Release();
	if(!m_pListenPipe)
	{
		CSynConnServer::UninitSynConnMap();
		return;
	}
	EndService();

	CSynConnServer::UninitSynConnMap();
}

EBeginServiceResult	CSynConnMgrServer::BeginService(const CAddress& addr,uint32 uSendBufferSize,uint32 uRecvBufferSize)
{
	SQR_TRY
	{	
		m_pListenPipe = CSyncAppServer::Inst()->m_pPipeReactor->
			CreateIPipe(this,uSendBufferSize,uRecvBufferSize,/*ePCF_SendWayEncryption |*/ ePCF_RecvWayDecryption | ePCF_SendWayCompression );

		CSyncAppServer::Inst()->Log("Network start listening");
		m_pListenPipe->Listen(addr); //这里不可能返回false
		m_szAddress = addr.GetAddress();
		m_uPort = addr.GetPort();

		Ast(m_pListenPipe);
		m_pMulticast = CSyncAppServer::Inst()->GetIMulticast();
		//当我们建立起conn mgr之后，服务器端发生异常才有可能去通知客户端
#ifndef _WIN32	
		SetLogCallBackFunc(&CSynConnMgrServer::SendErrMsgToConn);
#endif
		CSyncTimeSystemServer::Inst()->Register( this, 1000 );

		m_pClientGuidLogTick = new CClientGuidLogTick(this);
		//每多少分钟创建一个新的guid log
		CSyncTimeSystemServer::Inst()->Register(m_pClientGuidLogTick, GetLogFileInterval());

	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
		(new CCoreBeginServiceResult(false, "BeginService发生异常！"))->Add();
		return eBSR_PortBinded;
	}
	SQR_TRY_END;

	return eBSR_Success;
}

bool CSynConnMgrServer::EndService()
{
	if( NULL == m_pListenPipe )
		return false;

	CSyncTimeSystemServer::Inst()->UnRegister(m_pClientGuidLogTick);
	delete m_pClientGuidLogTick;

	CSyncTimeSystemServer::Inst()->UnRegister( this );

	m_pMulticast->Release();

	// 关闭监听的pipe
	m_pListenPipe->Release();
	CSyncAppServer::Inst()->Log("网络侦听结束");

	m_pListenPipe=NULL;
	ShutDownAllConn();

#ifndef _WIN32
	SetLogCallBackFunc(NULL);
#endif

	(new CCoreEndServiceResult())->Add();

	return true;
}

// 新的client连入
void CSynConnMgrServer::OnAccepted(IPipe* pPipe)
{
#ifdef WIN32
#define MAX_USUAL_TIME_PERCENT 95
#define MAX_CLIENT_CONN 40
	if(GetProcessTime() % 100 > MAX_USUAL_TIME_PERCENT)
	{
		if(m_listAllConn.size() > MAX_CLIENT_CONN)
			return;
	}
#endif
	CSynConnServer* pNew = new CSynConnServer(pPipe);
	m_listAllConn.push_front(pNew);
	pNew->m_itListConn=m_listAllConn.begin();
}

void CSynConnMgrServer::OnClientGuidSet(CSynConnServer* pConn)
{
	IPipe* pPipe = pConn->GetPipe();
	m_pMulticast->AddPipe(pPipe);
	(new CCoreOnNewConnAcceptedResult(pConn->GetGlobalID()))->Add();
	//CAddress addr;
	//pConn->GetRemoteAddress(addr);
	//m_pOffLineLog->Write(" new conn from ");
	//m_pOffLineLog->Write(addr.GetAddress());
	//m_pOffLineLog->Write(" \n");
}

void CSynConnMgrServer::OnListenFailed()
{
	ostringstream strm;
	strm<<m_szAddress.c_str()<<" : "<<m_uPort<< " 该端口已被占用 !";
	(new CCoreBeginServiceResult(false, strm.str().c_str()))->Add();
	//m_pOffLineLog->Write(" OnListenFailed ! Service will be shutdown !");
	//m_pOffLineLog->Write(" \n");
}

void CSynConnMgrServer::OnListenSuccessed()
{
	(new CCoreBeginServiceResult(true, "BeginService Successed !"))->Add();
}

void CSynConnMgrServer::OnConnected()
{
	GenErr("Shouldn't call this");
}

void CSynConnMgrServer::OnConnectFailed(EPipeConnFailedReason eReason)
{
	GenErr("Shouldn't call this");
}

void CSynConnMgrServer::OnDisconnected(EPipeDisconnectReason eReason,bool bGracefully)
{
	GenErr("Shouldn't call this");
}

void CSynConnMgrServer::OnDataReceived()
{
	GenErr("Shouldn't call this");
}

void CSynConnMgrServer::OnDataSent(uint32 uSentSize)
{
	GenErr("Shouldn't call this");
}

void CSynConnMgrServer::MulticastSendCmd(IMulticast*pMulticast, const void* pData, uint32 uSize)
{
	pMulticast->SendCmd(pData, uSize);
}

uint32 CSynConnMgrServer::Send(const void* pData,uint32 uSize)
{
	if( uSize <=0xff )
	{
		CGas2GacGC_Small_Shell_Message cmd;
		cmd.usbDataLength = static_cast<uint8>(uSize);
		MulticastSendCmd(m_pMulticast, reinterpret_cast<const void*>(&cmd), sizeof(cmd));
	}
	else if(uSize <= 0xffff )
	{
		CGas2GacGC_Middle_Shell_Message cmd;
		cmd.udbDataLength = static_cast<uint16>(uSize);
		MulticastSendCmd(m_pMulticast, reinterpret_cast<const void*>(&cmd), sizeof(cmd));
	}
	else
	{
		CGas2GacGC_Big_Shell_Message cmd;
		cmd.uqbDataLength = uSize;
		MulticastSendCmd(m_pMulticast, reinterpret_cast<const void*>(&cmd), sizeof(cmd));
	}
	m_pMulticast->SendCmd(pData, uSize);
	return uSize;
}

ISend* CSynConnMgrServer::GetIS()const
{
	return const_cast<CSynConnMgrServer*>(this);
}

uint32 CSynConnMgrServer::GetConnNum() const
{
	return (uint32)m_listAllConn.size();
}

void CSynConnMgrServer::SetOffLineLog(const char* msg)
{
	m_pOffLineLog->Write(msg);
}


void CSynConnMgrServer::SendErrMsgToConn(const char* szError, CLogOwner* pOwner)
{
	uint32 uLen = strlen(szError);

	SendErrMsgToConn(szError, uLen, pOwner);
}

void CSynConnMgrServer::SendErrMsgToConn(const char* szError, uint32 uLen, CLogOwner* pOwner)
{
	if(pOwner == NULL)
	{
		CSynConnMgrServer* pSynConnMgr = CSyncAppServer::Inst()->GetConnMgr();
		for(TestingConn_t::iterator it=pSynConnMgr->m_lstTestingConn.begin();
			it!=pSynConnMgr->m_lstTestingConn.end();++it)
		{
			(*it)->SendErrMsg(szError, uLen);
		}	
	}
	else
	{
		CSynConnServer* pConnServer = dynamic_cast<CSynConnServer*>(pOwner);
		if(!pConnServer)
			return;

		pConnServer->SendErrMsg(szError, uLen);
	}		
}

void CSynConnMgrServer::OnTick()
{
	CGas2GacGC_Tell_Server_Time cmd;
	cmd.uobGlobalTime= CSyncTimeSystemServer::Inst()->GetFrameTime();
	MulticastSendCmd(m_pMulticast, reinterpret_cast<const void*>(&cmd), sizeof(cmd));
}

void CSynConnMgrServer::CreateClientGuidLog()
{
	if (m_pClientGuidLog)
	{
		return;
	}

	char szBuffer[100];
	const char* szProcessGuid = GetProcessStringGUID();
	sprintf(szBuffer, "ClientGuid_%sv%u.log", szProcessGuid, m_uGuidLogFileNum++);
	m_pClientGuidLog = CLog::CreateLog(szBuffer);
	uint32 uVersion = GetCurrentRevision();
	sprintf(szBuffer, "%u", uVersion);
	m_pClientGuidLog->Write(szBuffer);
}

//记录客户端guid信息
void CSynConnMgrServer::LogClientGuidInfo(const char* szMsg)
{
	if (!m_pClientGuidLog)
	{
		CreateClientGuidLog();
	}

	m_pClientGuidLog->Write(szMsg);
}

void CSynConnMgrServer::CheckClientGuidLog()
{
	//如果没有log，那么我们不用处理
	if (!m_pClientGuidLog)
	{
		return;
	}

	//否则，删除log，等到下一次有数据时候在创建
	m_pClientGuidLog->Release();
	m_pClientGuidLog = NULL;
}

