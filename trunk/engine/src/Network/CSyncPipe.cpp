#include "stdafx.h"
#include "CSyncPipe.h"
#include "CPipeState.h"
#include "CPipeState_Connected.h"
#include "CSyncPipeReactor.h"
#include "ExpHelper.h"
#include "BaseHelper.h"
#include "CAsynPipeHandler.h"
#include "CMulticast.h"
#include "LogHelper.h"
#include "BaseHelper.h"
#include "CPipeState_Disconnected.h"
#include "CSyncPipe.inl"
#include "TSqrAllocator.inl"


CSyncPipe::CSyncPipe(IPipeHandler* pHandler,size_t stOutBufferSize,size_t stInBufferSize, uint8 uFlag)
:m_InBuffer(stInBufferSize,(uFlag & ePCF_RecvWayDecompression) !=0,(uFlag & ePCF_RecvWayDecryption) !=0 )
,m_OutBuffer(stOutBufferSize,(uFlag & ePCF_SendWayCompression) != 0,(uFlag & ePCF_SendWayEncryption) !=0 )
{
	m_pState		=NULL;
	m_pHandler		=NULL;
	m_pReactor		=NULL;

	m_uCreationFlag=uFlag;
	
	IntSetHandler(pHandler);
	new CPipeState_Disconnected(this);
}


CSyncPipe::CSyncPipe(IPipeHandler* pHandler,size_t stOutBufferSize,size_t stInBufferSize,
					 const SOCKET& Socket, uint8 uFlag,bool& bDisconnected)
:m_InBuffer(stInBufferSize,(uFlag & ePCF_RecvWayDecompression) !=0,(uFlag & ePCF_RecvWayDecryption) !=0 )
,m_OutBuffer(stOutBufferSize,(uFlag & ePCF_SendWayCompression) != 0,(uFlag & ePCF_SendWayEncryption) !=0 )
{
	m_pState		=NULL;
	m_pHandler		=NULL;
	m_pReactor		=NULL;
	
	m_uCreationFlag=uFlag;
	
	IntSetHandler(pHandler);

	new CPipeState_Connected(this,Socket,bDisconnected);
}


CSyncPipe::~CSyncPipe(void)
{
	if( m_pReactor )
	{
#ifdef _WIN32
		if(m_pReactor->m_pCallingBackPipe==this)
			m_pReactor->m_pCallingBackPipe=NULL;
#endif
		m_pReactor->Unregister(this);
	}

	m_pState->ShutDown(false);	
	delete m_pState;

	IntSetHandler(NULL);
}

void CSyncPipe::SetHandler(IPipeHandler* pHandler)
{
	if( !pHandler )
	{
		Release();
		return;
	}
	IntSetHandler( pHandler );
}

void CSyncPipe::IntSetHandler(IPipeHandler* pHandler)
{
	AdvDelRef(m_pHandler);

	m_pHandler=pHandler;

	AdvAddRef(m_pHandler);
}


IPipeHandler* CSyncPipe::GetHandler()const
{
	return m_pHandler;
}

IPipeReactor* CSyncPipe::GetReactor()const
{
	return m_pReactor;
}

bool CSyncPipe::IsConnected()const
{
	return dynamic_cast<CPipeState_Connected*>(m_pState) != NULL;
}

bool CSyncPipe::IsConnecting()const
{
	return m_pState->IsConnecting();
}

bool CSyncPipe::Listen(const CAddress& Address)
{
	return m_pState->Listen(Address);
}


bool CSyncPipe::Connect(const CAddress& Address)
{
	return m_pState->Connect(Address);
}

void CSyncPipe::ShutDown()
{
	ShutDown(false);
}

void CSyncPipe::ShutDown(bool bNeedCallback)
{
	m_pState->ShutDown(bNeedCallback);
	m_OutBuffer.OutBufferClear();
	m_InBuffer.InBufferClear();	
}

size_t CSyncPipe::Send(const void* pData,size_t stSize)
{
	return m_pState->Send( pData,stSize );
}

size_t CSyncPipe::GetSendBufferSize()const
{
	return m_OutBuffer.OutBufferGetSize();
}

void* CSyncPipe::GetRecvData()const
{
	return m_InBuffer.InBufferGetData();
}

size_t CSyncPipe::GetRecvDataSize()const
{
	return m_InBuffer.InBufferGetDataSize();
}

void CSyncPipe::PopRecvData(size_t stSize)
{
	const bool bWasFull= (m_InBuffer.InBufferGetLeftSpaceSize()==0);
	
	if( m_InBuffer.InBufferPopData(stSize) && IsConnected() )
	{
		//PopData有可能会触发数据解压，那么就需要产生新的回调来告知上层
		//如果连接已经断开就不要触发回调了，否则上层使用会觉得为什么链接已经断开了还能收到数据。
		AddEventNewDataReceived();
	}

	if( !bWasFull || stSize == 0 )
		return;
	
	CPipeState_Connected*const pStateConnected=dynamic_cast<CPipeState_Connected*>( m_pState );
	if( !pStateConnected )
		return;

#ifdef _WIN32
	pStateConnected->OverlappedRecv();
#else
	pStateConnected->EpollEventRegister();
#endif
}


void CSyncPipe::ClearRecvBuffer()
{
	PopRecvData(m_InBuffer.InBufferGetDataSize());
}


bool CSyncPipe::RecvBufferFull()const
{
	return m_InBuffer.InBufferIsFull();
}


size_t CSyncPipe::GetRecvBufferSize()const
{
	return m_InBuffer.InBufferGetSize();
}



bool CSyncPipe::GetLocalAddress(CAddress& Address)const
{
	if( !m_pState->IsBusy() )
		return false;

	CPipeState_Busy* pState = class_cast< CPipeState_Busy* >( m_pState );

	sockaddr_in Addr;
	socklen_t nSize=sizeof(Addr);
	memset(&Addr,0,sizeof(Addr));

	int nErrorCode;

	if(getsockname(pState->m_Socket,reinterpret_cast<sockaddr*>(&Addr),&nSize))
	{
		nErrorCode=SocketGetLastError();
		return false;
	}

	Address.SetPort(ntohs(Addr.sin_port));
	Address.SetAddress(inet_ntoa(Addr.sin_addr));
	return true;
}


bool CSyncPipe::GetRemoteAddress(CAddress& Address)const
{
	if( !m_pState->IsBusy() )
		return false;

	CPipeState_Busy* pState = class_cast< CPipeState_Busy* >( m_pState );

	sockaddr_in Addr;
	socklen_t nSize=sizeof(Addr);
	memset(&Addr,0,sizeof(Addr));

	int nErrorCode;

	if(getpeername(pState->m_Socket,reinterpret_cast<sockaddr*>(&Addr),&nSize))
	{
		nErrorCode=SocketGetLastError();
		return false;
	}

	Address.SetPort(ntohs(Addr.sin_port));
	Address.SetAddress(inet_ntoa(Addr.sin_addr));
	return true;
}


void CSyncPipe::Release()
{
	delete this;
}


bool CSyncPipe::AddToMulticast(IMulticast* pMulticast)
{
	pair<SetWithinMulticast_t::iterator, bool> p = m_setWithinMulticast.insert(class_cast<CMulticast*>(pMulticast));
	return p.second;
}

bool CSyncPipe::DelFrMulticast(IMulticast* pMulticast)
{
	SetWithinMulticast_t::iterator it = m_setWithinMulticast.find(class_cast<CMulticast*>(pMulticast));
	if (it == m_setWithinMulticast.end())
		return false;

	m_setWithinMulticast.erase(it);
	return true;
}


void CSyncPipe::DelFromAllMulticast()
{
	for (	SetWithinMulticast_t::iterator it = m_setWithinMulticast.begin();
			it != m_setWithinMulticast.end(); )
	{
		(*(it++))->DelPipe(this);
	}
	Ast( m_setWithinMulticast.empty() );
}

float CSyncPipe::GetDataFlowSpeed()
{
	return m_pState->GetDataFlowSpeed();
}


size_t CSyncPipe::GetSendBufferFreeSize()const
{
	return m_OutBuffer.OutBufferGetFreeSize();
}

void CSyncPipe::PrintAndClearHistory( ostream& os )
{
	while( !m_quePipeStateHistory.empty() )
	{
		os<< m_quePipeStateHistory.front() <<endl;
		m_quePipeStateHistory.pop();
	}
}

void CSyncPipe::WriteHistory( const string& sHistory )
{
	m_quePipeStateHistory.push( sHistory );
}
