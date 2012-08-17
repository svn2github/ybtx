#pragma once
//$Id$
//这个文件包含了引擎的主循环，客户端和服务段的程序框架从这个文件定义的TApp派生出来
#include "TApp.h"
#include "IPipeModule.h"
#include "IPipeReactor.h"
#include "IMulticast.h"
#include "ExpHelper.h"
#include "TimeHelper.h"
#include "TConnectionMgr.h"
#include "CCoreMsgMgr.h"
#include "Console.h"
#include "BaseHelper.h"
#include "ErrLogHelper.h"
#include "CPkgFile.h"
#include "ExpHelper.h"
#include "MemoryHelper.h"
#include "ThreadHelper.h"
#include "CDistortedTick.h"

#ifndef _WIN32
#include "CCoreSigMsgMgr.h"
#endif

template<typename Traits>
TApp<Traits>::TApp()
: m_bQuit(false)
, m_pHandler(NULL)
{
	if( Inst() )
		GenErr(" App is already inited.");

	IntInst() = static_cast<ImpApp_t*>(this);

	if( !ImpAppConfig_t::Inst() )
		GenErr("AppConfig must be inited before App initialization.");

	uint32 uCyc = ImpAppConfig_t::Inst()->GetBaseTickCyc();

	if(uCyc==0)
		GenErr("uCyc can't be 0.");
	
#ifdef _WIN32
	if ( _configthreadlocale(_ENABLE_PER_THREAD_LOCALE) == -1 )
		GenErr("_configthreadlocale failed.");
#endif

	ImpTimeSystem_t::Init(uCyc);

	m_pPipeModule = GetIPipeModule();

	static_cast<ImpApp_t*>(this)->CreatePipeReactor();

	Ast( !ImpConnMgr_t::Inst() );
	ImpConnMgr_t::IntInst()=new ImpConnMgr_t;
}

template<typename Traits>
TApp<Traits>::~TApp()
{
	delete ImpConnMgr_t::Inst();
	ImpConnMgr_t::IntInst()=NULL;

	m_pPipeReactor->Release();
	ReleaseIPipeModule( m_pPipeModule );

	SetHandler( NULL );

	ImpTimeSystem_t::Unit();

	IntInst() = NULL;
}


template<typename Traits>
typename TApp<Traits>::ImpApp_t*& TApp<Traits>::IntInst()
{
	static ImpApp_t* ls_pInst= NULL;
	return ls_pInst;
}

template<typename Traits>
typename TApp<Traits>::ImpApp_t* TApp<Traits>::Inst()
{
	return IntInst();
}


template<typename Traits>
void TApp<Traits>::SetHandler(ImpAppHandler_t* pHandler)
{
	AdvDelRef(m_pHandler);
	m_pHandler=pHandler;
	AdvAddRef(m_pHandler);
}

template<typename Traits>
typename TApp<Traits>::ImpAppHandler_t* TApp<Traits>::GetHandler()const
{
	return m_pHandler;
}


template<typename Traits>
void TApp<Traits>::CreatePipeReactor()
{
	uint32 uNetThreadNum=ImpAppConfig_t::Inst()->GetNetThreadNum();

	if( uNetThreadNum == 0 )
	{
		m_pPipeReactor = m_pPipeModule->GetSyncIPipeReactor();
	}
	else
	{
		m_pPipeReactor = m_pPipeModule->GetAsynIPipeReactor( uNetThreadNum );
	}	
}


template<typename Traits>
const char* TApp<Traits>::GetThreadName()
{
	return "AppThread";
}

template<typename Traits>
IPipe* TApp<Traits>::GetIPipe(IPipeHandler* pHandler)
{
	return GetIPipe(pHandler,1024,1024,false);
}

template<typename Traits>
IPipe* TApp<Traits>::GetIPipe(IPipeHandler* pHandler,size_t stSendBufferSize,size_t stRecvBufferSize,bool bUseSomeDataSentCallback)
{
	return m_pPipeReactor->
		CreateIPipe(pHandler,uint32(stSendBufferSize),uint32(stRecvBufferSize)
		,bUseSomeDataSentCallback?ePCF_DataSentCallback:0 );
}

template<typename Traits>
IMulticast* TApp<Traits>::GetIMulticast()
{
	return m_pPipeReactor->CreateMulticast();
}



template<typename Traits>
void TApp<Traits>::ReactorCancelBlock()
{
	m_pPipeReactor->CancelBlock();
}


template<typename Traits>
typename TApp<Traits>::ImpConnMgr_t*
TApp<Traits>::GetConnMgr()const
{
	return ImpConnMgr_t::Inst();
}



template<typename Traits>
uint64 TApp<Traits>::GetFrameTime()const
{
	return uint64( ImpTimeSystem_t::Inst()->GetFrameTime() );
}


template<typename Traits>
void TApp<Traits>::RegisterTick(CTick* pTick,uint32 uCyc)
{
	ImpTimeSystem_t::Inst()->Register(pTick,uCyc);
}

template<typename Traits>
void TApp<Traits>::UnRegisterTick(CTick* pTick)
{
	ImpTimeSystem_t::Inst()->UnRegister(pTick);
}

template<typename Traits>
void TApp<Traits>::RegisterDistortedTick(CDistortedTick* pTick, uint32 uCyc)
{
	if(!pTick || pTick->m_pDistortedTime)
		return;
	ImpTimeSystem_t::Inst()->Register(pTick->m_pTick,uCyc);
}

template<typename Traits>
void TApp<Traits>::UnRegisterDistortedTick(CDistortedTick* pTick)
{
	if(!pTick || pTick->m_pDistortedTime)
		return;
	ImpTimeSystem_t::Inst()->UnRegister(pTick->m_pTick);
}

template<typename Traits>
void TApp<Traits>::IntQuit()
{
	m_bQuit=true;
	m_pPipeReactor->CancelBlock();
}


template<typename Traits>
int TApp<Traits>::Run()
{
	HTHREAD hThread;
	sqr::GetCurrentThread(&hThread);

#ifdef _WIN32
	HANDLE hProcess = ::GetCurrentProcess();
	Ver( DuplicateHandle( hProcess , hThread ,hProcess , &hThread,0,FALSE, 0 ) );
	//两个App线程的优先级都要调高一些，所以就直接放在这里了
	SetThreadPriority( &hThread, THREAD_PRIORITY_ABOVE_NORMAL );
#endif
	
	const char* szThreadName = ImpApp_t::GetThreadName();
	SetThreadName(szThreadName);

	WatchThread( hThread , szThreadName, ImpTimeSystem_t::Inst()->GetRealTimePtr(), 30*1000 ,2,1000 );

	ImpTimeSystem_t::Inst()->Reset();
	static_cast<ImpApp_t*>(this)->OnBeginMainCycleDerived();

	LowerCurThreadPriority();

	while( static_cast<ImpApp_t*>(this)->MainFrame() );

	static_cast<ImpApp_t*>(this)->OnMainCycleEndedDerived();
	ImpTimeSystem_t::Inst()->Reset();

	DoNotWatchThread( hThread );

#ifdef _Win32
	CloseHandle( hThread );
#endif	
	return 0;
}

template<typename Traits>
void TApp<Traits>::OnBeginMainCycleDerived()
{
}

template<typename Traits>
void TApp<Traits>::OnMainCycleEndedDerived()
{	
}

template<typename Traits>
bool TApp<Traits>::MainFrame()
{
	//PushLogicTime不能自己追帧，这样会造成时间相关的时间被优先处理，其他像网络、数据库等消息被放到了次优先级
	ImpTimeSystem_t::Inst()->PushLogicTime();

#ifndef _WIN32
	static_cast<ImpApp_t*>(this)->FlushSigMsgDerived();
#endif
	static_cast<ImpApp_t*>(this)->OnMainFrameEndedDerived();
	
	int32 nTimeError=ImpTimeSystem_t::Inst()->GetTimeError();

	if( nTimeError>=0 )
		nTimeError = static_cast<ImpApp_t*>(this)->OnIdle( uint32(nTimeError) );
	
	if( nTimeError < 0 )
		nTimeError = 0;

	uint32 uResult = m_pPipeReactor->GetEvent( uint32( nTimeError ) );

	//printf("%d",uResult);
	
#ifdef _WIN32

	for( ;; )
	{
		switch( uResult )
		{
		case eGER_TimedOut:
			return true;
		case eGER_SysMsg:
			static_cast<ImpApp_t*>(this)->OnSysMsg();
			break;
		case eGER_Iocp:
			static_cast<ImpApp_t*>(this)->OnIocp();
			break;
		case eGER_NetMsg:
			break;
		case eGER_Canceled:
			static_cast<ImpApp_t*>(this)->OnCanceled();
			if( m_bQuit )
				return false;
			break;
		default:
			{
				ostringstream strm;
				strm<<"m_pPipeReactor->GetEvent return unknown value "<<uResult;
				GenErr(strm.str());
			}
		}
		uResult = m_pPipeReactor->GetEvent( 0 );
	}

#else

	if( uResult & eGER_Canceled )
		static_cast<ImpApp_t*>(this)->OnCanceled();

#endif

	return !m_bQuit;
}


template<typename Traits>
void TApp<Traits>::OnMainFrameEndedDerived()
{
}

template<typename Traits>
uint32 TApp<Traits>::OnIdle( uint32 uTimeLeft )
{
	return uTimeLeft;
}

#ifdef _WIN32

template<typename Traits>
void TApp<Traits>::OnSysMsg()
{
	uint32 uErrorIndex = 0;
	SQR_TRY	
	{
		MSG msg;
		while( PeekMessageW( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			uErrorIndex = 1;
			if( !GetMessage( &msg, NULL, 0, 0 ) )
				GenErr( "GetMessage shouldn't return 0." );
			uErrorIndex = 2;
			TranslateMessage( &msg );
			uErrorIndex = 3;
			DispatchMessageW( &msg );
			uErrorIndex = 0;
		}
	}
	SQR_CATCH(exp)
	{
		ostringstream strm;	
		strm << "TApp::OnSysMsg 异常"
				<< " 发生位置为: "
				<<uErrorIndex
				<<" LastErrorID :"
				<<GetLastError()
				<<endl;
		exp.AppendMsg(strm.str().c_str());
		LogExp(exp);
	}
	SQR_TRY_END;
}

template<typename Traits>
void TApp<Traits>::OnIocp()
{
}


#endif

template<typename Traits>
void TApp<Traits>::OnCanceled()
{
}

#ifndef _WIN32
	
template<typename Traits>
void TApp<Traits>::FlushSigMsgDerived()
{
	CCoreSigMsgMgr::Inst().FlushSigMsg();
}

#endif

