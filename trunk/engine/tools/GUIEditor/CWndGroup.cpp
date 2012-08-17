// CWndGroup.cpp : implementation file
//

#include <string.h>

#include "stdafx.h"
#include "GUIEditor.h"
#include "CWndGroup.h"
#include "GUIEditorDlg.h"
#include "WndStyle.h"
#include "SQRClock.h"
#include "SQRWndOffSet.h"
#include "CXmlConfig.inl"
#include "CodeCvs.h"
#include "../src/gui/SQRGUIManager.h"
#include "PkgStream.h"
#include "TraceHelper.h"
#include "CodeCvs.h"
#include "CFontMgr.h"
#include "CLangCfg.h"
#include "CPkgFile.h"
#include "CPathMgr.h"
#include "GUILayoutHandler.h"
#include "StringHelper.h"
#include "CFontCfg.h"
#include "CGuiCoder.h"
#include "SQRIcon.h"

#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#define  MAX_PATH  PATH_MAX
#else
#include <direct.h>
#endif


#ifndef _WIN32
#define _chdir chdir
#define _mkdir mkdir
#define _getcwd getcwd
#endif

#define Res_Template m_pMainDlg->GetGUIRootWnd()->GetWndHandle()->m_pGUI->m_ResTemplate


// CWndGroup

CWndGroup::CWndGroup( CGUIEditorDlg* pMainDlg )
: CFormView(CWndGroup::IDD)
, m_pMainDlg( pMainDlg )
, m_CursorID(IDC_EDITOR_ARROW)
, m_EditID(IDC_RADIO_POS)
, m_pCurEditWnd( NULL )
, ChoosehItem( NULL )
, m_RememberItem( NULL )
, bFocus( FALSE )
, m_pCurMainWnd(NULL)
, m_bResLoaded(false)
, m_Mark_Resize(false)
{
	m_pPrevEditWnd = NULL;
	m_pPrevMainWnd = NULL;
}

CWndGroup::~CWndGroup()
{
	if ( CLangCfg::HasInst() )
		CLangCfg::Destroy();

	if ( CFontCfg::HasInst() )
	{
		CFontCfg::Destroy();
	}
}

bool CWndGroup::InitCfg()
{
	// 读取xml获取路径
	wstring strConfigPath;
	wstring strVersionPath;

	strConfigPath  = L"./etc/GUIConfig.xml";
	strVersionPath = L"./VersionSetting.ini";

	ifstream strmConfig( strConfigPath.c_str() );
	if (!strmConfig)
	{
		GenErr("Can't open GUIConfig.xml");
		return false;
	}

	CXmlConfig *pGUIXmlCfg = new CXmlConfig("directory", strmConfig);
	strmConfig.close();

	string strRootPath = pGUIXmlCfg->Get<string>( "system_config","gui_res" );
	m_strRootPath.assign(utf8_to_utf16(strRootPath.c_str()));
	m_pMainDlg->SetGUIPath(extract_parent_dir(m_strRootPath));
	
	string strFxPath = pGUIXmlCfg->Get<string>( "system_config","fx_path" );
	m_pMainDlg->SetFxPath(utf8_to_utf16(strFxPath.c_str()));
	
	string strEmt_Res = pGUIXmlCfg->Get<string>( "system_config","emt_res" );
	m_pMainDlg->SetEmtPath(utf8_to_utf16(strEmt_Res.c_str()));

	string strFlash_Res = pGUIXmlCfg->Get<string>( "system_config","flash_res" );
	m_pMainDlg->SetFlashPath(utf8_to_utf16(strFlash_Res.c_str()));
	m_pMainDlg->SetTexFontPath(utf8_to_utf16(strFlash_Res.c_str()));
	m_pMainDlg->SetTexPath();

	string strPic_Res = pGUIXmlCfg->Get<string>( "system_config","Pic_res" );
	m_pMainDlg->SetPicResPath(utf8_to_utf16(strPic_Res.c_str()));
	
	// 初始化语言系统
	CLangCfg::Create();
	CLangCfg::GetInst()->LoadConfig("./LangConfig.xml");
	CFontCfg::Create();

	// 读取语言配置
	int nCurLan = 0;
	stringstream ss;
	string strLanguageId = pGUIXmlCfg->Get<string>( "language_config","language_id" );
	ss << strLanguageId;
	ss >> nCurLan;
	m_pMainDlg->SetLanguageID(nCurLan);

	// 拼接各级路径
	m_strAucPath.assign(m_strRootPath);
	m_strAucPath += L"\\*.auc";

	// use for xml
	m_strLayoutPath.assign(m_strRootPath);
	m_strLayoutPath.erase(m_strLayoutPath.length() - 10, 10 );
	m_strLayoutPath += L"src\\Layout\\*.xml";

	// 读取当前auf版本号
	uint32 Res = 0, Version = AUF_ADD_AUDIO;
	m_nAufVersion = Version;

	SafeDelete(pGUIXmlCfg);

	return true;
}

void CWndGroup::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WND_TREE, m_WndTree);
	DDX_Control(pDX, IDC_BTN_ALIGN_LEFT, m_AlignButton[Align_Left]);
	DDX_Control(pDX, IDC_BTN_ALIGN_RIGHT, m_AlignButton[Align_Right]);
	DDX_Control(pDX, IDC_BTN_ALIGN_TOP, m_AlignButton[Align_Top]);
	DDX_Control(pDX, IDC_BTN_ALIGN_BOTTOM, m_AlignButton[Align_Bottom]);
	DDX_Control(pDX, IDC_BTN_ALIGN_WIDTH, m_AlignButton[Align_Width]);
	DDX_Control(pDX, IDC_BTN_ALIGN_HEIGHT, m_AlignButton[Align_Height]);
}

BEGIN_MESSAGE_MAP(CWndGroup, CFormView)
	ON_BN_CLICKED(IDC_CLOCK, OnBnClickedClock)
	ON_BN_CLICKED(IDC_AWNDMOVE, OnBnClickedWndMove)
	ON_BN_CLICKED(IDC_AARROW, OnBnClickedArrow)
	ON_BN_CLICKED(IDC_ABUTTON, OnBnClickedButton)
	ON_BN_CLICKED(IDC_ACHECKBUTTON, OnBnClickedCheckbutton)
	ON_BN_CLICKED(IDC_ACOMBOBOX, OnBnClickedCombobox)
	ON_BN_CLICKED(IDC_AEDIT, OnBnClickedEdit)
	ON_BN_CLICKED(IDC_ARICHTEXT, OnBnClickedRichwnd)
	ON_BN_CLICKED(IDC_ALISTCTRL, OnBnClickedListctrl)
	ON_BN_CLICKED(IDC_APROGRESS, OnBnClickedProgress)
	ON_BN_CLICKED(IDC_ASCROLLBAR, OnBnClickedScrollbar)
	ON_BN_CLICKED(IDC_ASPIN, OnBnClickedSpin)
	ON_BN_CLICKED(IDC_ASTATIC, OnBnClickedStatic)
	ON_BN_CLICKED(IDC_ATREECTRL, OnBnClickedTreectrl)
	ON_BN_CLICKED(IDC_CHART, &CWndGroup::OnBnClickedChart)
	ON_BN_CLICKED(IDC_AICON, OnBnClickedAicon)
	ON_BN_CLICKED(IDC_VIDEO, OnBnClickedVideo)
	ON_BN_CLICKED(IDC_FLASHCONTROL, OnBnClickedFlashcontrol)
	ON_BN_CLICKED(IDC_IECONTROL, &CWndGroup::OnBnClickedIEcontrol)
	ON_BN_CLICKED(IDC_BTN_ALIGN_LEFT, OnBnClickedAlignLeft)
	ON_BN_CLICKED(IDC_BTN_ALIGN_RIGHT, OnBnClickedAlignRight)
	ON_BN_CLICKED(IDC_BTN_ALIGN_TOP, OnBnClickedAlignTop)
	ON_BN_CLICKED(IDC_BTN_ALIGN_BOTTOM, OnBnClickedAlignBottom)
	ON_BN_CLICKED(IDC_BTN_ALIGN_WIDTH, OnBnClickedAlignWidth)
	ON_BN_CLICKED(IDC_BTN_ALIGN_HEIGHT, OnBnClickedAlignHeight)

	ON_NOTIFY(TVN_KEYDOWN, IDC_WND_TREE, OnTvnKeydownWndTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_WND_TREE, OnTvnSelchangedWndTree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_WND_TREE, OnTvnEndlabeleditWndTree)
	ON_BN_CLICKED(IDC_IMPORT, OnBnClickedImport)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSaveCoder)
	ON_BN_CLICKED(IDC_SAVE_AS, OnBnClickedSaveAs)
	ON_BN_CLICKED(IDC_LOAD, OnBnClickedLoad)
	ON_NOTIFY(NM_CLICK, IDC_WND_TREE, &CWndGroup::OnNMClickWndTree)
	ON_NOTIFY(NM_KILLFOCUS, IDC_WND_TREE, &CWndGroup::OnNMKillfocusWndTree)
END_MESSAGE_MAP()
//ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
BOOL CWndGroup::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
					   const RECT& rect, ::CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	BOOL re = CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	UpdateData( FALSE );

	m_ImageList.Create( 16, 16, ILC_COLOR16|ILC_MASK, 0, 1 );
	m_WndTree.SetImageList( &m_ImageList, TVSIL_NORMAL );
	m_ImageList.Add( LoadIcon( ::AfxGetInstanceHandle(), MAKEINTRESOURCE( IDI_WNDG_NOR ) ) );
	m_ImageList.Add( LoadIcon( ::AfxGetInstanceHandle(), MAKEINTRESOURCE( IDI_WNDG_SEL ) ) );
	m_ImageList.Add( LoadIcon( ::AfxGetInstanceHandle(), MAKEINTRESOURCE( IDI_EDITORWND ) ) );
	m_ImageList.Add( LoadIcon( ::AfxGetInstanceHandle(), MAKEINTRESOURCE( IDI_EDITORWNDUNIT ) ) );
	m_hRoot = m_WndTree.InsertItem( L"窗口单元", 0, 1 );

	( ( ::CButton* )GetDlgItem( IDC_ABUTTON ) )		->SetIcon( AfxGetApp()->LoadIcon( IDI_BUTTON ) );
	( ( ::CButton* )GetDlgItem( IDC_AWNDMOVE ) )	->SetIcon( AfxGetApp()->LoadIcon( IDI_WNDMOVE ) );
	( ( ::CButton* )GetDlgItem( IDC_ACHECKBUTTON ) )->SetIcon( AfxGetApp()->LoadIcon( IDI_CHECKBUTTON ) );
	( ( ::CButton* )GetDlgItem( IDC_ACOMBOBOX ) )	->SetIcon( AfxGetApp()->LoadIcon( IDI_COMBOBOX ) );
	( ( ::CButton* )GetDlgItem( IDC_AEDIT ) )		->SetIcon( AfxGetApp()->LoadIcon( IDI_EDIT ) );
	( ( ::CButton* )GetDlgItem( IDC_ARICHTEXT ) )	->SetIcon( AfxGetApp()->LoadIcon( IDI_RICHWND ) );
	( ( ::CButton* )GetDlgItem( IDC_APROGRESS ) )	->SetIcon( AfxGetApp()->LoadIcon( IDI_PROGRESS ) );
	( ( ::CButton* )GetDlgItem( IDC_ASCROLLBAR ) )	->SetIcon( AfxGetApp()->LoadIcon( IDI_SCROLL ) );
	( ( ::CButton* )GetDlgItem( IDC_ASPIN ) )		->SetIcon( AfxGetApp()->LoadIcon( IDI_SPIN ) );
	( ( ::CButton* )GetDlgItem( IDC_ASTATIC ) )		->SetIcon( AfxGetApp()->LoadIcon( IDI_STATIC ) );
	( ( ::CButton* )GetDlgItem( IDC_ALISTCTRL ) )	->SetIcon( AfxGetApp()->LoadIcon( IDI_LISTCTRL ) );
	( ( ::CButton* )GetDlgItem( IDC_ATREECTRL ) )	->SetIcon( AfxGetApp()->LoadIcon( IDI_TREE ) );
	( ( ::CButton* )GetDlgItem( IDC_AARROW ) )		->SetIcon( AfxGetApp()->LoadIcon( IDI_ARROW ) );
	( ( ::CButton* )GetDlgItem( IDC_CLOCK ) )		->SetIcon( AfxGetApp()->LoadIcon( IDI_CLOCK) );
	( ( ::CButton* )GetDlgItem( IDC_CHART ) )		->SetIcon( AfxGetApp()->LoadIcon( IDI_CHART) );
	( ( ::CButton* )GetDlgItem( IDC_AICON ) )		->SetIcon( AfxGetApp()->LoadIcon( IDI_ICON) );
	( ( ::CButton* )GetDlgItem( IDC_VIDEO ) )		->SetIcon( AfxGetApp()->LoadIcon( IDI_VIDEO) );

	CheckRadioButton( IDC_ABUTTON, IDC_AARROW, IDC_AARROW );
	m_ComandList.Init( this );

	return re;
}

BOOL CWndGroup::DestroyWindow()
{
	HTREEITEM hMainWndItem = m_WndTree.GetChildItem( m_hRoot );
	while( hMainWndItem != NULL )
	{
		HTREEITEM hChild = m_WndTree.GetChildItem( hMainWndItem );
		while( hChild != NULL )
		{
			delete ( (SQRWnd*)m_WndTree.GetItemData( hChild ) );
			hChild = m_WndTree.GetNextItem( hChild, TVGN_NEXT );
		}
		delete ( (SQRWnd*)m_WndTree.GetItemData( hMainWndItem ) );
		hMainWndItem = m_WndTree.GetNextItem( hMainWndItem, TVGN_NEXT );
	}

	return CFormView::DestroyWindow();
}

CRect CWndGroup::CurWndRect(int n)
{
	CRect rect;

	if( m_CurPos != m_HitPos && m_EditID == IDC_RADIO_POS && m_CursorID != IDC_EDITOR_ARROW )
	{
		CFRect rt( min( m_HitPos.x, m_CurPos.x ), min( m_HitPos.y, m_CurPos.y ),
			max( m_HitPos.x, m_CurPos.x ), max( m_HitPos.y, m_CurPos.y ) );
		rt.left *= n;
		rt.right *= n;
		rt.top *= n;
		rt.bottom *= n;
		rt.left--;
		rt.top--;

		rect = rt;
	}
	else if( m_pCurEditWnd )
	{
		CFRect rt;
		m_pCurEditWnd->GetWndRect( rt );
		rt.left *= n;
		rt.right *= n;
		rt.top *= n;
		rt.bottom *= n;
		rt.left--;
		rt.top--;

		rect = rt;
	}
	return rect;
}

void CWndGroup::DrawEditLine( CGraphic* pGraphic, int n )
{
	if( m_CurPos != m_HitPos && m_EditID == IDC_RADIO_POS && m_CursorID != IDC_EDITOR_ARROW )
	{
		CFRect rt( min( m_HitPos.x, m_CurPos.x ), min( m_HitPos.y, m_CurPos.y ),
			max( m_HitPos.x, m_CurPos.x ), max( m_HitPos.y, m_CurPos.y ) );
		rt.left *= n;
		rt.right *= n;
		rt.top *= n;
		rt.bottom *= n;
		rt.left--;
		rt.top--;
		VerColor2D v[5]=
		{
			VerColor2D( rt.left,  rt.top,    0,	0xff0000ff ),
			VerColor2D( rt.right, rt.top,    0,	0xff0000ff ),
			VerColor2D( rt.right, rt.bottom, 0,	0xff0000ff ),
			VerColor2D( rt.left,  rt.bottom, 0,	0xff0000ff ),
			VerColor2D( rt.left,  rt.top,    0,	0xff0000ff ),
		};
		VerColor2D* pVB;
		pGraphic->GetRenderStack( RS_FONT, NULL, NULL, PT_LINESTRIP, 5, 4, 
			VerColor2D::Format, sizeof(VerColor2D), (void**)&pVB, NULL );
		memcpy( pVB, v, sizeof( VerColor2D )*5 );
	}
	else if( m_pCurEditWnd )
	{
		CFRect rt;
		m_pCurEditWnd->GetWndRect( rt );
		rt.left *= n;
		rt.right *= n;
		rt.top *= n;
		rt.bottom *= n;
		rt.left--;
		rt.top--;
		VerColor2D v[5]=
		{
			VerColor2D( rt.left,  rt.top,    0,	0xff0000ff ),
			VerColor2D( rt.right, rt.top,    0,	0xff0000ff ),
			VerColor2D( rt.right, rt.bottom, 0,	0xff0000ff ),
			VerColor2D( rt.left,  rt.bottom, 0,	0xff0000ff ),
			VerColor2D( rt.left,  rt.top,    0,	0xff0000ff ),
		};
		VerColor2D* pVB;
		RenderState * pRS;
		pGraphic->GetRenderStack( RS_FONT, NULL, NULL, PT_LINESTRIP, 5, 4, 
			VerColor2D::Format, sizeof(VerColor2D), (void**)&pVB, NULL, (void**)&pRS );
		pRS->m_ZTestEnable = false;
		pRS->m_ZTestFun    = CMP_ALWAYS;
		memcpy( pVB, v, sizeof( VerColor2D )*5 );
	}
}

void CWndGroup::Check()
{
	m_PreString.clear();
	m_ComandList.WriteCurEditToString( m_pCurEditWnd, m_PreString );
}

void CWndGroup::ReCheck()
{
	m_CurString.clear();
	m_ComandList.WriteCurEditToString( m_pCurEditWnd, m_CurString );
	if( m_CurString != m_PreString )
		m_ComandList.PushCommand( m_pCurEditWnd, eEditProp, m_CurString, m_PreString );
}

void CWndGroup::OnShow( UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	if( uMessage == WM_KEYDOWN && m_EditID != IDC_RADIO_PREVIEW )
	{
		if( wParam == 'Z' && ( GetKeyState( VK_CONTROL )&0x8000 ) )
			m_ComandList.UnDo();
		if( wParam == 'Y' && ( GetKeyState( VK_CONTROL )&0x8000 ) )
			m_ComandList.Redo();
		if( wParam == 'C' && ( GetKeyState( VK_CONTROL )&0x8000 ) )
			CtrlC();
		if( wParam == 'V' && ( GetKeyState( VK_CONTROL )&0x8000 ) )
			CtrlV();
		if( wParam == 'D' && ( GetKeyState( VK_CONTROL )&0x8000 ) )
			CtrlD();
		return;
	}

	SQRWnd* pCurEdit = m_pCurEditWnd;
	Check();
	switch( m_EditID )
	{
	case IDC_RADIO_POS:
		OnMessageEditPos( uMessage, wParam, lParam );
		break;
	case IDC_RADIO_TEX:
		OnMessageEditTex( uMessage, wParam, lParam );
		break;
	case IDC_RADIO_PREVIEW:
		OnMessagePreview( uMessage, wParam, lParam );
		break;
	default:
		break;
	}
	ReCheck();

	unsigned nFlag = EDIT_POSITION;
	if( pCurEdit != m_pCurEditWnd )
		nFlag |= EDIT_TEXTURE;
	if( m_EditID == IDC_RADIO_POS && m_CurString != m_PreString )
		UpdateToPropWnd( nFlag );
}

void CWndGroup::CtrlC()
{
	m_pPrevEditWnd = m_pCurEditWnd;
	m_pPrevMainWnd = m_pCurMainWnd;
}

void CWndGroup::CtrlV()
{
	if( m_pPrevMainWnd == NULL )
		return;

	if( m_pPrevEditWnd == m_pCurEditWnd && m_pPrevMainWnd == m_pCurMainWnd )
	{
		if( m_pPrevEditWnd->GetParent() == m_pPrevMainWnd )
		{
			HTREEITEM hMainWndItem = m_WndTree.GetChildItem( m_hRoot );
			while( hMainWndItem != NULL )
			{
				if( (SQRWnd*)m_WndTree.GetItemData( hMainWndItem ) == m_pCurMainWnd )
					break;
				hMainWndItem = m_WndTree.GetNextItem( hMainWndItem, TVGN_NEXT );
			}

			SQRWnd* pWnd = SQRWndClass::CreateWndObject( m_pPrevEditWnd->GetClassName() );
			WndCreateParam param;
			param.width = param.height = 100;
			param.pParentWnd = m_pPrevMainWnd;
			param.font_size = 12;
			pWnd->Create( param );
			string str;
			m_ComandList.WriteCurEditToString( m_pPrevEditWnd, str );
			m_ComandList.ReadFromString( pWnd, str );

			wchar_t Buf[256];
			wsprintf( Buf, L"%s%u", L"Child", time( NULL )*100 + ( GetTickCount()%1000 )/10 );
			HTREEITEM hChild = m_WndTree.InsertItem( Buf, 2, 2, hMainWndItem );
			m_WndTree.SetItemData( hChild, (DWORD)(LPARAM)pWnd );
			m_WndTree.Expand( hMainWndItem, TVE_EXPAND );
			m_pCurEditWnd = pWnd;

			for( int i = 1; ; i++ )
			{
				SQRWnd* pUint = pWnd->GetResWnd( i );
				if( !pUint )
					break;
				if( pUint == pWnd )
					continue;
				wsprintf( Buf, L"Child%04u", i );
				m_WndTree.SetItemData( m_WndTree.InsertItem( Buf, 3, 3, hChild ), (DWORD)(LPARAM)pUint );
			}
			m_WndTree.Expand( hChild, TVE_EXPAND );

			UpdateToPropWnd( EDIT_POSITION|EDIT_TEXTURE );
		}
	}
	else
	{
		if( string( m_pPrevEditWnd->GetClassName() ) == string( m_pCurEditWnd->GetClassName() ) )
		{
			string str;
			m_ComandList.WriteCurEditToString( m_pPrevEditWnd, str );
			m_ComandList.ReadFromString( m_pCurEditWnd, str );

			UpdateToPropWnd( EDIT_POSITION|EDIT_TEXTURE );
		}
	}
}

void CWndGroup::CtrlD()
{
	if( m_pPrevMainWnd == NULL )
		return;

	if( m_pPrevEditWnd != m_pCurEditWnd && strcmp( m_pPrevEditWnd->GetClassName(), m_pCurEditWnd->GetClassName() ) == 0 )
	{
		CFRect rtPre, rtCur;
		m_pPrevEditWnd->GetWndRect( rtPre );
		m_pCurEditWnd->GetWndRect( rtCur );
		m_pPrevEditWnd->SetWndRect( rtCur );

		IMAGE_PARAM IP;
		for( IP.CtrlMask = 1; ; IP.CtrlMask++ )
		{
			if( m_pPrevEditWnd->GetResWnd( IP.CtrlMask ) == NULL )
			{
				m_pPrevEditWnd->SetWndRect( rtPre );
				UpdateToPropWnd( EDIT_POSITION );
				return;
			}

			for( IP.StateMask = IP_MASK; IP.StateMask < IP_COUNT; IP.StateMask++ )
			{
				WND_IMAGE_LIST* pListSrc = m_pPrevEditWnd->GetWndBkImage( IP );
				if( pListSrc == NULL )
					continue;

				WND_IMAGE_LIST* pListDes = m_pCurEditWnd->GetWndBkImage( IP );

				for( int i = 0; i < pListSrc->GetImageCount(); i++ )
				{
					IMAGE& Image = pListSrc->GetImage(i);
					pListDes->AddImage( m_pCurEditWnd->GetGraphic(), -1, Image.pTexture ? Image.pTexture->GetName() : "",
						&Image.rtTexture, Image.posWnd, Image.dwColor, Image.dwLockFlag );
				}
			}
		}
	}
}

void CWndGroup::UpdateToPropWnd( unsigned nFlag )
{
	m_PropWnd->Update( nFlag );
	m_StyleWnd->Update( nFlag );
}

void CWndGroup::UpdateItemToProp()
{
	if (m_PropWnd)
	{
		m_PropWnd->m_PropList.ItemToProp();
	}
}

void CWndGroup::HideCurShowWin()
{
	if( m_pCurMainWnd )
		m_pCurMainWnd->ShowWnd( FALSE );
	for( size_t i = 0; i < mShowWndlist.size(); ++i )
		mShowWndlist[i]->ShowWnd(FALSE);

	mShowWndlist.clear();
	ClearSelectedWnd();
}


void CWndGroup::OnMessageEditPos( UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	SQRWnd* pCurWnd  = NULL;
	SQRWnd* pLastWnd = NULL;
	CFPos point( float(LOWORD(lParam)), float(HIWORD(lParam)) );
	
	switch( uMessage ) 
	{
	case WM_MOUSEMOVE:
		if( IDC_EDITOR_ARROW == m_CursorID || ( wParam&MK_LBUTTON ) == 0 )
		{
			m_HitPos = point;
			// 使用鼠标定义窗体大小，也需要m_Hitpos
			// 在当前鼠标在自定义大小状态
#if 0
			if ( IDC_EDITOR_ARROW == m_CursorID && m_pCurEditWnd)
			{
				// 判断是否可以进入定义大小状态
				CFRect rt;
				m_pCurEditWnd->GetWndRect( rt );

				if (  m_CurPos.x > rt.left - 3
					&&m_CurPos.x < rt.right + 3
					&&m_CurPos.y > rt.top - 3
					&&m_CurPos.y < rt.bottom + 3 )
				{
					if(  abs( m_CurPos.x - rt.left  ) < 3 
						|| abs( m_CurPos.x - rt.right ) < 3 )
					{
						::SetCursor( ::LoadCursor( NULL, IDC_SIZEWE ) );
						m_Mark_Resize = true;
					}
					else if (  abs( m_CurPos.y - rt.top   ) < 3
						|| abs( m_CurPos.y - rt.bottom) < 3 )
					{
						::SetCursor( ::LoadCursor( NULL, IDC_SIZENS ) );
						m_Mark_Resize = true;
					}
					else
					{
						::SetCursor( ::LoadCursor( NULL, IDC_ARROW ) );
						m_Mark_Resize = false;
					}
				}
				else
				{
					::SetCursor( ::LoadCursor( NULL, IDC_ARROW ) );
					m_Mark_Resize = false;
				}

				if ( m_Mark_Resize && ( wParam&MK_LBUTTON ) )
				{
					// 根据鼠标移动设置窗口的大小
					if ( abs( m_CurPos.x - rt.left  ) < 3 )
					{
						rt.left += point.x - m_CurPos.x;
					}
					else if ( abs( m_CurPos.x - rt.right ) < 3 )
					{
						rt.right += point.x - m_CurPos.x;
					}
					else if ( abs( m_CurPos.y - rt.top   ) < 3 )
					{
						rt.top += point.y - m_CurPos.y;
					}
					else if ( abs( m_CurPos.y - rt.bottom) < 3 )
					{
						rt.bottom += point.y - m_CurPos.y;
					}
					m_pCurEditWnd->SetWndRect(rt);

					m_CurPos = point;
					break;
				}
			}
			else
			{
				m_Mark_Resize = false;
			}
#endif
			

			if( wParam&MK_LBUTTON )
			{
				if( GetKeyState( VK_SPACE )&0x8000 )
				{
					if( m_pCurMainWnd )
					{
						CFRect rt;
						m_pCurMainWnd->GetWndRect( rt );
						rt = rt + (point - m_CurPos);
						m_pCurMainWnd->SetWndRect( rt );
					}
				}
				else
				{
					if( m_pCurEditWnd )
					{
						CFRect rt;
						m_pCurEditWnd->GetWndRect( rt );
						rt = rt + (point - m_CurPos);
						m_pCurEditWnd->SetWndRect( rt );
					}
				}
				//m_pMainDlg->UpdateRect();
			}
		}
		m_CurPos = point;
		break;

	case WM_LBUTTONUP:
		m_CurPos = point;
		if( IDC_EDITOR_ARROW != m_CursorID && m_HitPos != point )
			CreateChild();
		m_HitPos = point;
		break;

	case WM_LBUTTONDOWN:
		if( GetKeyState( VK_SPACE )&0x8000 || m_Mark_Resize )
			return;

		m_pMainDlg->RecieveMsg( WM_MBUTTONDBLCLK, wParam, lParam );
		
		if( NULL != m_pCurMainWnd )
		{
			pLastWnd = m_pCurMainWnd->GetLastMouseMsgWnd();
			findResWnd( pLastWnd, ChoosehItem );
		}
		if( pCurWnd != m_pCurEditWnd )
			m_PropWnd->m_PropList.ItemToProp( false );
	
		if( m_pCurEditWnd == m_pMainDlg->GetGUIRootWnd() )
			m_pCurEditWnd = NULL;
		else if( m_pCurEditWnd != m_pCurMainWnd )
		{
			CheckRadioButton( IDC_ABUTTON, IDC_AARROW, IDC_AARROW );
			CheckRadioButton( IDC_RADIO_POS, IDC_RADIO_PREVIEW, IDC_RADIO_POS );
			m_CursorID = IDC_EDITOR_ARROW;
		}
		::SetCursor( ::LoadCursor( NULL, IDC_ARROW ) );
		break;
	default:
		break;
	}

}

void CWndGroup::OnMessageEditTex( UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	if( m_pCurEditWnd == NULL )
		return;

	SQRWnd* pLastWnd = m_pCurEditWnd->GetLastMouseMsgWnd();
	if( NULL != pLastWnd && NULL != ChoosehItem )
	{
		findResWnd( pLastWnd, ChoosehItem );
	}
	m_PropWnd->MoveText( uMessage, wParam, lParam );

}

void CWndGroup::OnMessagePreview( UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	m_pMainDlg->RecieveMsg( uMessage, wParam, lParam );
	if( WM_SETCURSOR == uMessage )
		::SetCursor( ::LoadCursor( NULL, IDC_ARROW ) );
}

void CWndGroup::InitWnd( SQRWnd* pWnd )
{
	if( pWnd->GetClassName() == string( "CTreeCtrl" ) )
	{
		WND_IMAGE_LIST List;
		CFRect frt( 0, 0, 16, 16 );
		List.AddImage( m_pMainDlg->GetGraphic(), 0, "", &frt, CFPos(0.0f, 0.0f), 0x80ffff00 );
		SQRTreeCtrl* pTree = static_cast<SQRTreeCtrl*>(pWnd);

		HTREENODE pNode;
		pNode = pTree->InsertNode( NULL, "一级树节点" );

		pTree->InsertNode( pNode, "二级树节点", &List, &List, 1 );	// 有首尾图
		pNode = pTree->InsertNode( pNode, "二级树节点" );			// 无首尾图
	}

	if( pWnd->GetClassName() == string( "SQRListCtrl" ) )
	{
		SQRListCtrl* pList = static_cast<SQRListCtrl*>(pWnd);
		pList->InsertColumn( 0, pList->GetWndWidth() );

		for( int i = 0; i < 2; i++)
		{
			pList->InsertItem( i, 20 );
			pList->GetSubItem( i, 0 )->SetWndText( "列表元素" );
		}			
	}
}

void CWndGroup::CreateChild()
{
	CFRect rt( min( m_HitPos.x, m_CurPos.x ), min( m_HitPos.y, m_CurPos.y ),
		max( m_HitPos.x, m_CurPos.x ), max( m_HitPos.y, m_CurPos.y ) );

	if( !m_pCurMainWnd || m_CursorID == IDC_EDITOR_ARROW )
		return;

	HTREEITEM hMainWndItem = m_WndTree.GetChildItem( m_hRoot );
	while( hMainWndItem != NULL )
	{
		if( (SQRWnd*)m_WndTree.GetItemData( hMainWndItem ) == m_pCurMainWnd )
			break;;
		hMainWndItem = m_WndTree.GetNextItem( hMainWndItem, TVGN_NEXT );
	}

	m_pCurMainWnd->ScreenToWnd( rt );
	SQRWnd* pChild = NULL;
	if( m_CursorID == IDC_EDITOR_CLOCK )		pChild = new SQRClock;
	if( m_CursorID == IDC_EDITOR_WNDMOVE )		pChild = new SQRWndOffSet;
	if( m_CursorID == IDC_EDITOR_BUTTON )		pChild = new SQRButton;
	if( m_CursorID == IDC_EDITOR_CHECKBUTTON )	pChild = new SQRCheckButton;
	if( m_CursorID == IDC_EDITOR_COMBOBOX )		pChild = new SQRComboBox;
	if( m_CursorID == IDC_EDITOR_EDIT )			pChild = new SQREdit;
	if( m_CursorID == IDC_EDITOR_RICHWND )		pChild = new SQRRichText;
	if( m_CursorID == IDC_EDITOR_PROGRESS )		pChild = new SQRProgress;
	if( m_CursorID == IDC_EDITOR_SCROLLBAR )	pChild = new SQRScrollBar;
	if( m_CursorID == IDC_EDITOR_SPIN )			pChild = new SQRSpin;
	if( m_CursorID == IDC_EDITOR_STATIC )		pChild = new SQRStatic;
	if( m_CursorID == IDC_EDITOR_TREECTRL )		pChild = new SQRTreeCtrl;
	if( m_CursorID == IDC_EDITOR_LISTCTRL )		pChild = new SQRListCtrl;
	if( m_CursorID == IDC_EDITOR_CHART )		pChild = new SQRChart;
	if( m_CursorID == IDC_EDITOR_VIDEO )		pChild = new SQRVideo;

	if( m_CursorID == IDC_EDITOR_FLASHCONTROL )		pChild = new SQRFlashWnd;
	if( m_CursorID == IDC_EDITOR_IECONTROL )		pChild = new SQRWebBrowser;

	if( pChild )
	{
		WndCreateParam param;
		param.szTitle = pChild->GetClassName();
		param.uStyle = WS_CHILD;
		param.x = rt.left;
		param.y = rt.top;
		param.width = rt.Width();
		param.height = rt.Height();
		param.pParentWnd = m_pCurMainWnd;
		param.font_size = 14.0f;
		pChild->Create( param );
		wchar_t Buf[256];
		wsprintf( Buf, L"%s%u", L"Child", time( NULL )*100 + ( GetTickCount()%1000 )/10 );
		HTREEITEM hChild = m_WndTree.InsertItem( Buf, 2, 2, hMainWndItem );
		m_WndTree.SetItemData( hChild, (DWORD)(LPARAM)pChild );
		m_WndTree.Expand( hMainWndItem, TVE_EXPAND );
		m_pCurEditWnd = pChild;

		for( int i = 1; ; i++ )
		{
			SQRWnd* pUint = pChild->GetResWnd( i );
			if( !pUint )
				break;
			if( pUint == pChild )
				continue;
			wsprintf( Buf, L"Child%04u", i );
			m_WndTree.SetItemData( m_WndTree.InsertItem( Buf, 3, 3, hChild ), (DWORD)(LPARAM)pUint );
		}
		m_WndTree.Expand( hChild, TVE_EXPAND );

		InitWnd( pChild );
	}
}

void CWndGroup::OnTvnKeydownWndTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if( pTVKeyDown->wVKey == VK_INSERT )
	{
		HTREEITEM hChildItem = m_WndTree.GetChildItem( m_hRoot );
		while( hChildItem != NULL )
		{
			SQRWnd* pWnd = (SQRWnd*)m_WndTree.GetItemData( hChildItem );
			pWnd->ShowWnd( FALSE );
			hChildItem = m_WndTree.GetNextItem(hChildItem, TVGN_NEXT);
		}
		if ( m_CursorID == IDC_EDITOR_ARROW )			
		{
			m_pCurMainWnd = new SQRWnd;
		}
		else 
		{
			MessageBox(L"根窗口只能够是CDialog", L"", 0);
			return;
		}
		if( IDC_EDITOR_CLOCK  == m_CursorID )		m_pCurMainWnd = new SQRClock;
		if( IDC_EDITOR_ARROW  == m_CursorID )		m_pCurMainWnd = new SQRWnd;
		if( IDC_EDITOR_BUTTON == m_CursorID )		m_pCurMainWnd = new SQRButton;
		if( IDC_EDITOR_CHECKBUTTON == m_CursorID )	m_pCurMainWnd = new SQRCheckButton;
		if( IDC_EDITOR_COMBOBOX == m_CursorID )		m_pCurMainWnd = new SQRComboBox;
		if( IDC_EDITOR_EDIT == m_CursorID )			m_pCurMainWnd = new SQREdit;
		if( IDC_EDITOR_RICHWND == m_CursorID )		m_pCurMainWnd = new SQRRichText;
		if( IDC_EDITOR_PROGRESS == m_CursorID )		m_pCurMainWnd = new SQRProgress;
		if( IDC_EDITOR_SCROLLBAR == m_CursorID )	m_pCurMainWnd = new SQRScrollBar;
		if( IDC_EDITOR_SPIN == m_CursorID )			m_pCurMainWnd = new SQRSpin;
		if( IDC_EDITOR_STATIC == m_CursorID )		m_pCurMainWnd = new SQRStatic;
		if( IDC_EDITOR_TREECTRL == m_CursorID )		m_pCurMainWnd = new SQRTreeCtrl;
		if( IDC_EDITOR_LISTCTRL == m_CursorID )		m_pCurMainWnd = new SQRListCtrl;
		if( IDC_EDITOR_CHART == m_CursorID )		m_pCurMainWnd = new SQRChart;
		if( IDC_EDITOR_AICON == m_CursorID )		m_pCurMainWnd = new SQRIcon;
		if( IDC_EDITOR_VIDEO == m_CursorID )		m_pCurMainWnd = new SQRVideo;

		if( IDC_EDITOR_FLASHCONTROL == m_CursorID )		m_pCurMainWnd = new SQRFlashWnd;
		if( IDC_EDITOR_IECONTROL == m_CursorID )		m_pCurMainWnd = new SQRWebBrowser;

		wchar_t Buf[256];
		wsprintf( Buf, L"%s%u", L"GUI", time( NULL )*100 + ( GetTickCount()%1000 )/10 );
		hChildItem = m_WndTree.InsertItem( Buf, 2, 2, m_hRoot );
		string	u8_str = utf16_to_utf8(Buf);

		WndCreateParam param;
		param.szTitle = u8_str.c_str();
		param.width = 500;
		param.height = 300;
		param.pParentWnd = m_pMainDlg->GetGUIRootWnd();
		param.font_size = 14;
		m_pCurMainWnd->Create( param );
		m_WndTree.SetItemData( hChildItem, (DWORD)(LPARAM)m_pCurMainWnd );
		m_WndTree.Expand( m_hRoot, TVE_EXPAND );
		m_pCurEditWnd = m_pCurMainWnd;

		InitWnd( m_pCurMainWnd );

		UpdateToPropWnd( EDIT_POSITION|EDIT_TEXTURE );
	}
	if ( pTVKeyDown->wVKey == VK_DELETE )
	{
		HTREEITEM hSel = m_WndTree.GetSelectedItem();
		if( hSel == m_hRoot )
			return;
		int nImage;
		m_WndTree.GetItemImage( hSel, nImage, nImage );
		if( nImage == 3 )
			return;

		SQRWnd* pWnd = (SQRWnd*)m_WndTree.GetItemData( hSel );
		if( pWnd == m_pCurMainWnd )
		{
			m_DelFileNameList.push_back(m_CreateInfo[pWnd].szFileName);	//add by lx
			m_pCurMainWnd = NULL;
			SQRWnd* pChildWnd = pWnd->FindFirstChild();
			while( pChildWnd )
			{
				SQRWnd* pCur = pChildWnd;
				pChildWnd = pChildWnd->FindNextWnd();
				if (pCur->IsSysCreated())
				{
					m_CreateInfo.erase( pCur );
					delete pCur;
				}
			}
		}

		m_CreateInfo.erase( pWnd );
		m_pCurEditWnd = NULL;
		m_WndTree.DeleteItem( hSel );
		delete pWnd;
		pWnd = NULL;

		UpdateToPropWnd( EDIT_POSITION|EDIT_TEXTURE );
	}

	if( pTVKeyDown->wVKey == 'C' && ( GetKeyState( VK_CONTROL )&0x8000 ) )
		CtrlC();
	if( pTVKeyDown->wVKey == 'V' && ( GetKeyState( VK_CONTROL )&0x8000 ) )
		CtrlV();
	if( pTVKeyDown->wVKey == 'D' && ( GetKeyState( VK_CONTROL )&0x8000 ) )
		CtrlD();
}

//这里做一些修改，因为只有选中的元素才可能被选中子项
void CWndGroup::findResWnd( SQRWnd *pWnd, HTREEITEM hItem )
{
	SQRWnd* pWndEx = (SQRWnd*)m_WndTree.GetItemData( hItem );
	if( pWnd == pWndEx )
	{
		m_WndTree.SelectItem( hItem );
		if( !(GetKeyState( VK_CONTROL )&0x8000 ) ) 
		{
			mChooseWndlist.clear();	
		}
		addMoveChildWnd( pWndEx );
		return; 
	}
	HTREEITEM hChildItem = m_WndTree.GetChildItem( hItem );
	while( hChildItem != NULL )
	{
		pWndEx = (SQRWnd*)m_WndTree.GetItemData( hChildItem );
		if( pWnd == pWndEx )
		{
			m_WndTree.SelectItem( hChildItem );
			
			if( !(GetKeyState( VK_CONTROL )&0x8000) ) 
			{
				mChooseWndlist.clear();	
			}
			addMoveChildWnd( pWndEx );
			return;
		}
		hChildItem = m_WndTree.GetNextItem(hChildItem, TVGN_NEXT);
	}
	HTREEITEM Item = m_WndTree.GetParentItem( hItem );
	if( NULL != Item )
	{
		findResWnd( pWnd, Item );
	}	
}

void CWndGroup::addMoveChildWnd( SQRWnd *pWnd )
{
	for( size_t i = 0; i < mChooseWndlist.size(); ++i ) 
	{
		if( pWnd == mChooseWndlist[i] )
		{
			break;
		}
	}
	bFocus = FALSE;
	mChooseWndlist.push_back( pWnd );
}

void CWndGroup::OnTvnSelchangedWndTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM hChildItem = m_WndTree.GetChildItem( m_hRoot );
	while( hChildItem != NULL )
	{
		SQRWnd* pWnd = (SQRWnd*)m_WndTree.GetItemData( hChildItem );
		pWnd->ShowWnd( FALSE );
		hChildItem = m_WndTree.GetNextItem(hChildItem, TVGN_NEXT);
	}

	// 记录数据，更新显示
	if ( m_EditID != IDC_RADIO_PREVIEW )
	{
		if ( !(pNMTreeView->itemNew.hItem) )
		{
			return;
		}
		SQRWnd* pCurWnd = (SQRWnd*)m_WndTree.GetItemData( pNMTreeView->itemNew.hItem );
		if( !pCurWnd )
		{
			m_pCurMainWnd = NULL;
			m_pCurEditWnd = NULL;
			UpdateToPropWnd( EDIT_POSITION|EDIT_TEXTURE );
			return;
		}

		SQRWnd* pCurMain = pCurWnd;
		while( pCurMain->GetParent() != m_pMainDlg->GetGUIRootWnd() && pCurMain != m_pMainDlg->GetGUIRootWnd() )
			pCurMain = pCurMain->GetParent();

		if( pCurWnd != m_pCurEditWnd )
			m_PropWnd->m_PropList.ItemToProp();
		///当前编辑wnd的主窗口
		m_pCurMainWnd = pCurMain;
		///当前编辑wnd
		m_pCurEditWnd = pCurWnd;

		if( m_pCurMainWnd )
			m_pCurMainWnd->ShowWnd( TRUE );
		if( GetKeyState( VK_CONTROL )&0x8000 )
		{
			mShowWndlist.push_back( m_pCurMainWnd );
			for( size_t i = 0; i < mShowWndlist.size(); ++i )
			{
				mShowWndlist[i]->ShowWnd(true);
			}
		}
		else
		{
			mShowWndlist.clear();
			ClearSelectedWnd();
		}
		if(m_pCurEditWnd)
			PushSelectedWnd(m_pCurEditWnd);
		UpdateToPropWnd( EDIT_POSITION|EDIT_TEXTURE );

		ChoosehItem = pNMTreeView->itemNew.hItem;
		bFocus = TRUE;
		m_pImageProp->m_vChooseImgList.clear();
		m_pMainDlg->UpdateRect();

		// 记录窗体
		m_RememberItem = ChoosehItem;
	}
	else
	{
		// 仅预览使用
		if ( !(pNMTreeView->itemNew.hItem) )
		{
			return;
		}
		SQRWnd* pCurWnd = (SQRWnd*)m_WndTree.GetItemData( pNMTreeView->itemNew.hItem );
		if( !pCurWnd )
		{
			m_pCurMainWnd = NULL;
			m_pCurEditWnd = NULL;
			return;
		}

		SQRWnd* pCurMain = pCurWnd;
		while( pCurMain->GetParent() != m_pMainDlg->GetGUIRootWnd() && pCurMain != m_pMainDlg->GetGUIRootWnd() )
			pCurMain = pCurMain->GetParent();

		m_pCurMainWnd = pCurMain;
		m_pCurEditWnd = pCurWnd;

		if( m_pCurMainWnd )
			m_pCurMainWnd->ShowWnd( TRUE );
		if( GetKeyState( VK_CONTROL )&0x8000 )
		{
			mShowWndlist.push_back( m_pCurMainWnd );
			for( size_t i = 0; i < mShowWndlist.size(); ++i )
			{
				mShowWndlist[i]->ShowWnd(true);
			}
		}
		else
		{
			mShowWndlist.clear();
			ClearSelectedWnd();
		}
		if(m_pCurEditWnd)
			PushSelectedWnd(m_pCurEditWnd);
		ChoosehItem = pNMTreeView->itemNew.hItem;
		bFocus = TRUE;
		m_pImageProp->m_vChooseImgList.clear();
		m_pMainDlg->UpdateRect();
	}
}

void CWndGroup::OnTvnEndlabeleditWndTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hChildItem = m_WndTree.GetChildItem( m_hRoot );
	CString pText = pTVDispInfo->item.pszText;
	if( pText.GetLength() > 0 )
	{
		LPTSTR p = pText.GetBuffer();
		if ( !( ( p[0] >= 65 && p[0] <= 90 ) || ( p[0] >= 97 && p[0] <= 122 ) ) )
		{
			MessageBox(L"名称要以英文字母开头");
			return;
		}

		while( hChildItem != NULL )
		{
			CString text = m_WndTree.GetItemText( hChildItem );
			hChildItem   = m_WndTree.GetNextItem(hChildItem, TVGN_NEXT);

			if( 0 == text.Compare( pTVDispInfo->item.pszText ) )
			{
				MessageBox(L"有同样名字的窗体了");
				return;
			}
		}
	}
	*pResult = 0;
	if( pTVDispInfo->item.pszText && pTVDispInfo->item.pszText[0] )
		m_WndTree.SetItemText( pTVDispInfo->item.hItem, pTVDispInfo->item.pszText );
	UpdateToPropWnd( EDIT_POSITION|EDIT_TEXTURE );
}

void CWndGroup::OnBnClickedImport()
{
	// TODO: Add your control notification handler code here
	static wchar_t Filter[] = L"GUI File (*.AIF)|*.AIF||";
	CString AIFPath = GetRegistKeyStr( L"Software\\GUIEditor", L"AIFPath" );
	AIFPath = GetFileName( TRUE, Filter, NULL, AIFPath );
	if( AIFPath.IsEmpty() )
		return;

	m_strFile = AIFPath;
	m_strFile.MakeLower();
	m_strFile.Replace( L".aif", L".auf" );
	SetWindowText( L"GUIEditor-" + m_strFile );

	SetRegistKeyStr( L"Software\\GUIEditor", L"AIFPath", AIFPath );

	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	ifstream File( AIFPath, ios::binary|ios::in );
	locale::global(loc);

	DWORD Res;
	DWORD Version;
	File.read( (char*)&Res, sizeof(DWORD) );
	File.read( (char*)&Version, sizeof(DWORD) );

	if( Res != MAKEFOURCC( 'A', 'I', 'F', 0 ) || Version != AUF_VERSION - 1 )
		return;

	switch(Version)
	{
	case 10003:
		{
			int nMainCount;
			File.read( (char*)&nMainCount, sizeof(int) );
			for( int i = 0; i < nMainCount; i++ )
			{
				HTREEITEM hMainItem = ImportItem_03( File, m_hRoot, m_pMainDlg->GetGUIRootWnd() );
				int nChildCount;
				File.read( (char*)&nChildCount, sizeof(int) );
				for( int j = 0; j < nChildCount; j++ )
					ImportItem_03( File, hMainItem, (SQRWnd*)m_WndTree.GetItemData( hMainItem ) );
				if( !i )
					m_WndTree.Expand( hMainItem, TVE_EXPAND );
			}
			m_WndTree.Expand( m_hRoot, TVE_EXPAND );
			NMTREEVIEWA a;
			HRESULT h;
			a.itemNew.hItem = m_hRoot;
			OnTvnSelchangedWndTree( reinterpret_cast<NMHDR*>(&a), &h );
		}
		break;
	case 10004:
		break;
	case 10005:
		break;
	}

}

HTREEITEM CWndGroup::ImportItem_03( ifstream& File, HTREEITEM hItem, SQRWnd* pParent )
{
	int Len;
	char szChildName[256];
	memset( szChildName, 0, 256 );
	File.read( (char*)&Len, sizeof(int) );
	File.read( szChildName, Len );

	int		ClassLen;
	char	szClass[256];
	int		WndTextLen;
	char	szWndText[256];
	DWORD	wsStyle;
	int		x;
	int		y;
	int		w;
	int		h;
	DWORD	GroupID;
	int     FNameLen;
	char	szFontName[256];
	int		nFontSize;

	memset( szClass, 0, 256 );
	memset( szWndText, 0, 256 );
	memset( szFontName, 0, 256 );
	File.read( (char*)&ClassLen, sizeof(int) );
	File.read( (char*)szClass, ClassLen );
	File.read( (char*)&WndTextLen, sizeof(int) );
	File.read( (char*)szWndText, WndTextLen );
	File.read( (char*)&wsStyle, sizeof(DWORD) );
	File.read( (char*)&x, sizeof(int) );
	File.read( (char*)&y, sizeof(int) );
	File.read( (char*)&w, sizeof(int) );
	File.read( (char*)&h, sizeof(int) );
	File.read( (char*)&GroupID, sizeof(DWORD) );
	File.read( (char*)&FNameLen, sizeof(int) );
	File.read( (char*)szFontName, FNameLen );
	File.read( (char*)&nFontSize, sizeof(int) );

	SQRWnd* pWnd = SQRWndClass::CreateWndObject( szClass );
	
	WndCreateParam param;
	param.szTitle = szWndText;
	param.uStyle = wsStyle;
	param.x = (float)x;
	param.y = (float)y;
	param.width = (float)w;
	param.height = (float)h;
	param.pParentWnd = pParent;
	param.nFontID = string(szFontName).find("楷体") != string::npos ? 0 : 1;
	param.font_size = (float)nFontSize;
	param.uGroupID = GroupID;
	pWnd->Create( param );
	hItem = m_WndTree.InsertItem( utf8_to_utf16(szChildName).c_str(), 2, 2, hItem );
	m_WndTree.SetItemData( hItem, (DWORD)(LPARAM)pWnd );


	int nImageCount;
	File.read( (char*)&nImageCount, sizeof(int) );
	for( int i = 0; i < nImageCount; i++ )
	{
		IMAGE_PARAM IP;
		int Len;
		char TextName[256];
		DWORD Color;
		RECT rt;
		memset( TextName, 0, 256 );
		File.read( (char*)&IP, sizeof(IMAGE_PARAM) );
		File.read( (char*)&Len, sizeof(int) );
		File.read( TextName, Len );
		File.read( (char*)&Color, sizeof( DWORD ) );
		File.read( (char*)&rt, sizeof( RECT ) );

		CFRect frt = rt;
		WND_IMAGE_LIST List;
		List.AddImage( m_pMainDlg->GetGraphic(), 0, TextName, &frt, CFPos(0.0f, 0.0f), Color, 0 );
		pWnd->SetWndBkImage( IP, List );
	}

	for( int nCtrlMask = 2; ; nCtrlMask++ )
	{
		SQRWnd* pChild = pWnd->GetResWnd( nCtrlMask );
		if( !pChild )
			break;
		wchar_t Buf[256];
		SQRWnd* pUint = pChild->GetResWnd( nCtrlMask );
		wsprintf( Buf, L"Child%04u", nCtrlMask );
		m_WndTree.SetItemData( m_WndTree.InsertItem( Buf, 3, 3, hItem ), (DWORD)(LPARAM)pUint );
		m_WndTree.Expand( hItem, TVE_EXPAND );
	}
	return hItem;
}

void CWndGroup::WinListCreateFromRes()
{
	SQRWnd* ParentWnd = m_pMainDlg->GetGUIRootWnd();

	// 从资源加载列表
	UIMap<UIString, WndResParam*>::iterator itr = Res_Template.begin();
	while( itr != Res_Template.end() )
	{
		if ( !ParentWnd || !ParentWnd->GetWndHandle()->m_pGUI )
			return;
		SQRGUIManager* pGUI		= ParentWnd->GetWndHandle()->m_pGUI;
		WndResParam* pParam	= itr->second;
		if ( !pParam )
			return;
		string szUseClass = "SQRDialog";
		SQRWnd* pWnd = SQRWndClass::CreateWndObject( szUseClass.c_str() );
		if( !pWnd )
			break;

		WndCreateParam param;
		wstring tt_win_name = utf8_to_utf16(itr->first.c_str());
		param.szTitle		= pParam->m_WindowTitle;
		param.uStyle		= pParam->m_WndStyle;
		param.x			 = (float)pParam->m_x;
		param.y			 = (float)pParam->m_y;
		param.width		 = (float)pParam->m_Width;
		param.height	 = (float)pParam->m_Height;
		//param.szSource   = pParam->m_SourceName;//add by guo
		param.pParentWnd = ParentWnd;
		GUI_FONT* pFont = pWnd->GetFontInfo(pParam->m_uStyleIndex);
		if (pFont)
		{
			param.nFontID	 = pFont->m_nFontID;
			param.font_size  = (float)pFont->m_FontSize;
		}
		else
		{
			param.nFontID = pParam->m_nFontID;
			param.font_size  = (float)pParam->m_FontSize;
		}
		
		param.uGroupID   = pParam->m_GroupID;

		int32 re = pWnd->Create( param );
		if ( re == GERROR )
		{
			delete pWnd;
		}


		pWnd->SetLockFlag( pParam->m_uLockFlage );
		if (pFont)
		{
			pWnd->SetFontGradualColor( pFont->m_uFontGradualColor );
			pWnd->SetFontEffectMask( pFont->m_uFontEffectMask );
		}
		else
		{
			pWnd->SetFontGradualColor( pParam->m_uFontGradualColor );
			pWnd->SetFontEffectMask( pParam->m_uFontEffectMask );
		}
		
		pWnd->SetIsDrawBackground( pParam->m_isShowBackground );
		pWnd->SetStyleIndex( pParam->m_uStyleIndex );
		pWnd->SetMouseOverDesc( pParam->m_szDesc );
		//pWnd->SetszSourceName( pParam->m_SourceName );//add by guo

		// effect
		pWnd->SetMsgFx( eMF_Click,		pParam->m_MsgFx[eMF_Click] );
		pWnd->SetMsgFx( eMF_Open,		pParam->m_MsgFx[eMF_Open] );
		pWnd->SetMsgFx( eMF_Close,		pParam->m_MsgFx[eMF_Close] );

		// sound
		pWnd->SetMsgSound( eMS_Click,	pParam->m_MsgSound[eMS_Click] );
		pWnd->SetMsgSound( eMS_Open,	pParam->m_MsgSound[eMS_Open] );
		pWnd->SetMsgSound( eMS_Close,	pParam->m_MsgSound[eMS_Close] );

		///flash info
		UIMap<EventStateMask, UIString>::iterator flsitr = pParam->m_FlashWndStr.begin();
		UIMap<EventStateMask, UIString>::iterator flsitrend = pParam->m_FlashWndStr.end();
		while ( flsitr != flsitrend )
		{
			if (flsitr->first < IP_COUNT)
				pWnd->SetFlashInfo( flsitr->first, flsitr->second.c_str() );
			++flsitr;
		}


		pWnd->GetWndHandle()->m_pResParam	= pParam;
		pWnd->GetWndHandle()->m_WindowName	= itr->first;
		


		//创建透明纹理
		pWnd->GetWndHandle()->m_strTransName	= pParam->m_TransTexture;
		if( !pWnd->GetWndHandle()->m_strTransName.empty() )
		{
			pWnd->GetGraphic()->CreateTexture( L"", 
				(pWnd->GetWndHandle()->m_strTransName).c_str() 
				, &( pWnd->GetWndHandle()->m_pTransparenTex ) );

			if (NULL == pWnd->GetWndHandle()->m_pTransparenTex)
			{
				MessageBox(L"create texture falied!");
			}
			if ( m_pMainDlg )
			{
				wstring tt_tex_name = utf8_to_utf16((pWnd->GetWndHandle()->m_strTransName).c_str());
				if (tt_tex_name.length() > 0)
					m_pMainDlg->PushResData(tt_win_name, tt_tex_name);
			}
		}

		m_CreateInfo[pWnd].szClassName		= utf8_to_utf16(pParam->m_UseClassName);
		m_CreateInfo[pWnd].szCreateInfo	= utf8_to_utf16(pParam->m_UseInfo);


		CString t_str1 = m_strRootPath.c_str();	
		t_str1.Delete( t_str1.GetLength() - 8, 8 );
		CString t_str2 = L"src\\Layout\\ybtx.";
		CString t_str3 = utf8_to_utf16(itr->first.c_str()).c_str();	
		CString t_str4 = L".xml";

		m_CreateInfo[pWnd].szFileName		= t_str1 + t_str2 + t_str3 + t_str4;

		HTREEITEM MainItem = m_WndTree.InsertItem( utf8_to_utf16(itr->first.c_str()).c_str(), 2, 2, m_hRoot );
		m_WndTree.SetItemData( MainItem, (DWORD)(LPARAM)pWnd );
		m_FileNameList.push_back( utf8_to_utf16(itr->first.c_str()).c_str() );

		for ( int32 i = 0; i < (int32)pParam->m_ImageList.size(); i++ )
		{
			ChildWndInfo& Info = pParam->m_ImageList[i];
			if ( i )
			{
				pWnd->GetResWnd( i + 1 )->SetWndRect( Info.rtWnd );
				pWnd->GetResWnd( i + 1 )->SetStyleIndex(Info.m_uStyleIndex);
				pWnd->GetResWnd( i + 1 )->SetLockFlag(Info.m_uLockFlag);

				wchar_t Buf[256];
				wsprintf( Buf, L"Child%04u", i );
				m_WndTree.SetItemData( 
					m_WndTree.InsertItem( Buf, 3, 3, MainItem )
					, (DWORD)(LPARAM)pWnd->GetResWnd( i + 1 ) );
			}

			for ( int32 j = 0; j < (int32)Info.ChildBkList.size(); j++ )
			{
				IMAGE_PARAM IP;
				IP.CtrlMask= WORD( i + 1 );
				IP.StateMask= WORD( Info.ChildBkList[j].nStateMask );
				WND_IMAGE_LIST* pImage = pWnd->GetWndBkImage( IP );

				if (pFont)
				{
					pWnd->SetWndTextColor( IP, pFont->m_EventFont[(EventStateMask)(IP.StateMask)].m_FontColor );
					pWnd->SetWndTextBcColor( IP, pFont->m_EventFont[(EventStateMask)(IP.StateMask)].m_uFontBkColor );
				}
				else
				{
					pWnd->SetWndTextColor( IP, Info.ChildBkList[j].uTextColor );
					pWnd->SetWndTextBcColor( IP, Info.ChildBkList[j].uRimColor );
				}
				
				if ( pImage )
				{
					*pImage = pWnd->GetWndHandle()->m_pGUI->LoadBKRes( Info.ChildBkList[j].ImageList );

					if ( m_pMainDlg )
					{
						for (int32 ct_img = 0; ct_img < pImage->GetImageCount(); ++ct_img)
						{
							IMAGE& img = pImage->GetImage(ct_img);
							wstring tt_tex_name = utf8_to_utf16(img.GetTexName());
							if (tt_tex_name.length() > 0)
								m_pMainDlg->PushResData(tt_win_name, tt_tex_name);
						}
					}
				}
					
			}
		}


		vector<SQRWnd*> ChildList;
		UIMap<UIString, WndResParam*>::iterator it = pParam->m_Child.begin();
		for ( ; it != pParam->m_Child.end(); it++ )
		{
			WndResParam& CP = *it->second;

			string szChildClass = CP.m_UseClassName;
			SQRWnd* pChild = SQRWndClass::CreateWndObject( szChildClass.c_str() );
			if (!pChild)
			{
				string szChildInf = CP.m_ClassName;
				pChild = SQRWndClass::CreateWndObject( szChildInf.c_str() );
			}

			if ( pChild )
			{
				WndCreateParam childParam;

				childParam.szTitle		= CP.m_WindowTitle;
				childParam.uStyle		= CP.m_WndStyle;
				childParam.x			= (float)CP.m_x;
				childParam.y			= (float)CP.m_y;
				childParam.width		= (float)CP.m_Width;
				childParam.height		= (float)CP.m_Height;
				childParam.szSource     = CP.m_SourceName;//add by guo
				childParam.pParentWnd	= pWnd;
				GUI_FONT* pCFont = pChild->GetFontInfo(CP.m_uStyleIndex);
				if (pCFont)
				{
					childParam.nFontID	 = pCFont->m_nFontID;
					childParam.font_size  = (float)pCFont->m_FontSize;
				}
				else
				{
					childParam.nFontID		= CP.m_nFontID;
					childParam.font_size	= (float)CP.m_FontSize;
				}
				
				childParam.uGroupID		= CP.m_GroupID;

				re = pChild->Create( childParam );
				if ( re == GERROR )
					delete pChild;
				else
				{
					pChild->GetWndHandle()->m_bNewByGUISys = true;
					ChildList.push_back( pChild );
					pChild->SetLockFlag( CP.m_uLockFlage );
					if (pCFont)
					{
						pChild->SetFontGradualColor( pCFont->m_uFontGradualColor );
						pChild->SetFontEffectMask( pCFont->m_uFontEffectMask );
					}
					else
					{
						pChild->SetFontGradualColor( CP.m_uFontGradualColor );
						pChild->SetFontEffectMask( CP.m_uFontEffectMask );
					}
					
					pChild->SetIsDrawBackground(  CP.m_isShowBackground );
					pChild->SetStyleIndex( CP.m_uStyleIndex );
					pChild->SetMouseOverDesc( CP.m_szDesc );

					//effect
					pChild->SetMsgFx( eMF_Click,	CP.m_MsgFx[eMF_Click] );
					pChild->SetMsgFx( eMF_Open,	CP.m_MsgFx[eMF_Open] );
					pChild->SetMsgFx( eMF_Close,	CP.m_MsgFx[eMF_Close] );

					//sound
					pChild->SetMsgSound( eMS_Click, CP.m_MsgSound[eMS_Click] );
					pChild->SetMsgSound( eMS_Open,  CP.m_MsgSound[eMS_Open] );
					pChild->SetMsgSound( eMS_Close, CP.m_MsgSound[eMS_Close] );

					///flash info
					UIMap<EventStateMask, UIString>::iterator citr = CP.m_FlashWndStr.begin();
					UIMap<EventStateMask, UIString>::iterator citrend = CP.m_FlashWndStr.end();
					while ( citr != citrend )
					{
						if (citr->first < IP_COUNT)
							pChild->SetFlashInfo( citr->first, citr->second.c_str() );
						++citr;
					}

					pChild->GetWndHandle()->m_pResParam	= &CP;
					pChild->GetWndHandle()->m_WindowName	= it->first;
					
					pChild->GetWndHandle()->m_strTransName	= CP.m_TransTexture;
					if(pChild->GetWndHandle()->m_strTransName.size()>0)
					{
						bool bEnableASynRead = CPkgFile::IsEnableAsyncRead();
						CPkgFile::EnableAsyncRead( false );
						pGUI->GetGraphic()->CreateTexture( L"", 
							(pChild->GetWndHandle()->m_strTransName).c_str()
							, &(pChild->GetWndHandle()->m_pTransparenTex) ); // szTextName
						CPkgFile::EnableAsyncRead( bEnableASynRead );

						if ( m_pMainDlg )
						{
							wstring tt_tex_name = utf8_to_utf16((pChild->GetWndHandle()->m_strTransName).c_str());
							if (tt_tex_name.length() > 0)
								m_pMainDlg->PushResData(tt_win_name, tt_tex_name);
						}
					}

					m_CreateInfo[pChild].szClassName		= utf8_to_utf16(CP.m_UseClassName);
					m_CreateInfo[pChild].szCreateInfo		= utf8_to_utf16(CP.m_UseInfo);
					m_CreateInfo[pChild].szFileName			= L"";

					HTREEITEM ChildItem = m_WndTree.InsertItem( utf8_to_utf16(it->first.c_str()).c_str(), 2, 2, MainItem );
					m_WndTree.SetItemData( ChildItem, (DWORD)(LPARAM)pChild );

					for ( int32 i = 0; i < (int32)CP.m_ImageList.size(); i++ )
					{
						ChildWndInfo& Info = CP.m_ImageList[i];
						if ( i )
						{
							pChild->GetResWnd( i + 1 )->SetWndRect( Info.rtWnd );
							pChild->GetResWnd( i + 1 )->SetStyleIndex(Info.m_uStyleIndex);
							pChild->GetResWnd( i + 1 )->SetLockFlag(Info.m_uLockFlag);

							wchar_t Buf[256];
							wsprintf( Buf, L"Child%04u", i );
							m_WndTree.SetItemData( 
								m_WndTree.InsertItem( Buf, 3, 3, ChildItem )
								, (DWORD)(LPARAM)pChild->GetResWnd( i + 1 ) );
						}
						for ( int32 j = 0; j < (int32)Info.ChildBkList.size(); j++ )
						{
							IMAGE_PARAM IP;
							IP.CtrlMask=WORD( i + 1 );
							IP.StateMask=WORD( Info.ChildBkList[j].nStateMask );
							WND_IMAGE_LIST* pImage = pChild->GetWndBkImage( IP );

							if (pCFont)
							{
								pChild->SetWndTextColor( IP, pCFont->m_EventFont[(EventStateMask)(IP.StateMask)].m_FontColor );
								pChild->SetWndTextBcColor( IP, pCFont->m_EventFont[(EventStateMask)(IP.StateMask)].m_uFontBkColor );
							}
							else
							{
								pChild->SetWndTextColor( IP, Info.ChildBkList[j].uTextColor );
								pChild->SetWndTextBcColor( IP, Info.ChildBkList[j].uRimColor );
							}
							
							if ( pImage )
							{
								*pImage = pWnd->GetWndHandle()->m_pGUI->LoadBKRes( Info.ChildBkList[j].ImageList );
								if ( m_pMainDlg )
								{
									for (int32 ct_img = 0; ct_img < pImage->GetImageCount(); ++ct_img)
									{
										IMAGE& img = pImage->GetImage(ct_img);
										wstring tt_tex_name = utf8_to_utf16(img.GetTexName());
										if (tt_tex_name.length() > 0)
											m_pMainDlg->PushResData(tt_win_name, tt_tex_name);
									}
								}
							}
						}
					} // end of for
				}
				pChild->SetszClassName(szChildClass.c_str());
			} // if end
		}
		itr++;
	}
}


void CWndGroup::OnBnClickedLoad()
{
	GetDlgItem(IDC_LOAD)->EnableWindow(FALSE);

	m_RememberItem = NULL;
	HideCurShowWin();

	// 销毁已经创建的窗体
	HTREEITEM hChildItem = m_WndTree.GetChildItem( m_hRoot );
	while( hChildItem != NULL )
	{
		SQRWnd* pWnd = (SQRWnd*)m_WndTree.GetItemData( hChildItem );
		pWnd->DestroyWnd();
		hChildItem = m_WndTree.GetNextItem(hChildItem, TVGN_NEXT);
	}

	m_WndTree.DeleteAllItems();
	m_hRoot = m_WndTree.InsertItem( L"窗口单元", 0, 1 );
	path	= m_strRootPath.c_str();

	if (   m_pMainDlg 
		&& m_pMainDlg->GetGUIRootWnd() 
		&& m_pMainDlg->GetGUIRootWnd()->GetWndHandle()
		&& m_pMainDlg->GetGUIRootWnd()->GetWndHandle()->m_pGUI
		&& m_pMainDlg->GetGUIRootWnd()->GetWndHandle()->m_pGUI->m_ResTemplate.size() != 0)
	{
		// 从WndResParam中读数据，完成列表数的创建，完成窗体内容的创建
		WinListCreateFromRes();
	}
	else
	{
		MessageBox(L"对应文件夹下没有LAYOUT资源", L"提示");
	}
 
	m_WndTree.Expand( m_hRoot, TVE_EXPAND );
	NMTREEVIEWA a; 
	HRESULT h;
	a.itemNew.hItem = m_hRoot;
	OnTvnSelchangedWndTree( reinterpret_cast<NMHDR*>(&a), &h );

	GetDlgItem(IDC_LOAD)->EnableWindow();

	m_bResLoaded = true;

	reloadStaticTexts();
}

BOOL CWndGroup::isHold(wchar_t *name)
{
	for (size_t i = 0; i < m_TreeItemList.size(); ++i)
	{
		if (0 == wcscmp(name, m_TreeItemList[i].szBuffer))
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CWndGroup::isUser(wchar_t *name)
{
	for (size_t i = 0; i < m_TreeItemList.size(); ++i)
	{
		if ( FALSE == m_TreeItemList[i].isUser && 0 == wcscmp(name, m_TreeItemList[i].szBuffer))
		{
			m_TreeItemList[i].isUser = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

void CWndGroup::MoveWnd() 
{
	if( TRUE == bFocus )
	 	return;

	CFRect rt;
	for(size_t i = 0; i < mChooseWndlist.size(); ++i )
	{
		mChooseWndlist[i]->GetWndRect(rt);	
		//按左键
		if(!(GetKeyState( VK_SPACE )&0x8000))
		{
			if( GetKeyState( VK_LEFT )&0x8000 )
			{
				rt.left  -= 1;
				rt.right -= 1;
			}
			else if( GetKeyState( VK_RIGHT )&0x8000 )
			{
				rt.left  += 1;
				rt.right += 1;			
			}
			else if( GetKeyState( VK_UP )&0x8000 )
			{
				rt.top    -= 1;
				rt.bottom -= 1;
			}
			else if( GetKeyState( VK_DOWN )&0x8000 )
			{
				rt.top    += 1;
				rt.bottom += 1;
			}
			mChooseWndlist[i]->SetWndRect(rt);
		}
	}
}

void CWndGroup::WriteTextItem(ofstream& file, string& idx, string& txt)
{
	file.write((char*)idx.c_str(),idx.size());
	file.write("\t",1);
	string temp_txt = txt;
	while (temp_txt[temp_txt.length() - 1] == '\n')
		temp_txt = temp_txt.substr(0,temp_txt.length() - 1);
	file.write((char*)temp_txt.c_str(),temp_txt.size());
	file.write("\n",1);
}


void CWndGroup::OnBnClickedSave()
{
	if( m_nAufVersion < AUF_ADD_AUDIO )
		MessageBox(L"这个版本修改了文件格式，保存前请先将gui/res下的所有auc文件解锁", L"提示");
	EnableWindow( false );
	SetFileAttributes( m_strFile, FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE );
	m_strFile = m_strRootPath.c_str();

	int nMainCount = 0;
	HTREEITEM hMainItem;
	wchar_t szbuffer[64] = L"";
	hMainItem = m_WndTree.GetChildItem( m_hRoot );
	m_TreeItemList.clear();

	//删除队列文件
	vector<CString>::iterator iter;
	vector<CString>::iterator iterdel;
	for (iterdel = m_DelFileNameList.begin(); iterdel != m_DelFileNameList.end(); iterdel++)
	{
		for (iter = m_FileNameList.begin() ;iter != m_FileNameList.end(); iter++)
		{
			if (*iter == *iterdel)
			{
				_wremove((*iter).GetString());
			}
		}
	}
	m_DelFileNameList.clear();

	// 维护当前所有节点树
	while( hMainItem != NULL )
	{
		wcscpy(szbuffer, m_WndTree.GetItemText(hMainItem));
		if (TRUE == isHold(szbuffer))
		{
			ItemNode mNode;
			mNode.isUser = FALSE;
			wcscpy(mNode.szBuffer, szbuffer);
			m_TreeItemList.push_back(mNode);
			nMainCount++;
		}
		hMainItem = m_WndTree.GetNextItem(hMainItem, TVGN_NEXT);
	}
	// layout解析相关handler 
	GUILayoutHandler* LayoutHandler = new GUILayoutHandler();
	LayoutHandler->SetXmlResSaver(m_ResSaver);
	ClearResData(eRes_T_GUI);

	// 清空当前静态文字XML文件，并写入这些文件，国际化相关
	string szCurLan = CFontMgr::GetInst()->GetLangFolder();
	string szStaticTextPath = szCurLan + "\\gui\\" + "GUIStaticText.txt";
	locale textloc = locale::global(locale("")); // 要打开的文件路径包含中文，设置全局locale为本地环境
	string strFileName = utf16_to_gbk(CPkgFile::FindPathInclude(L"lang")) + "/" 
						 + szStaticTextPath;
	ofstream outputFile( strFileName.c_str(), ios::binary|ios::out );
	locale::global(textloc);
	if (!outputFile)
	{
		GenErr("Can't open GUIStaticText.txt");
		exit(1);
	}
	string idx = "GUIidx", txt = "GUItxt";
	WriteTextItem(outputFile, idx, txt);

	// 初始化 hMainItem 返回root的第一个节点
	hMainItem = m_WndTree.GetChildItem( m_hRoot );

	while( hMainItem != NULL )
	{
		// 如果数据已经存储过，continue
		wcscpy(szbuffer, m_WndTree.GetItemText(hMainItem));
		if (FALSE == isUser(szbuffer))		
		{
			hMainItem = m_WndTree.GetNextItem(hMainItem, TVGN_NEXT);
			continue;
		}

		CString szChildNameMain = m_WndTree.GetItemText( hMainItem );
		int Len = utf16_to_utf8(szChildNameMain.GetString()).length();

		CString szFileName = m_strFile;
		szFileName += "/ybtx.";
		szFileName += szChildNameMain;
		szFileName += ".auc";

		locale loc = locale::global(locale("")); // 要打开的文件路径包含中文，设置全局locale为本地环境
		string sub_auf_save_path = utf16_to_gbk(CPkgFile::FindPathInclude(L"gui")) + "/" + utf16_to_gbk(szFileName.GetString());
		ofstream Save( sub_auf_save_path.c_str(), ios::binary|ios::out );
		locale::global(loc);

		// 写static text信息
		string sChildNameMainForXml( utf16_to_utf8(szChildNameMain.GetString()) );
		SQRWnd* pWnd = (SQRWnd*)m_WndTree.GetItemData( hMainItem );
		string sTitle( pWnd->GetWndText() );
		string sDesc( pWnd->GetMouseOverDesc() );

		if ( "" != sTitle )
		{
			// idx
			string name_idx = sChildNameMainForXml + "#title#";
			WriteTextItem(outputFile,name_idx,sTitle);
		}
		if ( "" != sDesc )
		{
			// idx
			string name_idx = sChildNameMainForXml + "#desc#";
			WriteTextItem(outputFile,name_idx,sDesc);
		}

		string szUseClass( utf16_to_utf8(m_CreateInfo[pWnd].szClassName).c_str() );
		string szUseInf( utf16_to_utf8(m_CreateInfo[pWnd].szCreateInfo).c_str() );

		pWnd->SetszCreateInfo(szUseInf.c_str());
		pWnd->SetszWindowName(utf16_to_utf8(szChildNameMain.GetString()).c_str());
		pWnd->SetszClassName(szUseClass.c_str());
		//////////////////////////////////////////////////////////////////////////
		// 保存二进制内容 设置好新加的属性
		SaveItem( Save, hMainItem );

		//////////////////////////////////////////////////////////////////////////
		// ---Layout 文件保存
		CString LayoutFile = m_strFile;
		LayoutFile.Delete( LayoutFile.GetLength() - 10, 10 );

		LayoutFile += "src\\Layout\\ybtx.";
		LayoutFile += szChildNameMain;
		LayoutFile += ".xml";

		TiXmlDocument		LayoutDoc;
		TiXmlElement RootWin(::sqr::GUILayoutHandler::WindowElement.c_str());

		RootWin.SetAttribute(::sqr::GUILayoutHandler::WindowNameAttr.c_str(),utf16_to_utf8(szChildNameMain.GetString()));


		if ( szUseClass.find("CWnd") != string::npos || 
			szUseClass.find("CButton") != string::npos || 
			szUseClass.find("CChart") != string::npos || 
			szUseClass.find("CCheckButton") != string::npos || 
			szUseClass.find("CClock") != string::npos || 
			szUseClass.find("CClockPart") != string::npos || 
			szUseClass.find("CComboBox") != string::npos || 
			szUseClass.find("CControl") != string::npos || 
			szUseClass.find("CDialog") != string::npos || 
			szUseClass.find("CEdit") != string::npos || 
			szUseClass.find("CItemWnd") != string::npos || 
			szUseClass.find("CListCtrl") != string::npos || 
			szUseClass.find("CProgress") != string::npos || 
			szUseClass.find("CRichText") != string::npos || 
			szUseClass.find("CRichString") != string::npos || 
			szUseClass.find("CRichToolTip") != string::npos || 
			szUseClass.find("CRootWnd") != string::npos || 
			szUseClass.find("CScrollBar") != string::npos || 
			szUseClass.find("CSpin") != string::npos || 
			szUseClass.find("CStatic") != string::npos || 
			szUseClass.find("CVideo") != string::npos || 
			szUseClass.find("CTipHolder") != string::npos || 
			szUseClass.find("CToolTip") != string::npos || 
			szUseClass.find("CTreeCtrl") != string::npos || 
			szUseClass.find("CWndEx") != string::npos || 
			szUseClass.find("CWndOffSet") != string::npos || 
			szUseClass.find("CMsgBoxHandlerHolder") != string::npos || 
			szUseClass.find("CHolderHandler") != string::npos || 
			szUseClass.find("CHolder") != string::npos  )
		{
			szUseClass = szUseClass.substr(1, szUseClass.size() - 1);
			szUseClass = "SQR" + szUseClass;
		}
		LayoutHandler->CWnd2Element(pWnd, &RootWin, szUseClass, szUseInf);
		// ---结束部分 Layout 文件保存


		int nChildCount = 0;
		HTREEITEM hChildItem; 
		hChildItem = m_WndTree.GetChildItem( hMainItem );
		while( hChildItem != NULL )
		{
			nChildCount++;
			hChildItem = m_WndTree.GetNextItem(hChildItem, TVGN_NEXT);
		}

		Save.write( (char*)&nChildCount, sizeof(int) );
		hChildItem = m_WndTree.GetChildItem( hMainItem );
		
		
		while( hChildItem != NULL )
		{
			CString szChildName = m_WndTree.GetItemText( hChildItem );
			int Len = utf16_to_utf8(szChildName.GetString()).length();
			Save.write( (char*)&Len, sizeof(int) );
			Save.write( utf16_to_utf8(szChildName.GetString()).c_str(), Len );

			////////////////////////////////////////////////////////////////////////////
			// 保存文字		
			SQRWnd* pChildWnd = (SQRWnd*)m_WndTree.GetItemData( hChildItem );
			string sChildTitle( pChildWnd->GetWndText() );
			string sChildDesc( pChildWnd->GetMouseOverDesc() );
			string sChildName = utf16_to_utf8((wstring)szChildName);

			if ( "" != sChildTitle )
			{
				// idx
				string name_idx = sChildNameMainForXml + "#" + sChildName + "#title#";
				WriteTextItem(outputFile,name_idx,sChildTitle);
			}
			if ( "" != sChildDesc )
			{
				// idx
				string name_idx = sChildNameMainForXml + "#" + sChildName + "#desc#";
				WriteTextItem(outputFile,name_idx,sChildDesc);
			}

			string szChildClass( utf16_to_utf8(m_CreateInfo[pChildWnd].szClassName).c_str() );
			string szChildInf( utf16_to_utf8(m_CreateInfo[pChildWnd].szCreateInfo).c_str() );
			pChildWnd->SetszCreateInfo(szChildInf.c_str());
			pChildWnd->SetszWindowName(utf16_to_utf8(szChildName.GetString()).c_str());
			pChildWnd->SetszClassName(szChildClass.c_str());
			////////////////////////////////////////////////////////////////////////////
			// 保存二进制文件
			SaveItem( Save, hChildItem );

			//////////////////////////////////////////////////////////////////////////
			// ---Layout 文件保存
			TiXmlElement ChildWin(::sqr::GUILayoutHandler::WindowElement.c_str());

			ChildWin.SetAttribute(::sqr::GUILayoutHandler::WindowNameAttr.c_str(), utf16_to_utf8(szChildName.GetString()));
			

			if ( szChildClass.find("CWnd") != string::npos || 
				szChildClass.find("CButton") != string::npos || 
				szChildClass.find("CChart") != string::npos || 
				szChildClass.find("CCheckButton") != string::npos || 
				szChildClass.find("CClock") != string::npos || 
				szChildClass.find("CClockPart") != string::npos || 
				szChildClass.find("CComboBox") != string::npos || 
				szChildClass.find("CControl") != string::npos || 
				szChildClass.find("CDialog") != string::npos || 
				szChildClass.find("CEdit") != string::npos || 
				szChildClass.find("CItemWnd") != string::npos || 
				szChildClass.find("CListCtrl") != string::npos || 
				szChildClass.find("CProgress") != string::npos || 
				szChildClass.find("CRichText") != string::npos || 
				szChildClass.find("CRichString") != string::npos || 
				szChildClass.find("CRichToolTip") != string::npos || 
				szChildClass.find("CRootWnd") != string::npos || 
				szChildClass.find("CScrollBar") != string::npos || 
				szChildClass.find("CSpin") != string::npos || 
				szChildClass.find("CStatic") != string::npos || 
				szChildClass.find("CVideo") != string::npos || 
				szChildClass.find("CTipHolder") != string::npos || 
				szChildClass.find("CToolTip") != string::npos || 
				szChildClass.find("CTreeCtrl") != string::npos || 
				szChildClass.find("CWndEx") != string::npos || 
				szChildClass.find("CWndOffSet") != string::npos || 
				szChildClass.find("CMsgBoxHandlerHolder") != string::npos || 
				szChildClass.find("CHolderHandler") != string::npos || 
				szChildClass.find("CHolder") != string::npos  )
			{
				szChildClass = szChildClass.substr(1, szChildClass.size() - 1);
				szChildClass = "SQR" + szChildClass;
			}

			LayoutHandler->CWnd2Element(pChildWnd, &ChildWin, szChildClass, szChildInf);

			RootWin.InsertEndChild(ChildWin);

			hChildItem = m_WndTree.GetNextItem(hChildItem, TVGN_NEXT);
		}

		string xml_save_path = utf16_to_gbk(CPkgFile::FindPathInclude(L"gui")) + "/" + utf16_to_gbk(LayoutFile.GetString());
		LayoutDoc.InsertEndChild(RootWin);
		LayoutDoc.SaveFile(xml_save_path);

		hMainItem = m_WndTree.GetNextItem(hMainItem, TVGN_NEXT);
	}
	outputFile.close();

	// save texture res
	SaveResData();
	EnableWindow( true );
}

bool IsReadOnly(wstring s)
{
	DWORD fileAttrs = GetFileAttributes(s.c_str());
	return (INVALID_FILE_ATTRIBUTES != fileAttrs && (fileAttrs & FILE_ATTRIBUTE_READONLY));
}

void CWndGroup::OnBnClickedSaveCoder()
{
	if( m_nAufVersion < AUF_ADD_AUDIO )
		MessageBox(L"保存前请先将gui/res下的所有auc文件解锁", L"提示");
	EnableWindow( false );
	SetFileAttributes( m_strFile, FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE );
	m_strFile = m_strRootPath.c_str();

	int nMainCount = 0;
	HTREEITEM hMainItem;
	wchar_t szbuffer[64] = L"";
	hMainItem = m_WndTree.GetChildItem( m_hRoot );
	m_TreeItemList.clear();

	//删除队列文件
	vector<CString>::iterator iter;
	vector<CString>::iterator iterdel;
	for (iterdel = m_DelFileNameList.begin(); iterdel != m_DelFileNameList.end(); iterdel++)
	{
		for (iter = m_FileNameList.begin() ;iter != m_FileNameList.end(); iter++)
		{
			if (*iter == *iterdel)
			{
				_wremove((*iter).GetString());
			}
		}
	}
	m_DelFileNameList.clear();

	// 维护当前所有节点树
	while( hMainItem != NULL )
	{
		wcscpy(szbuffer, m_WndTree.GetItemText(hMainItem));
		if (TRUE == isHold(szbuffer))
		{
			ItemNode mNode;
			mNode.isUser = FALSE;
			wcscpy(mNode.szBuffer, szbuffer);
			m_TreeItemList.push_back(mNode);
			nMainCount++;
		}
		hMainItem = m_WndTree.GetNextItem(hMainItem, TVGN_NEXT);
	}
	// layout解析相关handler 
	GUILayoutHandler* LayoutHandler = new GUILayoutHandler();
	LayoutHandler->SetXmlResSaver(m_ResSaver);
	ClearResData(eRes_T_GUI);

	// 清空当前静态文字XML文件，并写入这些文件，国际化相关
	string szCurLan = CFontMgr::GetInst()->GetLangFolder();
	string szStaticTextPath = szCurLan + "\\gui\\" + "GUIStaticText.txt";
	locale textloc = locale::global(locale("")); // 要打开的文件路径包含中文，设置全局locale为本地环境
	string strFileName = utf16_to_gbk(CPkgFile::FindPathInclude(L"lang")) + "/" 
		+ szStaticTextPath;
	ofstream outputFile( strFileName.c_str(), ios::binary|ios::out );
	locale::global(textloc);
	if (!outputFile)
	{
		GenErr("Can't open GUIStaticText.txt");
		exit(1);
	}
	string idx = "GUIidx", txt = "GUItxt";
	WriteTextItem(outputFile, idx, txt);

	// 初始化 hMainItem 返回root的第一个节点
	hMainItem = m_WndTree.GetChildItem( m_hRoot );

	while( hMainItem != NULL )
	{
		// 如果数据已经存储过，continue
		wcscpy(szbuffer, m_WndTree.GetItemText(hMainItem));
		if (FALSE == isUser(szbuffer))		
		{
			hMainItem = m_WndTree.GetNextItem(hMainItem, TVGN_NEXT);
			continue;
		}

		CString szChildNameMain = m_WndTree.GetItemText( hMainItem );
		int Len = utf16_to_utf8(szChildNameMain.GetString()).length();

		// 写static text信息
		string sChildNameMainForXml( utf16_to_utf8(szChildNameMain.GetString()) );
		SQRWnd* pWnd = (SQRWnd*)m_WndTree.GetItemData( hMainItem );
		string sTitle( pWnd->GetWndText() );
		string sDesc( pWnd->GetMouseOverDesc() );

		if ( "" != sTitle )
		{
			// idx
			string name_idx = sChildNameMainForXml + "#title#";
			WriteTextItem(outputFile,name_idx,sTitle);
		}
		if ( "" != sDesc )
		{
			// idx
			string name_idx = sChildNameMainForXml + "#desc#";
			WriteTextItem(outputFile,name_idx,sDesc);
		}

		string szUseClass( utf16_to_utf8(m_CreateInfo[pWnd].szClassName).c_str() );
		string szUseInf( utf16_to_utf8(m_CreateInfo[pWnd].szCreateInfo).c_str() );

		pWnd->SetszCreateInfo(szUseInf.c_str());
		pWnd->SetszWindowName(utf16_to_utf8(szChildNameMain.GetString()).c_str());
		pWnd->SetszClassName(szUseClass.c_str());

		//////////////////////////////////////////////////////////////////////////
		// ---Layout 文件保存
		CString LayoutFile = m_strFile;
		LayoutFile.Delete( LayoutFile.GetLength() - 10, 10 );

		LayoutFile += "src\\Layout\\ybtx.";
		LayoutFile += szChildNameMain;
		LayoutFile += ".xml";

		TiXmlDocument		LayoutDoc;
		TiXmlElement RootWin(::sqr::GUILayoutHandler::WindowElement.c_str());

		RootWin.SetAttribute(::sqr::GUILayoutHandler::WindowNameAttr.c_str(),utf16_to_utf8(szChildNameMain.GetString()));
		LayoutHandler->CWnd2Element(pWnd, &RootWin, szUseClass, szUseInf);
		// ---结束部分 Layout 文件保存

		int nChildCount = 0;
		HTREEITEM hChildItem; 
		hChildItem = m_WndTree.GetChildItem( hMainItem );
		while( hChildItem != NULL )
		{
			nChildCount++;
			hChildItem = m_WndTree.GetNextItem(hChildItem, TVGN_NEXT);
		}

		hChildItem = m_WndTree.GetChildItem( hMainItem );


		while( hChildItem != NULL )
		{
			CString szChildName = m_WndTree.GetItemText( hChildItem );

			////////////////////////////////////////////////////////////////////////////
			// 保存文字		
			SQRWnd* pChildWnd = (SQRWnd*)m_WndTree.GetItemData( hChildItem );
			string sChildTitle( pChildWnd->GetWndText() );
			string sChildDesc( pChildWnd->GetMouseOverDesc() );
			string sChildName = utf16_to_utf8((wstring)szChildName);

			if ( "" != sChildTitle )
			{
				// idx
				string name_idx = sChildNameMainForXml + "#" + sChildName + "#title#";
				WriteTextItem(outputFile,name_idx,sChildTitle);
			}
			if ( "" != sChildDesc )
			{
				// idx
				string name_idx = sChildNameMainForXml + "#" + sChildName + "#desc#";
				WriteTextItem(outputFile,name_idx,sChildDesc);
			}

			string szChildClass( utf16_to_utf8(m_CreateInfo[pChildWnd].szClassName).c_str() );
			string szChildInf( utf16_to_utf8(m_CreateInfo[pChildWnd].szCreateInfo).c_str() );
			pChildWnd->SetszCreateInfo(szChildInf.c_str());
			pChildWnd->SetszWindowName(utf16_to_utf8(szChildName.GetString()).c_str());
			pChildWnd->SetszClassName(szChildClass.c_str());

			//////////////////////////////////////////////////////////////////////////
			// ---Layout 文件保存
			TiXmlElement ChildWin(::sqr::GUILayoutHandler::WindowElement.c_str());

			ChildWin.SetAttribute(::sqr::GUILayoutHandler::WindowNameAttr.c_str(), utf16_to_utf8(szChildName.GetString()));
			LayoutHandler->CWnd2Element(pChildWnd, &ChildWin, szChildClass, szChildInf);
			RootWin.InsertEndChild(ChildWin);

			hChildItem = m_WndTree.GetNextItem(hChildItem, TVGN_NEXT);
		}

		string xml_save_path = utf16_to_gbk(CPkgFile::FindPathInclude(L"gui")) + "/" + utf16_to_gbk(LayoutFile.GetString());
		LayoutDoc.InsertEndChild(RootWin);
		LayoutDoc.SaveFile(xml_save_path);

		// 用CODER保存GUI RES
		CString szFileCoderName = m_strFile;
		szFileCoderName += "/";
		szFileCoderName += szChildNameMain;
		szFileCoderName += ".aud";

		string sub_aud_save_path = utf16_to_gbk(CPkgFile::FindPathInclude(L"gui")) 
								 + "/" + utf16_to_gbk(szFileCoderName.GetString());

		CGuiFormat gui_format_corder;
		gui_format_corder.RegisterCoder(new CGUIResCoder);
		if( !IsReadOnly(gbk_to_utf16(sub_aud_save_path)) )
			gui_format_corder.Save( sub_aud_save_path, (SQRWnd*)pWnd);

		hMainItem = m_WndTree.GetNextItem(hMainItem, TVGN_NEXT);
	}
	outputFile.close();

	// save texture res
	SaveResData();
	EnableWindow( true );
}

void CWndGroup::SaveItem( ofstream& File, HTREEITEM hItem )
{
	SQRWnd* pWnd = (SQRWnd*)m_WndTree.GetItemData( hItem );
	CFRect rt;
	pWnd->GetWndRect( rt );
	pWnd->GetParent()->ScreenToWnd( rt );

	//////////////////////////////////////////////////////////////////////////
	///save sound wnd property
	for( int i = 0; i < eMS_Count; ++i )
	{
		ESoundMsg eSoundType = static_cast<ESoundMsg>(i);
		LPCSTR szSoundInfo	= pWnd->GetMsgSound(eSoundType);
		int nSoundLen = (int)strlen( szSoundInfo );

		File.write( (char*)&nSoundLen,  sizeof(nSoundLen) );
		File.write( (char*)szSoundInfo, nSoundLen );
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	///save flash property
	//数量
	UIMap<EventStateMask, FlashWndInfo*> flashInfo = pWnd->GetFlashInfoMap();
	// 清空无用数据
	UIMap<EventStateMask, FlashWndInfo*>::iterator flsitr = flashInfo.begin();
	UIMap<EventStateMask, FlashWndInfo*>::iterator flsitrend = flashInfo.end();
	while (flsitr != flsitrend)
	{
		if (!(flsitr->second))
		{
			flashInfo.erase(flsitr);
			flsitr = flashInfo.begin();
			flsitrend = flashInfo.end();
			continue;
		}
		++flsitr;
	}
	int fls_co = (int)(flashInfo.size());
	File.write( (char*)&fls_co, sizeof(fls_co) );
	//每个数量的信息
	flsitr = flashInfo.begin();
	flsitrend = flashInfo.end();
	while (flsitr != flsitrend)
	{
		// 状态
		int st = flsitr->first;
		File.write( (char*)&st, sizeof(st) );
		// 数量
		LPCSTR szFlsInfo = flsitr->second->m_FlashName.c_str();
		int nFlsLen = (int)strlen( szFlsInfo );

		File.write( (char*)&nFlsLen, sizeof(nFlsLen) );
		File.write( (char*)szFlsInfo, nFlsLen );
		++flsitr;
	}
	//////////////////////////////////////////////////////////////////////////

	int		ClassLen		= (int)strlen( pWnd->GetClassName() );
	LPCSTR  szClass			= pWnd->GetClassName();
	int		WndTextLen		= (int)strlen( pWnd->GetWndText() );
	LPCSTR	szWndText		= pWnd->GetWndText();
	DWORD	wsStyle			= pWnd->GetStyle();

	int		x				= (int)(rt.left > 0.0f ? rt.left + 0.5f : rt.left - 0.5f);
	int		y				= (int)(rt.top > 0.0f ? rt.top + 0.5f : rt.top - 0.5f);
	int		w				= (int)(rt.Width() > 0.0f ? rt.Width() + 0.5f : rt.Width() - 0.5f);
	int		h				= (int)(rt.Height() > 0.0f ? rt.Height() + 0.5f : rt.Height() - 0.5f);	

	DWORD	GroupID			= pWnd->GetGroupID();
	int32	nFontID			= pWnd->GetFontID();
	int		nFontSize		= (int)(pWnd->GetFontSize()/CFontCfg::GetInst()->GetFontZoom());
	bool	bBold			= pWnd->IsFontBold();
	unsigned  uLockFlag     = pWnd->GetLockFlag();
	int     nDescLen		= (int)strlen( pWnd->GetMouseOverDesc() );
	LPCSTR  szDescInfo		= pWnd->GetMouseOverDesc();

	string  use_class_utf8	= utf16_to_utf8(m_CreateInfo[pWnd].szClassName);
	int     nUseClassLen	= (int)use_class_utf8.length();
	LPCSTR  szUseClass		= use_class_utf8.c_str();

	string  use_info_utf8	= utf16_to_utf8(m_CreateInfo[pWnd].szCreateInfo);
	int     nUseInfLen		= (int)use_info_utf8.length();
	LPCSTR  szUseInf		= use_info_utf8.c_str();

	int     nClickFxLen		= (int)strlen( pWnd->GetMsgFx(eMF_Click) );
	LPCSTR  szClickFxInfo	= pWnd->GetMsgFx(eMF_Click);
	int     nOpenFxLen		= (int)strlen( pWnd->GetMsgFx(eMF_Open) );
	LPCSTR  szOpenFxInfo	= pWnd->GetMsgFx(eMF_Open);
	int     nCloseFxLen		= (int)strlen( pWnd->GetMsgFx(eMF_Close) );
	LPCSTR  szCloseFxInfo	= pWnd->GetMsgFx(eMF_Close);
	int		SourceLen		= (int)strlen( pWnd->GetszSourceName() );
	LPCSTR  szSource		= pWnd->GetszSourceName();//add by guo

	DWORD fontGradualColor	= pWnd->GetFontGradualColor();
	DWORD fontEffectMask	= pWnd->GetFontEffectMask();
	bool  bShow				= pWnd->IsDrawBackground();
	DWORD StyleIndex		= pWnd->GetStyleIndex();

	File.write( (char*)&ClassLen, sizeof(int) );
	File.write( (char*)szClass, ClassLen );
	File.write( (char*)&WndTextLen, sizeof(int) );
	File.write( (char*)szWndText, WndTextLen );
	File.write( (char*)&wsStyle, sizeof(DWORD) );
	File.write( (char*)&x, sizeof(int) );
	File.write( (char*)&y, sizeof(int) );
	File.write( (char*)&w, sizeof(int) );
	File.write( (char*)&h, sizeof(int) );
	File.write( (char*)&GroupID, sizeof(DWORD) );
	
	File.write( (char*)&nFontID, sizeof(int32) );
	File.write( (char*)&bBold, sizeof(bool) );
	File.write( (char*)&nFontSize, sizeof(int) );

	File.write(	(char*)&fontGradualColor, sizeof(DWORD));		 
	File.write(	(char*)&fontEffectMask, sizeof(DWORD));
	File.write(	(char*)&bShow, sizeof(bool));
	File.write( (char*)&StyleIndex, sizeof(DWORD) );
	
	//保存透明纹理
	int FileNameLen = pWnd->GetWndHandle()->m_strTransName.size();
	File.write( (char*)&FileNameLen, sizeof(int) );
	File.write( (char*)pWnd->GetWndHandle()->m_strTransName.c_str(), FileNameLen );

	File.write( (char*)&uLockFlag, sizeof(unsigned) );
	File.write( (char*)&nDescLen, sizeof(int) );
	File.write( (char*)szDescInfo, nDescLen );
	File.write( (char*)&nUseClassLen, sizeof(int) );
	File.write( (char*)szUseClass, nUseClassLen );
	File.write( (char*)&nUseInfLen, sizeof(int) );
	File.write( (char*)szUseInf, nUseInfLen );
	File.write( (char*)&nClickFxLen, sizeof(int) );
	File.write( (char*)szClickFxInfo, nClickFxLen );
	File.write( (char*)&nOpenFxLen, sizeof(int) );
	File.write( (char*)szOpenFxInfo, nOpenFxLen );
	File.write( (char*)&nCloseFxLen, sizeof(int) );
	File.write( (char*)szCloseFxInfo, nCloseFxLen );
	File.write( (char*)&SourceLen, sizeof(int) );//add by guo
	File.write( (char*)szSource, SourceLen );

	int nChildCount = 0;
	for( int nCtrlMask = 1; ; nCtrlMask++ )
	{
		SQRWnd* pChild = pWnd->GetResWnd( nCtrlMask );
		if( !pChild )
			break;
		nChildCount++;
	}

	File.write( (char*)&nChildCount, sizeof(int) );
	for( int nCtrlMask = 1; ; nCtrlMask++ )
	{
		SQRWnd* pChild = pWnd->GetResWnd( nCtrlMask );
		if( !pChild )
			break;
		if( nCtrlMask > 1 )
		{
			CFRect rt;
			pChild->GetWndRect( rt );

			int		x				= (int)(rt.left > 0.0f ? rt.left + 0.5f : rt.left - 0.5f);
			int		y				= (int)(rt.top > 0.0f ? rt.top + 0.5f : rt.top - 0.5f);
			int		w				= (int)(rt.Width() > 0.0f ? rt.Width() + 0.5f : rt.Width() - 0.5f);
			int		h				= (int)(rt.Height() > 0.0f ? rt.Height() + 0.5f : rt.Height() - 0.5f);	

			DWORD ChildStyle	 = pChild->GetStyleIndex(); 
			DWORD LockFlag		 = pChild->GetLockFlag();
			
			File.write( (char*)&x, sizeof(int) );
			File.write( (char*)&y, sizeof(int) );
			File.write( (char*)&w, sizeof(int) );
			File.write( (char*)&h, sizeof(int) );
			File.write( (char*)&ChildStyle, sizeof(DWORD) );
			File.write( (char*)&LockFlag,sizeof(DWORD));
		}

		//记录下来有多少种状态
		int nStateCount = 0;
		IMAGE_PARAM IP ( 1, 0 );
		for( IP.StateMask = IP_MASK; IP.StateMask < IP_COUNT; IP.StateMask++ )
			if( pChild->GetWndBkImage( IP ) )
				nStateCount++;

		File.write( (char*)&nStateCount, sizeof(int) );
		for( IP.StateMask = IP_MASK; IP.StateMask < IP_COUNT; IP.StateMask++ )
		{
			WND_IMAGE_LIST* pWI = pChild->GetWndBkImage( IP );
			if( pWI )
			{
				int nImageCount = pWI->GetImageCount();
				unsigned nStateColor = pChild->GetWndTextColor( IP );
				unsigned uRimColor = pChild->GetWndTextBcColor( IP );
				int nStateMask = IP.StateMask;

				File.write( (char*)&nStateMask, sizeof(int) );
				File.write( (char*)&nStateColor, sizeof(unsigned) );
				File.write(	(char*)&uRimColor, sizeof(unsigned));     // 写入文字边框颜色
				File.write( (char*)&nImageCount, sizeof(int) );

				for( int i = 0; i < nImageCount; i++ )
				{
					IMAGE& Image = pWI->GetImage(i);
					string	TextName  = gbk_to_utf8(Image.GetTexName());
					slash_to_backslash(TextName);

					int		Len = (int)TextName.size();
					File.write( (char*)&Len, sizeof(int) );
					File.write( TextName.c_str(), Len );
					File.write( (char*)&Image.rtTexture, sizeof( CFRect ) );
					File.write( (char*)&Image.posWnd, sizeof( CFPos ) );
					File.write( (char*)&Image.dwColor, sizeof( unsigned ) );
					File.write( (char*)&Image.dwLockFlag, sizeof( unsigned ) );
				}
			}
		}
	}
}

void CWndGroup::OnBnClickedSaveAs()
{
	static wchar_t Filter[] = L"GUI File (*.AUF)|*.AUF||";
	CString AUFPath = GetRegistKeyStr( L"Software\\GUIEditor", L"AUFPath" );
	AUFPath = GetFileName( FALSE, Filter, NULL, AUFPath );
	if( AUFPath.IsEmpty() )
		return;

	AUFPath.MakeLower();
	LPCWSTR Ext = (LPCWSTR)AUFPath + wcslen( AUFPath ) - 4;
	if( _wcsicmp( L".auf", Ext ) )
		AUFPath += L".auf";

	m_strFile = AUFPath;

	OnBnClickedSave();
}

void CWndGroup::OnBnClickedAlignLeft()
{
	if (m_SelectedWnd.size() <= 1)
		return;
	SQRWnd* tempWnd = m_SelectedWnd[0];
	CFRect rt; tempWnd->GetWndRect(rt);
	CFRect desrt;
	for (uint i = 1; i < m_SelectedWnd.size(); ++i)
	{
		SQRWnd* desWnd = m_SelectedWnd[i];	
		desWnd->GetWndRect(desrt);
		float wth = desrt.Width();
		desrt.left = rt.left;
		desrt.right = desrt.left + wth;
		desWnd->SetWndRect(desrt);
	}
	UpdateToPropWnd( EDIT_POSITION|EDIT_TEXTURE );
}

void CWndGroup::OnBnClickedAlignRight()
{
	if (m_SelectedWnd.size() <= 1)
		return;
	SQRWnd* tempWnd = m_SelectedWnd[0];
	CFRect rt; tempWnd->GetWndRect(rt);
	CFRect desrt;
	for (uint i = 1; i < m_SelectedWnd.size(); ++i)
	{
		SQRWnd* desWnd = m_SelectedWnd[i];
		desWnd->GetWndRect(desrt);
		float wth = desrt.Width();
		desrt.right = rt.right;
		desrt.left = desrt.right - wth;
		desWnd->SetWndRect(desrt);
	}
	UpdateToPropWnd( EDIT_POSITION|EDIT_TEXTURE );
}

void CWndGroup::OnBnClickedAlignTop()
{
	if (m_SelectedWnd.size() <= 1)
		return;
	SQRWnd* tempWnd = m_SelectedWnd[0];
	CFRect rt; tempWnd->GetWndRect(rt);
	CFRect desrt;
	for (uint i = 1; i < m_SelectedWnd.size(); ++i)
	{
		SQRWnd* desWnd = m_SelectedWnd[i];
		desWnd->GetWndRect(desrt);
		float hit = desrt.Height();
		desrt.top = rt.top;
		desrt.bottom = desrt.top + hit;
		desWnd->SetWndRect(desrt);
	}
	UpdateToPropWnd( EDIT_POSITION|EDIT_TEXTURE );
}

void CWndGroup::OnBnClickedAlignBottom()
{
	if (m_SelectedWnd.size() <= 1)
		return;
	SQRWnd* tempWnd = m_SelectedWnd[0];
	CFRect rt; tempWnd->GetWndRect(rt);
	CFRect desrt;
	for (uint i = 1; i < m_SelectedWnd.size(); ++i)
	{
		SQRWnd* desWnd = m_SelectedWnd[i];
		desWnd->GetWndRect(desrt);
		float hit = desrt.Height();
		desrt.bottom = rt.bottom;
		desrt.top = desrt.bottom - hit;
		desWnd->SetWndRect(desrt);
	}
	UpdateToPropWnd( EDIT_POSITION|EDIT_TEXTURE );
}

void CWndGroup::OnBnClickedAlignWidth()
{
	if (m_SelectedWnd.size() <= 1)
		return;
	SQRWnd* tempWnd = m_SelectedWnd[0];
	CFRect rt; tempWnd->GetWndRect(rt);
	CFRect desrt;
	for (uint i = 1; i < m_SelectedWnd.size(); ++i)
	{
		SQRWnd* desWnd = m_SelectedWnd[i];
		desWnd->GetWndRect(desrt);
		desrt.right = desrt.left + rt.Width();
		desWnd->SetWndRect(desrt);
	}
	UpdateToPropWnd( EDIT_POSITION|EDIT_TEXTURE );
}

void CWndGroup::OnBnClickedAlignHeight()
{
	if (m_SelectedWnd.size() <= 1)
		return;
	SQRWnd* tempWnd = m_SelectedWnd[0];
	CFRect rt; tempWnd->GetWndRect(rt);
	CFRect desrt;
	for (uint i = 1; i < m_SelectedWnd.size(); ++i)
	{
		SQRWnd* desWnd = m_SelectedWnd[i];
		desWnd->GetWndRect(desrt);
		desrt.bottom = desrt.top + rt.Height();
		desWnd->SetWndRect(desrt);
	}
	UpdateToPropWnd( EDIT_POSITION|EDIT_TEXTURE );
}

void CWndGroup::PushSelectedWnd(SQRWnd* wnd)
{
	vector<SQRWnd*>::iterator it = m_SelectedWnd.begin();
	vector<SQRWnd*>::iterator eit = m_SelectedWnd.end();
	while(it != eit)
	{
		if (*it == wnd)
			return;
		++it;
	}

	m_SelectedWnd.push_back(wnd);
	if (m_SelectedWnd.size() > 1)
	{
		for (int i = 0; i < Align_Count; ++i)
			m_AlignButton[i].EnableWindow(TRUE);
	}
	else
	{
		for (int i = 0; i < Align_Count; ++i)
			m_AlignButton[i].EnableWindow(FALSE);
	}
}

void CWndGroup::ClearSelectedWnd()
{
	m_SelectedWnd.clear();
	for (int i = 0; i < Align_Count; ++i)
		m_AlignButton[i].EnableWindow(FALSE);
}

vector<SQRWnd*>* CWndGroup::GetSelectedWndList()
{
	return &m_SelectedWnd;
}

void CWndGroup::OnNMClickWndTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CWndGroup::OnNMKillfocusWndTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	bFocus = FALSE;
}

void CWndGroup::reloadStaticTexts()
{
	// 重新加载静态文本资源
	SQRGUIManager* pGUI = static_cast<SQRGUIManager*>( IGUIManager::GetInst() );
	pGUI->LoadStaticText();

	HTREEITEM hMainItem;
	hMainItem = m_WndTree.GetChildItem( m_hRoot );
	m_TreeItemList.clear();

	// 保存GUI窗口树的所有子节点信息
	wchar_t szbuffer[64] = L"";
	while( hMainItem != NULL )
	{
		wcscpy(szbuffer, m_WndTree.GetItemText(hMainItem));
		if (TRUE == isHold(szbuffer))
		{
			ItemNode node;
			node.hItem  = hMainItem;
			node.isUser = FALSE;
			wcscpy(node.szBuffer, szbuffer);
			m_TreeItemList.push_back(node);
		}
		hMainItem = m_WndTree.GetNextItem(hMainItem, TVGN_NEXT);
	}

	// 利用之前保存的子节点信息重新加载GUI静态文本
	TreeItemList::iterator it;
	for ( it = m_TreeItemList.begin(); it != m_TreeItemList.end(); ++it )
	{
		reloadDialogStaticText( *it );	
	}

	UpdateToPropWnd( EDIT_POSITION|EDIT_TEXTURE );
}

void CWndGroup::reloadDialogStaticText( const ItemNode& treeItemNode )
{
	typedef SQRGUIManager::MapResStaticInfo MapResStaticInfo;

	HTREEITEM	 hMainItem = treeItemNode.hItem;
	SQRWnd* pWnd	   = (SQRWnd*)m_WndTree.GetItemData(hMainItem);
	SQRGUIManager* pGUI	   = static_cast<SQRGUIManager*>( IGUIManager::GetInst() );
	
	MapResStaticInfo *pResStaticInfoContainer = pGUI->GetResStaticInfoContainer();
	MapResStaticInfo::iterator itMainInfo	  = pResStaticInfoContainer->find( utf16_to_gbk(treeItemNode.szBuffer).c_str() );
	if ( itMainInfo == pResStaticInfoContainer->end() )
		return;
		
	ResInfo mainInfo = itMainInfo->second;

	// 重新设定主窗口的文字
	pWnd->SetWndText( mainInfo.szTitle.c_str() );
	pWnd->SetMouseOverDesc( mainInfo.szDesc.c_str() );

	// 重新设定各字窗口的文字
	HTREEITEM hChildItem = m_WndTree.GetChildItem( hMainItem );	
	ResInfo::ChildInfoMap& childInfoList = mainInfo.mapChildInfo;
	while ( NULL != hChildItem )
	{
		SQRWnd* pChildWnd		= (SQRWnd*)m_WndTree.GetItemData(hChildItem);
		string		 szChildIndex	= utf16_to_utf8( (LPCWSTR)m_WndTree.GetItemText(hChildItem) );
		
		ResInfo::ChildInfoMap::iterator itChild = childInfoList.find( szChildIndex.c_str() );
		if ( itChild != childInfoList.end() )
		{
			string szChildTitle	= itChild->second.szTitle.c_str();
			string szChildDesc	= itChild->second.szDesc.c_str();
			pChildWnd->SetWndText( szChildTitle.c_str() );
			pChildWnd->SetMouseOverDesc( szChildDesc.c_str() );
		}

		hChildItem = m_WndTree.GetNextItem(hChildItem, TVGN_NEXT);
	}
}

void CWndGroup::SetCurLanguage( int32 iCurLan )
{
	if ( !m_bResLoaded )
		return;

	// 切换语言
	bool bLanguageChanged = CFontMgr::GetInst()->SetLangIndex(iCurLan);

	CFontCfg::GetInst()->LoadFongConfig(CLangCfg::GetInst()->GetLanFolder(iCurLan));
	CFontMgr::GetInst()->SetLangFolder(CLangCfg::GetInst()->GetLanFolder(iCurLan));
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

	if ( !CFontMgr::GetInst()->LoadGDIFonts() )
	{
		MessageBox(L"您没有设置语言包所在路径，请设置(例如x:\\ybtx\\designer\\lang");
		return;
	}

	if ( !bLanguageChanged )
		return;
	reloadStaticTexts();
}