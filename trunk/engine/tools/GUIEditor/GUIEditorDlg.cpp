#include "stdafx.h"
#include "GUIEditor.h"
#include "GUIEditorDlg.h"
#include "CCamera.h"
#include "BaseHelper.h"
#include "tinyxml/tinyxml.h"
#include <dbghelp.h>
#include "CWindowTarget.h"
#include "CRenderGroup.h"
#include "WndStyleRender.h"
#include "CGuiFxPlayer.h"
#include "IAudioModule.h"
#include "IEffectMgrModule.h"
#include "WndAreaRenderer.h"
#include "../src/gui/SQRGUIManager.h"
#include "CodeCvs.h"
#include "CodeCvs.h"
#include "CLangCfg.h"
#include "CFontMgr.h"
#include "CFontCfg.h"
#include "CRenderPipeline.h"


#pragma comment(linker, "/defaultlib:dbghelp.lib")
//////////////////////////////////////////////////////////////////////////
#pragma region Helper

BEGIN_MESSAGE_MAP(CEditPan, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	ON_WM_SIZE()
END_MESSAGE_MAP()

#define CTRL_A		( 'A' - 'A' + 1 )
#define CTRL_C		( 'C' - 'A' + 1 )
#define CTRL_V		( 'V' - 'A' + 1 )
#define CTRL_X		( 'X' - 'A' + 1 )
class CExpReport
{
public:
	CExpReport();
	virtual ~CExpReport();

	virtual const TCHAR* GetLogFileName() = 0;
	virtual void		OnOutPutLog( unsigned int uExceptionCode, void* pExceptionAddress ) = 0;
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
	HANDLE hReportFile = CreateFile( g_pExceptionReport->GetLogFileName(),
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

	const wchar_t* GetLogFileName() {return L"gui\\guieditordebug.dmp";}
	virtual void		OnOutPutLog( unsigned int uExceptionCode, void* pExceptionAddress ) {};
};
//////////////////////////////////////////////////////////////////////////

mydebug god;

class CGUIFileDialog : public CFileDialog
{
public:
	DECLARE_DYNAMIC(CGUIFileDialog)
	CString			m_FloderName;

	CGUIFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		::CWnd* pParentWnd = NULL);
	virtual INT_PTR DoModal( CString& RootPath );

	DECLARE_MESSAGE_MAP()
	virtual void OnFolderChange();
};

IMPLEMENT_DYNAMIC(CGUIFileDialog, CFileDialog)
CGUIFileDialog::CGUIFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
							   DWORD dwFlags, LPCTSTR lpszFilter, ::CWnd* pParentWnd) :
CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}
BEGIN_MESSAGE_MAP(CGUIFileDialog, CFileDialog)
END_MESSAGE_MAP()

// CGUIFileDialog message handlers

INT_PTR CGUIFileDialog::DoModal( CString& RootPath )
{
	// TODO: Add your specialized code here and/or call the base class
	INT_PTR re = CFileDialog::DoModal();
	if( re == IDOK )
		RootPath = m_FloderName;

	return re;
}

void CGUIFileDialog::OnFolderChange()
{
	// TODO: Add your specialized code here and/or call the base class
	m_FloderName = GetFolderPath();

	CFileDialog::OnFolderChange();
}
//------------------------------------------------------------------------------
LRESULT
CShow::SendCtrlNotifyMsg( DWORD Notify, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	NotifyMessage NotifyMsg;
	NotifyMsg.hrd.hwndFrom = m_hWnd;
	NotifyMsg.hrd.idFrom = GetDlgCtrlID();
	NotifyMsg.hrd.code = Notify;
	NotifyMsg.pWnd = this;
	NotifyMsg.Message = uMessage;
	NotifyMsg.wParam = wParam;
	NotifyMsg.lParam = lParam;
	return GetParent()->SendMessage( WM_NOTIFY, NotifyMsg.hrd.idFrom, (LPARAM) (LPNMHDR)&NotifyMsg );
}

//------------------------------------------------------------------------------
LRESULT
CShow::WindowProc( UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	if( WM_GETDLGCODE == uMessage )
		return DLGC_WANTALLKEYS;
	if( WM_LBUTTONDOWN < uMessage && WM_MBUTTONDBLCLK > uMessage )
		SetFocus();
	if( uMessage >= WM_KEYFIRST && uMessage <= WM_KEYLAST )
		return SendCtrlNotifyMsg( NM_CHILDETOPARENT, uMessage, wParam, lParam ); 
	if( uMessage >= WM_IME_SETCONTEXT && uMessage <= WM_IME_KEYUP )
		return SendCtrlNotifyMsg( NM_CHILDETOPARENT, uMessage, wParam, lParam ); 

	//将鼠标信息传给父窗口
	if( uMessage >= WM_MOUSEFIRST && uMessage <= WM_MOUSELAST || uMessage == WM_SETCURSOR )
		return SendCtrlNotifyMsg( NM_CHILDETOPARENT, uMessage, wParam, lParam ); 

	if( WM_LBUTTONDOWN < uMessage && WM_MBUTTONDBLCLK > uMessage )
		SetFocus();

	return CStatic::WindowProc( uMessage, wParam, lParam );
}

//------------------------------------------------------------------------------
UINT
CEditPan::AddToList( CView* pView, int Image, LPWSTR ItemName, int id )
{
	CRect rect;
	GetClientRect(&rect);
	pView->Create( NULL, NULL, WS_CHILD, rect, this, id, NULL );
	pView->UpdateData( FALSE );

	TCITEM item;
	memset(&item,0,sizeof(TCITEM));
	item.mask = TCIF_PARAM;
	if( Image>= 0 )
		item.mask |= TCIF_IMAGE;
	if( ItemName )
		item.mask |= TCIF_TEXT;
	item.iImage    = Image;
	item.pszText   = ItemName;
	item.lParam    = (LPARAM)pView;

	UINT n = GetItemCount();
	n = InsertItem(n,&item);

	SetCurSel( n );
	ShowTab( n );
	SendMessage( WM_SIZE );

	return n;
}

//------------------------------------------------------------------------------
void
CEditPan::ShowTab( int Num )
{
	SetCurSel( Num );
	for( int i = 0; i < GetItemCount(); i++ )
	{
		TCITEM Item;
		memset( &Item, 0, sizeof(TCITEM) );
		Item.mask = TCIF_PARAM;
		GetItem( i, &Item );
		( (CView*)Item.lParam )->ShowWindow( i == Num ? SW_SHOW : SW_HIDE );
	}
}

//------------------------------------------------------------------------------
BOOL
CEditPan::Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	m_Font.CreateFont( 12, 0, 0, 0, 400, 0, 0, 0, 134, 3, 2, 2, 2, L"" );
	BOOL re = CTabCtrl::Create(dwStyle, rect, pParentWnd, nID);
	SetFont( &m_Font );
	ShowWindow(SW_SHOW);
	SetPadding( CSize( 2,4 ) );
	SetMinTabWidth( 12 );

	return re;
}

//------------------------------------------------------------------------------
void
CEditPan::OnSelchange( NMHDR* pNMHDR, LRESULT* pResult )
{
	ShowTab( GetCurSel() );
}

//------------------------------------------------------------------------------
void
CEditPan::OnSize( UINT nType, int cx, int cy )
{
	if( !GetSafeHwnd() )
		return;
	CRect rect;
	GetClientRect(&rect);
	for( int i = 0; i < GetItemCount(); i++ )
	{
		TCITEM Item;
		memset( &Item, 0, sizeof(TCITEM) );
		Item.mask = TCIF_PARAM;
		GetItem( i, &Item );
		( (CView*)Item.lParam )->SetWindowPos( NULL, 5, 5, rect.Width() - 30, rect.Height() - 30, SWP_NOZORDER );
	}
}
// CSelectSizeDlg dialog

class CSelectSizeDlg : public ::CDialog
{
	DECLARE_DYNAMIC(CSelectSizeDlg)
	unsigned m_uID;

public:
	CSelectSizeDlg() : CDialog( IDD_DIALOG_SELECT_SIZE, NULL ), m_uID(IDD_GUIEDITOR_DIALOG_800){}
	static unsigned GetSelectSizeDialogID()
	{
		CSelectSizeDlg Dlg;
		Dlg.DoModal();

		return Dlg.m_uID;
	}

protected:

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog()
	{
		CDialog::OnInitDialog();

		// TODO:  Add extra initialization here
		CheckRadioButton( IDC_RADIO_800, IDC_RADIO_1024, IDC_RADIO_1024 );
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	virtual void OnOK()
	{
		m_uID = IsDlgButtonChecked( IDC_RADIO_1024 ) ? IDD_GUIEDITOR_DIALOG_1024 : IDD_GUIEDITOR_DIALOG_800;
		CDialog::OnOK();		
	}
};
// CSelectSizeDlg dialog

IMPLEMENT_DYNAMIC(CSelectSizeDlg, CDialog)

BEGIN_MESSAGE_MAP(CSelectSizeDlg, CDialog)
END_MESSAGE_MAP()
#pragma endregion Helper
//////////////////////////////////////////////////////////////////////////


// CGUIEditorDlg 对话框

unsigned CGUIEditorDlg::GetDialogID()
{
	return CSelectSizeDlg::GetSelectSizeDialogID();
}

CRect CGUIEditorDlg::RootWndRect()
{
	CRect rect;

	if ( m_pGUIRootWnd )
	{
		CFRect rt;
		m_pGUIRootWnd->GetWndRect( rt );
		rt.left		*= m_n;
		rt.right	*= m_n;
		rt.top		*= m_n;
		rt.bottom	*= m_n;

		rect = rt;
	}

	return rect;
}


#pragma warning(push)
#pragma warning(disable:4355)
CGUIEditorDlg::CGUIEditorDlg( ::CWnd* pParent /*=NULL*/ )
	: CDialog( IDD_GUIEDITOR_DIALOG_1024, pParent )
	, m_WndGroup( this )
	, m_pAudioSystem(NULL)
	, m_pEffectManger(NULL)
	, m_pGUIRootWnd(new SQRRootWnd())
{
	m_pBkTex = NULL;
}

CGUIEditorDlg::~CGUIEditorDlg()
{
	if ( m_pGUIRootWnd )
	{
		delete m_pGUIRootWnd;
		m_pGUIRootWnd = NULL;
	}
}
#pragma warning(pop)

void CGUIEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHOW, m_Show);
	DDX_Control(pDX, IDC_EDIR_TAB, m_EditTab);
	DDX_Control(pDX, IDC_SLIDER_ZOOM, m_zoomSlider);
	DDX_Control(pDX, IDC_SLIDER_WIN, m_winSlider);
	DDX_Control(pDX, IDC_SHOW_ZOOM, m_zoom);
	DDX_Control(pDX, IDC_SHOW_PIX, m_win);
}

BEGIN_MESSAGE_MAP(CGUIEditorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_RADIO_POS,				OnBnClickedRadioPos)
	ON_BN_CLICKED(IDC_RADIO_TEX,				OnBnClickedRadioTex)
	ON_BN_CLICKED(IDC_RADIO_PREVIEW,			OnBnClickedRadioPreview)
	ON_BN_CLICKED(IDC_SHOWALPHA,				OnBnClickedShowAlpha)
	ON_BN_CLICKED(IDC_REDO,						OnBnClickedRedo)
	ON_BN_CLICKED(IDC_UNDO,						OnBnClickedUndo)
	ON_BN_CLICKED(IDC_SET_WORKPATH,				OnBnClickedSetWorkpath)
	ON_BN_CLICKED(IDC_BTN_BKTEX,				OnBnClickedBtnBktex)
	ON_BN_CLICKED(IDC_SET_BK_COLOR,				&CGUIEditorDlg::OnBnClickedSetBkColor)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ZOOM,	&CGUIEditorDlg::OnSliderZoomChange)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_WIN,	&CGUIEditorDlg::OnSliderWinChange)
	ON_CBN_SELCHANGE(IDC_LANGUAGE_LIST, &CGUIEditorDlg::OnCbnSelchangeLanguageList)
END_MESSAGE_MAP()


void CGUIEditorDlg::OnCancel()
{
	m_WndGroup.DestroyWindow();
	m_WndStyle.DestroyWindow();
	m_WndStyleEditor.DestroyWindow();
	m_EmtEditor.DestroyWindow();
	m_FlashEditor.DestroyWindow();
	m_TexFontEditor.DestroyWindow();
	m_GUIResDlg.DestroyWindow();
	m_pGUIRootWnd->DestroyWnd();
	CWndAreaRendere::Destroy();
	CDialog::DestroyWindow();  
	SafeDelete(m_pFxPlayer);
	if (m_pAudioSystem->IsOpen())
		m_pAudioSystem->Close();
	CGraphic::Destroy();
}

// CGUIEditorDlg 消息处理程序

BOOL CGUIEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// IDM_ABOUTBOX 必须在系统命令范围内。
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
    

	if ( !CEditorConfig::GetInst()->InitEditorConfig() )
	{
		MessageBox(L"您没有设置工作路径，请将resbin,lang和GUI文件夹所在目录添加到搜索路径中");
		m_PathBrowser.DoModal();
	}

	m_PathBrowser.Initialize();
	m_WndGroup.InitCfg();

	///shader配置文件
	wstring config = L"./shaderconfig/shaderconfig.xml";

	TiXmlDocument doc;
	ifstream in(config.c_str());
	if (!in)
		GenErr("Can't open shaderconfig.xml");
	in >> doc;

	TiXmlNode* pRoot = doc.FirstChild("shaderdirectory");

	::sqr::RenderSettings settings;
	settings.SetWindowHandle(m_Show.m_hWnd);

	RECT rect;
	::GetClientRect(m_Show.m_hWnd, &rect);
	rect.right	= rect.left + 1024;
	rect.bottom	= rect.top + 768;
	settings.SetWidth(rect.right - rect.left);
	settings.SetHeight(rect.bottom - rect.top);

	m_pGraphic = CGraphic::GetInst();
	CMainWindowTarget::GetInst()->Initialize(settings);
	CRenderGroup*	pGroup = new CRenderGroup;
	CMainWindowTarget::GetInst()->SetRenderGroup(pGroup);
	m_pGraphic->InitShader(utf8_to_utf16(pRoot->FirstChild("shader_path")->ToElement()->GetText()));
	CTextureFontMgr::GetInst()->Initialize(utf16_to_utf8(m_emtPath).c_str(), m_pGraphic);
	SetN(1);
	this->createAudioSystem();

	m_pEffectManger = GetIEffectMgrModule()->CreateEffectMgr();
	m_pEffectManger->InitClassDesc();
	m_pFxPlayer = new CGuiFxPlayer(m_pEffectManger, utf16_to_utf8(m_fxPath).c_str());
	
	// 设置语言
	CFontMgr::GetInst()->SetLangIndex(m_nCurLan);

	CFontCfg::GetInst()->LoadFongConfig(CLangCfg::GetInst()->GetLanFolder(m_nCurLan));

	CFontMgr::GetInst()->SetLangFolder(CLangCfg::GetInst()->GetLanFolder(m_nCurLan));
	CFontMgr::GetInst()->SetDisFontName(CFontCfg::GetInst()->GetDisFontName());
	CFontMgr::GetInst()->ClearFontInfo();
	CFontMgr::GetInst()->UnloadGdiFonts();

	size_t font_count = CFontCfg::GetInst()->GetGDIFontCount();
	for ( size_t font_index = 0; font_index < font_count; ++font_index )
	{
		wstring szFontName = CFontCfg::GetInst()->GetGDIFontName(font_index);
		string  szFontFile = CFontCfg::GetInst()->GetGDIFontFileName(font_index);
		CFontMgr::GetInst()->AddFontInfo(szFontName.c_str(), szFontFile.c_str());
	}

	while ( !CFontMgr::GetInst()->LoadGDIFonts() )
	{
		MessageBox(L"您没有设置语言包所在路径，请设置(例如x:\\ybtx\\designer\\lang");
		m_PathBrowser.DoModal();
	}

	RECT rt_tmp;
	m_Show.GetClientRect( &rt_tmp );
	rt_tmp.right	= rt_tmp.left + 1024;
	rt_tmp.bottom	= rt_tmp.top + 768;
	CFRect rt(rt_tmp);

	if (m_strGUIPath == L"")
		m_pGUIRootWnd->CreateRootWnd( m_pFxPlayer, rt, m_pGraphic, 0, NULL, NULL, NULL, TRUE );
	else
	{
		wstring strResPath = m_strGUIPath + L"\\Layout";
		wstring styPath = m_strGUIPath + L"\\";
		m_pGUIRootWnd->CreateRootWnd( m_pFxPlayer, rt, m_pGraphic, 0, NULL, utf16_to_utf8(strResPath).c_str(), utf16_to_utf8(styPath).c_str(), TRUE, true );
	}
	FlashDataManager::GetInst()->Initialize(utf16_to_utf8(m_emtPath).c_str(), m_pGraphic);
	// 加载GUI静态文本资源
	m_pGUIRootWnd->LoadStaticText();
	
	m_pGUIRootWnd->SetDrawHandler(this);
	
	SetTimer( 1, 20, NULL );

	// 设置摄像机
	::sqr::CCamera* pCamera = CMainWindowTarget::GetInst()->GetCamera();
	pCamera->SetPosition(::sqr::CVector3f( 0, 0, -1 ));
	pCamera->lookAt(::sqr::CVector3f( 0, 0, 0 ));

	CWndAreaRendere::Create();
	CRect root_rect = RootWndRect();
	CWndAreaRendere::GetInst()->SetRootRect( root_rect );

	// 初始化编辑器界面
	m_EditTab.AddToList( &m_WndGroup,		0, L"窗口列表",		1001 );
	m_EditTab.AddToList( &m_WndProp,		0, L"窗口属性",		1002 );
	m_EditTab.AddToList( &m_WndStyle,		0, L"窗口风格",		1003 );
	m_EditTab.AddToList( &m_WndStyleEditor, 0, L"GUI默认风格",	1004 );
	m_EditTab.AddToList( &m_EmtEditor,		0, L"表情编辑",		1005 );
	m_EditTab.AddToList( &m_FlashEditor,	0, L"指示信息编辑",	1006 );
	m_EditTab.AddToList( &m_TexFontEditor,	0, L"贴图字编辑",	1007 );
	m_EditTab.AddToList( &m_GUIResDlg,		0, L"窗体贴图关联",	1008 );

	m_EditTab.ShowTab(0);

	// 设置缩放滚动条
	m_zoomSlider.SetRange(0,1000,true);
	m_zoomSlider.SetPos(500);
	m_winSlider.SetRange(0,1000,true);
	m_winSlider.SetPos(1000);

	m_zoomSlider.EnableWindow(FALSE);
	m_winSlider.EnableWindow(FALSE);

	m_WndProp.Init( &m_WndGroup );
	m_WndGroup.SetPropWnd( &m_WndProp, &m_WndStyle );
	m_WndStyle.Init(this, &m_WndGroup, &m_WndProp );

	CheckRadioButton( IDC_RADIO_POS, IDC_RADIO_PREVIEW, IDC_RADIO_POS );
	CheckDlgButton( IDC_SIZEFOLLOW, m_bSizeFollow );

	// 初始化语言列表
	::CComboBox combTmp;
	combTmp.SubclassDlgItem(IDC_LANGUAGE_LIST, this);
	int32 nLanCount = CLangCfg::GetInst()->GetLanCount();
	for ( int iCurLan = 0; iCurLan < nLanCount; ++iCurLan )
	{
		string szLanName = CLangCfg::GetInst()->GetLanName(iCurLan);
		combTmp.InsertString( combTmp.GetCount(), utf8_to_utf16(szLanName).c_str() );
	}
	combTmp.SetCurSel( CFontMgr::GetInst()->GetLangIndex() );
	combTmp.UnsubclassWindow();

	// 给每个saver控件设置saver
	m_PicSaver.Load(m_PicResPath);
	m_WndGroup.SetXmlResSaver( &m_PicSaver );		// 保存
	m_WndStyleEditor.SetXmlResSaver( &m_PicSaver );
	m_EmtEditor.SetXmlResSaver( &m_PicSaver );
	m_FlashEditor.SetXmlResSaver( &m_PicSaver );
	m_TexFontEditor.SetXmlResSaver( &m_PicSaver );

	// 设置CWndAreaRendere多选的WND
	CWndAreaRendere::GetInst()->SetWndRenderList(m_WndGroup.GetSelectedWndList());
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CGUIEditorDlg::UpdateRect(void)
{
	CFRect rt;
	m_pGUIRootWnd->GetWndRect(rt);
	m_pGUIRootWnd->ResizeRect(rt);
}

void CGUIEditorDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

void CGUIEditorDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your uMessage handler code here and/or call default
	DataExchage();

	static float FPS=0;
	static FLOAT fLastTime = 0.0f;
	static DWORD dwFrames  = 0L;
	FLOAT fTime = GetTickCount()/1000.0f;
	++dwFrames;

	// Update the scene stats once per second
	if( fTime - fLastTime > 1.0f )
	{
		FPS    = dwFrames / (fTime - fLastTime);
		fLastTime = fTime;
		dwFrames  = 0L;
	}
	if(!::sqr::CMainWindowTarget::GetInst())
		return;

	::sqr::CMainWindowTarget::GetInst()->GetGroup()->EnableFog( FALSE );
	if (m_pAudioSystem->IsOpen())
	{
		m_pAudioSystem->Update();
	}

	CSyncSystem::BeginSyncFun();
	if( SUCCEEDED(::sqr::CMainWindowTarget::GetInst()->IsRenderActive()) )
	{
		
		if(m_pBkTex)
		{
			RECT rt;
			m_Show.GetClientRect( &rt );

			m_pGraphic->DrawRect( rt, 0xffffffff, m_pBkTex );
		}

		if( IDC_RADIO_PREVIEW == m_WndGroup.m_EditID )
		{
			CWndAreaRendere::GetInst()->SetIsRender(false);
			CWndAreaRendere::GetInst()->SetIsRootRender(true);
		}	
		else
		{
			CWndAreaRendere::GetInst()->SetIsRender(true);
			CWndAreaRendere::GetInst()->SetIsRootRender(false);
		}

		CRect rect = m_WndGroup.CurWndRect(int( int(m_n) ));
		CWndAreaRendere::GetInst()->SetCurRect( rect );

		::sqr::CMainWindowTarget::GetInst()->Render();
	}
	
	if( (GetKeyState( VK_LEFT )&0x8000 ) || 
		(GetKeyState( VK_RIGHT )&0x8000 ) ||
		( GetKeyState( VK_UP )&0x8000) ||
		( GetKeyState( VK_DOWN )&0x8000 ) )
	{
		m_WndGroup.MoveWnd();
	}
}

void CGUIEditorDlg::DrawAfterChildren()
{
	if( IDC_RADIO_PREVIEW == m_WndGroup.m_EditID )
		return;

	m_WndGroup.DrawEditLine( m_pGraphic, int( m_n ) );
	m_WndProp.DrawEditLine( m_pGraphic, int( m_n ) );
}

BOOL CGUIEditorDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	LPNMHDR pNMHDR = (LPNMHDR)lParam;
	if( pNMHDR->code == NM_CHILDETOPARENT )
	{
		NotifyMessage* NotifyMsg = (NotifyMessage*)pNMHDR;
		if( NotifyMsg->pWnd == &m_Show && m_WndGroup.GetSafeHwnd() )
		{
			if( NotifyMsg->Message == WM_KEYDOWN && NotifyMsg->wParam == VK_ADD )
				SetN( GetN()*1.1f );
			else if( NotifyMsg->Message == WM_KEYDOWN && NotifyMsg->wParam == VK_SUBTRACT )
				SetN( GetN()*0.9f );
			else
			{
				// CPoint point( int(LOWORD(NotifyMsg->lParam)/GetN()), int(HIWORD(NotifyMsg->lParam)/GetN()) );
				// 去掉缩放
				CPoint point( int(LOWORD(NotifyMsg->lParam)), int(HIWORD(NotifyMsg->lParam)) );
				if( m_WndStyleEditor.m_StyleRender.IsStateInRender() )
					m_WndStyleEditor.StyleProc(NotifyMsg->Message,NotifyMsg->wParam,MAKELONG( point.x, point.y ));
				else if (m_EmtEditor.m_EmtRender.IsStateInRender())
					return CDialog::OnNotify(wParam, lParam, pResult);
				else if (m_FlashEditor.m_FlashRender.IsStateInRender())
					return CDialog::OnNotify(wParam, lParam, pResult);
				else if (m_TexFontEditor.m_FontRender.IsStateInRender())
					m_TexFontEditor.ProcEditRect(NotifyMsg->Message,NotifyMsg->wParam,MAKELONG( point.x, point.y ));
				else
				 m_WndGroup.OnShow( NotifyMsg->Message, NotifyMsg->wParam, MAKELONG( point.x, point.y ) );
			}
			
		}
	}
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CGUIEditorDlg::DataExchage()
{
}

void CGUIEditorDlg::EditWndRect( CPoint p )
{
}

void CGUIEditorDlg::OnBnClickedRedo()
{
	// TODO: Add your control notification handler code here
	m_WndGroup.m_ComandList.Redo();
}

void CGUIEditorDlg::OnBnClickedUndo()
{
	// TODO: Add your control notification handler code here
	m_WndGroup.m_ComandList.UnDo();
}

void CGUIEditorDlg::OnBnClickedSetWorkpath()
{

	// TODO: Add your control notification handler code here
	//CGUIFileDialog Dlg( TRUE, NULL, L"工作目录", 
	//	OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR|OFN_NOVALIDATE, L"" );
	//Dlg.m_ofn.lpstrInitialDir = GetRootPath();
	//Dlg.DoModal( GetRootPath() );

	m_PathBrowser.DoModal();

	//HKEY hKey = NULL;
	//RegCreateKey( HKEY_LOCAL_MACHINE, L"Software\\GUIEditor", &hKey );
	//RegSetValueEx( hKey, L"RootPath", NULL, REG_SZ, (BYTE*)(LPCTSTR)GetRootPath(), GetRootPath().GetLength() );
	//RegCloseKey(hKey);
	::SetCurrentDirectory( GetRootPath() );
}

void CGUIEditorDlg::OnBnClickedBtnBktex()
{
	static wchar_t Filter[] = L"GUI Texture (*.tex)|*.tex||";
	CString AUFPath = GetRegistKeyStr( L"Software\\GUIEditor", L"AUFPath" );
	AUFPath = GetFileName( TRUE, Filter, NULL, AUFPath );
	if( AUFPath.IsEmpty() )
		return;

	SafeRelease(m_pBkTex);
	m_pGraphic->CreateTexture( L"", utf16_to_utf8(AUFPath.GetBuffer()).c_str(), &m_pBkTex );	
}

void CGUIEditorDlg::OnSliderZoomChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	static float lastf = 0.0f;
	float f= ((float)(m_zoomSlider.GetPos() - 500))/1063.0f;	// zoom = limit2(zoom,0.68f,1.32f);
	if (lastf == f)
	{
		return;
	}
	lastf = f;

	f = 1.0f + f;
	// do the change
	f = limit2(f,0.68f,1.32f);
	// show change
	char s_f[16];
	sprintf_s( s_f, "%.2f", f );
	wchar_t s_t_f[16];
	for (int i = 0; i < 16; i++)
	{
		s_t_f[i] = s_f[i];
	}

	m_zoom.SetWindowText((LPCTSTR)s_t_f);

	IGUIManager::GetInst()->GetRootWnd()->SetOrgZoom(f);
}

void CGUIEditorDlg::OnSliderWinChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	static float lastf = 0.0f;
	float f= ((float)(m_winSlider.GetPos() - 1000))/1200.0f;
	if (lastf == f)
	{
		return;
	}
	lastf = f;

	f = 1.0f + f;
	f = f > 1.0f ? 1.0f : f;

	// do the change, m_DgTpId == IDD_GUIEDITOR_DIALOG_768
	float w = 1024.0f;
	float h = 768.0f;

	char s_f[32];
	sprintf_s(s_f,"%d*%d",int(w * f),int(h * f));

	wchar_t s_t_f[32];
	for (int i = 0; i < 32; i++)
	{
		s_t_f[i] = s_f[i];
	}
	m_win.SetWindowText((LPCTSTR)s_t_f);

	m_pGUIRootWnd->SetWndSize( 
		0.0f
		, 0.0f
		, (float)(w * f)
		, (float)(h * f)
		);

	CRect root_rect = RootWndRect();
	CWndAreaRendere::GetInst()->SetRootRect( root_rect );
}

void CGUIEditorDlg::SetGUIPath(const wstring& GUIPath)
{
	//wstring newpath = GUIPath;
	//newpath = L"../" + newpath;
	m_strGUIPath = GUIPath;
	//wchar_t path[MAX_PATH];
	//GetFullPathName(newpath.c_str(), MAX_PATH, path, NULL);
	//GetResPath() = path;

	wstring texPath;
	for (int i = 0; i < 3; i++)
	{
		m_PathBrowser.GetPathByIndex(texPath,i);
		if (texPath.find(L"gui") != wstring::npos)
		{
			GetResPath() = (texPath + GUIPath).c_str();
			return;
		}
	}
}

void CGUIEditorDlg::SetTexPath()
{
	wstring texPath;
	for (int i = 0; i < 3; i++)
	{
		m_PathBrowser.GetPathByIndex(texPath,i);
		if (texPath.find(L"resbin") != wstring::npos)
		{
			GetTexPath() = texPath.c_str();
			return;
		}
	}
}

void CGUIEditorDlg::OnBnClickedShowAlpha()
{ 
	EndPreView();
	m_WndGroup.m_EditID = IDC_SHOWALPHA;	
	m_pGUIRootWnd->ShowWndArea(true);
}


void CGUIEditorDlg::OnBnClickedRadioPos()		
{ 
	EndPreView();
	m_WndGroup.m_EditID = IDC_RADIO_POS;
	m_WndStyleEditor.m_StyleRender.SetShowRect(false);
	m_EmtEditor.m_EmtRender.SetShowRect(false);
	m_FlashEditor.m_FlashRender.SetShowRect(false);
	m_TexFontEditor.m_FontRender.SetShowRect(false);
	m_pGUIRootWnd->ShowWndArea(false);
}

void CGUIEditorDlg::OnBnClickedRadioTex()		
{ 
	EndPreView();
	m_WndGroup.m_EditID = IDC_RADIO_TEX;	
	m_WndStyleEditor.m_StyleRender.SetShowRect(true);
	m_EmtEditor.m_EmtRender.SetShowRect(true);
	m_FlashEditor.m_FlashRender.SetShowRect(true);
	m_TexFontEditor.m_FontRender.SetShowRect(true);
	m_pGUIRootWnd->ShowWndArea(false);
}

void CGUIEditorDlg::OnBnClickedRadioPreview()	
{ 
	StartPreView();
	m_WndGroup.m_EditID = IDC_RADIO_PREVIEW;
	m_pGUIRootWnd->ShowWndArea(false);
}


void CGUIEditorDlg::StartPreView()
{
	m_zoomSlider.EnableWindow(TRUE);
	m_winSlider.EnableWindow(TRUE);

	m_WndProp.EnableWindow(FALSE);
	m_WndStyleEditor.EnableWindow(FALSE);
	m_WndStyle.EnableWindow(FALSE);
	m_EmtEditor.EnableWindow(FALSE);
	m_FlashEditor.EnableWindow(FALSE);
	m_TexFontEditor.EnableWindow(FALSE);

	if (m_pFxPlayer)
		m_pFxPlayer->SetBePreView(true);

}

void CGUIEditorDlg::EndPreView()
{
	// 恢复
	if (m_pFxPlayer)
		m_pFxPlayer->SetBePreView(false);

	m_winSlider.SetPos(1000);
	m_pGUIRootWnd->SetWndSize( 0.0f, 0.0f, 1024.0f, 768.0f );

	m_zoomSlider.SetPos(500);
	IGUIManager::GetInst()->GetRootWnd()->SetOrgZoom(1.0f);

	m_WndProp.EnableWindow(TRUE);
	m_WndStyleEditor.EnableWindow(TRUE);
	m_WndStyle.EnableWindow(TRUE);
	m_EmtEditor.EnableWindow(TRUE);
	m_FlashEditor.EnableWindow(TRUE);
	m_TexFontEditor.EnableWindow(TRUE);

	m_zoomSlider.EnableWindow(FALSE);
	m_winSlider.EnableWindow(FALSE);

	// 更新当前选中数据
	if (m_WndGroup.m_RememberItem)
	{
		m_WndGroup.m_WndTree.SelectItem(m_WndGroup.m_RememberItem);
	}
	else
	{
		m_WndGroup.m_WndTree.SelectItem(m_WndGroup.m_hRoot);
	}
	

}


//------------------------------------------------------------------------------
void
CGUIEditorDlg::SetFxPath( const wstring& path )
{
	m_fxPath = path;
}

//------------------------------------------------------------------------------
void
CGUIEditorDlg::SetEmtPath( const wstring& path )
{
	m_emtPath = path;
	m_EmtEditor.m_emtPath = path;
}

//------------------------------------------------------------------------------
void
CGUIEditorDlg::SetPicResPath( const wstring& path )
{
	m_PicResPath = path;
}

//-------------------------------------------------------------------------------
void
CGUIEditorDlg::SetFlashPath( const wstring& path )
{
	m_flashPath = path;
	wstring Str = path;
	Str = Str.substr(0, Str.size() - 15);
	Str += L"FlashInfo.xml";
	m_FlashEditor.m_FlashPath = Str;
}

//-------------------------------------------------------------------------------
void
CGUIEditorDlg::SetTexFontPath( const wstring& path )
{
	m_flashPath = path;
	wstring Str = path;
	Str = Str.substr(0, Str.size() - 15);
	Str += L"TextureFontCfg.xml";
	m_TexFontEditor.m_TexFontPath = Str;
}

void CGUIEditorDlg::OnBnClickedSetBkColor()
{
	CColorDialog dialog(0, CC_FULLOPEN, this);
	if (IDOK == dialog.DoModal())
	{
		COLORREF color = dialog.GetColor();
		DWORD d3dColor = 0xff000000 | GetRValue(color) << 16 | GetGValue(color) << 8 | GetBValue(color);
		CMainWindowTarget::GetInst()->GetGroup()->SetFogColor(d3dColor);
	}
}

LRESULT CGUIEditorDlg::RecieveMsg( uint32 message, WPARAM wParam, LPARAM lParam )
{
	return m_pGUIRootWnd->RecieveMsg(message, wParam, lParam);
}

IGraphic* CGUIEditorDlg::GetGraphic()
{
	return m_pGUIRootWnd->GetGraphic();
}

void CGUIEditorDlg::SetLanguageID( int nLanID )
{
	m_nCurLan = nLanID;
}

void CGUIEditorDlg::OnCbnSelchangeLanguageList()
{
	::CComboBox combTmp;
	combTmp.SubclassDlgItem(IDC_LANGUAGE_LIST, this);

	int32 nCurSel = combTmp.GetCurSel();
	m_WndGroup.SetCurLanguage(nCurSel);

	combTmp.UnsubclassWindow();
}

void CGUIEditorDlg::createAudioSystem()
{
	m_pAudioSystem = GetAudioModule()->GetAudioSystem();

	SQR_TRY
	{
		if (!m_pAudioSystem->Open())
		{
			MessageBox(L"请更新res/sound or resbin/sound, AudioSystem打开失败");
			return;
		}
		
		if( !m_pAudioSystem->LoadSoundBank("sound/gui", false) )
			MessageBox(L"gui 资源音效加载失败!");
	}
	SQR_CATCH(exp)
	{
		string str = exp.ErrorMsg();
		MessageBox(L"请更新res/sound or resbin/sound, 音效系统打开失败");
	}
	SQR_TRY_END;
}


void CGUIEditorDlg::PushResData(const wstring& win, const wstring& res)
{
	m_GUIResDlg.PushResData(win, res);
}