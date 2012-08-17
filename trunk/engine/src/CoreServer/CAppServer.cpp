#include "stdafx.h"
#include <ctime>
#include "CAppServer.h"
#include "IAppServerHandler.h"
#include "CSyncAppServer.h"
#include "CSynConnMgrServer.h"
#include "CTickMgr.h"
#include "IDatabase.h"
#include "IPipeModule.h"
#include "IPipeReactor.h"
#include "CAppConfigServer.h"
#include "CAsynTimeSystemServer.h"
#include "CSyncTimeSystemServer.h"
#include "CCoreSceneMgrServer.h"
#include "CConnMgrServer.h"
#include "CMultiMsgSender.h"
#include "CCoreServerThreadMgr.h"
#include "CCoreMsgMgr.h"
#include "CCoreThreadMsgAppEvent.h"
#include "CCoreMsgSignal.h"
#include "CCoreTrMirrorBufferMgr.h"
#include "BaseHelper.h"
#include "ErrLogHelper.h"
#include "TApp.inl"
#include "CMetaSceneMgrServer.h"
#include "CAsynServerTrMsgBufferSwapper.h"
#include "CCoreObjVariantSyncStreamJob.h"
#include "TAppWithConsole.inl"
#include "CDistortedTick.h"
#ifndef _WIN32
#include "CCoreSigMsgMgr.h"
#endif
#include <csignal>
#include "CThreadWatcher.h"
#include "CCoreConnServerJob.h"
#include "CCoreServerAppJob.h"
#include "CCoreServerJob.inl"
#include "CAsynServerProxyBank.h"
#include "CSyncVariantServer.h"
#include "TRootVariantPoolMgr.inl"

template class TRootVariantPoolMgr<CSyncVariantServer>;

template class TApp<CTraitsServer>;
template class TAppWithConsole<CTraitsServer>;

void CAppServer::Init()
{
	new CAppServer;
	CSyncAppServer::Init();
}

void CAppServer::Unit()
{
	CSyncAppServer::Unit();
	delete Inst();
}

#ifdef _WIN32
namespace sqr 
{
	static void OnErrLogCallBack(const char* szErr, CLogOwner* pOwner)
	{
		ShowErrorMsg(szErr, "Server Error", true);
	}
}
#endif

//--------------------------------------------------------------------------
// class CAppServer
//--------------------------------------------------------------------------
CAppServer::CAppServer()
: m_bDoingQuit(false)
, m_bCleanUp(false)
{
	InitAsynServerProxyBank();

	new CAsynServerTrMsgBufferSwapper;
	CCoreTrMirrorBufferMgr::Init();

	CCoreObjVariantSyncStreamJob::ms_CreateJobThreadID = GetCurTID();

#ifdef _WIN32
	if (CAppConfigServer::Inst()->TestingServerEnabled())
	{
		SetLogCallBackFunc(OnErrLogCallBack);
	}
#endif
	
	CCoreMsgMgr::Init();
	
	new CMetaSceneMgrServer;
	new CCoreSceneMgrServer;

	CreateDatabase();

	CThreadWatcher::Inst();

	TRootVariantPoolMgr<CSyncVariantServer>::Create();
}


CAppServer::~CAppServer()
{
	TRootVariantPoolMgr<CSyncVariantServer>::Destroy();

	ReleaseDatabase();

	delete CCoreSceneMgrServer::Inst();
	delete CMetaSceneMgrServer::Inst();	
	
	CCoreMsgMgr::Unit();
	
#ifdef _WIN32
	if (CAppConfigServer::Inst()->TestingServerEnabled())
		SetLogCallBackFunc(NULL);
#endif

	CCoreTrMirrorBufferMgr::Unit();
	delete CAsynServerTrMsgBufferSwapper::Inst();

	UnitAsynServerProxyBank();	
}

CMultiMsgSender* CAppServer::GetMultiMsgSender()const
{
	CMultiMsgSender* pMultiMsgSender = new CMultiMsgSender();
	return pMultiMsgSender;
}

bool CAppServer::DoingQuit()const
{
	return m_bDoingQuit;
}

bool CAppServer::Quit()
{
	if(m_bDoingQuit)
		return false;

	(new CCoreServerAppQuitJob())->Add();
	
	m_bDoingQuit = true;

	return true;
}

void CAppServer::CreatePipeReactor()
{
	m_pPipeReactor=m_pPipeModule->GetSyncIPipeReactor();
}

IDatabase* CAppServer::GetDatabase() const
{
	return m_pDatabase;
}

void CAppServer::Log(const char* szMsg)
{
	return CSyncAppServer::Inst()->Log(szMsg);
}

void CAppServer::PostThreadAppEvent(CAppThreadEvent* pEvent)
{
	CCoreThreadMsgAppEvent* pAppEvent = new CCoreThreadMsgAppEvent(pEvent);
	if(CCoreMsgMgr::Inst().PostThreadMsg(pAppEvent))
		ReactorCancelBlock();
}

void CAppServer::CreateDatabase()
{
	m_pDatabase = ::GetDatabase();
	m_pDatabase->BeginDbThread();
}

void CAppServer::ReleaseDatabase()
{
	m_pDatabase->EndDbThread();
	m_pDatabase->Release();
	m_pDatabase = NULL;
}

#ifndef _WIN32
void CAppServer::SigHandler(int signum)
{
	CCoreMsgMgr::Inst().PostThreadMsg(new CCoreMsgSignal( signum ));
	CAppServer::Inst()->ReactorCancelBlock();
}
#else
BOOL WINAPI CAppServer::HandlerRoutine(DWORD dwCtrlType)
{
	switch( dwCtrlType )
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
		CCoreMsgMgr::Inst().PostThreadMsg( new CCoreMsgSignal( SIGINT ) );
		break;
	case CTRL_CLOSE_EVENT:
	default:
		CCoreMsgMgr::Inst().PostThreadMsg( new CCoreMsgSignal( SIGTERM ) );
		break;
	}
	CAppServer::Inst()->ReactorCancelBlock();
	return TRUE;
}
#endif

void CAppServer::OnMainFrameEndedDerived()
{
	ParentApp_t::OnMainFrameEndedDerived();

	CCoreMsgMgr::Inst().FlushMsg();

	CAsynServerTrMsgBufferSwapper::Inst()->FlushAllLeftMsg();
}

void CAppServer::OnCanceled()
{
	//下列代码可以优化，可以通过在Cancel之前设置一些原子变量来设定，下列哪些函数需要调用
	//现在每次都调用所有的信息交换函数，要调用多次锁操作，其实是有浪费的

	CCoreMsgMgr::Inst().FlushThreadMsg();
	CAsynServerTrMsgBufferSwapper::Inst()->HandleAllRightMsg();
}

void CAppServer::OnBeginMainCycleDerived()
{
#ifdef _WIN32
	if(!SetConsoleCtrlHandler(HandlerRoutine,TRUE))
		GenErr("system call SetConsoleCtrlHandler failed.");	
	
	//if ( ImpAppConfig_t::Inst()->AutoMinimizeConsole() )
	//{
	//	HWND hConsoleWnd=::GetConsoleWindow();
	//	if( hConsoleWnd )
	//		SendMessage(hConsoleWnd,WM_SYSCOMMAND,SC_MINIMIZE,0);
	//}
#else
	CCoreSigMsgMgr::Inst().SetSigHandler(&SigHandler);
#endif
}

uint32 CAppServer::OnIdle( uint32 uTimeLeft )
{
	static const uint32 uMaxSleepTime=CAppConfigServer::Inst()->GetAppSleepTime();
	uint32 uSleepTime = uTimeLeft > uMaxSleepTime ? uMaxSleepTime : uTimeLeft;

	if( !sqr::Sleep( uSleepTime ) )
		return CAsynTimeSystemServer::Inst()->GetTimeError();

	int nTimeLeft = int32(uTimeLeft) - int32(uSleepTime);

	return uint32( nTimeLeft > 0 ? nTimeLeft : 0 );
}


void CAppServer::PrintRegisterTickNum(uint32 uLuaTickNum)
{
	size_t stAllTickNum = CAsynTimeSystemServer::Inst()->GetRegisterTickNum();
	size_t stCppTickNum = stAllTickNum - uLuaTickNum;

	(new CCorePrintRegisterTickNumJob(stCppTickNum, (size_t)uLuaTickNum))->Add();	
}

void CAppServer::PrintRegisterTickInfo()
{
	string* strInfo = new string;
	CAsynTimeSystemServer::Inst()->GetRegisterTickInfo(*strInfo);

	(new CCorePrintRegisterTickInfoJob(strInfo))->Add();
}

void CAppServer::CleanUp()
{
	(new CCoreServerAppCleanJob())->Add();
	
	for(;;)
	{
		CAsynServerTrMsgBufferSwapper::Inst()->FlushAllLeftMsg();

		m_pPipeReactor->GetEvent( uint32( -1 ) );

		CAsynServerTrMsgBufferSwapper::Inst()->HandleAllRightMsg();
		
		if(m_bCleanUp)
			return;
	}

}

void CAppServer::IntCleanUp()
{
	m_bCleanUp = true;
}

const char* CAppServer::GetThreadName()
{
	return "AsynAppThread";
}

