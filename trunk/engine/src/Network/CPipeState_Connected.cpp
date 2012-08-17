#include "stdafx.h"
#include "TimeHelper.h"
#include "CSyncPipe.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "CSyncPipeReactor.h"
#include "CPipeState_Connected.h"
#include "CPipeState_Disconnected.h"
#include "TSqrAllocator.inl"
#include "CSyncPipe.inl"


//******************************************************************
// CPipeState_Connected
//******************************************************************


CPipeState_Connected::CPipeState_Connected(CSyncPipe* pPipe,const SOCKET& Socket,bool& bDisconnected)
:CPipeState_Busy(pPipe,Socket)
{
#ifdef _WIN32
	m_pLastOverlappedRecvWritePos=NULL;
	m_pOverlappedSend=NULL;
	m_pOverlappedRecv=NULL;
#endif
	
	CPipeState* pState=GetPipe()->m_pState;
	GetPipe()->m_pState=NULL;
	delete pState;
	GetPipe()->m_pState=this;

	Register();

	bDisconnected = !SendShakeHandMsg();
}

CPipeState_Connected::~CPipeState_Connected()
{
	GetPipe()->DelFromAllMulticast();
	Unregister();
}

inline bool CPipeState_Connected::SendWayEncryption()const
{
	return (GetPipe()->m_uCreationFlag & ePCF_SendWayEncryption) != 0;
}

inline bool CPipeState_Connected::RecvWayDecryption()const
{
	return (GetPipe()->m_uCreationFlag & ePCF_RecvWayDecryption) != 0;
}

inline bool CPipeState_Connected::SendWayCompression()const
{
	return (GetPipe()->m_uCreationFlag & ePCF_SendWayCompression) != 0;
}

inline bool CPipeState_Connected::RecvWayDecompression()const
{
	return (GetPipe()->m_uCreationFlag & ePCF_RecvWayDecompression) != 0;
}


bool CPipeState_Connected::SendShakeHandMsg()
{
	if( !SendWayEncryption() )
		return true;

	uint32 uKey=GetPipe()->m_OutBuffer.m_Encoder.Reset();

	//printf("send %d\n",int(sizeof(uKey)) );
	//这里不能使用OverlappedSend,因为会触发OnDataSent回调，握手数据发送成功是不应该通知上层的。
	int nResult = send(m_Socket,reinterpret_cast<const char*>(&uKey),sizeof(uKey),0);

	if( nResult == sizeof(uKey) )
		return true;

	if( nResult == SOCKET_ERROR )
	{
		int nErrorCode=SocketGetLastError();

		switch( nErrorCode )
		{
#ifdef _WIN32
		case WSA_OPERATION_ABORTED:
#else
		case EWOULDBLOCK:
		case ENOBUFS:
		case EPIPE:
#endif
		case ECONNABORTED:
		case EHOSTUNREACH:
		case ECONNRESET:
			break;
		default:
			{
				ostringstream strm;
				strm<<"send@SendShakeHandMsg failed, last error:"<<nErrorCode;
				LogErr( strm.str() );
			}
		}

		return false;
	}
	else
	{
		char szMsg[32];
		sprintf( szMsg, "%d", nResult );
		LogErr( "SendShakeHandMsg failed to send the whole key.",szMsg );
	}

	return false;
}

size_t CPipeState_Connected::Send(const void* pData,size_t stSize)
{
	Ast( GetPipe()->m_pState == static_cast<CPipeState*>(this) );
	if( !stSize )
		return 0;

	const char* pByteData = reinterpret_cast<const char*>(pData);

	CSyncPipeOutBuffer& OutBuffer=GetPipe()->m_OutBuffer;

	const bool bNeedToSend = OutBuffer.NoDataToSend();

	size_t stPushedSize = OutBuffer.OutBufferPushData( pByteData, stSize );	

	if( bNeedToSend )
#ifndef _WIN32
		EpollEventRegister();
#else
		if( OverlappedSend() )
			return 0;
#endif

	CSyncPipeReactor * pReactor = GetPipe()->m_pReactor;
	if( pReactor )
		m_DataFlowCounter.DataFlow( stPushedSize,pReactor->m_uLastCheckTime );
	
	return stPushedSize;
}

void CPipeState_Connected::AddDisconnectEventAndChangeState(EPipeDisconnectReason eReason,bool bGracefully)
{
	CSyncPipe*const pPipe=GetPipe();
	new CPipeState_Disconnected(pPipe);
	//这两行的顺序不能掉转，因为CPipeState_Disconnected析构可能会触发OnDataReceived事件，如果掉转了顺序，这个事件就在OnDisconnected之后触发了
	pPipe->AddEventDisconnected(eReason,bGracefully);	
}


#ifdef _WIN32

bool CPipeState_Connected::OverlappedRecv()
{
	//puts("overlapped recv");
	Ast( !m_pOverlappedRecv );
	
	CSyncPipeInBuffer& Buffer=GetPipe()->m_InBuffer;

	WSABUF wsBuffer;
	wsBuffer.len= uint32( Buffer.InBufferGetLeftSpaceSize() );
	if(wsBuffer.len==0)
		return false;

	Ast( !m_pLastOverlappedRecvWritePos );
	m_pLastOverlappedRecvWritePos = Buffer.InBufferGetWritePos();
	wsBuffer.buf=m_pLastOverlappedRecvWritePos;

	DWORD dwRecvSize;
	DWORD dwFlags=0;

	m_pOverlappedRecv=new WSAOVERLAPPED;
	memset(m_pOverlappedRecv,0,sizeof(WSAOVERLAPPED));
	m_pOverlappedRecv->hEvent=this;

	//puts("overlapped recv confirm");
	int nResult=WSARecv( m_Socket,&wsBuffer,1,&dwRecvSize,&dwFlags,m_pOverlappedRecv,
		(LPWSAOVERLAPPED_COMPLETION_ROUTINE)&RecvCompletionRoutine );

	switch(nResult)
	{
	case 0:
		break;
	case SOCKET_ERROR:
		nResult=SocketGetLastError();
		switch(nResult)
		{
		case WSA_IO_PENDING:
			break;
		case EHOSTUNREACH:
		case ECONNRESET:
			//连接断开Resetby peer
			AddDisconnectEventAndChangeState(ePDR_REMOTE,false);
			return true;
		case ETIMEDOUT:
		case ECONNABORTED:
			AddDisconnectEventAndChangeState(ePDR_ERROR,false);
			return true;
#ifdef _WIN32
		case WSAEFAULT:
			DebugBreak();
			break;
#endif
		default:
			{
				ostringstream strm;
				strm<<"WSARecv failed and SocketGetLastError return:"<<nResult<<".";
				GenErr(strm.str());
			}
		}
		break;
	default:
		{
			ostringstream strm;
			strm<<"WSARecv failed with error code:"<<nResult;
			GenErr(strm.str());
		}
	}

	return false;
}


bool CPipeState_Connected::OverlappedSend()
{
	//puts("overlapped send");
	Ast( !m_pOverlappedSend );	

	CSyncPipeOutBuffer& Buffer=GetPipe()->m_OutBuffer;

	if( Buffer.NoDataToSend() )
	{
		//puts("overlapped send abort");
		return false;
	}
	
	WSABUF* pBuf;

	DWORD dwBufferCount;
	DWORD dwByteSent=0;

	Buffer.PrepareSendData(pBuf,dwBufferCount);
	m_pOverlappedSend=new WSAOVERLAPPED;	
	memset(m_pOverlappedSend,0,sizeof(WSAOVERLAPPED));
	m_pOverlappedSend->hEvent=this;

	int nResult=WSASend(m_Socket,pBuf,dwBufferCount,&dwByteSent,0,m_pOverlappedSend,
		(LPWSAOVERLAPPED_COMPLETION_ROUTINE)&SendCompletionRoutine);
	
	bool bStateChanged=false;

	switch( nResult )
	{
	case 0:
		break;
	case SOCKET_ERROR:
		nResult=SocketGetLastError();
		switch(nResult)
		{
		case WSA_IO_PENDING:
			break;
		case EHOSTUNREACH:
		case WSAEINVAL:	//在WSASend的时候，如果Socket已经被对方shutdown，居然会报这个错误，是在难以理解
		case ECONNRESET:
			//连接断开Resetby peer
			AddDisconnectEventAndChangeState(ePDR_REMOTE,false);
			bStateChanged = true;
			break;
		case ETIMEDOUT:
		case ECONNABORTED:
			AddDisconnectEventAndChangeState(ePDR_ERROR,false);
			bStateChanged = true;
			break;
		default:
			{
				ostringstream strm;
				strm<<"WSASend failed and SocketGetLastError return:"<<nResult<<".";
				cout<<strm.str()<<endl;
				GenErr(strm.str());
			}
		}
		break;
	default:
		{
			ostringstream strm;
			strm<<"WSASend failed and SocketGetLastError() is:"<<nResult;
			GenErr(strm.str());
		}
	}

	//必须放在这里delete,否则前面的GetLastError会返回0
	delete[] pBuf;	
	
	return bStateChanged;
}
#endif


#ifdef _WIN32
void CALLBACK CPipeState_Connected::RecvCompletionRoutine(uint32 uError,uint32 uRecvSize,WSAOVERLAPPED* pOverlapped,uint32 /*uFlags*/)
{
	if( pOverlapped->hEvent )
	{
		CPipeState_Connected*const pState=reinterpret_cast<CPipeState_Connected*>(pOverlapped->hEvent);
		Ast( pState->m_pOverlappedRecv==pOverlapped );
		pState->m_pOverlappedRecv=NULL;
		pState->OnRecvCompletion(uError,uRecvSize);
	}
	delete pOverlapped;
}
#endif


inline bool CPipeState_Connected::OnRecvCompletion(uint32 uError,uint32 uRecvSize)
{
	//printf("recv %d\n",int(uRecvSize));
	CSyncPipeInBuffer* pBuffer=&GetPipe()->m_InBuffer;

	switch(uError)
	{
	case 0:
		if( uRecvSize==0 )
		{
#ifndef _WIN32
			//这里是查错代码
			if( GetPipe()->m_pState != this )
			{
				ostringstream strm;
				strm<<"calling state is:"<<typeid( *this ).name()<<endl;

				if( !GetPipe()->m_pState )
				{
					strm<<"pipe state is NULL.";
				}
				else
				{
					strm<<"pipe state is "<<typeid( *GetPipe()->m_pState ).name()<<endl;
				}

				GetPipe()->PrintAndClearHistory( strm );
				
				LogErr("Pipe state is not consistent.",strm.str());
			}
#endif
			//连接断开(Gracefully)
			//puts("4");
			AddDisconnectEventAndChangeState( ePDR_REMOTE, true );
			return true;
		}
		else
		{
			CSyncPipeInBuffer::EProcessRecvDataResult eResult;
#ifdef _WIN32
			eResult = pBuffer->InBufferTellAddedDataPosAndSize( m_pLastOverlappedRecvWritePos, size_t(uRecvSize) );
			m_pLastOverlappedRecvWritePos=NULL;
#else
			eResult = pBuffer->InBufferTellAddedDataSize( size_t(uRecvSize) );
#endif
			switch( eResult )
			{
			case CSyncPipeInBuffer::ePRDR_Error:
				//连接断开(Gracefully)
				AddDisconnectEventAndChangeState( ePDR_ERROR,false );
				return true;
			case CSyncPipeInBuffer::ePRDR_NewData:
				GetPipe()->AddEventNewDataReceived();
				break;
			default:
				break;
			}
#ifdef _WIN32
			if( GetPipe()->IsSyncCallback() )
			{
				//puts("1");
				if( !m_pLastOverlappedRecvWritePos )
				{
					//puts("3");
					OverlappedRecv();
				}
			}
			else
			{
				//puts("2");
				OverlappedRecv();
			}				
#endif
		}
		break;
	case ETIMEDOUT:	
#ifndef _WIN32
	case EWOULDBLOCK:
#else
	case WSA_OPERATION_ABORTED:
	case WSAECONNABORTED:
#endif
		break;
#ifdef _WIN32
	case WSAENOBUFS:
		AddDisconnectEventAndChangeState( ePDR_ERROR,false );
		return true;
#endif
	case EHOSTUNREACH:
	case ECONNRESET:
		AddDisconnectEventAndChangeState( ePDR_REMOTE,false );
		return true;
	default:
		{
			ostringstream strm;
			strm<<"recv failed with error code "<<uError<<".";
			GenErr(strm.str());
		}
	}
	return false;
}


#ifdef _WIN32
void CALLBACK CPipeState_Connected::SendCompletionRoutine(uint32 uError,uint32 uSentSize,WSAOVERLAPPED* pOverlapped,uint32 /*uFlags*/)
{
	if( pOverlapped->hEvent )
	{
		CPipeState_Connected*const pState=reinterpret_cast<CPipeState_Connected*>(pOverlapped->hEvent);
		Ast( pState->m_pOverlappedSend==pOverlapped );
		pState->m_pOverlappedSend=NULL;
		pState->OnSendCompletion(uError,uSentSize);
	
	}
	delete pOverlapped;
}
#endif



inline bool CPipeState_Connected::OnSendCompletion(uint32 uError,uint32 uSentSize)
{
	//printf("Send completion %d\n",uSentSize);
	CSyncPipeOutBuffer* pBuffer = &GetPipe()->m_OutBuffer;

	switch( uError )
	{
	case 0:
		{
			const bool bWasFull=pBuffer->OutBufferIsFull();
			pBuffer->OutBufferPopChunkData(uSentSize);
			
			if( GetPipe()->NeedSomeDataSentCallback() )
				GetPipe()->AddEventSomeDataSent( uSentSize );

			if( bWasFull )
				GetPipe()->AddEventSendBufferFree();
#ifdef _WIN32
			OverlappedSend();
#endif
			break;
		}
#ifdef _WIN32
	case WSA_OPERATION_ABORTED:
		break;
#endif
#ifndef _WIN32
	case EWOULDBLOCK:
		break;
	case ENOBUFS:
	case EPIPE:
#endif
	case ECONNABORTED:
	case EHOSTUNREACH:
	case ECONNRESET:
		//连接断开Resetby peer
		//此处m_Socket有可能是NULL，说明当前State正在析构过程中，不应该再次触发析构
		AddDisconnectEventAndChangeState( ePDR_REMOTE,false );
		return true;
	default:
		{
			ostringstream strm;
			strm<<"send failed with error code "<<uError<<".";
			GenErr(strm.str());
		}
	}
	return false;	
}



void CPipeState_Connected::ProcessEvent(bool bRead,bool bWrite,bool /*bError*/)
{

#ifndef _WIN32
	//读数据
	if(bRead)
	{
		CSyncPipeInBuffer* pBuffer=&GetPipe()->m_InBuffer;

		int nLeftSpace=(uint32)pBuffer->InBufferGetLeftSpaceSize();
		int nResult=recv(m_Socket,pBuffer->InBufferGetWritePos(),nLeftSpace,0);

		if(OnRecvCompletion((nResult==SOCKET_ERROR)?SocketGetLastError():0,nResult))
			return;
	}


	if( bWrite )
	{
		int nDataSize,nResult;
		CSyncPipeOutBuffer* pBuffer=&GetPipe()->m_OutBuffer;

		do
		{
			nDataSize=(uint32)pBuffer->OutBufferGetChunkDataSize();

			if( nDataSize==0 )
				break;

			nResult = send( m_Socket,pBuffer->OutBufferGetChunkData(),nDataSize,0 );

			if( OnSendCompletion( (nResult==SOCKET_ERROR)?SocketGetLastError():0,nResult ) )
				return;

		}while( nDataSize==nResult );
	}

	EpollEventRegister();
#else
	GenErr("CPipeState_Connected does not process event on WIN32!");
#endif

	return;
}

#ifndef _WIN32
void CPipeState_Connected::EpollEventRegister()
{
	uint32 uEvent=0;

	if( GetPipe()->m_InBuffer.InBufferGetLeftSpaceSize()>0 )
		uEvent=uEvent|EPOLLIN;

	if( !GetPipe()->m_OutBuffer.NoDataToSend() )
		uEvent=uEvent|EPOLLOUT;

	EpollCtl(EPOLL_CTL_MOD,uEvent);
}
#endif


bool CPipeState_Connected::Listen(const CAddress& Address)
{
	return false;
}

bool CPipeState_Connected::Connect(const CAddress& Address)
{
	return false;
}

void CPipeState_Connected::ShutDown(bool IfCallback)
{
	CSyncPipe*const pPipe=GetPipe();
	new CPipeState_Disconnected(pPipe);
	if (IfCallback)
		pPipe->AddEventDisconnected(ePDR_USER,false);
}

void CPipeState_Connected::Register()
{
	if( !GetPipe()->m_pReactor )
		return;

#ifndef _WIN32
	GetPipe()->WriteHistory("Epoll Add");

	EpollCtl(EPOLL_CTL_ADD,0);
	EpollEventRegister();
#else
	if( OverlappedRecv() )
		return;//断开连接
	OverlappedSend();
#endif
}

void CPipeState_Connected::Unregister()
{
	if( !GetPipe()->m_pReactor )
		return;

#ifndef _WIN32
	GetPipe()->WriteHistory("Epoll Del");

	EpollCtl(EPOLL_CTL_DEL,0);
#else
	if( m_pOverlappedSend )
		m_pOverlappedSend->hEvent=NULL;
	m_pOverlappedSend=NULL;

	if( m_pOverlappedRecv )
		m_pOverlappedRecv->hEvent=NULL;
	m_pOverlappedRecv=NULL;

	if( ! CancelIo( HANDLE(m_Socket) ) )
	{
		ostringstream strm;
		strm<<"CancelIo failed with error code:"<<SocketGetLastError()<<".";
		GenErr(strm.str());
	}
#endif
}

float CPipeState_Connected::GetDataFlowSpeed()
{
	CSyncPipeReactor* pReactor = GetPipe()->m_pReactor;

	if( !pReactor )
		return 0;

	return m_DataFlowCounter.GetFlowSpeed( pReactor->m_uLastCheckTime );
}
