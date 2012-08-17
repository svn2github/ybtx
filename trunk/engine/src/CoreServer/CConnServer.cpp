#include "stdafx.h"
#include "CConnServer.h"
#include "IConnMgrServerHandler.h"
#include "CConnMgrServer.h"
#include "TIDPtrMap.inl"
#include "CSynConnServer.h"
#include "ExpHelper.h"
#include "CCoreObjectMediator.h"
#include "CodeCvs.h"
#include "ErrLogHelper.h"
#include "TSqrAllocator.inl"
#include "CCoreConnServerJob.h"
#include "CCoreServerJob.inl"

char*	CConnServer::ms_pCurRecvBuffer=NULL;			//保存最近的一次网络数据的buffer起始地址，用于减少网络数据复制。
size_t	CConnServer::ms_uCurRecvBufferSize=0;

CConnServer::IDVecConnServer& CConnServer::GetIDVecConnServer()
{
	static IDVecConnServer ms_vecConn;
	return ms_vecConn;
}

CConnServer* CConnServer::GetConnServer(uint32 uID)
{
	const uint32 uIndex = TIDPtrMap<CConnServer*>::GetIndexByID(uID);

	IDVecConnServer& vecConnServer = GetIDVecConnServer();

	if( vecConnServer.size() <= uIndex )
		return NULL;

	return vecConnServer[ uIndex ];
}

CConnServer::CConnServer(uint32 uID, char* pAddress, uint32 uPort)
{
	m_uID				=	uID;
	m_bShutDownCalled	=	false;
	m_uLatency			=	0;
	m_bTestingConn		=	false;
	m_pMedObj			=	NULL;

	m_Address.SetAddress(pAddress);
	m_Address.SetPort(uPort);

	const uint32 uIndex = TIDPtrMap<CConnServer*>::GetIndexByID(uID);

	IDVecConnServer& vecConn = GetIDVecConnServer();

	if( vecConn.size() <= uIndex )
		vecConn.resize( uIndex+1 );

	vecConn[uIndex] = this;
}

CConnServer::~CConnServer(void)
{
	if (m_pMedObj)
		m_pMedObj->m_pConn = NULL;

	const uint32 uIndex = TIDPtrMap<CConnServer*>::GetIndexByID(m_uID);

	IDVecConnServer& vecConn = GetIDVecConnServer();

	vecConn[uIndex] = NULL;
}

uint32 CConnServer::Send(const void* pData,uint32 uSize)
{
	if (uSize == 0)
		return 0;

	(new CCoreConnServerSendJob(m_uID, pData, uSize))->Add();

	return uSize;
}


void CConnServer::ShutDown(const char* szMsg)
{
	m_bShutDownCalled = true;

	(new CCoreShutDownJob(m_uID, szMsg))->Add();
}

void CConnServer::LogOnOffLineMsg(const char* msg)
{
	(new CCoreLogOnOffLineMsgJob(m_uID, msg))->Add();
}

CCoreObjectMediator* CConnServer::GetObjectMediator()
{
	return m_pMedObj;
}

void CConnServer::SetMediator(CCoreObjectMediator* pMediator)
{
	m_pMedObj = pMediator;
}

void* CConnServer::GetRecvData()const
{
	if( ms_pCurRecvBuffer )
		return ms_pCurRecvBuffer;

	return reinterpret_cast<void*>( const_cast<char*> ( ( m_strRecvBuffer.data() ) ) );
}

size_t CConnServer::GetRecvDataSize()const
{
	if( ms_pCurRecvBuffer )
		return ms_uCurRecvBufferSize;

	return m_strRecvBuffer.size();
}

void CConnServer::PopRecvData(size_t stSize)
{
	if( ms_pCurRecvBuffer )
	{
		if( stSize < ms_uCurRecvBufferSize )
			m_strRecvBuffer.assign( ms_pCurRecvBuffer + stSize, ms_uCurRecvBufferSize - stSize );
		ms_pCurRecvBuffer = NULL;
	}
	else
	{
		m_strRecvBuffer.erase(0,stSize);
	}
}

void CConnServer::CacheRecvData(char* pData,size_t stSize)
{
	if( m_strRecvBuffer.empty() )
	{
		ms_pCurRecvBuffer = pData;
		ms_uCurRecvBufferSize = stSize;	
	}
	else
	{
		m_strRecvBuffer.append( pData , stSize );
	}
}

void CConnServer::StoreRecvData()
{
	if( !ms_pCurRecvBuffer )
		return;

	Ast( m_strRecvBuffer.empty() );
	m_strRecvBuffer.append( ms_pCurRecvBuffer , ms_uCurRecvBufferSize );
	ms_pCurRecvBuffer = NULL;
}

void CConnServer::SetLatency(uint32 uLatency)
{
	m_uLatency = uLatency;
}

uint32 CConnServer::GetLatency()const
{
	return m_uLatency;
}

bool CConnServer::IsShuttingDown()const
{
	return m_bShutDownCalled;
}

bool CConnServer::GetRemoteAddress(CAddress& Address)const
{
	Address = m_Address;
	return true;
}

void CConnServer::SendErrMsg(const char* szError)
{
	if (!m_bTestingConn)
		return;

	uint32 uID = GetGlobalID();
	(new CCoreSendErrMsgToConnJob(uID, szError))->Add();
}

void CConnServer::SetConnValue(const char* szKey, const char* szValue)
{
	CLogOwner::SetValue(szKey, szValue);

	uint32 uID = GetGlobalID();
	(new CCoreConnSetValueJob(uID, szKey, szValue))->Add();
}
