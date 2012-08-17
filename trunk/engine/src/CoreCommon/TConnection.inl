#pragma once
#include "TConnection.h"
#include "IPipe.h"
#include "ExpHelper.h"
#include "TTickHandler.h"
#include <fstream>
#include <exception>
#include "ErrLogHelper.h"
#include "CExpCounter.h"
#include "TimeHelper.h"
#include "TMsgDispatcher.h"
#include <time.h>

template<typename Traits>
char* TConnection<Traits>::ms_pCurRecvBuffer=NULL;

template<typename Traits>
size_t TConnection<Traits>::ms_uCurRecvBufferSize=0;

template<typename Traits>
TConnection<Traits>::TConnection()
{
	m_bIsGuidSet		=false;
	m_pTickDoShutDown	=NULL;
	m_bShakedHand		=false;
	m_bShutDownCalled	=false;
	m_uConnCorruptedNum	=0;
	m_pPipe			=NULL;
	m_uExpNum		=0;
	m_pExpCounter = new CExpCounter(3000, 5);
}

template<typename Traits>
TConnection<Traits>::~TConnection(void)
{
	if(m_pTickDoShutDown)
	{
		ImpTimeSystem_t::Inst()->UnRegister(m_pTickDoShutDown);
		delete m_pTickDoShutDown;
		m_pTickDoShutDown = NULL;
	}
	delete m_pExpCounter;
}

template<typename Traits>
void TConnection<Traits>::Init(IPipe* pPipe)
{
	Ast( !m_pPipe );
	m_pPipe = pPipe;
}

template<typename Traits>
bool TConnection<Traits>::IsShuttingDown()const
{
	return m_bShutDownCalled;
}

template<typename Traits>
bool TConnection<Traits>::DispatchIsCanceled()const
{
	Ast( m_pPipe );
	return m_pTickDoShutDown != NULL || !m_pPipe->IsConnected();
}


template<typename Traits>
IPipe* TConnection<Traits>::GetPipe()const
{
	return m_pPipe;
}

template<typename Traits>
inline void TConnection<Traits>::SendCoreCmd(const void* pData,unsigned int uSize)
{
	if(!pData || uSize == 0)
		return;

	SendCoreMsg(pData,uSize);
}

template<typename Traits>
void TConnection<Traits>::SendCoreMsg(const void* pData,unsigned int uSize)
{
	if (m_pTickDoShutDown)
		return;

	if( !m_pPipe || !m_pPipe->IsConnected() )
		return;

	if ( uSize != m_pPipe->Send(pData,uSize) )
	{
		ostringstream strm;

		const char* szUserName = GetValue("UserName");
		szUserName = szUserName?szUserName:"";
		this->LogTime(strm);
		strm << " reason:发送缓冲区溢出 account:" << szUserName << " localip: remoteip: " << endl;

		static_cast<ImpConn_t*>(this)->LogOnOffLineMsg(strm.str().c_str());

		m_pPipe->ShutDown(true);
	}
}

template<typename Traits>
unsigned TConnection<Traits>::OnNetMessage(void* pData,unsigned int uSize,void* pArg)
{
	uint32 uProcessedSize;
	
	EDispatchResult eResult = static_cast<ImpConn_t*>(this)->Dispatch(pData,uSize,pArg,uProcessedSize);

	switch( eResult ) 
	{
	case eSuccess:
	case eCanceled:
	case eCallAgain:
		return uProcessedSize;
	default:
		break;
	}

	const char* szErrInfo;

	switch( eResult )
	{	
	case eHandlerOver:
		szErrInfo = "eHandlerOver";
		break;
	case eHandlerNotFound:
		szErrInfo = "eHandlerNotFound";
		break;
	case eCallError:
		szErrInfo = "eCallError";
		break;
	default:
		szErrInfo = "UnknownError";
		break;
	}

	CAddress addr;
	m_pPipe->GetRemoteAddress(addr);

	//strm << "\"" << szErrInfo << "\" 数据长度为:" << uSize;

	//LogErr( "Dispatch返回错误码",strm.str(),this );

	if ( ++m_uConnCorruptedNum > ImpAppConfig_t::Inst()->GetConnExpBearNum() )
	{
		ostringstream strm;	
		strm.str("Dispatch返回错误码:");
		strm << "\"" << szErrInfo << "\" 数据长度为:" << uSize;
		CoreShutDown( strm.str().c_str() );
	}
	
	return uProcessedSize;
}

template<typename Traits>
void TConnection<Traits>::OnConnectFailed(EPipeConnFailedReason eReason)
{
}

template<typename Traits>
void TConnection<Traits>::OnConnected()
{
}

template<typename Traits>
void TConnection<Traits>::OnDisconnected(EPipeDisconnectReason eReason,bool bGracefully)
{
	DoShutDownAndCallback();
}

template<typename Traits>
void TConnection<Traits>::OnDataReceived()
{
	SQR_TRY
	{
		uint32 uProcessedSize=OnNetMessage(m_pPipe->GetRecvData(),(uint32)m_pPipe->GetRecvDataSize(),NULL);

		if( uProcessedSize==0 && m_pPipe->RecvBufferFull() )
			GenErr("Receive buffer is full");

		GetPipe()->PopRecvData(uProcessedSize);
	}
	//这些异常可能是由于消息id错误等引起的，不能忽略，直接断线
	SQR_CATCH(exp)
	{
#ifdef FindBugOfTBaseMsgDispatcher
		ImpConn_t* pConn = static_cast<ImpConn_t*>(this);
		ostringstream strm;
		strm << pConn->m_bFlag1 << " "
			<< pConn->m_bFlag2 << " "
			<< pConn->m_bFlag3 << " "
			<< pConn->m_bFlag4 << " "
			<< pConn->m_bFlag5 << " "
			<< pConn->m_bFlag6 << " ";
#endif
		exp.AppendMsg(strm.str());
		LogExp(exp);
		CoreShutDown("这些异常可能是由于消息id错误等引起的，不能忽略，直接断线");
	}
	SQR_TRY_END;
}

template<typename Traits>
void TConnection<Traits>::OnDataSent(uint32 uSentSize)
{
}


template<typename Traits>
inline void TConnection<Traits>::CacheRecvData(char* pData,size_t stSize)
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

template<typename Traits>
void TConnection<Traits>::StoreRecvData()
{
	if( !ms_pCurRecvBuffer )
		return;

	Ast( m_strRecvBuffer.empty() );
	m_strRecvBuffer.append( ms_pCurRecvBuffer , ms_uCurRecvBufferSize );
	ms_pCurRecvBuffer = NULL;
}


template<typename Traits>
void* TConnection<Traits>::GetRecvData() const
{
	if( ms_pCurRecvBuffer )
		return ms_pCurRecvBuffer;
	return reinterpret_cast<void*>( const_cast<char*> ( ( m_strRecvBuffer.data() ) ) );
}

template<typename Traits>
size_t TConnection<Traits>::GetRecvDataSize() const
{
	if( ms_pCurRecvBuffer )
		return ms_uCurRecvBufferSize;
	return m_strRecvBuffer.size();
}

template<typename Traits>
void TConnection<Traits>::PopRecvData(size_t stSize)
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

template<typename Traits>
inline bool TConnection<Traits>::IsConnected()const
{
	return true;
}


template<typename Traits>
void TConnection<Traits>::DoShutDownAndCallback()
{
	static_cast<ImpConn_t*>(this)->DoShutDownAndCallbackDerived();
}

template<typename Traits>
bool TConnection<Traits>::IntShutDown()
{
	if( m_bShutDownCalled )
		return false;

	m_bShutDownCalled = true;

	return RegDoShutDownTick();
}

/************************************************************************/
/*                                                                      
ShutDown机制
ShutDown分为主动断线和被动断线
在逻辑层面上面，调用ShutDown就是主动断线
如果是另一端断线导致这边断线，或者是由于错误导致了引擎的断线，则是被动断线
在主动断线的过程中IsShuttingDown一直为true，而被动断线则为false
同样，我们需要一个标志表明是否是主动断线还是被动断线
如果在主动断线的过程中，出现了被动断线，这时候标志仍然为主动断线
如果在被动断线的过程中，出现了主动断线，这时候强制把标志设置为主动断线
*/
/************************************************************************/

template<typename Traits>
void TConnection<Traits>::LogTime(ostringstream& stream)
{
	time_t T;
	struct tm * timenow;

	time ( &T );
	timenow = localtime ( &T ); 

	stream << " time:" << 
		timenow->tm_mon + 1<< "-" <<
		timenow->tm_mday << "-" <<
		timenow->tm_hour << ":" <<
		timenow->tm_min << ":" <<
		timenow->tm_sec << " ";
}

template<typename Traits>
void TConnection<Traits>::CoreShutDown(const char* msg)
{
	if (m_bShutDownCalled)
		return;

	if (RegDoShutDownTick())
	{
		ostringstream strm;
		CAddress addr;
		m_pPipe->GetRemoteAddress(addr);
		CAddress LocalAddr;
		m_pPipe->GetLocalAddress(LocalAddr);
		const char* szUserName = GetValue("UserName");
		szUserName = szUserName?szUserName:"";
		this->LogTime(strm);
		strm << " account:" << szUserName << " reason:" << msg 
			<< " remoteip:" << addr.GetAddress() 
			<< " localip:" << LocalAddr.GetAddress() << endl;

		static_cast<ImpConn_t*>(this)->LogOnOffLineMsg(strm.str().c_str());
	}
}

template<typename Traits>
void TConnection<Traits>::LogOnOffLineMsg(const char* szMsg)
{
}

template<typename Traits>
bool TConnection<Traits>::RegDoShutDownTick()
{
	if(m_pTickDoShutDown)
		return false;

	m_pTickDoShutDown=new TickFuncHandler_t(this);

	ImpTimeSystem_t::Inst()->Register(m_pTickDoShutDown,0);
	return true;
}

template<typename Traits>
void TConnection<Traits>::UnRegDoShutdownTick()
{
	if( !m_pTickDoShutDown )
		return;

	ImpTimeSystem_t::Inst()->UnRegister(m_pTickDoShutDown);

	delete m_pTickDoShutDown;
	m_pTickDoShutDown=NULL;
}


template<typename Traits>
void TConnection<Traits>::OnCaughtNetException()
{
	if( !m_pExpCounter->AddExpCounter(GetProcessTime() ) )
		return;
	
	char* szBuf = "发生的错误超过了容忍限度，系统将断线";
	LogErr(szBuf, this);
	CoreShutDown(szBuf);
}

template<typename Traits>
bool TConnection<Traits>::GetLocalAddress(CAddress& Address) const
{
	bool bResult = GetPipe()->IsConnected();
	if(bResult)
		GetPipe()->GetLocalAddress(Address);

	return bResult;
}

template<typename Traits>
bool TConnection<Traits>::GetRemoteAddress(CAddress& Address) const
{
	bool bResult = GetPipe()->IsConnected();
	if(bResult)
		GetPipe()->GetRemoteAddress(Address);

	return bResult;
}
