#include "stdafx.h"

#include "IAppServerHandler.h"
#include "CSyncAppServer.h"
#include "CSyncTimeSystemServer.h"
#include "CSyncCoreSceneMgrServer.h"
#include "CSyncMetaSceneMgrServer.h"
#include "ThreadHelper.h"
#include "CConnMgrServer.h"
#include "CAppConfigServer.h"
#include "ErrLogHelper.h"
#include "CSynMultiMsgSender.h"
#include "IDatabase.h"
#include "CLog.h"
#include "CCoreTrMirrorBufferMgr.h"
#include "CCoreServerThreadMgr.h"
#include "CSynConnMgrServer.h"
#include "CSyncCoreMsgMgr.h"
#include "CCoreServerAppResult.h"
#include "CCoreServerResult.inl"
#include "CSyncServerTrMsgBufferMgrSwapper.h"
#include "StringHelper.h"
#include "CDistortedTick.h"
#include "CInstanceConfigServer.h"
#include "CCacheVariantServer.h"
#include "CSqrAllocPoolMgr.h"
#include "TRootVariantPoolMgr.inl"

template class TRootVariantPoolMgr<CCacheVariantServer>;

#ifndef _WIN32
#include "CCoreSigMsgMgr.h"
#endif

#include "TCoreApp.inl"

template class TApp<CSyncTraitsServer>;
template class TExpSafeApp<CSyncTraitsServer>;
template class TCoreApp<CSyncTraitsServer>;

void CSyncAppServer::Init()
{
	CSyncCoreMsgMgr::Init();

	new CSyncServerTrMsgBufferMgrSwapper;

	new CSyncAppServer();
}

void CSyncAppServer::Unit()
{
	delete Inst();
	IntInst() = NULL;

	delete CSyncServerTrMsgBufferMgrSwapper::Inst();

	CSyncCoreMsgMgr::Unit();
}

uint32& CSyncAppServer::GetCounter()
{
	static uint32 ms_uCounter = 0;
	return ms_uCounter;
}

//--------------------------------------------------------------------------
// class CSyncAppServer
//--------------------------------------------------------------------------
CSyncAppServer::CSyncAppServer()
: m_bCleanUp(false)
{
	CreateAppLog();

	CCoreServerThreadMgr::Init();

	TRootVariantPoolMgr<CCacheVariantServer>::Create();
}


CSyncAppServer::~CSyncAppServer()
{
	TRootVariantPoolMgr<CCacheVariantServer>::Destroy();

	GetConnMgr()->EndService();

	CCoreServerThreadMgr::Unit();

	ReleaseAppLog();
}


CSynMultiMsgSender* CSyncAppServer::GetMultiMsgSender(uint32 uID)const
{
	return new CSynMultiMsgSender(uID);
}

void CSyncAppServer::OnBeginMainCycleDerived()
{
}

void CSyncAppServer::OnMainFrameEndedDerived()
{
	CSyncAppServer::GetCounter() = 0;
	ParentApp_t::OnMainFrameEndedDerived();

	CSyncCoreMsgMgr::Inst().FlushMsg();

	CCoreServerThreadMgr::Inst()->FlushRightMsg();

	CSyncServerTrMsgBufferMgrSwapper::Inst()->FlushAllLeftMsg();
}

bool CSyncAppServer::OnCanceled()
{
	//下列代码可以优化，可以通过在Cancel之前设置一些原子变量来设定，下列哪些函数需要调用
	//现在每次都调用所有的信息交换函数，要调用多次锁操作，其实是有浪费的
	CSyncCoreMsgMgr::Inst().FlushThreadMsg();
	CCoreServerThreadMgr::Inst()->HandleAllLeftMsg();
	CSyncServerTrMsgBufferMgrSwapper::Inst()->HandleAllRightMsg();
	return true;
}


#ifdef _WIN32
void CSyncAppServer::OnSysMsg()
{
	MSG msg;
	while( PeekMessage(&msg,NULL,0,0,1) )
	{
		if( msg.message != WM_NULL )
			GenErr("App Engine thread receive unknown windows message");
	}
}
#endif

bool CSyncAppServer::DoingQuit()const
{
	return CSyncCoreSceneMgrServer::Inst()->Halting();
}

bool CSyncAppServer::Quit()
{
	if( DoingQuit() )
		return false;

	CSyncCoreSceneMgrServer::Inst()->Halt();

	return true;
}


void CSyncAppServer::OnSceneMgrHalted()
{
	CSyncServerTrMsgBufferMgrSwapper::Inst()->NotifyRightToQuit();

	IntQuit();
}

namespace sqr
{
	class CLogMemTick
		:public CTick
	{
	public:
		void OnTick()
		{
			CSyncAppServer::Inst()->LogMem();
		}
	};
}

void CSyncAppServer::CreateAppLog()
{
	string sAppLog = format("%s_%d.log", CInstanceConfigServer::Inst()->GetInstanceName(), GetCurPID());
	m_pAppLog = CLog::CreateLog(sAppLog.c_str());

	m_pAppLog->EnableTime(true);

	string sMemLog = format("Mem_%d.log",GetCurPID());
	m_pMemLog = CLog::CreateLog(sMemLog.c_str());
	m_pMemLog->EnableTime(true);

	m_pMemTick=new CLogMemTick;
	RegisterTick( m_pMemTick,CAppConfigServer::Inst()->GetLogMemInterval()*1000 );
}

void CSyncAppServer::ReleaseAppLog()
{
	UnRegisterTick( m_pMemTick );
	delete m_pMemTick;
	m_pMemTick=NULL;

	m_pMemLog->Release();
	m_pMemLog=NULL;

	m_pAppLog->Release();
	m_pAppLog=NULL;
}

void CSyncAppServer::LogMem()
{
	ostringstream strm;
	CSqrAllocPoolMgr::Inst().GetAllMemInfo(strm);
	m_pMemLog->Write( strm.str().c_str() );
}

void CSyncAppServer::Log(const char* sMsg)
{
	printf("%s\n",sMsg);
	m_pAppLog->Write(sMsg);
}


#ifndef _WIN32
void CSyncAppServer::FlushSigMsgDerived()
{	
}
#endif

uint32 CSyncAppServer::OnIdle( uint32 uTimeLeft )
{
	static const uint32 uMaxSleepTime = CAppConfigServer::Inst()->GetAppSleepTime();

	uint32 uSleepTime = uTimeLeft > uMaxSleepTime ? uMaxSleepTime : uTimeLeft;
	if( !sqr::Sleep( uSleepTime ) )
		return CSyncTimeSystemServer::Inst()->GetTimeError();

	int nTimeLeft = int32(uTimeLeft) - int32(uSleepTime);

	//return true告知基类的循环，这个函数进行了耗时操作，需要重新计算timeerror
	return uint32( nTimeLeft > 0 ? nTimeLeft : 0 );
}


void CSyncAppServer::OnMainCycleEndedDerived()
{
	(new CCoreServerAppQuitResult())->Add();
}

void CSyncAppServer::CleanUp()
{
	for(;;)
	{
		bool bSuc = CCoreServerThreadMgr::Inst()->FlushRightMsg();
		if(m_bCleanUp && bSuc)
			return;

		m_pPipeReactor->GetEvent( uint32( -1 ) );

		CCoreServerThreadMgr::Inst()->HandleAllLeftMsg();
	}
}

void CSyncAppServer::IntCleanUp()
{
	m_bCleanUp = true;
}


const char* CSyncAppServer::GetThreadName()
{
	return "SyncAppThread";
}
