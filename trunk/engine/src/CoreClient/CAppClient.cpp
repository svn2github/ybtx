#include "stdafx.h"
#include "CScriptAppClient.h"
#include "CAppClient.h"
#include "TCoreApp.inl"
#include "CMainWnd.h"
#include "CRenderSystemClient.h"
#include "CCoreSceneMgrClient.h"
#include "CMetaSceneMgrClient.h"
#include "CTimeSystemClient.h"
#include "CoreCommon.h"
#include "CGraphic.h"
#include "CPkgFile.h"
#include "CoreClientTest.h"
#include "CIme.h"
#include "CConnMgrClient.h"
#include "CCoreSceneClient.h"
#include "CRenderSystemClient.h"
#include "IAppClientHandler.h"
#include "CAppConfigClient.h"
#include "CCamera.h"
#include "CSkyMgr.h"
//#include "CCameraTick.h"
#include "IAudioModule.h"
#include "CGuiHandler.h"
#include "SyncSystem.h"
#include "CRenderSlot.h"
#include "CXmlConfig.h"

#include <fstream>
#include "CGraphicExp.h"

#include "CErrNotifyIcon.h"
#include "PkgCommon.h"
#include "CodeCvs.h"
#include "CCoreMsg.h"
#include "CAppThreadEvent.h"
#include "CCoreThreadMsgAppEvent.h"
#include "TAppWithConsole.inl"
#include "CLanguageSetting.h"
#include "CDistortedTick.h"
#include "CTextureFontMgr.h"
#include "FlashDataManager.h"
#include "CDataScene.h"
#include "CCallStackInfo.h"
#include "CClientProxyBank.h"
#include "CSyncVariantClient.h"
#include "TRootVariantPoolMgr.inl"
#include "CSceneConfig.h"
#include <dbghelp.h>
#include "CDebugText.h"
#include "SQRYYCtrl.h"
#include "CSyncVariantClientHolderPool.h"

#pragma comment(lib, "dbghelp.lib")

template class TApp<CTraitsClient>;
template class TExpSafeApp<CTraitsClient>;
template class TCoreApp<CTraitsClient>;
template class TAppWithConsole<CTraitsClient>;

uint64	CAppClient::st_StaticCyc[5] = { 66666,50000,33333,22222,16666 }; 
uint64	CAppClient::st_StaticLevel[4] =	{ 50000,33333,22222,16666 };
uint64	CAppClient::st_StaticLevelUp[4] = { 35500,27500,18500,14500 };

void CAppClient::Init()
{
	new CAppClient;
}

void CAppClient::Unit()
{
	delete Inst();
	IntInst() = NULL;
}

//--------------------------------------------------------------------------
// class CAppClient
//--------------------------------------------------------------------------

CAppClient::CAppClient()
:m_pMainWnd(NULL),m_pRootWnd(NULL),m_bIsRendering(false)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	CCoreMsgMgr::Init();

	if (!SymInitialize(GetCurrentProcess(),NULL,TRUE))
	{
		SQR_THROW("dbghelp Init error");
	}

	//UnHandledExceptionFilter会导致debugbreak被捕获，导致在windows开发时无法立即启动调试器
	if( CatchErrorEnabled() )
		m_pPrevSEFilter=SetUnhandledExceptionFilter( &SEFilter );

	//CPkgFile::EnableAsyncRead( false );
	CPkgFile::ReadThreadInit();
	const uint32 uWidth		 = CSceneConfig::GetInst()->GetWindowWidth();
	const uint32 uHeight	 = CSceneConfig::GetInst()->GetWindowHeight();
	const wstring sTitle	 = CAppConfigClient::Inst()->GetWindowTitle();
	const wchar_t* szAppIcon = CAppConfigClient::Inst()->GetAppIconFileName();
	const string sGTPort	 = CAppConfigClient::Inst()->GetGTPortName();

	RECT rtWnd = { 0, 0, uWidth, uHeight };
	m_pMainWnd = new CMainWnd( this,hInstance, rtWnd.right, rtWnd.bottom,
		sTitle.c_str(),szAppIcon,szAppIcon );

	if (CAppConfigClient::Inst()->IsTestClient())
	{
		CErrNotifyIcon::Init(m_pMainWnd->GetHwnd(), hInstance);
		SetLogCallBackFunc(&OnErrLogCallBack);	
	}

	CIme::InitIme( m_pMainWnd->GetHwnd() ); //CRenderSystem消息传递依赖于CIME
	new CRenderSystemClient( m_pMainWnd->GetHwnd() );//得到主窗口句柄,传给渲染系统

	// 设置初始语言
	CLanguageSetting::Inst()->SetCurLangID(0);

	CFRect rtClient( 0, 0, 1024, 768 );//设置gui跟窗口的分辨率

	string ChopString("Layout");
	string CompletePath(CAppConfigClient::Inst()->GetGuiRes());
	string strGUIPath = CompletePath.substr(0, CompletePath.find(ChopString));

	string fxPath = CAppConfigClient::Inst()->GetGuiFxPath();
	m_pGuiFxRenderer = new CGuiHandler(IEffectManager::GetInst(), fxPath);

	m_pTGInst = new TGManager;
	m_pTGInst->InitTGManager(sGTPort);

	// 设置表情库
	CTextureFontMgr::GetInst()->Initialize(CAppConfigClient::Inst()->GetEmtRes(), CGraphic::GetInst());

	m_pRootWnd = new SQRRootWnd;
	bool bInitRes = !(CAppConfigClient::Inst()->IsTestClient());
	m_pRootWnd->CreateRootWnd( m_pGuiFxRenderer, rtClient, CGraphic::GetInst(), m_pMainWnd->GetHwnd()
		,IEffectManager::GetInst(),	CAppConfigClient::Inst()->GetGuiRes().c_str(), strGUIPath.c_str(), false, bInitRes );
	m_pRootWnd->LoadStaticText();
	m_pRootWnd->SetWndSize(0,0, (float)rtWnd.right, (float)rtWnd.bottom );
	CDebugText::Create();
	

	FlashDataManager::GetInst()->Initialize(CAppConfigClient::Inst()->GetEmtRes(), CGraphic::GetInst());

	::GetWindowRect( m_pMainWnd->GetHwnd(), (RECT*)&m_rtPreWnd );

	//设置所有等待函数的等待精度到最小
	TIMECAPS tc;
	if ( TIMERR_NOERROR != timeGetDevCaps(&tc,sizeof(tc)) )
		GenErr("timeGetDevCaps failed.");
	m_uTimerPeriodMin = tc.wPeriodMin;
	timeBeginPeriod( m_uTimerPeriodMin );

	SYSTEM_INFO si;
	GetSystemInfo( &si );
	m_uProcessorNum = si.dwNumberOfProcessors;

	m_uLimitCyc		= CAppConfigClient::Inst()->GetRenderCycInForground() * 1000;
	m_uLimitMaxBackCyc = CAppConfigClient::Inst()->GetRenderCycInForground()* 1000;
	//m_uWarningCyc	= m_uLimitCyc;
	//m_uLimitNum		= 0;
	m_u64LastTime	= 0;
	m_uLimitNum		= 0;
	m_bIsRendered	= false;
	m_uLimitFPS		= 100;
	m_ActiveIndex	= 4;
	m_uMaxNum = 0;
	m_uMinNum = 0;


	OnRestore();
	TRootVariantPoolMgr<CSyncVariantClient>::Create();
	CSyncVariantClientHolderPool::Create();
}

CAppClient::~CAppClient()
{
	CSyncVariantClientHolderPool::Destroy();
	TRootVariantPoolMgr<CSyncVariantClient>::Destroy();
	EnableRendering(false);

	timeEndPeriod( m_uTimerPeriodMin );

	CCoreSceneMgrClient::Inst()->IntDestroyMainScene();

	CDebugText::Destroy();
	m_pRootWnd->DestroyWnd();
	
	SafeDelete(m_pRootWnd);
	SafeDelete(m_pGuiFxRenderer);
	SafeDelete(m_pTGInst);

	CPkgFile::ReadThreadUnit();
	delete CRenderSystem::GetInst();

	if (CAppConfigClient::Inst()->IsTestClient())
	{
		SetLogCallBackFunc(NULL);
		CErrNotifyIcon::Unit();
	}

	delete m_pMainWnd;

	CIme::UnitIme();

	CCoreMsgMgr::Unit();

	if( m_pPrevSEFilter )
		SetUnhandledExceptionFilter( m_pPrevSEFilter );

	SymCleanup(GetCurrentProcess());

	UnitClientProxyBank();
}

void CAppClient::EnableRendering(bool bEnable)
{
	m_bIsRendering = bEnable;
	CRenderSystem::GetInst()->SetAudioMute(!bEnable);
	/*if(bEnable)
	ImpTimeSystem_t::Inst()->Register( m_pTickRender, CAppConfigClient::Inst()->GetRenderTickCyc(), true);
	else
	ImpTimeSystem_t::Inst()->UnRegister( m_pTickRender );*/
}

bool CAppClient::DebugConsoleOn()const
{
	return CAppConfigClient::Inst()->ShowConsole();
}

void CAppClient::SetMainWndTitle(const char* title)
{
	wstring utf16_str;
	utf8_to_utf16(utf16_str, title);
	SetWindowTextW(m_pMainWnd->GetHwnd(), utf16_str.c_str());
}


void CAppClient::Render()
{
	/*if( CTimeSystemClient::Inst()->GetTimeError() + CAppConfigClient::Inst()->GetSkipFrameWaitTime() < 0 )
	return;*/

	//uint64 uRendTime= GetProcessTime();
	if( m_bIsRendering )
		CRenderSystemClient::Inst()->Render();
	//CFrameRateAdviser::Inst()->TellFrameTime((uint32)(GetProcessTime() - uRendTime));
}
#define MIN_TIME 3000
uint32 CAppClient::OnIdle( uint32 uTimeError )
{	
	uint64 uTime= GetHDProcessTime();
	uint64 nUsedTime= uTime - m_u64LastTime;

	if(m_pMainWnd->IsActive())
	{
		m_uLimitCyc = CAppConfigClient::Inst()->GetRenderCycInForground()*1000; 
		m_uLimitFPS = 1000000/m_uLimitCyc;
		CRenderSystem::GetInst()->SetAudioMute(false);
	}
	else
	{
		m_uLimitCyc = CAppConfigClient::Inst()->GetRenderCycInBackground()*1000;
		m_uMaxNum = m_uLimitFPS;
		m_uLimitFPS = 1000000/m_uLimitCyc;
		CRenderSystem::GetInst()->SetAudioMute(true);
	}

	m_u64LastTime = uTime;
	m_uUseTime += nUsedTime;
	m_bIsRendered = false;

	//if( !m_pMainWnd->IsActive() || (m_pMainWnd->IsActive() && m_uUseTime >= m_uLimitCyc) )
	CRenderSlotOwner::Inst().Update(nUsedTime);
	if ( m_uUseTime < m_uLimitCyc )			
	{
		if(CSyncSystem::BeginSyncFun( max(MIN_TIME,(m_uLimitCyc - m_uUseTime )) ))
			return CTimeSystemClient::Inst()->GetTimeError();
	}
	else
	{
		CSyncSystem::BeginSyncFun( uint64(MIN_TIME) );
	}

	SQR_TRY
	{		
		//GenErr("test");
		//uint64 BeginTime = GetHDProcessTime();
		//m_bIsRendered	= true;

		CRenderSystem::GetInst()->UpdateAudioSystem();

		if( m_bIsRendering )
			((CRenderSystemClient*)CRenderSystem::GetInst())->Render();
		else
			((CRenderSystemClient*)CRenderSystem::GetInst())->Update();

		m_uUseTime	  = 0;//GetHDProcessTime() - BeginTime;
		//DebugText(0xffffff00, "ActiveIndex: %u,%u : UseTime: %u", m_ActiveIndex,m_uLimitCyc,m_uUseTime);
	}
	SQR_CATCH(exp)
	{
		GfkLogExpOnce(exp);
	}
	SQR_TRY_END;

	//return true告知基类的循环，这个函数进行了耗时操作，需要重新计算timeerror
	return CTimeSystemClient::Inst()->GetTimeError();
}

uint32 CAppClient::OnInputMessage(uint32 uMsg,uint32 wParam,uint32 lParam)
{

	SQR_TRY
	{
#ifdef WM_MOUSEWHEEL
		if ( uMsg == WM_MOUSEWHEEL )
		{
			int x = (int)LOWORD(lParam);
			int y = (int)HIWORD(lParam);
			int capHeight	= GetSystemMetrics(SM_CYCAPTION);
			int borderWidth	= GetSystemMetrics(SM_CYBORDER);

			RECT rt;
			HWND hMainWnd	= m_pMainWnd->GetHwnd();
			GetWindowRect( hMainWnd, &rt );

			if ( x >= rt.left && y >= rt.top + capHeight && x < rt.right && y < rt.bottom )
			{
				x -= rt.left + 4*borderWidth;	//这个地方*4的方法还需要考虑下
				y -= rt.top + capHeight + 4*borderWidth;
				lParam = MAKELONG(x, y);
			}
			else
				return 0;
		}
#endif
		return m_pRootWnd ? m_pRootWnd->RecieveMsg( uMsg, wParam, lParam ) : 0;

	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;

	return NULL;
}

void CAppClient::OnMinimize()
{
	EnableRendering(false);
}

void CAppClient::OnRestore()
{
	if ( !m_rtPreWnd.Width() || !m_rtPreWnd.Height() )
		return;

	CIRect rtClient, rtWnd;
	HWND hWnd=m_pMainWnd->GetHwnd();
	RECT realClientRect;
	GetClientRect(hWnd, &realClientRect);
	realClientRect.bottom	= max(realClientRect.bottom,realClientRect.top + 1);
	realClientRect.right	= max(realClientRect.right,realClientRect.left + 1);
	if(CYYManager::HasInst() && CYYManager::GetInst()->GetYYCtrl())
		CYYManager::GetInst()->GetYYCtrl()->DestroyYY();
	CMainWindowTarget::GetInst()->RestoreSize(false);
	CMainWindowTarget::GetInst()->GetRect(rtClient);

	m_rtPreWnd = rtClient;
	if (rtClient.Width() == 0 || rtClient.Height() == 0)
		return;
	m_pMainWnd->m_fWndRatio = float(rtClient.Height()) / float(rtClient.Width());
	m_pRootWnd->SetWndSize( (float)rtClient.left, (float)rtClient.top, (float)rtClient.Width(), (float)rtClient.Height() );

	int bufferWidth = rtClient.Width();
	int bufferHeight = rtClient.Height();
	if (rtClient.left > 0)
		bufferWidth += rtClient.left * 2;
	if (rtClient.top > 0)
		bufferHeight += rtClient.top * 2;
	CVector2f definition;
	definition.x = float(bufferWidth) / float(realClientRect.right);
	definition.y = float(bufferHeight) / float(realClientRect.bottom);
	m_pRootWnd->SetDefinition(definition);

	EnableRendering(true);


	if ( CClientRenderScene::HasInst() )
	{
		CRenderScene* pRenderScene=CClientRenderScene::GetInst();
		pRenderScene->SetCameraOffset( pRenderScene->GetCameraOffset() );

		pRenderScene->GetDataScene()->GetSkyMgr()->ResetSkyBoxWnd();
		CRenderSystemClient::Inst()->SetCameraInfo((float)rtClient.Width(), (float)rtClient.Height());
	}

	if (CMainWindowTarget::GetInst()->GetBeProtected() == false)
		CMainWindowTarget::GetInst()->ReSaveSize((uint)(rtClient.Width()), (uint)(rtClient.Height()));
}


void CAppClient::OnClose()
{
	if ( ! GetHandler() )
		return;

	GetHandler()->OnCloseMainWnd();
}

LONG WINAPI CAppClient::SEFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	CCallStackInfo info(pExceptionInfo->ContextRecord);

	const char* szThreadName = sqr::GetThreadName();

	ostringstream strm;
	if (szThreadName)
	{
		strm << "线程" << szThreadName << "发生未捕获异常";
	}
	else
	{
		PCONTEXT pContext = pExceptionInfo->ContextRecord;
		IMAGEHLP_MODULE pModule;
		memset(&pModule,0,sizeof(IMAGEHLP_MODULE));
		pModule.SizeOfStruct = sizeof(pModule);
		if (SymGetModuleInfo(GetCurrentProcess(),pContext->Eip,&pModule))
		{
			strm << "Module: "<<pModule.ModuleName <<" 发生未捕获异常";
		}
		else
		{
			strm << "程序发生未捕获异常";
		}
	}

	CError error(strm.str().c_str(), info.GetText());

	LogDumpExp(error);

	CreateCoreDump(pExceptionInfo);

	//CXmlConfig xmlErrMsg;

	//xmlErrMsg.LoadConfig(

	wchar_t* wzError=L"发生未知的错误，这个错误很有可能是显卡和声卡等硬件的驱动程序的缺陷所致，请尝试更新您的电脑上的硬件设备的驱动程序来解决问题。";

	MessageBoxW(NULL,wzError, L"提示信息：",MB_OK|MB_ICONERROR);

	TerminateProcess(GetCurrentProcess(), 1);

	return EXCEPTION_EXECUTE_HANDLER;
}

void CAppClient::Quit()
{
	IntQuit();
}

void CAppClient::OnBeginMainCycleDerived()
{
	InitClientProxyBank();

	if(!CAppConfigClient::Inst()->ShowConsole())
		return;

	if(!SetConsoleCtrlHandler(HandlerRoutine,TRUE))
		GenErr("system call SetConsoleCtrlHandler failed.");
}

void CAppClient::OnMainFrameEndedDerived()
{
	ParentApp_t::OnMainFrameEndedDerived();
	CCoreMsgMgr::Inst().FlushMsg();
}

namespace sqr
{
	class CClientBreakSignal : public CCoreMsg
	{
	private:
		~CClientBreakSignal()
		{
			IAppClientHandler* pHandler = CAppClient::Inst()->GetHandler();
			if(pHandler)
				pHandler->OnBreak();
		}
	};
}

BOOL WINAPI CAppClient::HandlerRoutine(DWORD dwCtrlType)
{
	switch( dwCtrlType )
	{
	case CTRL_C_EVENT:
		CCoreMsgMgr::Inst().PostMsg(new (CCoreMsgMgr::Inst())
			CClientBreakSignal);
		break;
	default:
		break;
	}
	return TRUE;
}


void CAppClient::OnErrLogCallBack(const char* szError, CLogOwner* )
{
	CAppClient::Inst()->TellErrMessage(szError, "Client Error");
}

void CAppClient::ShowNotifyErrIcon()
{
	if(!CErrNotifyIcon::Inst())
		return;

	char szTime[64];
	time_t T;
	struct tm * timenow;
	time ( &T );
	timenow = localtime ( &T ); 

	sprintf(szTime, "上次错误发生时间: %02d:%02d:%02d", timenow->tm_hour,
		timenow->tm_min, timenow->tm_sec);

	CErrNotifyIcon::Inst()->ShowErrIcon(szTime);
}

void CAppClient::TellErrMessage(const char* szErr, const char* szType)
{
	bool bTestingClient = CAppConfigClient::Inst()->IsTestClient();
	if (!bTestingClient)
		return;

	ShowNotifyErrIcon();

	bool bErrMsgShow = CAppConfigClient::Inst()->BeErrMsgShow();

	if(!bErrMsgShow)
		return;

	ShowErrorMsg(szErr, szType, true);
}


void CAppClient::PostThreadAppEvent(CAppThreadEvent* pEvent)
{
	bool bPosted = CCoreMsgMgr::Inst().PostThreadMsg(new CCoreThreadMsgAppEvent(pEvent));

	if(bPosted)
		ReactorCancelBlock();
}

void CAppClient::OnCanceled()
{
	CCoreMsgMgr::Inst().FlushThreadMsg();
}
