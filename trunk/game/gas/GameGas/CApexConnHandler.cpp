#include "stdafx.h"
#include "CApexConnHandler.h"
#include "IPipeReactor.h"
#include "CAppServer.h"
#include "CApexProxy.h"
#include "CAddress.h"


CApexConnHandler::CApexConnHandler()
:m_Pipe(NULL)
{
}

CApexConnHandler::~CApexConnHandler()
{
}

void CApexConnHandler::StartApexProxy(const CAddress& addr)
{
	m_pAddress = addr;
	m_Pipe = CAppServer::Inst()->GetIPipe(this,96*1024,100*1024,false);
	m_Pipe->Connect(m_pAddress);
}

void CApexConnHandler::StopApexProxy(void)
{
	if( NULL == m_Pipe )
		return;

	m_Pipe->Release();
	m_Pipe=NULL;
}


void CApexConnHandler::OnConnected()
{
	CAppServer::Inst()->UnRegisterTick(this);
}

void CApexConnHandler::OnConnectFailed(EPipeConnFailedReason eReason)
{
	CAppServer::Inst()->UnRegisterTick(this);
	CAppServer::Inst()->RegisterTick(this, 5*60*1000);
}

void CApexConnHandler::OnDisconnected(EPipeDisconnectReason eReason,bool bGracefully)
{
	CAppServer::Inst()->UnRegisterTick(this);
	CAppServer::Inst()->RegisterTick(this, 5*60*1000);
}

void CApexConnHandler::OnDataReceived()
{
	uint32 uDealDataLen = (const_cast<CApexProxy*>(CApexProxy::GetInst()))->RecvConnData(m_Pipe->GetRecvData(), m_Pipe->GetRecvDataSize());
	m_Pipe->PopRecvData(uDealDataLen);
	if (m_Pipe->GetRecvDataSize() != 0 && uDealDataLen != 0)
	{
		OnDataReceived();
	}
}

void CApexConnHandler::OnDataSent(uint32 uSentSize)
{
}

void CApexConnHandler::Send(const void* pData,uint32 uSize)
{
	m_Pipe->Send(pData,uSize);
}

void CApexConnHandler::OnTick()
{
	m_Pipe->Connect(m_pAddress);
}
