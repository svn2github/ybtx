#include "stdafx.h"
#include "CPipeState_Connecting.h"
#include "CPipeState_Disconnected.h"
#include "CPipeState_Connected.h"
#include "CSyncPipe.h"
#include "ErrLogHelper.h"
#include "ExpHelper.h"
#include "CSyncPipe.inl"

//******************************************************************
// CPipeState_Connecting
//******************************************************************

CPipeState_Connecting::CPipeState_Connecting(CSyncPipe* pPipe,const CAddress& Address,uint32& uResult)
:CPipeState_Busy(pPipe)
{
	pPipe->m_OutBuffer.OutBufferClear();
	pPipe->m_InBuffer.InBufferClear();

	CPipeState* pState=GetPipe()->m_pState;
	GetPipe()->m_pState=NULL;
	delete pState;
	GetPipe()->m_pState=this;

	const SOCKET& Socket=m_Socket;

#ifdef _WIN32
	SQR_TRY
	{
		m_Event.Create(Socket);
	}
	SQR_CATCH (exp)
	{
		LogExp(exp);
		closesocket(m_Socket);
		throw;
	}
	SQR_TRY_END;
#endif

	Register();
	
	sockaddr_in saiAddress;
	::memset(&saiAddress,0,sizeof(saiAddress));

	saiAddress.sin_addr.s_addr = inet_addr(Address.GetAddress());
	saiAddress.sin_port = htons(static_cast<u_short>(Address.GetPort()));
	saiAddress.sin_family = AF_INET;


	//连接
	int nResult;

	nResult=connect(Socket,reinterpret_cast<sockaddr*>(&saiAddress),sizeof(sockaddr));

	if(SOCKET_ERROR!=nResult)
	{
		uResult=0;
	}
	else
	{
		int nError=SocketGetLastError();
		switch(nError)
		{
#if defined(__linux__)
		case EINPROGRESS:
#elif defined(_WIN32)
		case EWOULDBLOCK:
#endif
			uResult=1;
			break;
#ifdef _WIN32
		case ECONNABORTED://Windows下还真会出现这种情况
		case ENOBUFS:
			uResult=2;
			GetPipe()->AddEventConnectFailed(ePCFR_OSERROR);
			return;
#endif
		case ECONNRESET:
		case ECONNREFUSED:
			uResult=2;
			GetPipe()->AddEventConnectFailed(ePCFR_REFUSED);
			return;
		case ETIMEDOUT:
		case ENETRESET:
		case EHOSTUNREACH:
		case ENETUNREACH:
			uResult=2;
			GetPipe()->AddEventConnectFailed(ePCFR_UNREACHABLE);
			return;
		default:
			closesocket(m_Socket);
			stringstream strm;
			strm<<"connect failed with error code "<<nError<<".";
			GenErr(strm.str());
		}
	}

	m_RemoteAddress=Address;
}


bool CPipeState_Connecting::SwitchToConnected()
{
	CSyncPipe* pPipe=GetPipe();
	
	Unregister();

	SOCKET Socket=m_Socket;
	m_Socket=0;//赋值后再CPipeState_Busy析构的时候，就不会close这个socket
	
	bool bDisconnected;
	new CPipeState_Connected( pPipe,Socket,bDisconnected);
	pPipe->AddEventConnected();	

	if( bDisconnected )
	{
		new CPipeState_Disconnected( pPipe );
		pPipe->AddEventDisconnected( ePDR_ERROR,false );	
		return false;
	}
	
	return true;
}


CPipeState_Connecting::~CPipeState_Connecting()
{
	if(m_Socket)
		Unregister();
#ifdef _WIN32
	m_Event.Destroy();
#endif
}


CPipeState* CPipeState_Connecting::GetPipeState()const
{
	return const_cast<CPipeState_Connecting*>(this);
}

void CPipeState_Connecting::Register()
{
	if(!GetPipe()->m_pReactor)
		return;

#ifndef _WIN32
	EpollCtl(EPOLL_CTL_ADD,EPOLLOUT|EPOLLERR);
#else
	EventAdd(FD_CONNECT|FD_CLOSE);
#endif
}


void CPipeState_Connecting::Unregister()
{
	if( !GetPipe()->m_pReactor )
		return;

#ifndef _WIN32
	EpollCtl(EPOLL_CTL_DEL,0);
#else
	EventDel();
#endif
}


void CPipeState_Connecting::ProcessEvent(bool bRead,bool bWrite,bool bError)
{
	EPipeConnFailedReason eReason = ePCFR_UNREACHABLE;

	if(bError)
	{
		int nErrorCode;
		socklen_t nLength=sizeof(nErrorCode);
		int nResult;

		nResult=getsockopt(m_Socket,SOL_SOCKET,SO_ERROR,reinterpret_cast<char*>(&nErrorCode),&nLength);
		if(nResult)
		{
			stringstream strm;
			strm<<"getsockopt failed with error code "<<nResult<<".";
			GenErr(strm.str());
		}

		switch(nErrorCode)
		{
		case ECONNRESET:
		case ECONNREFUSED:
			eReason=ePCFR_REFUSED;
			break;
		case EADDRNOTAVAIL:
			eReason=ePCFR_INVALIDADDR;
			break;
		case ECONNABORTED:
			eReason=ePCFR_OSERROR;
			break;
		case ETIMEDOUT:
		case ENOTCONN:
		case EHOSTUNREACH:
		case ENETUNREACH:
			eReason=ePCFR_UNREACHABLE;
			break;
		default:
			{
				ostringstream strm;
				strm<<"connect failed with error code:"<<nErrorCode;
				GenErr(strm.str());
			}
			break;
		}
		CSyncPipe* pPipe=GetPipe();
		new CPipeState_Disconnected(pPipe);
		pPipe->AddEventConnectFailed(eReason);
		return;
	}

	if(bWrite)
	{

#ifndef _WIN32
		int nErrorCode;
		socklen_t nLength=sizeof(nErrorCode);
		int nResult;

		nResult=getsockopt(m_Socket,SOL_SOCKET,SO_ERROR,reinterpret_cast<char*>(&nErrorCode),&nLength);
		if(nResult)
		{
			stringstream strm;
			strm<<"getsockopt failed with error code "<<nResult<<".";
			GenErr(strm.str());
		}

		switch(nErrorCode)
		{
		case 0:
			{
#endif
				SwitchToConnected();
				return;
#ifndef _WIN32
			}
		case ECONNREFUSED:
			eReason=ePCFR_REFUSED;
			break;
		case ETIMEDOUT:
			eReason=ePCFR_TIMEDOUT;
			break;
		case EADDRNOTAVAIL:
			eReason=ePCFR_INVALIDADDR;
			break;
		case EHOSTUNREACH:
		case ENETUNREACH:
			eReason=ePCFR_UNREACHABLE;
			break;
		default:
			{
				ostringstream strm;
				strm<<"connect failed with error code:"<<nErrorCode;
				GenErr(strm.str());
			}
			break;
		}
		CSyncPipe* pPipe=GetPipe();
		new CPipeState_Disconnected(pPipe);
		pPipe->AddEventConnectFailed(eReason);
		return;
#endif
	}
}

bool CPipeState_Connecting::Listen(const CAddress& Address)
{
	return false;
}

bool CPipeState_Connecting::Connect(const CAddress& Address)
{
	return false;
}

void CPipeState_Connecting::ShutDown(bool IfCallback)
{
	CSyncPipe* pPipe=GetPipe();
	new CPipeState_Disconnected(pPipe);
	if (IfCallback)
		pPipe->AddEventConnectFailed(ePCFR_CANCELED);
}

bool CPipeState_Connecting::IsConnecting()const
{
	return true;
}
