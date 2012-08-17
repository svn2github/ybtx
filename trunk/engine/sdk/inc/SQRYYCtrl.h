#pragma once
#include "SQRControl.h"

namespace sqr
{
	class SQRYYCtrl; 

	typedef int (*PFN_INITYYINTERFACE)();	                //初始化接口
	typedef FARPROC (*PFN_GETYYINTERFACE)( const char * );  //获得接口函数

	typedef int (*PFN_FREEINGAME)();
	typedef int (*PFN_ISPIPSUCCESS)();
	typedef int (*PFN_RUNSERVICE)( const char* );
	typedef int (*PFN_LOADINGAME)( const char* );
	typedef int (*PFN_CHECKCLIENT)( const char*, void* );

	typedef int (*PFN_MOUSEINPUT)( UINT );
	typedef int (*PFN_SETMAINWND)( HWND );
	typedef int (*PFN_CREATEUI)( void* );
	typedef int (*PFN_DESTORYUI)();
	typedef int (*PFN_RENDERGUI)();
	typedef int (*PFN_GAMEWNDMSG)( UINT, WPARAM, LPARAM );


	typedef int (*PFN_JOINCHANNEL)( int );
	typedef int (*PFN_SETTEAMADMIN)( int );
	typedef int (*PFN_SETUSERNAME)( const char* );
	typedef int (*PFN_JOINTEAM)( const char*, int );
	typedef int (*PFN_SETTEAMDEVICE)( int, const char* );
	typedef int (*PFN_SETTEAMVOICE)( int, int );
	typedef int (*PFN_LOCKTEAMVOICE)( int , int );


	typedef int (*PFN_GETPIPSHOW)();
	typedef int (*PFN_SETPIPSHOW)( int );
	typedef int (*PFN_SETMSGSHOW)( int );
	typedef int (*PFN_SETMOUSESHOW)( int );
	typedef int (*PFN_LOCKWINDOW)( int );
	typedef int (*PFN_MOVEWINDOW)( int, int );


	typedef int (*PFN_LOGINSUCCEED)();
	typedef int (*PFN_LOGINFAILURE)( const char* );
	typedef int (*PFN_SETLOGINKEY)( const char* );
	typedef int (*PFN_SHOWOPENID)( int, int, int );
	typedef int (*PFN_SETCALLBACK)( void* );

	class CYYManager 
		: public virtual CDynamicObject
		, public Singleton<CYYManager>
	{
	public:
		HMODULE m_hFactory;
		PFN_GETYYINTERFACE  m_pfnGetInterface;
		PFN_INITYYINTERFACE m_pfnInitInterface;

	public:
		CYYManager(void);
		~CYYManager(void);

		BOOL InitFactory();
		/////////////////////////////////////////////////////////////////////////
		//all game must be call this func to show yy window
		PFN_FREEINGAME   m_pfnFreeGame;
		PFN_RUNSERVICE   m_pfnRunService;
		PFN_LOADINGAME   m_pfnLoadInGame;
		PFN_CHECKCLIENT  m_pfnCheckClient;
		PFN_ISPIPSUCCESS m_pfnIsPipSuccess;

		/////////////////////////////////////////////////////////////////////////
		//sometimes call this func to show yy window
		PFN_MOUSEINPUT m_pfnMouseInput;
		PFN_SETMAINWND m_pfnSetMainWnd;
		PFN_CREATEUI   m_pfnCreateUI;
		PFN_DESTORYUI  m_pfnDestoryUI;
		PFN_RENDERGUI  m_pfnRenderGUI;
		PFN_GAMEWNDMSG m_pfnGameWndMsg;

		/////////////////////////////////////////////////////////////////////////
		//game used yy voice channel
		PFN_JOINCHANNEL     m_pfnJoinChannel;    
		PFN_SETTEAMADMIN    m_pfnSetTeamAdmin;
		PFN_SETUSERNAME     m_pfnSetUserName;
		PFN_JOINTEAM        m_pfnJoinTeam;
		PFN_SETTEAMDEVICE   m_pfnSetTeamDevice;
		PFN_SETTEAMVOICE    m_pfnSetTeamVoice;
		PFN_LOCKTEAMVOICE   m_pfnLockTeamVoice;

		/////////////////////////////////////////////////////////////////////////
		//game to channel yy voice window
		PFN_GETPIPSHOW      m_pfnGetPipShow;
		PFN_SETPIPSHOW      m_pfnSetPipShow;
		PFN_SETMSGSHOW      m_pfnSetMsgShow;
		PFN_SETMOUSESHOW    m_pfnMouseShow;
		PFN_LOCKWINDOW      m_pfnLockWnd;
		PFN_MOVEWINDOW      m_pfnMoveWnd;

		/////////////////////////////////////////////////////////////////////////
		//game to used open id func 
		PFN_SHOWOPENID      m_pfnShowOpid;
		PFN_LOGINSUCCEED    m_pfnLoginSucceed;
		PFN_LOGINFAILURE    m_pfnLoginFailure;
		PFN_SETLOGINKEY     m_pfnSetLoginKey;
		PFN_SETCALLBACK     m_pfnSetCallBack;
	public:
		void  InitYY();
		void  SetYYCtrl(SQRYYCtrl* yyCtrl);
		SQRYYCtrl*  GetYYCtrl();

		LRESULT FilterAndProcYYMsg( uint32 message, WPARAM wParam, LPARAM lParam );

		static void CreateYYMgr();
		static void DestroyYYMgr();

	private:
		SQRYYCtrl*	m_pYYCtrl;
	};

	class GUI_API SQRYYCtrl 
		: public SQRControl
	{
		DYNAMIC_DECLARE_WND( SQRYYCtrl )
		enum LocationStyle
		{
			eLLeftTop		= 0,
			eLCenter		= 1,
			eLWCenterTop	= 2,
			eLCount			= 3,
		};
	public:
		SQRYYCtrl(void);
		virtual ~SQRYYCtrl(void);

				void	DrawWndBackground();
				virtual int		CallBackChar( int nType, char* wParam );
				virtual int		CallBackInt( int nType, uint32 wParam );
				void	DestroyYY(void);
		
				void	SetLocationStyle(uint32 sty);
				void	_ResetWndPos(float x, float y);
				void	_ResizeRect( const CFRect& rt ,bool refreshbak = true);
				void	_SetWndRect( const CFRect& rt );
		virtual void	OnScreenPosChanged();
				void	ShowWnd( bool bShow );
		virtual int32	DestroyWnd();

				int		LoginSucceed();
				int		LoginFailure( const char* msg );
				int		SetLoginKEY( const char* msg );
	protected:
		void	_RefreshYY();
	private:
		LocationStyle	m_eLocationType;
		static bool		m_bFirstFrame;
	};
}
