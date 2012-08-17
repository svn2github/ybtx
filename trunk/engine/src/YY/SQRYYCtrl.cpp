#include "stdafx.h"
#include "SQRYYCtrl.h"
#include "SQRRootWnd.h"
#include "CGraphic.h"
#include "CDx9YYAdapter.h"
#include "CWindowTarget.h"

#define YY_LOGIN_WND_WIDTH	285
#define YY_LOGIN_WND_HEIGHT	175
#define CALL_BACK_MSG_YYLOGIN  3

int YYPIPCallBack( int nType, WPARAM wParam )
{
	SQRYYCtrl*	tempYY = CYYManager::GetInst()->GetYYCtrl();
	if (tempYY)
	{
		if ( nType == CALL_BACK_MSG_YYLOGIN )
			return tempYY->CallBackChar(nType, (char *)wParam);
		else
			return tempYY->CallBackInt(nType, (uint32)wParam);
	}
	return 0;
}

CYYManager::CYYManager(void)
{
	m_hFactory = NULL;
	m_pfnGetInterface = NULL;

	m_pfnFreeGame		= NULL;
	m_pfnRunService		= NULL;
	m_pfnLoadInGame		= NULL;
	m_pfnCheckClient	= NULL;
	m_pfnIsPipSuccess	= NULL;
	m_pfnMouseInput		= NULL;
	m_pfnSetMainWnd		= NULL;
	m_pfnCreateUI		= NULL;
	m_pfnDestoryUI		= NULL;
	m_pfnRenderGUI		= NULL;
	m_pfnGameWndMsg		= NULL;
	m_pfnSetUserName	= NULL;
	m_pfnJoinTeam		= NULL;
	m_pfnSetTeamDevice	= NULL;
	m_pfnSetTeamVoice	= NULL;
	m_pfnLockTeamVoice	= NULL;
	m_pfnGetPipShow	 = NULL;
	m_pfnSetPipShow	 = NULL;
	m_pfnSetMsgShow	 = NULL;
	m_pfnMouseShow	 = NULL;
	m_pfnLockWnd	 = NULL;
	m_pfnMoveWnd	 = NULL;
	m_pfnShowOpid	 = NULL;
	m_pfnLoginSucceed	 = NULL;
	m_pfnLoginFailure	 = NULL;
	m_pfnSetLoginKey	 = NULL;
	m_pfnSetCallBack	 = NULL;

	m_pYYCtrl		=NULL;
	InitYY();
}

CYYManager::~CYYManager(void)
{
	if (m_pfnDestoryUI)
		m_pfnDestoryUI();
}

BOOL CYYManager::InitFactory()
{
	WCHAR wzSelfPath[MAX_PATH]; 
	memset( wzSelfPath, 0, sizeof(wzSelfPath) );
	GetModuleFileNameW( NULL, wzSelfPath, sizeof(wzSelfPath) );
	LPWSTR lpInsertPos = wcsrchr( wzSelfPath, L'\\' );
	*lpInsertPos = L'\0';
	lstrcatW( wzSelfPath, L"\\pipFactory.dll" );
	m_hFactory = LoadLibraryW( wzSelfPath );
	if( m_hFactory == NULL ) return FALSE;

	m_pfnInitInterface = (PFN_INITYYINTERFACE)GetProcAddress( m_hFactory, "YYPIP_InitInterface" );
	if( m_pfnInitInterface == NULL ) return FALSE;

	m_pfnGetInterface = (PFN_GETYYINTERFACE)GetProcAddress( m_hFactory, "YYPIP_GetInterface" );
	if( m_pfnGetInterface == NULL ) return FALSE;

	//call factory init func
	if( m_pfnInitInterface() == -1 ) return FALSE;

	/////////////////////////////////////////////////////////////////////////
	//all game must be call this func to show yy window
	m_pfnRunService = (PFN_RUNSERVICE)m_pfnGetInterface( "YYPIP_RunService" );
	if( m_pfnRunService == NULL ) return FALSE;

	m_pfnLoadInGame = (PFN_LOADINGAME)m_pfnGetInterface( "YYPIP_LoadInGame" );
	if( m_pfnLoadInGame == NULL ) return FALSE;

	m_pfnFreeGame = (PFN_FREEINGAME)m_pfnGetInterface( "YYPIP_FreeInGame" );
	if( m_pfnFreeGame == NULL ) return FALSE;

	m_pfnCheckClient = (PFN_CHECKCLIENT)m_pfnGetInterface( "YYPIP_CheckYYClient" );
	if( m_pfnCheckClient == NULL ) return FALSE;

	m_pfnIsPipSuccess = (PFN_ISPIPSUCCESS)m_pfnGetInterface( "YYPIP_IsPipRunSuccess" );
	if( m_pfnIsPipSuccess == NULL ) return FALSE;

	/////////////////////////////////////////////////////////////////////////
	//sometimes call this func to show yy window
	m_pfnMouseInput = (PFN_MOUSEINPUT)m_pfnGetInterface( "YYPIP_MouseInput" );
	if( m_pfnMouseInput == NULL ) return FALSE;

	m_pfnSetMainWnd = (PFN_SETMAINWND)m_pfnGetInterface( "YYPIP_SetMainWnd" );
	if( m_pfnSetMainWnd == NULL ) return FALSE;

	m_pfnCreateUI = (PFN_CREATEUI)m_pfnGetInterface( "YYPIP_CreateUI" );
	if( m_pfnCreateUI == NULL ) return FALSE;

	m_pfnDestoryUI = (PFN_DESTORYUI)m_pfnGetInterface( "YYPIP_DestoryUI" );
	if( m_pfnDestoryUI == NULL ) return FALSE;

	m_pfnRenderGUI = (PFN_RENDERGUI)m_pfnGetInterface( "YYPIP_RenderGUI" );
	if( m_pfnRenderGUI == NULL ) return FALSE;

	m_pfnGameWndMsg = (PFN_GAMEWNDMSG)m_pfnGetInterface( "YYPIP_GameWndMessage" );
	if( m_pfnGameWndMsg == NULL ) return FALSE;


	/////////////////////////////////////////////////////////////////////////
	//game used yy voice channel
	m_pfnJoinChannel = (PFN_JOINCHANNEL)m_pfnGetInterface( "YYPIP_JoinChannel" );
	if( m_pfnJoinChannel == NULL ) return FALSE;

	m_pfnSetTeamAdmin = (PFN_SETTEAMADMIN)m_pfnGetInterface( "YYPIP_SetTeamAdmin" );
	if( m_pfnSetTeamAdmin == NULL ) return FALSE;

	m_pfnSetUserName = (PFN_SETUSERNAME)m_pfnGetInterface( "YYPIP_SetUserName" );
	if( m_pfnSetUserName == NULL ) return FALSE;

	m_pfnJoinTeam = (PFN_JOINTEAM)m_pfnGetInterface( "YYPIP_JoinTeam" );
	if( m_pfnJoinTeam == NULL ) return FALSE;

	m_pfnSetTeamDevice = (PFN_SETTEAMDEVICE)m_pfnGetInterface( "YYPIP_SetTeamDevice" );
	if( m_pfnSetTeamDevice == NULL ) return FALSE;

	m_pfnSetTeamVoice = (PFN_SETTEAMVOICE)m_pfnGetInterface( "YYPIP_SetTeamVoice" );
	if( m_pfnSetTeamVoice == NULL ) return FALSE;

	m_pfnLockTeamVoice = (PFN_LOCKTEAMVOICE)m_pfnGetInterface( "YYPIP_LockTeamVoice" );
	if( m_pfnLockTeamVoice == NULL ) return FALSE;


	/////////////////////////////////////////////////////////////////////////
	//game to channel yy voice window
	m_pfnGetPipShow = (PFN_GETPIPSHOW)m_pfnGetInterface( "YYPIP_GetPipShow" );
	if( m_pfnGetPipShow == NULL ) return FALSE;

	m_pfnSetPipShow = (PFN_SETPIPSHOW)m_pfnGetInterface( "YYPIP_SetPipShow" );
	if( m_pfnSetPipShow == NULL ) return FALSE;

	m_pfnSetMsgShow = (PFN_SETMSGSHOW)m_pfnGetInterface( "YYPIP_SetMsgShow" );
	if( m_pfnSetMsgShow == NULL ) return FALSE;

	m_pfnMouseShow = (PFN_SETMOUSESHOW)m_pfnGetInterface( "YYPIP_SetMouseShow" );
	if( m_pfnMouseShow == NULL ) return FALSE;

	m_pfnLockWnd = (PFN_LOCKWINDOW)m_pfnGetInterface( "YYPIP_LockWindow" );
	if( m_pfnLockWnd == NULL ) return FALSE;

	m_pfnMoveWnd = (PFN_MOVEWINDOW)m_pfnGetInterface( "YYPIP_MoveWindow" );
	if( m_pfnMoveWnd == NULL ) return FALSE;


	/////////////////////////////////////////////////////////////////////////
	//game to used open id func 
	m_pfnShowOpid = (PFN_SHOWOPENID)m_pfnGetInterface( "YYPIP_ShowOpenId" );
	if( m_pfnShowOpid == NULL ) return FALSE;

	m_pfnLoginSucceed = (PFN_LOGINSUCCEED)m_pfnGetInterface( "YYPIP_LoginSucceed" );
	if( m_pfnLoginSucceed == NULL ) return FALSE;

	m_pfnLoginFailure = (PFN_LOGINFAILURE)m_pfnGetInterface( "YYPIP_LoginFailure" );
	if( m_pfnLoginFailure == NULL ) return FALSE;

	m_pfnSetLoginKey = (PFN_SETLOGINKEY)m_pfnGetInterface( "YYPIP_SetLoginKey" );
	if( m_pfnSetLoginKey == NULL ) return FALSE;

	m_pfnSetCallBack = (PFN_SETCALLBACK)m_pfnGetInterface( "YYPIP_SetCallBackFunc" );
	if( m_pfnSetCallBack == NULL ) return FALSE;

	return TRUE;
}

void  CYYManager::InitYY()
{
	if( !CYYManager::GetInst()->InitFactory())
		return;

	string YYInitParam = "LG_YBTX|"; //暂时还没有放在配制表里
	void*  YYUIParam = NULL;
	switch(CGraphic::GetInst()->GetCreateType())
	{
	case eRCT_DX9:
		YYInitParam += "YY3D9|";
		YYUIParam = GetDx9UIParam();
		break;
	case eRCT_DX10:
		YYInitParam += "YY3D10|";
		break;
	case eRCT_DX11:
		YYInitParam += "YY3D11|";
		break;
	case eRCT_OPENGL:
		YYInitParam += "YYOPENGL|";
		break;
	case eRCT_OTHER:
		YYInitParam += "YY3D9|";
		break;
	}
	YYInitParam += "NONE";
	CYYManager::GetInst()->m_pfnLoadInGame(YYInitParam.c_str());//这个参数需要和YY技术人员重新取得
	
	WCHAR wzYYFilePath[MAX_PATH]; 
	memset( wzYYFilePath, 0, sizeof(wzYYFilePath) );
	GetModuleFileNameW( NULL, wzYYFilePath, sizeof(wzYYFilePath) );
	LPWSTR lpInsertPos = wcsrchr( wzYYFilePath, L'\\' );
	*lpInsertPos = L'\0';
	lstrcatW( wzYYFilePath, L"\\yy.zip" );
	CYYManager::GetInst()->m_pfnCheckClient( utf16_to_utf8(wzYYFilePath).c_str(), NULL );

	CYYManager::GetInst()->m_pfnSetMainWnd( CMainWindowTarget::GetInst()->GetHwnd() );
	if(YYUIParam)
		CYYManager::GetInst()->m_pfnCreateUI( YYUIParam );

	CYYManager::GetInst()->m_pfnSetCallBack( YYPIPCallBack );
}

void  CYYManager::SetYYCtrl(SQRYYCtrl* yyCtrl)
{
	m_pYYCtrl = yyCtrl;
}

SQRYYCtrl*   CYYManager::GetYYCtrl()
{
	return m_pYYCtrl;
}

LRESULT CYYManager::FilterAndProcYYMsg( uint32 message, WPARAM wParam, LPARAM lParam )
{
	if( m_pfnGameWndMsg )
	{  
		if( m_pfnGameWndMsg( message, wParam,lParam ) == 0 ) return 0;
	}
	return 1;
}

void  CYYManager::CreateYYMgr()
{
	if (!CYYManager::HasInst())
		CYYManager::Create();
}
void  CYYManager::DestroyYYMgr()
{
	if (CYYManager::HasInst())
		CYYManager::Destroy();
}
//----------------------------------------
bool SQRYYCtrl::m_bFirstFrame = true;
DYNAMIC_CREATE_WND( SQRYYCtrl, SQRControl )
SQRYYCtrl::SQRYYCtrl(void)
: m_eLocationType(eLLeftTop)
{
	if (CYYManager::HasInst())
		CYYManager::GetInst()->SetYYCtrl(this);
}

SQRYYCtrl::~SQRYYCtrl(void)
{
	SQRYYCtrl::DestroyWnd();
}

int32 SQRYYCtrl::DestroyWnd()
{
	m_bFirstFrame = true;
	if (CYYManager::HasInst())
	{
		if (CYYManager::GetInst()->GetYYCtrl() == this)
			CYYManager::GetInst()->SetYYCtrl(NULL);
	}
	return SQRControl::DestroyWnd();
}

void	SQRYYCtrl::DrawWndBackground()
{
	SQR_TRY	
	{
		FlushDraw();
		CGraphic::GetInst()->DrawStack();
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("SQRYYCtrl::DrawWndBackground");
		GfkLogExp(exp);	
		return;
	}
	SQR_TRY_END;
	
	if(CYYManager::HasInst() && CYYManager::GetInst()->m_pfnRenderGUI)
	{
		if (m_bFirstFrame)
		{
			m_bFirstFrame = false;
			_RefreshYY();
		}
		CYYManager::GetInst()->m_pfnRenderGUI();
	}
}

int	SQRYYCtrl::CallBackChar( int nType,char* wParam )
{
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_2_RET(int,nType, wParam) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	return 0;
}

int	SQRYYCtrl::CallBackInt( int nType,uint32 wParam )
{
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_2_RET(int,nType, wParam) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	return 0;
}

void	SQRYYCtrl::SetLocationStyle(uint32 sty)
{
	if (sty > (uint32)eLCount)
		m_eLocationType = eLLeftTop;
	else
		m_eLocationType = (LocationStyle)sty;
}

void	SQRYYCtrl::_ResetWndPos(float x, float y)
{
	SQRControl::_ResetWndPos(x,y);
	_RefreshYY();
}

void	SQRYYCtrl::_ResizeRect( const CFRect& rt ,bool refreshbak )
{
	SQRControl::_ResizeRect(rt,refreshbak);
	_RefreshYY();
}

void	SQRYYCtrl::_SetWndRect( const CFRect& rt )
{
	SQRControl::_SetWndRect(rt);
	_RefreshYY();
}

void	SQRYYCtrl::OnScreenPosChanged()
{
	SQRWnd::OnScreenPosChanged();
	_RefreshYY();
}

void	SQRYYCtrl::ShowWnd( bool bShow )
{
	SQRControl::ShowWnd(bShow);
	_RefreshYY();
}

void SQRYYCtrl::_RefreshYY()
{
	WndHandle* pWndH = GetWndHandle();
	if (!IsCreated() || !CYYManager::HasInst())
		return;

	if(CYYManager::GetInst()->m_pfnShowOpid)
	{
		if (GetRootWnd() && GetRootWnd()->GetWndHandle() )
		{
			float rootx = GetRootWnd()->GetWndHandle()->m_xScreen;
			float rooty = GetRootWnd()->GetWndHandle()->m_yScreen;
			
			int32 posx = 0;
			int32 posy = 0;
			switch(m_eLocationType)
			{
			case eLCenter:
				posx = (int32)(GetWndLeft() + (GetWndWidth() - YY_LOGIN_WND_WIDTH) * 0.5f + rootx);
				posy = (int32)(GetWndTop() + (GetWndHeight() - YY_LOGIN_WND_HEIGHT) * 0.5f + rooty);
				break;
			case eLWCenterTop:
				posx = (int32)(GetWndLeft() + (GetWndWidth() - YY_LOGIN_WND_WIDTH) * 0.5f + rootx);
				posy = (int32)(GetWndTop() + rooty);
				break;
			case eLLeftTop:
			default:
				posx = (int32)(GetWndLeft() + rootx);
				posy = (int32)(GetWndTop() + rooty);
			}
			CYYManager::GetInst()->m_pfnShowOpid( IsShow() ? 1 : 0, posx, posy );
		}
		else
			CYYManager::GetInst()->m_pfnShowOpid( IsShow() ? 1 : 0, 0, 0 );

	}
}

void SQRYYCtrl::DestroyYY( void )
{
	if(CYYManager::GetInst()->m_pfnDestoryUI)
		CYYManager::GetInst()->m_pfnDestoryUI();
}

int		SQRYYCtrl::LoginSucceed()
{
	if(CYYManager::GetInst()->m_pfnLoginSucceed)
		return CYYManager::GetInst()->m_pfnLoginSucceed();
	return -1;
}

int		SQRYYCtrl::LoginFailure( const char* msg )
{
	if(CYYManager::GetInst()->m_pfnLoginFailure)
		return CYYManager::GetInst()->m_pfnLoginFailure(msg);
	return -1;
}

int		SQRYYCtrl::SetLoginKEY( const char* msg )
{
	if(CYYManager::GetInst()->m_pfnSetLoginKey)
		return CYYManager::GetInst()->m_pfnSetLoginKey(msg);
	return -1;
}
