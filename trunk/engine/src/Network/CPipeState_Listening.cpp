#include "stdafx.h"
#include "CPipeState_Listening.h"
#include "CPipeState_Disconnected.h"
#include "CSyncPipe.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "CSyncPipe.inl"

//******************************************************************
// CPipeState_Listening
//******************************************************************

CPipeState_Listening::CPipeState_Listening(CSyncPipe* pPipe,SOCKET Socket)
:CPipeState_Busy(pPipe,Socket)
{
	CPipeState* pState=GetPipe()->m_pState;
	GetPipe()->m_pState=NULL;
	delete pState;
	GetPipe()->m_pState=this;

#ifdef _WIN32
	SQR_TRY
	{
		m_Event.Create(m_Socket);
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
		closesocket(m_Socket);
		throw;
	}
	SQR_TRY_END;
#endif

	Register();

	if( listen(m_Socket,SOMAXCONN) )
	{
		ostringstream strm;
		strm<<"listen failed with error code:"<<SocketGetLastError()<<".";
		GenErr(strm.str());
	}
}

CPipeState_Listening::~CPipeState_Listening()
{
	Unregister();

#ifdef _WIN32
	m_Event.Destroy();
#endif
}

int CPipeState_Listening::CreateBindedSocket(SOCKET& SocketOut,const CAddress& Address)
{
	SOCKET Socket=CreateSocket();

	sockaddr_in saiAddress;
	memset(&saiAddress,0,sizeof(saiAddress));

	saiAddress.sin_addr.s_addr = inet_addr( Address.GetAddress() );
	saiAddress.sin_port = htons( static_cast<u_short>( Address.GetPort() ) );
	saiAddress.sin_family = AF_INET;

	//bind
	if( bind( Socket,reinterpret_cast<sockaddr*>( &saiAddress) , sizeof(sockaddr) ) )
	{
		closesocket(Socket);
		return SocketGetLastError();
	}

	SocketOut=Socket;
	return 0;
}

CPipeState* CPipeState_Listening::GetPipeState()const
{
	return const_cast<CPipeState_Listening*>(this);
}


void CPipeState_Listening::ProcessEvent(bool bRead,bool /*bWrite*/,bool /*bError*/)
{
	if( !bRead )
		return;

	for(;;)
	{
		SOCKET Socket = accept( m_Socket , NULL , NULL );

		if( INVALID_SOCKET == Socket )
		{
			int nErrorCode=SocketGetLastError();
			switch(nErrorCode)
			{
#ifndef _WIN32
			case ECONNABORTED:
			case EMFILE:
#endif
			case EWOULDBLOCK:
				return;
			}
			ostringstream strm;
			strm<<"accept failed with error code:"<<nErrorCode<<".";
			GenErr(strm.str());
		}

		u_long uParam=1;
		if( SOCKET_ERROR == ioctlsocket( Socket , FIONBIO , &uParam ) )
		{
			closesocket( Socket );
			ostringstream strm;
			strm<<"ioctlsocket failed with error code "<< SocketGetLastError() <<".";
			GenErr(strm.str());
		}

		bool bDisconnected;
		CSyncPipe* pNewPipe = new CSyncPipe( NULL ,	GetPipe()->m_OutBuffer.OutBufferGetSize() ,
			GetPipe()->m_InBuffer.InBufferGetSize(),Socket , GetPipe()->m_uCreationFlag,bDisconnected );
		
		if( bDisconnected )
		{
			delete pNewPipe;
		}
		else
		{
			GetPipe()->AddEventAccepted( pNewPipe );
		}
	}
}

bool CPipeState_Listening::Listen(const CAddress& Address)
{
	return false;
}

bool CPipeState_Listening::Connect(const CAddress& Address)
{
	return false;
}

void CPipeState_Listening::ShutDown(bool IfCallback)
{
	new CPipeState_Disconnected(GetPipe());
}


void CPipeState_Listening::Register()
{
	if(!GetPipe()->m_pReactor)
		return;

#ifdef _WIN32
	EventAdd(FD_ACCEPT);
#else
	EpollCtl(EPOLL_CTL_ADD,EPOLLIN);
#endif
}


void CPipeState_Listening::Unregister()
{
	if( !GetPipe()->m_pReactor )
		return;

#ifdef _WIN32
	EventDel();
#else
	EpollCtl(EPOLL_CTL_DEL,0);
#endif
}

