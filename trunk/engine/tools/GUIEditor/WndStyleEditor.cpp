// WndStyleEditor.cpp : implementation file
//

#include "stdafx.h"
#include "GUIEditor.h"
#include "WndStyleEditor.h"
#include "SQRGUIStyles.h"
#include "CWindowTarget.h"
#include "StringHelper.h"
#include "CLangCfg.h"
#include "CFontMgr.h"
#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#define  MAX_PATH  PATH_MAX
#else
#include <direct.h>
#endif


// CWndStyleEditor

wstring TextureTypeList[IP_COUNT] =
{
	L"MASK",
	L"MOUSEDESC",
	L"ENABLE",
	L"DISABLE",
	L"MOUSEOVER",
	L"CLICKDOWN",
	L"UNCKECK_ENABLE",
	L"UNKECK_DISABLE",
	L"UNCKECK_MOUSEOVER",
	L"UNCKECK_CLICKDOWN",
	L"PROGRESS_FILL",
	L"TR_ENABLE",
	L"TR_DISABLE",
	L"LS_SEL",
	L"LS_UNSEL",
	L"FILL_FULL"
};

wstring BlockList[GBM_COUNT] =
{
	L"上中  边",
	L"左中  边",
	L"右中  边",
	L"下中  边",
	L"中部区域",
	L"左上  角",
	L"右上  角",
	L"左下  角",
	L"右下  角"
};

DWORD	BlockIDC[GBM_COUNT] = 
{
	IDC_BLOCK_TOP,
	IDC_BLOCK_LEFT,
	IDC_BLOCK_RIGHT,
	IDC_BLOCK_BOTTOM,
	IDC_BLOCK_CENTER,
	IDC_BLOCK_LEFT_TOP,
	IDC_BLOCK_RIGHT_TOP,
	IDC_BLOCK_LEFT_BOTTOM,
	IDC_BLOCK_RIGHT_BOTTOM,
};

#define UPDATE_STR(class_name)\
	m_strWnd = ::sqr::class_name::class##class_name.m_ClassName.c_str();\
	m_pStyleStruct =  SQRGUIStyleManager::GetInst()->GetActiveStyle()->GetStyleStruct(m_strWnd);\
	if(NULL==m_pStyleStruct)m_pStyleStruct = SQRGUIStyleManager::GetInst()->GetActiveStyle()->SetStyleStruct(m_strWnd,StyleStruct());\
	m_TextureEdit.SetWindowText( utf8_to_utf16(m_pStyleStruct->m_TexString.c_str()).c_str());

IMPLEMENT_DYNCREATE(CWndStyleEditor, CFormView)

CWndStyleEditor::CWndStyleEditor()
	: CFormView(CWndStyleEditor::IDD)
	, m_Mask(IP_COUNT)
	, m_pBlock(NULL)
	, m_BlockIndex(0)
	, m_pBlockPool(NULL)
{
}

CWndStyleEditor::~CWndStyleEditor()
{
}

void CWndStyleEditor::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STYLE_SELECT, m_StylesList);
	DDX_Control(pDX, IDC_WND_BKTYPE2, m_ListType);
	DDX_Control(pDX, IDC_EDIT1, m_TextureEdit);
	DDX_Control(pDX, IDC_EDIT_LEFT, m_rectLeft);
	DDX_Control(pDX, IDC_EDIT_TOP, m_rectTop);
	DDX_Control(pDX, IDC_EDIT_RIGHT, m_rectRight);
	DDX_Control(pDX, IDC_EDIT_BOTTOM, m_rectBottom);

	DDX_Control(pDX, IDC_WND_FONT_TYPE, m_FontName);
	DDX_Control(pDX, IDC_EDIT_FONT_SIZE, m_FontSize);
	DDX_Control(pDX, IDC_AEDIT_FONT_TEXTCOLOR, m_FontColor);
	DDX_Control(pDX, IDC_AEDIT_FONT_TEXTCOLOR1, m_FontColor2);
	DDX_Control(pDX, IDC_AEDIT_FONT_TEXTBCCOLOR, m_FontBKC);

	DDX_Control(pDX, IDC_WND_NineBlock, m_NineBlock);
	DDX_Control(pDX, IDC_TOP_HIGHT, m_TopHeight);
	DDX_Control(pDX, IDC_BOTTOM_HIGHT, m_BottomHeight);
	DDX_Control(pDX, IDC_LEFT_WIDTH, m_LeftWidth);
	DDX_Control(pDX, IDC_RIGHT_WIDTH, m_RightWidth);
	DDX_Control(pDX, IDC_BLOCK_INDEX, m_BlockPoolIndex);
}

BEGIN_MESSAGE_MAP(CWndStyleEditor, CFormView)
	ON_BN_CLICKED(IDC_Style_OK, &CWndStyleEditor::OnBnClickedStyleOk)
	ON_BN_CLICKED(IDC_Style_Add, &CWndStyleEditor::OnBnClickedStyleAdd)
	ON_BN_CLICKED(IDC_Style_Del, &CWndStyleEditor::OnBnClickedStyleDel)
	ON_BN_CLICKED(IDC_Style_Save, &CWndStyleEditor::OnBnClickedStyleSave)

	ON_BN_CLICKED(IDC_CLOCK, OnBnClickedClock)
	ON_BN_CLICKED(IDC_AWNDMOVE, OnBnClickedWndMove)
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
	ON_BN_CLICKED(IDC_CHART, OnBnClickedChart)
	ON_BN_CLICKED(IDC_AICON2, OnBnClickedAicon)
	ON_BN_CLICKED(IDC_CWnd, OnBnClickedCWnd)

	ON_BN_CLICKED(IDC_BTN_FONT_CLR, &CWndStyleEditor::OnBnClickedBtnClr1)
	ON_BN_CLICKED(IDC_BTN_FONT_CLR1, &CWndStyleEditor::OnBnClickedBtnClr2)
	ON_BN_CLICKED(IDC_BTN_FONT_BKTEX, &CWndStyleEditor::OnBnClickedBtnBkclr)
	ON_BN_CLICKED(IDC_BUTTON_FONT_OK, &CWndStyleEditor::OnBnClickedFontOk)

	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_STYLE_SOK, &CWndStyleEditor::OnBnClickedStyleSok)
	ON_BN_CLICKED(IDC_Select_Pic, &CWndStyleEditor::OnBnClickedSelectPic)
	ON_CBN_SELCHANGE(IDC_WND_BKTYPE2, &CWndStyleEditor::OnCbnSelchangeWndBktype2)
	ON_BN_CLICKED(IDC_Replace_All, &CWndStyleEditor::OnBnClickedReplaceAll)
	ON_BN_CLICKED(IDC_CHECK_AlphaBlend, &CWndStyleEditor::OnBnClickedCheckAlphablend)
	ON_BN_CLICKED(IDC_CHECK_ALPHATEST, &CWndStyleEditor::OnBnClickedCheckAlphatest)
	ON_EN_CHANGE(IDC_EDIT_TOP, &CWndStyleEditor::OnEnChangeEditTop)
	ON_EN_CHANGE(IDC_EDIT_LEFT, &CWndStyleEditor::OnEnChangeEditLeft)
	ON_EN_CHANGE(IDC_EDIT_RIGHT, &CWndStyleEditor::OnEnChangeEditRight)
	ON_EN_CHANGE(IDC_EDIT_BOTTOM, &CWndStyleEditor::OnEnChangeEditBottom)
	ON_CBN_SELCHANGE(IDC_WND_NineBlock, &CWndStyleEditor::OnCbnSelchangeWndNineblock)
	ON_BN_CLICKED(IDC_BLOCK_LEFT_TOP, &CWndStyleEditor::OnBnClickedBlockLeftTop)
	ON_BN_CLICKED(IDC_BLOCK_TOP, &CWndStyleEditor::OnBnClickedBlockTop)
	ON_BN_CLICKED(IDC_BLOCK_RIGHT_TOP, &CWndStyleEditor::OnBnClickedBlockRightTop)
	ON_BN_CLICKED(IDC_BLOCK_LEFT, &CWndStyleEditor::OnBnClickedBlockLeft)
	ON_BN_CLICKED(IDC_BLOCK_CENTER, &CWndStyleEditor::OnBnClickedBlockCenter)
	ON_BN_CLICKED(IDC_BLOCK_RIGHT, &CWndStyleEditor::OnBnClickedBlockRight)
	ON_BN_CLICKED(IDC_BLOCK_LEFT_BOTTOM, &CWndStyleEditor::OnBnClickedBlockLeftBottom)
	ON_BN_CLICKED(IDC_BLOCK_BOTTOM, &CWndStyleEditor::OnBnClickedBlockBottom)
	ON_BN_CLICKED(IDC_BLOCK_RIGHT_BOTTOM, &CWndStyleEditor::OnBnClickedBlockRightBottom)
	ON_BN_CLICKED(IDC_AutoTool, &CWndStyleEditor::OnBnClickedAutotool)
	ON_BN_CLICKED(IDC_ADD_BLOCK, &CWndStyleEditor::OnBnClickedAddBlock)
	ON_CBN_SELCHANGE(IDC_BLOCK_INDEX, &CWndStyleEditor::OnCbnSelchangeBlockIndex)
END_MESSAGE_MAP()


BOOL CWndStyleEditor::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	BOOL re = CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	UpdateData( FALSE );

	( ( ::CButton* )GetDlgItem( IDC_ABUTTON ) )		->SetIcon( AfxGetApp()->LoadIcon( IDI_BUTTON ) );
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
	( ( ::CButton* )GetDlgItem( IDC_AWNDMOVE ) )	->SetIcon( AfxGetApp()->LoadIcon( IDI_WNDMOVE ) );
	( ( ::CButton* )GetDlgItem( IDC_CLOCK ) )		->SetIcon( AfxGetApp()->LoadIcon( IDI_CLOCK ) );
	( ( ::CButton* )GetDlgItem( IDC_CHART ) )		->SetIcon( AfxGetApp()->LoadIcon( IDI_CHART ) );
	( ( ::CButton* )GetDlgItem( IDC_AICON2 ) )		->SetIcon( AfxGetApp()->LoadIcon( IDI_ICON ) );
	( ( ::CButton* )GetDlgItem( IDC_CWnd ) )		->SetIcon( AfxGetApp()->LoadIcon( IDI_CWND ) );
	
	for( int i = 0; i<GBM_COUNT; ++i )
	{
		m_NineBlock.AddString(BlockList[i].c_str());
		( ( ::CButton* )GetDlgItem( BlockIDC[i] ) )->SetCheck(0);
	}
	m_NineBlock.SetCurSel(GBM_CENTER);
	( ( ::CButton* )GetDlgItem( BlockIDC[GBM_CENTER] ) )->SetCheck(1);
	m_BlockIndex = GBM_CENTER;

	m_TopHeight.SetWindowText(L"10");
	m_BottomHeight.SetWindowText(L"10");
	m_LeftWidth.SetWindowText(L"10");
	m_RightWidth.SetWindowText(L"10");

	// 初始化字体信息
	int32 nCurLangIndex = CFontMgr::GetInst()->GetLangIndex();
	int32 nFontCount	= CFontCfg::GetInst()->GetGDIFontCount();
	for ( int32 font_id = 0; font_id < nFontCount; ++font_id )
	{
		wstring szCurFontName = CFontCfg::GetInst()->GetGDIFontName(font_id);
		m_FontName.AddString(szCurFontName.c_str());
	}
	m_FontName.SetItemHeight( -1, 12 );

	return re;
}


// CWndStyleEditor diagnostics

#ifdef _DEBUG
void CWndStyleEditor::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CWndStyleEditor::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CWndStyleEditor message handlers

void CWndStyleEditor::OnBnClickedStyleOk()
{
	// TODO: Add your control notification handler code here
	CString str;
	m_StylesList.GetWindowText(str);
	SQRGUIStyle* pStyle = SQRGUIStyleManager::GetInst()->ActiveStyle(utf16_to_utf8(str.GetBuffer()).c_str());
	( ( ::CButton* )GetDlgItem( IDC_ABUTTON ) )		->SetCheck( 0 ) ;
	( ( ::CButton* )GetDlgItem( IDC_ACHECKBUTTON ) )->SetCheck( 0 ) ;
	( ( ::CButton* )GetDlgItem( IDC_ACOMBOBOX ) )	->SetCheck( 0 ) ;
	( ( ::CButton* )GetDlgItem( IDC_AEDIT ) )		->SetCheck( 0 ) ;
	( ( ::CButton* )GetDlgItem( IDC_ARICHTEXT ) )	->SetCheck( 0 ) ;
	( ( ::CButton* )GetDlgItem( IDC_APROGRESS ) )	->SetCheck( 0 ) ;
	( ( ::CButton* )GetDlgItem( IDC_ASCROLLBAR ) )	->SetCheck( 0 ) ;
	( ( ::CButton* )GetDlgItem( IDC_ASPIN ) )		->SetCheck( 0 ) ;
	( ( ::CButton* )GetDlgItem( IDC_ASTATIC ) )		->SetCheck( 0 ) ;
	( ( ::CButton* )GetDlgItem( IDC_ALISTCTRL ) )	->SetCheck( 0 ) ;
	( ( ::CButton* )GetDlgItem( IDC_ATREECTRL ) )	->SetCheck( 0 ) ;
	( ( ::CButton* )GetDlgItem( IDC_AWNDMOVE ) )	->SetCheck( 0 ) ;
	( ( ::CButton* )GetDlgItem( IDC_CLOCK ) )		->SetCheck( 0 ) ;
	( ( ::CButton* )GetDlgItem( IDC_CHART ) )		->SetCheck( 0 ) ;
	( ( ::CButton* )GetDlgItem( IDC_AICON2 ) )		->SetCheck( 0 ) ;
	( ( ::CButton* )GetDlgItem( IDC_CWnd ) )		->SetCheck( 1 ) ;
	OnBnClickedCWnd();
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedStyleAdd()
{
	// TODO: Add your control notification handler code here
	CString str;
	m_StylesList.GetWindowText(str);
	SQRGUIStyleManager::GetInst()->AddStyle(utf16_to_utf8(str.GetBuffer()).c_str());

	CString path = L"..\\gui\\style\\" + str;
	_wmkdir(path.GetBuffer());
	RefreshList();
}

void CWndStyleEditor::OnBnClickedStyleDel()
{
	// TODO: Add your control notification handler code here
	CString str;
	m_StylesList.GetWindowText(str);
	SQRGUIStyleManager::GetInst()->DelStyle(utf16_to_utf8(str.GetBuffer()).c_str());
	RefreshList();
}

void CWndStyleEditor::OnBnClickedStyleSave()
{
	SQRGUIStyleManager::GetInst()->Save();
}

void CWndStyleEditor::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFormView::OnShowWindow(bShow, nStatus);
	if(bShow)
	{
		RefreshList();
		if(SQRGUIStyleManager::HasInst())
		{
			m_StylesList.SetWindowText(utf8_to_utf16(SQRGUIStyleManager::GetInst()->GetActiveName()).c_str());

			( ( ::CButton* )GetDlgItem( IDC_ABUTTON ) )		->SetCheck( 0 ) ;
			( ( ::CButton* )GetDlgItem( IDC_ACHECKBUTTON ) )->SetCheck( 0 ) ;
			( ( ::CButton* )GetDlgItem( IDC_ACOMBOBOX ) )	->SetCheck( 0 ) ;
			( ( ::CButton* )GetDlgItem( IDC_AEDIT ) )		->SetCheck( 0 ) ;
			( ( ::CButton* )GetDlgItem( IDC_ARICHTEXT ) )	->SetCheck( 0 ) ;
			( ( ::CButton* )GetDlgItem( IDC_APROGRESS ) )	->SetCheck( 0 ) ;
			( ( ::CButton* )GetDlgItem( IDC_ASCROLLBAR ) )	->SetCheck( 0 ) ;
			( ( ::CButton* )GetDlgItem( IDC_ASPIN ) )		->SetCheck( 0 ) ;
			( ( ::CButton* )GetDlgItem( IDC_ASTATIC ) )		->SetCheck( 0 ) ;
			( ( ::CButton* )GetDlgItem( IDC_ALISTCTRL ) )	->SetCheck( 0 ) ;
			( ( ::CButton* )GetDlgItem( IDC_ATREECTRL ) )	->SetCheck( 0 ) ;
			( ( ::CButton* )GetDlgItem( IDC_AWNDMOVE ) )	->SetCheck( 0 ) ;
			( ( ::CButton* )GetDlgItem( IDC_CLOCK ) )		->SetCheck( 0 ) ;
			( ( ::CButton* )GetDlgItem( IDC_CHART ) )		->SetCheck( 0 ) ;
			( ( ::CButton* )GetDlgItem( IDC_AICON2 ) )		->SetCheck( 0 ) ;
			( ( ::CButton* )GetDlgItem( IDC_CWnd ) )		->SetCheck( 1 ) ;
			( ( ::CButton* )GetDlgItem( IDC_CHECK_ALPHATEST ) )	->SetCheck( m_StyleRender.GetAlphaTest()	) ;
			( ( ::CButton* )GetDlgItem( IDC_CHECK_AlphaBlend ) )->SetCheck( m_StyleRender.GetAlphaBlend()	) ;
			OnBnClickedCWnd();
		}
		m_StyleRender.BeginRenderer();
	}
	else
		m_StyleRender.EndRenderer();
	m_bIsButtonDown = false;
}
void CWndStyleEditor::RefreshList(void)
{
	while( m_StylesList.DeleteString( 0 ) >=0 );
	if(SQRGUIStyleManager::HasInst())
	{
		SQRGUIStyleManager::STYLE_MAP::iterator it,eit = SQRGUIStyleManager::GetInst()->End();

		for( it = SQRGUIStyleManager::GetInst()->Begin(); it != eit; ++it )
			m_StylesList.AddString( utf8_to_utf16(it->second->GetStyleName()).c_str() );
	}
}

void CWndStyleEditor::OnBnClickedStyleSok()
{
	// 更改成保存所有的
	SQRGUIStyleManager::GetInst()->Save();
	SQRGUIStyleManager::STYLE_MAP::iterator itr = SQRGUIStyleManager::GetInst()->Begin();
	SQRGUIStyleManager::STYLE_MAP::iterator eitr = SQRGUIStyleManager::GetInst()->End();
	ClearResData(eRes_T_Style);
	while(itr != eitr)
	{
		SQRGUIStyle* pStyle = itr->second;
		pStyle->SetXmlResSaver(m_ResSaver);
		if(NULL != pStyle)
			pStyle->Save();
		++itr;
	}
	SaveResData();

	/*SQRGUIStyle* pStyle = SQRGUIStyleManager::GetInst()->GetActiveStyle();
	if(NULL != pStyle)
	{
		pStyle->Save();
	}*/
}

void CWndStyleEditor::OnBnClickedSelectPic()
{
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	// TODO: Add your control notification handler code here
	CString FileName = GetFileName(true, L"GUI Texture (*.tex *.dds)|*.tex;*.dds||");
	
	SQRGUIStyle* pStyle = SQRGUIStyleManager::GetInst()->GetActiveStyle();
	
	CString CurPath = utf8_to_utf16(pStyle->GetTexturePath()).c_str();
	CurPath.MakeLower();
	wstring Str = FileName.GetBuffer();
	wstring TexStr = Str.substr(Str.rfind(L"\\") + 1, Str.size());
	CString TexPath = Str.substr(0, Str.rfind(L"\\") + 1).c_str();

	if(m_pStyleStruct!=NULL &&
		Str.size()>0 &&
		m_pStyleStruct->m_TexString!=utf16_to_utf8(TexStr).c_str() &&
		TexPath == CurPath )
	{
		m_pStyleStruct->m_TexString = utf16_to_utf8(TexStr).c_str();
		m_pStyleStruct->Release();
		CGraphic::GetInst()->CreateTexture(L"",  
			(SQRGUIStyleManager::GetInst()->GetActiveStyle()->GetTexturePath()+utf16_to_utf8(TexStr)).c_str(),
			&(m_pStyleStruct->pTexture) );
		m_TextureEdit.SetWindowText(TexStr.c_str());
		RefreshMaskList();
	}
}

void CWndStyleEditor::OnBnClickedWndMove()     
{
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQRWndOffSet);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQRWndOffSet::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	}
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedClock()      
{
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQRClock);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQRClock::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	}
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedButton()	 
{
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQRButton);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQRButton::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	} 
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedCheckbutton()
{ 
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQRCheckButton);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQRCheckButton::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	}
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedCombobox()	 
{
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQRComboBox);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQRComboBox::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	}
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedEdit()	
{ 
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQREdit);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQREdit::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	}		
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedRichwnd()	  
{ 
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQRRichText);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQRRichText::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	}	
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedListctrl()	 
{
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQRListCtrl);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQRListCtrl::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	}
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedProgress()	  
{
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQRProgress);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQRProgress::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	}
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedScrollbar()  
{ 
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQRScrollBar);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQRScrollBar::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	}
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedSpin()		 
{ 
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQRSpin);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQRSpin::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	}	
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedStatic()
{ 
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQRStatic);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQRStatic::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	}
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedTreectrl()	
{
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQRTreeCtrl);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQRTreeCtrl::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	}	
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedChart()		  
{ 
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQRChart);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQRChart::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	}
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedAicon()
{
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQRIcon);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQRIcon::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	}
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::OnBnClickedCWnd()
{
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;
	while( m_ListType.DeleteString( 0 ) >=0 );
	UPDATE_STR(SQRWnd);
	for ( int i = IP_MASK; i < IP_COUNT; i++ )
	{
		if ( SQRWnd::IsHasEventState(static_cast<EventStateMask>(i)) )
		{
			int nIndex = m_ListType.AddString( TextureTypeList[i].c_str() );
			if ( nIndex < 0 )
				continue;
			m_ListType.SetItemData( nIndex, i );
		}
	}
	m_pBlock = NULL;
	RefreshMaskList();
}

void CWndStyleEditor::RefreshMaskList(void)
{
	//m_ListType.SetWindowText( TextureTypeList[::sqr::IP_ENABLE].c_str() );
	OnCbnSelchangeWndBktype2();
	RefreshView();
}

void CWndStyleEditor::RefreshView(void)
{
	CString Temp;
	Temp.Format(L"%d",int(m_Rect.left));
	m_rectLeft.SetWindowText(Temp.GetBuffer());

	Temp.Format(L"%d",int(m_Rect.top));
	m_rectTop.SetWindowText(Temp.GetBuffer());

	Temp.Format(L"%d",int(m_Rect.right));
	m_rectRight.SetWindowText(Temp.GetBuffer());

	Temp.Format(L"%d",int(m_Rect.bottom));
	m_rectBottom.SetWindowText(Temp.GetBuffer());

	if(m_pStyleStruct)
		m_StyleRender.SetBackTexture(m_pStyleStruct->pTexture);
	else
		m_StyleRender.SetBackTexture(NULL);
	m_StyleRender.SetDrawRect(m_Rect);
}

void CWndStyleEditor::RefreshBlockPoolIndex(void)
{
	m_BlockPoolIndex.SetWindowText(L"");
	while( m_BlockPoolIndex.DeleteString( 0 ) >=0 );
	if(m_pBlockPool==NULL)
		return;
	
	int FirstIndex = -1;
	for( uint i = 1;i<=BLOCK_POOL::MAX_BLOCK_NUM;++i )
	{
		if( (*m_pBlockPool)[i]!= NULL )
		{
			if(FirstIndex==-1)
				FirstIndex = i;
			wstring temp;
			format(temp, L"%d", i);
			m_BlockPoolIndex.AddString( temp.c_str() );
		}
	}	
	if(FirstIndex!=-1)
	{
		wstring temp;
		format(temp, L"%d", FirstIndex);
		m_BlockPoolIndex.SetWindowText(temp.c_str());
	}
	OnCbnSelchangeBlockIndex();
}

void CWndStyleEditor::OnCbnSelchangeWndBktype2()
{
	SQRGUIStyleManager* pStyleManager = SQRGUIStyleManager::GetInst();
	
	if(pStyleManager)
	{
		if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
			return;

		CString Mask;
		m_ListType.GetWindowText(Mask);
		wstring	mask = Mask.GetBuffer();
		m_pBlockPool = NULL;
		if(mask.size()>0)
		{
			for ( int i = IP_MASK; i < IP_COUNT; i++ )
			{
				if(TextureTypeList[i]==mask)
				{
					m_Mask = static_cast<EventStateMask>(i);
					m_pBlockPool = &(m_pStyleStruct->m_vecPool[m_Mask]);	
				}
			}
		}
		RefreshBlockPoolIndex();
	}	
}

void CWndStyleEditor::OnBnClickedReplaceAll()
{
	if(NULL == SQRGUIStyleManager::GetInst()->GetActiveStyle())
		return;

	SQRGUIStyle* pStyle = SQRGUIStyleManager::GetInst()->GetActiveStyle();
	CString Str;
	m_TextureEdit.GetWindowText(Str);
	Str.Trim();
	if(NULL!=pStyle&& Str.GetLength()>0)
	{
		SQRGUIStyle::RectMap::iterator it,eit = pStyle->End();
		string TexString = utf16_to_utf8(Str.GetBuffer());
		string TexPath = pStyle->GetTexturePath() + TexString;
		for( it=pStyle->Begin(); it!=eit; ++it )
		{
			it->second->m_TexString = TexString.c_str();
			it->second->Release();
			CGraphic::GetInst()->CreateTexture( L"", TexPath.c_str(), &(it->second->pTexture) );
		}
	}
}

void CWndStyleEditor::OnBnClickedCheckAlphablend()
{
	// TODO: Add your control notification handler code here
	m_StyleRender.SetAlphaBlend((( ::CButton* )GetDlgItem( IDC_CHECK_AlphaBlend ) )->GetCheck()==TRUE );

}

void CWndStyleEditor::OnBnClickedCheckAlphatest()
{
	// TODO: Add your control notification handler code here
	m_StyleRender.SetAlphaTest((( ::CButton* )GetDlgItem( IDC_CHECK_ALPHATEST ) )->GetCheck()==TRUE );
}

void CWndStyleEditor::StyleProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	if(m_StyleRender.IsStateInRender())
	{
		if(m_StyleRender.GetShowRect())
			ProcEditRect(uMessage,wParam,lParam);
		else
			ProcEditOffSet(uMessage,wParam,lParam);
	}
}

void CWndStyleEditor::ProcEditRect(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	float X = float( LOWORD(lParam) ) - m_StyleRender.m_fOffX;
	float Y = float( HIWORD(lParam) ) - m_StyleRender.m_fOffY;
	X = limit2(X,0.0f,float(m_StyleRender.m_Width));
	Y = limit2(Y,0.0f,float(m_StyleRender.m_Height));
	switch( uMessage ) 
	{
	case WM_MOUSEMOVE:
		if(m_bIsButtonDown)
		{
			if(Y>m_MouseButtonY)
			{
				m_Rect.top		= m_MouseButtonY;
				m_Rect.bottom	= Y;
			}
			else
			{
				m_Rect.top		= Y;
				m_Rect.bottom	= m_MouseButtonY;
			}


			if(X>m_MouseButtonX)
			{
				m_Rect.left	= m_MouseButtonX;
				m_Rect.right	= X;
			}
			else
			{
				m_Rect.left	= m_MouseButtonX;
				m_Rect.right	= X;
			}

			if(m_pBlock)
				((*m_pBlock)[m_BlockIndex]) = m_Rect;
			RefreshView();
		}
		break;
	case WM_LBUTTONUP:
		m_bIsButtonDown = false;
		break;

	case WM_LBUTTONDOWN:
		m_bIsButtonDown = true;
		m_MouseButtonX = X;
		m_MouseButtonY = Y;
		break;
	default:
		break;
	}
}

void CWndStyleEditor::ProcEditOffSet(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	float X = float( LOWORD(lParam) );
	float Y = float( HIWORD(lParam) );
	switch( uMessage ) 
	{
	case WM_MOUSEMOVE:
		if(m_bIsButtonDown)
			m_StyleRender.SetOffSet(X - m_MouseButtonX,Y - m_MouseButtonY);
		m_MouseButtonX = X;
		m_MouseButtonY = Y;
		break;
	case WM_LBUTTONUP:
		m_bIsButtonDown = false;
		break;

	case WM_LBUTTONDOWN:
		m_bIsButtonDown = true;
		break;
	default:
		break;
	}
}

void CWndStyleEditor::OnEnChangeEditTop()
{
	CString Str;
	m_rectTop.GetWindowText(Str);
	m_Rect.top = (float)_wtof( Str.GetBuffer() );
	m_StyleRender.SetDrawRect(m_Rect);
	if(m_pBlock)
		((*m_pBlock)[m_BlockIndex]) = m_Rect;
}

void CWndStyleEditor::OnEnChangeEditLeft()
{
	CString Str;
	m_rectLeft.GetWindowText(Str);
	m_Rect.left = (float)_wtof( Str.GetBuffer() );
	m_StyleRender.SetDrawRect(m_Rect);
	if(m_pBlock)
		((*m_pBlock)[m_BlockIndex]) = m_Rect;
}

void CWndStyleEditor::OnEnChangeEditRight()
{
	CString Str;
	m_rectRight.GetWindowText(Str);
	m_Rect.right = (float)_wtof( Str.GetBuffer() );
	m_StyleRender.SetDrawRect(m_Rect);
	if(m_pBlock)
		((*m_pBlock)[m_BlockIndex]) = m_Rect;
}

void CWndStyleEditor::OnEnChangeEditBottom()
{
	CString Str;
	m_rectBottom.GetWindowText(Str);
	m_Rect.bottom = (float)_wtof( Str.GetBuffer() );
	m_StyleRender.SetDrawRect(m_Rect);
	if(m_pBlock)
		((*m_pBlock)[m_BlockIndex]) = m_Rect;
}

void CWndStyleEditor::OnCbnSelchangeWndNineblock()
{
	// TODO: Add your control notification handler code here
	if(NULL==m_pBlock)
	{
		MessageBox(L"请选择窗口状态");
		return;
	}

	if(m_pStyleStruct&&m_Mask<IP_COUNT)
		((*m_pBlock)[m_BlockIndex]) = m_Rect;
	m_BlockIndex = m_NineBlock.GetCurSel();
	m_Rect = (*m_pBlock)[m_BlockIndex];

	for( int i = 0; i<GBM_COUNT; ++i )
		( ( ::CButton* )GetDlgItem( BlockIDC[i] ) )->SetCheck(0);
	( ( ::CButton* )GetDlgItem( BlockIDC[m_BlockIndex] ) )->SetCheck(1);
	RefreshView();
}

void CWndStyleEditor::OnBnClickedBlockLeftTop()
{
	m_NineBlock.SetCurSel(GBM_TOP_LEFT);
	OnCbnSelchangeWndNineblock();
}

void CWndStyleEditor::OnBnClickedBlockTop()
{
	m_NineBlock.SetCurSel(GBM_TOP);
	OnCbnSelchangeWndNineblock();
}

void CWndStyleEditor::OnBnClickedBlockRightTop()
{
	m_NineBlock.SetCurSel(GBM_TOP_RIGHT);
	OnCbnSelchangeWndNineblock();
}

void CWndStyleEditor::OnBnClickedBlockLeft()
{
	m_NineBlock.SetCurSel(GBM_LEFT);
	OnCbnSelchangeWndNineblock();
}

void CWndStyleEditor::OnBnClickedBlockCenter()
{
	m_NineBlock.SetCurSel(GBM_CENTER);
	OnCbnSelchangeWndNineblock();
}

void CWndStyleEditor::OnBnClickedBlockRight()
{
	m_NineBlock.SetCurSel(GBM_RIGHT);
	OnCbnSelchangeWndNineblock();
}

void CWndStyleEditor::OnBnClickedBlockLeftBottom()
{
	m_NineBlock.SetCurSel(GBM_BOTTOM_LEFT);
	OnCbnSelchangeWndNineblock();
}

void CWndStyleEditor::OnBnClickedBlockBottom()
{
	m_NineBlock.SetCurSel(GBM_BOTTOM);
	OnCbnSelchangeWndNineblock();
}

void CWndStyleEditor::OnBnClickedBlockRightBottom()
{
	m_NineBlock.SetCurSel(GBM_BOTTOM_RIGHT);
	OnCbnSelchangeWndNineblock();
}

void CWndStyleEditor::OnBnClickedAutotool()
{
	if(NULL == m_pBlock)
		return;
	GUI_BLOCK& Block = *m_pBlock;

	float fLeftWidth	= 0.0f;
	float fRightWidth	= 0.0f;
	float fTopHeight	= 0.0f;
	float fBottomHeight = 0.0f;

	CString Str;
	m_LeftWidth.GetWindowText(Str);
	fLeftWidth	= (float)_wtof(Str.GetBuffer());

	m_RightWidth.GetWindowText(Str);
	fRightWidth	= (float)_wtof(Str.GetBuffer());
	
	m_TopHeight.GetWindowText(Str);
	fTopHeight		= (float)_wtof(Str.GetBuffer());
	
	m_BottomHeight.GetWindowText(Str);
	fBottomHeight	= (float)_wtof(Str.GetBuffer());

	Block[GBM_TOP].top		= m_Rect.top;
	Block[GBM_TOP].left		= m_Rect.left	+ fLeftWidth;
	Block[GBM_TOP].right	= m_Rect.right	- fRightWidth;
	Block[GBM_TOP].bottom	= m_Rect.top	+ fTopHeight;

	Block[GBM_LEFT].top		= m_Rect.top	+ fTopHeight;
	Block[GBM_LEFT].left	= m_Rect.left;
	Block[GBM_LEFT].right	= m_Rect.left	+ fLeftWidth;
	Block[GBM_LEFT].bottom	= m_Rect.bottom	- fBottomHeight;

	Block[GBM_RIGHT].top	= m_Rect.top	+ fTopHeight;
	Block[GBM_RIGHT].left	= m_Rect.right	- fLeftWidth;
	Block[GBM_RIGHT].right	= m_Rect.right;
	Block[GBM_RIGHT].bottom	= m_Rect.bottom	- fBottomHeight;

	Block[GBM_BOTTOM].top	= m_Rect.bottom	- fBottomHeight;
	Block[GBM_BOTTOM].left	= m_Rect.left	+ fLeftWidth;
	Block[GBM_BOTTOM].right	= m_Rect.right	- fRightWidth;
	Block[GBM_BOTTOM].bottom= m_Rect.bottom;

	Block[GBM_CENTER].top		= m_Rect.top	+ fTopHeight;
	Block[GBM_CENTER].left		= m_Rect.left	+ fLeftWidth;
	Block[GBM_CENTER].right		= m_Rect.right	- fRightWidth;
	Block[GBM_CENTER].bottom	= m_Rect.bottom	- fBottomHeight;

	Block[GBM_TOP_LEFT].top		= m_Rect.top;
	Block[GBM_TOP_LEFT].left	= m_Rect.left;
	Block[GBM_TOP_LEFT].right	= m_Rect.left	+ fLeftWidth;
	Block[GBM_TOP_LEFT].bottom	= m_Rect.top	+ fTopHeight;

	Block[GBM_TOP_RIGHT].top	= m_Rect.top;
	Block[GBM_TOP_RIGHT].left	= m_Rect.right	- fRightWidth;
	Block[GBM_TOP_RIGHT].right	= m_Rect.right;
	Block[GBM_TOP_RIGHT].bottom	= m_Rect.top	+ fTopHeight;

	Block[GBM_BOTTOM_LEFT].top		= m_Rect.bottom	- fBottomHeight;
	Block[GBM_BOTTOM_LEFT].left		= m_Rect.left;
	Block[GBM_BOTTOM_LEFT].right	= m_Rect.left	+ fLeftWidth;
	Block[GBM_BOTTOM_LEFT].bottom	= m_Rect.bottom;

	Block[GBM_BOTTOM_RIGHT].top		= m_Rect.bottom	- fBottomHeight;
	Block[GBM_BOTTOM_RIGHT].left	= m_Rect.right	- fRightWidth;
	Block[GBM_BOTTOM_RIGHT].right	= m_Rect.right;
	Block[GBM_BOTTOM_RIGHT].bottom	= m_Rect.bottom;

	m_Rect = (*m_pBlock)[m_BlockIndex];
	OnCbnSelchangeWndNineblock();
}

void CWndStyleEditor::OnBnClickedAddBlock()
{
	// TODO: Add your control notification handler code here
	CString str;
	m_BlockPoolIndex.GetWindowText(str);
	int Index = _wtoi(str.GetBuffer());
	if(Index<1 || Index>32)
		::MessageBox(m_hWnd, L"输入数据非法，请保持在1~32之间", L"温馨提示",0);
	if((*m_pBlockPool)[Index]==NULL)
	{
		m_pBlockPool->InitBlock(Index);
		wstring temp;
		format(temp, L"%d", Index);
		m_BlockPoolIndex.AddString( temp.c_str() );
	}
	OnCbnSelchangeBlockIndex();
}

void CWndStyleEditor::OnCbnSelchangeBlockIndex()
{
	// TODO: Add your control notification handler code here
	CString str;
	int SelIndex = m_BlockPoolIndex.GetCurSel();
	if(SelIndex == -1)
		m_BlockPoolIndex.GetWindowText(str);
	else
		m_BlockPoolIndex.GetLBText(SelIndex,str);

	int Index = _wtoi(str.GetBuffer());
	if(Index<1 || Index>32)
		return;

	m_pBlock = (*m_pBlockPool)[Index];

	if(m_pBlock==NULL)
		return;

	m_Rect = (*m_pBlock)[m_BlockIndex];
	for( int i = 0; i<GBM_COUNT; ++i )
		( ( ::CButton* )GetDlgItem( BlockIDC[i] ) )->SetCheck(0);
	( ( ::CButton* )GetDlgItem( BlockIDC[GBM_CENTER] ) )->SetCheck(1);

	// 更新字体信息
	// 是否使用风格字体
	if (m_pStyleStruct)
	{
		StyleStruct::FontVector::iterator itr = m_pStyleStruct->m_vecFont.find(Index);
		if ( itr != m_pStyleStruct->m_vecFont.end())
		{
			wchar_t Buf[256];
			if (itr->second)
			{
				// 是否使用字体
				CheckDlgButton(IDC_CHECK_EDIT_FONT, TRUE);
				m_FontEffectMask = itr->second->m_uFontEffectMask;
				m_FontColor2.SetWindowText( _ultow( itr->second->m_uFontGradualColor, Buf, 16 ) );

				m_FontColor.SetWindowText( _ultow( itr->second->m_EventFont[m_Mask].m_FontColor, Buf, 16 ) );
				m_FontBKC.SetWindowText( _ultow( itr->second->m_EventFont[m_Mask].m_uFontBkColor, Buf, 16 ) );
				// 字体风格
				CheckDlgButton(IDC_CHECK_FONT_ITALIC, m_FontEffectMask & FontEffect::Italic);
				CheckDlgButton(IDC_CHECK_OUTLINE, m_FontEffectMask & FontEffect::Outline);
				CheckDlgButton(IDC_CHECK_SHADOW, m_FontEffectMask & FontEffect::Shadow);
				CheckDlgButton(IDC_CHECK_GRADUAL, m_FontEffectMask & FontEffect::Gradual);
				// 字体和大小
				m_FontName.SetCurSel(itr->second->m_nFontID);
				m_FontSize.SetWindowText( _ultow( itr->second->m_FontSize, Buf,10 ) );
			}
			else
				CheckDlgButton(IDC_CHECK_EDIT_FONT, FALSE);
		}
		else
			CheckDlgButton(IDC_CHECK_EDIT_FONT, FALSE);
	}
	
	
	RefreshView();

}

void CWndStyleEditor::OnBnClickedFontOk()
{
	CString str;
	int SelIndex = m_BlockPoolIndex.GetCurSel();
	if(SelIndex == -1)
		m_BlockPoolIndex.GetWindowText(str);
	else
		m_BlockPoolIndex.GetLBText(SelIndex,str);

	int Index = _wtoi(str.GetBuffer());
	if(Index<1 || Index>32)
		return;

	if (IsDlgButtonChecked(IDC_CHECK_EDIT_FONT))
	{
		StyleStruct::FontVector::iterator itr = m_pStyleStruct->m_vecFont.find(Index);
		if ( itr == m_pStyleStruct->m_vecFont.end() || itr->second == NULL)
			m_pStyleStruct->m_vecFont[Index] = new GUI_FONT();

		GUI_FONT* font_info = m_pStyleStruct->m_vecFont[Index];
		if (!font_info)
			return;
		// 添加信息 字体ID
		int32	nFontID	  = m_FontName.GetCurSel();
		if ( nFontID == CFontMgr::INVALID_FONT_ID )
			nFontID = 0;
		font_info->m_nFontID = nFontID;
		// 字体大小
		m_FontSize.GetWindowText(str);
		font_info->m_FontSize = (uint32)_wtoi( str.GetBuffer() );
		// 字体边缘颜色m_Mask
		m_FontColor.GetWindowText(str);
		font_info->m_EventFont[m_Mask].m_FontColor = ( wcstoul(L"0x" + str, 0, 0) );
		m_FontColor2.GetWindowText(str);
		font_info->m_uFontGradualColor = ( wcstoul(L"0x" + str, 0, 0) );
		m_FontBKC.GetWindowText(str);
		font_info->m_EventFont[m_Mask].m_uFontBkColor = ( wcstoul(L"0x" + str, 0, 0) );
		// 字体样式
		m_FontEffectMask = 0;
		if ( IsDlgButtonChecked(IDC_CHECK_FONT_ITALIC) )
			m_FontEffectMask |= FontEffect::Italic;
		if ( IsDlgButtonChecked(IDC_CHECK_FONT_OUTLINE) )
			m_FontEffectMask |= FontEffect::Outline;
		if ( IsDlgButtonChecked(IDC_CHECK_FONT_SHADOW) )
			m_FontEffectMask |= FontEffect::Shadow;
		if ( IsDlgButtonChecked(IDC_CHECK_FONT_GRADUAL) )
			m_FontEffectMask |= FontEffect::Gradual;
		font_info->m_uFontEffectMask = m_FontEffectMask;
	}
	else
	{
		StyleStruct::FontVector::iterator itr = m_pStyleStruct->m_vecFont.find(Index);
		if ( itr == m_pStyleStruct->m_vecFont.end())
			return;

		GUI_FONT* font_info = m_pStyleStruct->m_vecFont[Index];
		if (!font_info)
			delete font_info;
		m_pStyleStruct->m_vecFont[Index] = NULL;
	}
}



void CWndStyleEditor::OnBnClickedBtnClr1()
{
	this->OnSelectColor(IDC_AEDIT_FONT_TEXTCOLOR);
}

void CWndStyleEditor::OnBnClickedBtnClr2()
{
	this->OnSelectColor(IDC_AEDIT_FONT_TEXTCOLOR1);
}

void CWndStyleEditor::OnBnClickedBtnBkclr()
{
	this->OnSelectColor(IDC_AEDIT_FONT_TEXTBCCOLOR);
}

//------------------------------------------------------------------------------
void
CWndStyleEditor::OnSelectColor( int id )
{
	CColorDialog dialog(0, CC_FULLOPEN, this);
	if (IDOK == dialog.DoModal())
	{
		COLORREF color = dialog.GetColor();
		DWORD d3dColor = 0xff000000 | GetRValue(color) << 16 | GetGValue(color) << 8 | GetBValue(color);
		wchar_t text[9];
		_snwprintf(&text[0], 9, L"%x", d3dColor);
		SetDlgItemText(id, text);
	}
}