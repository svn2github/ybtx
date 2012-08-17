#include "stdafx.h"
#include "CCoreConnServerResult.h"
#include "CCoreServerResult.inl"
#include "CConnServer.h"
#include "ExpHelper.h"
#include "CConnMgrServer.h"
#include "IConnMgrServerHandler.h"
#include "ErrLogHelper.h"

CCoreDispatchShellMsgResult::CCoreDispatchShellMsgResult(uint32 uConnId, const void* pData, size_t stSize)
: m_uConnId(uConnId)
{
	m_pData = CloneData(pData, stSize);
	m_uSize = stSize;

}


CCoreDispatchShellMsgResult::~CCoreDispatchShellMsgResult()
{
	CConnServer* pConn = CConnServer::GetConnServer(m_uConnId);
	if (pConn)
	{
		pConn->CacheRecvData( (char*)m_pData , m_uSize);
		CConnMgrServer::Inst()->GetHandler()->OnDataReceived(pConn);
		pConn->StoreRecvData();
	}
}

CCoreCreateConnServerResult::CCoreCreateConnServerResult(uint32 uConnID, CAddress& address)
:m_uConnID(uConnID)
{
	m_sAddress = CloneString(address.GetAddress());
	m_uPort = address.GetPort();	
}

CCoreCreateConnServerResult::~CCoreCreateConnServerResult()
{
	new CConnServer(m_uConnID, m_sAddress, m_uPort);
	
}

CCoreDestoryConnServerResult::CCoreDestoryConnServerResult(uint32 uConnID)
:m_uConnID(uConnID)
{
}


CCoreDestoryConnServerResult::~CCoreDestoryConnServerResult()
{
	CConnServer* pConnServer = CConnServer::GetConnServer(m_uConnID);
	if (pConnServer)
	{
		delete pConnServer;	
		pConnServer = NULL;		
	}
}

CCoreOnDisconnectResult::CCoreOnDisconnectResult(uint32 uConnID)
:m_uConnId(uConnID)
{
}


CCoreOnDisconnectResult::~CCoreOnDisconnectResult()
{
	CConnServer* pConnServer = CConnServer::GetConnServer(m_uConnId);
	CConnMgrServer* pConnMgr=CConnMgrServer::Inst();
	if (pConnServer)
	{
		pConnMgr->GetHandler()->OnDisconnect(pConnServer);	
	}

	Ast( pConnMgr->m_uConnNum>0 );
	--pConnMgr->m_uConnNum;
}

CCoreOnNewConnAcceptedResult::CCoreOnNewConnAcceptedResult(uint32 uConnID)
:m_uConnID(uConnID)
{
}


CCoreOnNewConnAcceptedResult::~CCoreOnNewConnAcceptedResult()
{
	CConnServer* pConn = CConnServer::GetConnServer(m_uConnID);
	CConnMgrServer* pConnMgr = CConnMgrServer::Inst();
	if (pConnMgr && pConn)
	{
		pConnMgr->GetHandler()->OnAccepted(pConn);
	}
	++pConnMgr->m_uConnNum;
}

CCoreBeginServiceResult::CCoreBeginServiceResult(bool bSuccess , const char* szErrMsg)
:m_bSuccess(bSuccess)
,m_sError(CloneString(szErrMsg))
{
}

CCoreBeginServiceResult::~CCoreBeginServiceResult()
{
	CConnMgrServer* pConnMgr = CConnMgrServer::Inst();
	if (pConnMgr && pConnMgr->GetHandler())
	{
		if (m_bSuccess)
		{	
			pConnMgr->OnBeginServiceSucceeded();
		}
		else
			pConnMgr->OnBeginServiceFailed(m_sError);
	}
}

CCoreEndServiceResult::CCoreEndServiceResult()
{
}

CCoreEndServiceResult::~CCoreEndServiceResult()
{
	CConnMgrServer* pConnMgr = CConnMgrServer::Inst();
	if (pConnMgr)
		pConnMgr->OnServiceEnded();
}

CCoreCheatResult::CCoreCheatResult(uint32 uConnID)
:m_uConnID(uConnID)
{
}

CCoreCheatResult::~CCoreCheatResult()
{
	CConnServer* pConn = CConnServer::GetConnServer(m_uConnID);
	if(!pConn)
		return;

	CConnMgrServer* pConnMgr = CConnMgrServer::Inst();
	IConnMgrServerHandler* pHandler = pConnMgr->GetHandler();
	if(pHandler)
		pHandler->OnCheat(pConn);
}

CCoreConnSetValueResult::CCoreConnSetValueResult(uint32 uConnID, const char *szKey, const char *szValue)
: m_uConnID(uConnID)
, m_szKey(CloneString(szKey))
, m_szValue(CloneString(szValue))
{
}

CCoreConnSetValueResult::~CCoreConnSetValueResult()
{
	CConnServer* pConn = CConnServer::GetConnServer(m_uConnID);
	if(pConn)
	{
		pConn->SetValue(m_szKey, m_szValue);
	}
}

CCoreConnSetLatency::CCoreConnSetLatency(uint32 uConnID, uint16 uLatency)
: m_uConnID(uConnID),m_uLantency(uLatency)
{
}

CCoreConnSetLatency::~CCoreConnSetLatency()
{
	CConnServer* pConn = CConnServer::GetConnServer(m_uConnID);
	if(pConn)
	{
		pConn->SetLatency(m_uLantency);
	}
}


CCoreSetTestingConnResult::CCoreSetTestingConnResult(uint32 uConnID, bool bTestingConn)
: m_uConnID(uConnID)
, m_bTestingConn(bTestingConn)
{

}

CCoreSetTestingConnResult::~CCoreSetTestingConnResult()
{
	CConnServer* pConn = CConnServer::GetConnServer(m_uConnID);
	if(pConn)
	{
		pConn->m_bTestingConn = m_bTestingConn;
	}
}

