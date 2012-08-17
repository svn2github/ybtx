#include "stdafx.h"
#include "CCoreConnServerJob.h"
#include "CCoreServerJob.inl"
#include "CSynConnServer.h"
#include "CSynConnMgrServer.h"
#include "CSyncAppServer.h"
#include "CSynMultiMsgSender.h"

CCoreBeginServiceJob::CCoreBeginServiceJob(const CAddress& addr,uint32 uSendBufferSize,uint32 uRecvBufferSize)
:m_sAddress(CloneString(addr.GetAddress()))
,m_uPort(addr.GetPort())
,m_uSendBufferSize(uSendBufferSize)
,m_uRecvBufferSize(uRecvBufferSize)
{
}


CCoreBeginServiceJob::~CCoreBeginServiceJob()
{
	CAddress addr;
	addr.SetAddress(m_sAddress);
	addr.SetPort(m_uPort);
	CSyncAppServer::Inst()->GetConnMgr()->BeginService(addr, m_uSendBufferSize, m_uRecvBufferSize);
}


CCoreEndServiceJob::CCoreEndServiceJob()
{
}

CCoreEndServiceJob::~CCoreEndServiceJob()
{
	CSyncAppServer::Inst()->GetConnMgr()->EndService();
}

CCoreSendMsgJob::CCoreSendMsgJob(const void* pData, uint32 uSize)
:m_uSize(uSize)
,m_pData(CloneData(pData, uSize))
{
}


CCoreSendMsgJob::~CCoreSendMsgJob()
{
	CSyncAppServer::Inst()->GetConnMgr()->Send(m_pData, m_uSize);
}

CCoreSendErrMsgToAllConnJob::CCoreSendErrMsgToAllConnJob(const char* szError)
{
	m_uLen = (uint32)strlen(szError);
	m_sError = (char*)CloneData(szError, m_uLen + 1);
	m_sError[m_uLen] = '\0';
}

CCoreSendErrMsgToAllConnJob::~CCoreSendErrMsgToAllConnJob()
{
	CSyncAppServer::Inst()->GetConnMgr()->SendErrMsgToConn(m_sError, m_uLen, NULL);
}

CCoreSendErrMsgToConnJob::CCoreSendErrMsgToConnJob(uint32 uId, const char* szError)
:m_uId(uId)
{
	m_uLen = (uint32)strlen(szError);
	m_sError = (char*)CloneData(szError, m_uLen + 1);
	m_sError[m_uLen] = '\0';
}

CCoreSendErrMsgToConnJob::~CCoreSendErrMsgToConnJob()
{
	CSynConnServer* pSynConn = CSynConnServer::GetSynConn(m_uId);
	if(pSynConn)
	{
		CSyncAppServer::Inst()->GetConnMgr()->SendErrMsgToConn(m_sError, m_uLen, pSynConn);	
	}
}


CCoreConnServerSendJob::CCoreConnServerSendJob(uint32 uConnID, const void* pData, uint32 uSize)
:m_uConnID(uConnID)
,m_uSize(uSize)
,m_pData(CloneData(pData, uSize))
{
}


CCoreConnServerSendJob::~CCoreConnServerSendJob()
{
	CSynConnServer* pSynConn = CSynConnServer::GetSynConn(m_uConnID);
	if (pSynConn)
	{
		pSynConn->SendShellMessage(m_pData, m_uSize);
	}
}

CCoreShutDownJob::CCoreShutDownJob(uint32 uConnID, const char* szMsg)
:m_uConnID(uConnID)
,m_sMsg(CloneString(szMsg))
{
}


CCoreShutDownJob::~CCoreShutDownJob()
{
	CSynConnServer* pSynConn = CSynConnServer::GetSynConn(m_uConnID);
	if (pSynConn)
	{
		pSynConn->ShutDown(m_sMsg);
	}
}

CCoreLogOnOffLineMsgJob::CCoreLogOnOffLineMsgJob(uint32 uConnId, const char* msg)
:m_uConnID(uConnId)
,m_sMsg(CloneString(msg))
{
}

CCoreLogOnOffLineMsgJob::~CCoreLogOnOffLineMsgJob()
{
	CSynConnServer* pSynConn = CSynConnServer::GetSynConn(m_uConnID);
	if (pSynConn)
	{
		pSynConn->LogOnOffLineMsg(m_sMsg);
	}
}


CCoreMultiMsgSenderDoSendJob::CCoreMultiMsgSenderDoSendJob(uint32 uMultiMsgSenderID, const void* pData, uint32 uSize)
:m_uMultiMsgSenderID(uMultiMsgSenderID)
,m_uSize(uSize)
,m_pData(CloneData(pData, uSize))
{
}

CCoreMultiMsgSenderDoSendJob::~CCoreMultiMsgSenderDoSendJob()
{
	CSynMultiMsgSender* pMsgSender = CSynMultiMsgSender::GetSynMultiMsgSender(m_uMultiMsgSenderID);
	if (pMsgSender)
	{
		pMsgSender->Send(m_pData, m_uSize);
	}
}

CCoreMultiMsgSenderAddPipeJob::CCoreMultiMsgSenderAddPipeJob(uint32 uMultiMsgSenderID, uint32 uConnID)
:m_uMultiMsgSenderID(uMultiMsgSenderID)
,m_uConnID(uConnID)
{
}

CCoreMultiMsgSenderAddPipeJob::~CCoreMultiMsgSenderAddPipeJob()
{
	CSynMultiMsgSender* pMsgSender = CSynMultiMsgSender::GetSynMultiMsgSender(m_uMultiMsgSenderID);
	CSynConnServer* pSynConn = CSynConnServer::GetSynConn(m_uConnID);
	if (pMsgSender && pSynConn)
	{
		pMsgSender->AddPipe(pSynConn);
	}
}

CCoreMultiMsgSenderDelPipeJob::CCoreMultiMsgSenderDelPipeJob(uint32 uMultiMsgSenderID, uint32 uConnID)
:m_uMultiMsgSenderID(uMultiMsgSenderID)
,m_uConnID(uConnID)
{
}

CCoreMultiMsgSenderDelPipeJob::~CCoreMultiMsgSenderDelPipeJob()
{
	CSynMultiMsgSender* pMsgSender = CSynMultiMsgSender::GetSynMultiMsgSender(m_uMultiMsgSenderID);
	CSynConnServer* pSynConn = CSynConnServer::GetSynConn(m_uConnID);
	if (pMsgSender && pSynConn)
	{
		pMsgSender->DelPipe(pSynConn);
	}	
}

CCoreCreateSynMultiMsgSenderJob::CCoreCreateSynMultiMsgSenderJob(uint32 uMultiMsgSenderID)
:m_uMultiMsgSenderID(uMultiMsgSenderID)
{
}

CCoreCreateSynMultiMsgSenderJob::~CCoreCreateSynMultiMsgSenderJob()
{
	CSyncAppServer::Inst()->GetMultiMsgSender(m_uMultiMsgSenderID);
}

CCoreMultiMsgSenderDestoryJob::CCoreMultiMsgSenderDestoryJob(uint32 uMultiMsgSenderID)
:m_uMultiMsgSenderID(uMultiMsgSenderID)
{
}

CCoreMultiMsgSenderDestoryJob::~CCoreMultiMsgSenderDestoryJob()
{
	CSynMultiMsgSender* pSynMsgSender = CSynMultiMsgSender::GetSynMultiMsgSender(m_uMultiMsgSenderID);
	if (pSynMsgSender)
	{
		pSynMsgSender->Release();
	}
}

CCoreConnSetValueJob::CCoreConnSetValueJob(uint32 uConnID, const char *szKey, const char *szValue)
: m_uConnID(uConnID)
, m_szKey(CloneString(szKey))
, m_szValue(CloneString(szValue))
{
}

CCoreConnSetValueJob::~CCoreConnSetValueJob()
{
	CSynConnServer* pSynConn = CSynConnServer::GetSynConn(m_uConnID);
	if(pSynConn)
	{
		pSynConn->SetValue(m_szKey, m_szValue);
	}
}


