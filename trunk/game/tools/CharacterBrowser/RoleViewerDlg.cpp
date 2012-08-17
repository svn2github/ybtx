// RoleViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BaseTypes.h"
#include "RoleViewer.h"
#include "RoleViewerDlg.h"
#include "GTRenderMgr.h"
#include "CDataCache.h"
#include "Console.h"
#include "CCamera.h"
#include "ScaleDlg.h"
//#include "CMiniSceneConfig.h"
#include "ScriptMgr.h"
#include "CScript.h"
#include "CDataCache.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////
//
#include <dbghelp.h>
#pragma comment(linker, "/defaultlib:dbghelp.lib")

class CExpReport
{
public:
	CExpReport();
	virtual ~CExpReport();

	virtual std::string GetLogFileName() = 0;
	virtual void		OnOutPutLog( uint32 uExceptionCode, void* pExceptionAddress ) = 0;
};

CExpReport*			g_pExceptionReport	= NULL;
LPTOP_LEVEL_EXCEPTION_FILTER	g_previousFilter	= NULL;

LONG WINAPI WheatyUnhandledExceptionFilter( PEXCEPTION_POINTERS pExceptionInfo );

CExpReport::CExpReport()
{
	if( !g_pExceptionReport )
	{
		g_pExceptionReport = this;
		g_previousFilter = SetUnhandledExceptionFilter( WheatyUnhandledExceptionFilter );
	}
}

CExpReport::~CExpReport( )
{
	SetUnhandledExceptionFilter( g_previousFilter );
}


LONG WINAPI WheatyUnhandledExceptionFilter(	PEXCEPTION_POINTERS pExceptionInfo )
{
	TCHAR temp[4096] = {0};
	HANDLE hReportFile = CreateFile( g_pExceptionReport->GetLogFileName().c_str(),
		GENERIC_WRITE,
		0,
		0,
		OPEN_ALWAYS,
		FILE_FLAG_WRITE_THROUGH,
		0 );

	if( hReportFile )
	{
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = NULL;

		MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hReportFile, MiniDumpNormal, &ExInfo, NULL, NULL );

		CloseHandle( hReportFile );

		PEXCEPTION_RECORD prec = pExceptionInfo->ExceptionRecord;

		g_pExceptionReport->OnOutPutLog( prec->ExceptionCode, prec->ExceptionAddress );
	}


	return EXCEPTION_CONTINUE_SEARCH;
}


class mydebug : public CExpReport
{
	std::string GetLogFileName() 
	{
		SYSTEMTIME ti;
		GetLocalTime(&ti);

		ostringstream ost;
		ost << "..\\tools\\CharacterBrowser\\CharacterBrowser_" << ti.wMonth << "_" << ti.wDay << "_" << ti.wHour << "_" << ti.wMinute << ".dmp";

		string log(ost.str());

		return log;
	}
	virtual void OnOutPutLog( uint32 uExceptionCode, void* pExceptionAddress ) {};
};

mydebug god;
//////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public ::CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CRoleViewerDlg dialog




CRoleViewerDlg::CRoleViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRoleViewerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	if ( !m_PahtBrowser.Initialize() )
		m_PahtBrowser.DoModal();
	m_PahtBrowser.SetResPkgFindPath();
	CEditorConfig::GetInst()->InitEditorConfig();
}

CRoleViewerDlg::~CRoleViewerDlg()
{
	if( CGTRenderMgr::HasInst() )
		CGTRenderMgr::Destroy();
		CEditorConfig::GetInst()->SaveEditorConfig();
}

void CRoleViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHOW, m_Show);
	DDX_Control(pDX, IDC_TOOLS_TAB, m_EditorTools);
}

BEGIN_MESSAGE_MAP(CRoleViewerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CRoleViewerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CRoleViewerDlg::OnBnClickedCancel)
	ON_COMMAND(ID_SELFDEFINE, &CRoleViewerDlg::OnSelfdefine)
	ON_COMMAND(ID_ONEPOINTFIVE, &CRoleViewerDlg::OnOnepointfive)
	ON_COMMAND(ID_TWO, &CRoleViewerDlg::OnTwo)
	ON_COMMAND(ID_SceneSetting, &CRoleViewerDlg::OnSceneSetting)
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_PathSetting, &CRoleViewerDlg::OnPathSetting)
	ON_COMMAND(ID_VIDEOSETTING, &CRoleViewerDlg::OnVideoSetting)
	ON_COMMAND(ID_HIDEAPERTURE, &CRoleViewerDlg::OnHideaperture)
END_MESSAGE_MAP()


// CRoleViewerDlg message handlers

BOOL CRoleViewerDlg::OnInitDialog()
{	
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	::GetCurrentDirectory(1024, rootPath);

	DoDataExchange(false);
	m_EditorTools.PushView(&m_NpcView,0,"角色查看");
	m_EditorTools.PushView(&m_AvaView,0,"装备查看");
	m_EditorTools.ShowView(0);

	RECT rectEditor;
	m_EditorTools.GetClientRect(&rectEditor);	
	m_lSprit = rectEditor.right - rectEditor.left;
	
	//图形界面设置
	m_VideoSetting.Create(m_VideoSetting.IDD);
	m_VideoSetting.Initialize();
	m_VideoSetting.ShowWindow(SW_HIDE);

	new CGTRenderMgr(m_Show.GetRenderGroup());

	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	//ofstream File( FileName.data(), ios::binary|ios::out );
	locale::global(loc);//恢复全局locale
	SetTimer( 0, 30, NULL );
	return TRUE;  // return TRUE  unless you set the focus to a control
}
void CRoleViewerDlg::OnClose()
{
	gs_pScript->GCAll();
	#ifdef _DEBUG
		ShowConsole(false);
	#endif
	delete gs_pScript;
	gs_pScript = NULL;
	delete gs_pCahce;
	gs_pCahce = NULL;
	CDialog::OnClose();
}
void CRoleViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRoleViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{		
		CDialog::OnPaint();
	}
}

//BOOL CRoleViewerDlg::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
//{
//	// TODO: Add your specialized code here and/or call the base class
//	LPNMHDR pNMHDR = (LPNMHDR)lParam;
//	if( pNMHDR->code == NM_CHILDETOPARENT )
//	{
//		NotifyMessage* NotifyMsg = (NotifyMessage*)pNMHDR;
//		if( NotifyMsg->pWnd == &m_Show )
//			OnShow( NotifyMsg->Message, NotifyMsg->wParam, NotifyMsg->lParam );
//	}
//	return __super::OnNotify(wParam, lParam, pResult);
//}
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRoleViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CRoleViewerDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CRoleViewerDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CRoleViewerDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	OnDraw();
	__super::OnTimer(nIDEvent);
}

void CRoleViewerDlg::OnDraw() 
{
	CGTRenderMgr::GetInst()->OnRender();
	CDialog::OnPaint();
}

void CRoleViewerDlg::OnSelfdefine()
{
	CScaleDlg dlg;
	dlg.DoModal();
	float fScale = dlg.m_fScale;
	CGTRenderMgr::GetInst()->DisplayCharacterAperture(fScale);
	// TODO: Add your command handler code here
}

void CRoleViewerDlg::OnOnepointfive()
{
	CGTRenderMgr::GetInst()->DisplayCharacterAperture(1.5f);
	// TODO: Add your command handler code here
}


void CRoleViewerDlg::OnTwo()
{
	CGTRenderMgr::GetInst()->DisplayCharacterAperture(2.0f);
	// TODO: Add your command handler code here
}

void CRoleViewerDlg::OnSceneSetting()
{
	CGTRenderMgr::GetInst()->m_MiniScene.ShowSettingDlg();
}

void CRoleViewerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(m_EditorTools.m_hWnd!=NULL)
	{
		RECT rectEditor;
		RECT rectView;
		m_Show.GetClientRect(&rectView);
		rectView.right = cx - m_lSprit - rectView.left;
		rectView.bottom = cy - rectView.top;
		m_Show.MoveWindow( &rectView );

		rectEditor.top = rectView.top;
		rectEditor.right = cx - rectView.left;
		rectEditor.left = rectView.right;
		rectEditor.bottom = cy - rectView.top;
		m_EditorTools.MoveWindow( &rectEditor );
	}
	if(m_VideoSetting.GetSafeHwnd())
		m_VideoSetting.Initialize();
}

void CRoleViewerDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	RECT Rect;
	GetClientRect(&Rect);
	OnSize(0,Rect.right,Rect.bottom);
	// TODO: Add your message handler code here
}

BOOL CRoleViewerDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CRoleViewerDlg::OnPathSetting()
{
	m_PahtBrowser.DoModal();
}

void CRoleViewerDlg::OnVideoSetting()
{
	// TODO: Add your command handler code here
	m_VideoSetting.ShowWindow(SW_NORMAL);
}

void CRoleViewerDlg::OnHideaperture()
{
	// TODO: Add your command handler code here
	CGTRenderMgr::GetInst()->HideApertureAndRefer();
	CString csName;
	GetDlgItemText(ID_HIDEAPERTURE,csName);
}
