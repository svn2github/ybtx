#include "stdafx.h"
#include "GUIEditor.h"
#include "CBasePropList.h"
#include "CWndGroup.h"
#include "CodeCvs.h"
#include "CLangCfg.h"
#include "CFontMgr.h"
#include <afx.h>


#define WND_PROP_WIDTH			60


std::string& Class_Name(std::string& ClassName, SQRWnd* ptr)
{
	ClassName = typeid(*ptr).name();
#ifdef _WIN32
	std::string Tem = ClassName.substr(ClassName.length()-1, ClassName.length()-1);

	if ((0 == ClassName.substr(0, 11).compare("class sqr::")) && (0 != Tem.compare(">")))
	{
		ClassName = ClassName.substr( 11, ClassName.length() - 11);
	}
	else if (0 == ClassName.substr(0, 5).compare("sqr::") && (0 != Tem.compare(">")))
	{
		ClassName = ClassName.substr( 5, ClassName.length() - 5);
	}
	else if (0 == ClassName.substr(0, 6).compare("class ") && (0 != Tem.compare(">")))
	{
		ClassName = ClassName.substr( 6, ClassName.length() - 6);
	}
	else
	{
		ClassName = "";
	}
#else
	if(ClassName[0] == 'N' || ClassName[0] == 'n')
	{
		//在sqr的命名空间里面，格式为N3sqr
		//后面紧跟的数字就是这个类的长度
		uint32 uPos = 0;
		uint32 uLen = 0;
		for(uint32 i = 5; i < ClassName.size(); i++)
		{
			if(!isdigit(ClassName[i]))
			{
				uLen = atoi(ClassName.substr(5, i - 5).c_str());
				uPos = i;
				break;
			}
		}

		ClassName = ClassName.substr(uPos, uLen); 
	}

	else if(isdigit(ClassName[0]))
	{
		uint32 uPos = 0;
		uint32 uLen = 0;
		for(uint32 i = 1; i < ClassName.size(); i++)
		{
			if(!isdigit(ClassName[i]))
			{
				uLen = atoi(ClassName.substr(0, i).c_str());
				uPos = i;
				break;
			}
		}

		ClassName = ClassName.substr(uPos, uLen); 
	}
#endif
	return ClassName;
}

// CBasePropList

int CALLBACK EnumFontsProc( CONST LOGFONT *lplf, CONST TEXTMETRIC *lptm, DWORD dwType, LPARAM lpData )
{
	::CComboBox* pFontComboBox = (::CComboBox*)lpData;
	if( pFontComboBox->FindString( 0, lplf->lfFaceName ) == CB_ERR )
		pFontComboBox->AddString( lplf->lfFaceName );
	return TRUE;
}

// CBasePropList

IMPLEMENT_DYNAMIC(CBasePropList, CListCtrl)
CBasePropList::CBasePropList()
{
}

CBasePropList::~CBasePropList()
{
}


BEGIN_MESSAGE_MAP(CBasePropList, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
	ON_NOTIFY_REFLECT(NM_SETFOCUS, OnNMSetfocus)
	ON_WM_PAINT()
	ON_EN_KILLFOCUS( 1000, OnEnKillfocusEdit)
	ON_CBN_KILLFOCUS( 1001, OnEnKillfocusEdit)
	ON_EN_KILLFOCUS( 1002, OnEnKillfocusEdit)
END_MESSAGE_MAP()


void CBasePropList::Init( CWndGroup* pWndGroup )
{
	m_pWndGroup = pWndGroup;
}

void CBasePropList::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	CListCtrl::PreSubclassWindow();

	CRect rt;
	GetClientRect( &rt );
	InsertColumn( 0, L"Prop", LVCFMT_LEFT, WND_PROP_WIDTH );
	InsertColumn( 1, L"Data", LVCFMT_LEFT, rt.Width() - WND_PROP_WIDTH );
	SetExtendedStyle( GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT );

	InsertItem( ITEM_ID_CAPTION,	L"Caption" );
	InsertItem( ITEM_ID_STYLE,		L"Style" );
	InsertItem( ITEM_ID_XPOS,		L"xPos" );
	InsertItem( ITEM_ID_YPOS,		L"yPos" );
	InsertItem( ITEM_ID_WIDTH,		L"Width" );
	InsertItem( ITEM_ID_HEIGHT,		L"Height" );
	InsertItem( ITEM_ID_GROUPID,	L"GroupID" );
	InsertItem( ITEM_ID_FONTID,		L"FontName" );
	InsertItem( ITEM_ID_FONTSIZE,	L"FontSize" );
	InsertItem( ITEM_ID_BOLDFONT,	L"BoldFont" );
	InsertItem( ITEM_ID_DESC,		L"Desc" );
	InsertItem( ITEM_ID_LOCKFLAG,	L"LockFlag" );
	InsertItem( ITEM_ID_USERCLASS,	L"UserClass" );
	InsertItem( ITEM_ID_USERINFO,	L"UserInfo" );
	InsertItem( ITEM_ID_CLICKFX,	L"ClickFx" );
	InsertItem( ITEM_ID_OPENFX,		L"OpenFx" );
	InsertItem( ITEM_ID_CLOSEFX,	L"CloseFx" );
	InsertItem( ITEM_ID_STYLE2,		L"Style" );
	InsertItem( ITEM_ID_SOURCE,		L"Source" );

	rt.left = WND_PROP_WIDTH;
	rt.top = 0;
	rt.bottom = 13;
	m_Edit.Create( ES_AUTOHSCROLL, rt,  this, 1000 );
	m_Edit.SetFont( GetFont() );

	m_nEditItem = -1;

	rt.bottom += 300;
	m_WndFont.Create( CBS_DROPDOWN|WS_VSCROLL, rt, this, 1001 );
	m_WndFont.SetFont( GetFont() );

	m_BoldFont.Create( CBS_DROPDOWN|WS_VSCROLL, rt, this, 1001 );
	m_BoldFont.SetFont( GetFont() );
	m_BoldFont.SetItemHeight(-1, 12);
	m_BoldFont.AddString(L"yes");
	m_BoldFont.AddString(L"no");

	// 从语言配置中读取当前语言包含的字体，添加到字体下拉里表中
	int32 nCurLangIndex = CFontMgr::GetInst()->GetLangIndex();
	int32 nFontCount	= CFontCfg::GetInst()->GetGDIFontCount();

	for ( int32 font_id = 0; font_id < nFontCount; ++font_id )
	{
		wstring szCurFontName = CFontCfg::GetInst()->GetGDIFontName(font_id);
		m_WndFont.AddString(szCurFontName.c_str());
	}
	m_WndFont.SetItemHeight( -1, 12 );

	m_Lock.Create( CBS_DROPDOWN|WS_VSCROLL, rt, this, 1001 );
	m_Lock.SetFont( GetFont() );
	m_Lock.SetItemHeight( -1, 12 );

	m_Flag2String[0]											= L"居中"; 
	m_Flag2String[LOCK_LEFT]									= L"左居中"; 
	m_Flag2String[LOCK_RIGHT]									= L"右居中"; 
	m_Flag2String[LOCK_TOP]										= L"上居中"; 
	m_Flag2String[LOCK_BOTTOM]									= L"下居中"; 
	m_Flag2String[LOCK_LEFT|LOCK_TOP]							= L"左上"; 
	m_Flag2String[LOCK_TOP|LOCK_RIGHT]							= L"右上";
	m_Flag2String[LOCK_RIGHT|LOCK_BOTTOM]						= L"右下"; 
	m_Flag2String[LOCK_LEFT|LOCK_BOTTOM]						= L"左下";
	m_Flag2String[LOCK_LEFT|LOCK_TOP|LOCK_RIGHT]				= L"上锁定";  
	m_Flag2String[LOCK_TOP|LOCK_RIGHT|LOCK_BOTTOM]				= L"右锁定"; 
	m_Flag2String[LOCK_LEFT|LOCK_RIGHT|LOCK_BOTTOM]				= L"下锁定";  
	m_Flag2String[LOCK_LEFT|LOCK_BOTTOM|LOCK_TOP]				= L"左锁定"; 
	m_Flag2String[LOCK_LEFT|LOCK_TOP|LOCK_RIGHT|LOCK_BOTTOM]	= L"四边锁定";
	m_Flag2String[LOCK_LEFT|LOCK_TOP|LOCK_RIGHT|LOCK_UNZOOM]				= L"非缩放上锁定";  
	m_Flag2String[LOCK_TOP|LOCK_RIGHT|LOCK_BOTTOM|LOCK_UNZOOM]				= L"非缩放右锁定"; 
	m_Flag2String[LOCK_LEFT|LOCK_RIGHT|LOCK_BOTTOM|LOCK_UNZOOM]				= L"非缩放下锁定";  
	m_Flag2String[LOCK_LEFT|LOCK_BOTTOM|LOCK_TOP|LOCK_UNZOOM]				= L"非缩放左锁定"; 
	m_Flag2String[LOCK_LEFT|LOCK_TOP|LOCK_RIGHT|LOCK_BOTTOM|LOCK_UNZOOM]	= L"非缩放四边锁定";

	for( map<unsigned,CString>::iterator it = m_Flag2String.begin(); it != m_Flag2String.end(); it++ )
	{
		m_String2Flag[it->second] = it->first;
		m_Lock.AddString( it->second );
	}
}

void CBasePropList::SetToItem()
{
	SQRWnd* pCurEdit = m_pWndGroup->GetEditWnd();
	if( !pCurEdit )
		return;
	CFRect rt;
	wchar_t Buf[256];
	wsprintf( Buf, L"0x%08x", pCurEdit->GetStyle() );
	pCurEdit->GetWndRect( rt );


	int32	nLangIndex	= CFontMgr::GetInst()->GetLangIndex();
	int32	nFontID		= pCurEdit->GetFontID();
	wstring szFontName	= CFontCfg::GetInst()->GetGDIFontName(nFontID);

	wstring szBoldFont = pCurEdit->IsFontBold() ? L"yes" : L"no";

	SetItemText( ITEM_ID_CAPTION,	1, pCurEdit->GetWndTextW() );
	SetItemText( ITEM_ID_STYLE,		1, Buf );

	SetItemText( ITEM_ID_XPOS,		1, _itow( int32(rt.left > 0.0f ? rt.left + 0.5f : rt.left - 0.5f), Buf, 10 ) );
	SetItemText( ITEM_ID_YPOS,		1, _itow( int32(rt.top > 0.0f ? rt.top + 0.5f : rt.top - 0.5f), Buf, 10 ) );
	SetItemText( ITEM_ID_WIDTH,		1, _itow( int32(rt.right - rt.left > 0.0f ? rt.right - rt.left + 0.5f : rt.right - rt.left - 0.5f), Buf, 10 ) );
	SetItemText( ITEM_ID_HEIGHT,	1, _itow( int32(rt.bottom - rt.top > 0.0f ? rt.bottom - rt.top + 0.5f : rt.bottom - rt.top - 0.5f), Buf, 10 ) );

	//SetItemText( ITEM_ID_XPOS,		1, _itow( int32(rt.left), Buf, 10 ) );
	//SetItemText( ITEM_ID_YPOS,		1, _itow( int32(rt.top), Buf, 10 ) );
	//SetItemText( ITEM_ID_WIDTH,		1, _itow( int32(rt.right - rt.left), Buf, 10 ) );
	//SetItemText( ITEM_ID_HEIGHT,	1, _itow( int32(rt.bottom - rt.top), Buf, 10 ) );
	SetItemText( ITEM_ID_GROUPID,	1, _itow( pCurEdit->GetGroupID(), Buf, 10 ) );
	SetItemText( ITEM_ID_FONTID,	1, szFontName.c_str() );
	SetItemText( ITEM_ID_FONTSIZE,	1, _itow( int32(pCurEdit->GetFontSize()), Buf, 10 ) );
	SetItemText( ITEM_ID_BOLDFONT,	1, szBoldFont.c_str() );
	SetItemText( ITEM_ID_DESC,		1, utf8_to_utf16(pCurEdit->GetMouseOverDesc()).c_str());
	SetItemText( ITEM_ID_LOCKFLAG,	1, m_Flag2String[ pCurEdit->GetLockFlag() ] );
	string ClassName;
	ClassName = pCurEdit->GetszClassName();
	if ( ClassName.length() == 0 )
		Class_Name( ClassName, pCurEdit );
	SetItemText( ITEM_ID_USERCLASS, 1, utf8_to_utf16(ClassName.c_str()).c_str() );
	SetItemText( ITEM_ID_USERINFO,	1, L"");
	SetItemText( ITEM_ID_CLICKFX,	1, utf8_to_utf16(pCurEdit->GetMsgFx( eMF_Click )).c_str() );
	SetItemText( ITEM_ID_OPENFX,	1, utf8_to_utf16(pCurEdit->GetMsgFx( eMF_Open )).c_str() );
	SetItemText( ITEM_ID_CLOSEFX,	1, utf8_to_utf16(pCurEdit->GetMsgFx( eMF_Close )).c_str() );
	SetItemText( ITEM_ID_STYLE2,	1, _itow( pCurEdit->GetStyleIndex(), Buf, 10 ) );
	SetItemText( ITEM_ID_SOURCE,	1, utf8_to_utf16( pCurEdit->GetszSourceName()).c_str() );

	CString str;
	m_WndFont.GetWindowText(str);
	if( str != szFontName.c_str() )
		m_WndFont.SetWindowText(szFontName.c_str());
	m_Lock.GetWindowText(str);

	m_BoldFont.GetWindowText(str);
	if ( str != szBoldFont.c_str() )
		m_BoldFont.SetWindowText(szBoldFont.c_str());

	if( str != m_Flag2String[ pCurEdit->GetLockFlag() ] )
		m_Lock.SetWindowText(m_Flag2String[ pCurEdit->GetLockFlag() ]);
	m_Edit.GetWindowText(str);
	if( str != GetItemText( m_nEditItem, 1 ) )
		m_Edit.SetWindowText( GetItemText( m_nEditItem, 1 ) );
}

void CBasePropList::ItemToProp( bool bCheck )
{
	CString str;
	if( m_nEditItem == ITEM_ID_FONTID )
		m_WndFont.GetWindowText( str );
	else if ( m_nEditItem == ITEM_ID_BOLDFONT )
		m_BoldFont.GetWindowText(str);
	else if( m_nEditItem == ITEM_ID_LOCKFLAG )
	{	
		m_Lock.GetWindowText( str );
	    mWndImageProp = new CWndImageProp();
		if(mWndImageProp->GetImage())
			mWndImageProp->GetImage()->dwLockFlag = m_String2Flag[GetItemText( 10, 1 )];
	}
	else
		m_Edit.GetWindowText( str );
	SetItemText( m_nEditItem, 1, str );

	SQRWnd* pCurEdit = m_pWndGroup->GetEditWnd();
	if( !pCurEdit )
		return;
	CreateInfo& Info = m_pWndGroup->m_CreateInfo[pCurEdit];

	// 取得字体编号以及字体是否加粗信息
	int32	nFontID	  = m_WndFont.GetCurSel();
	if ( nFontID == CFontMgr::INVALID_FONT_ID )
		nFontID = pCurEdit->GetFontID();
	bool	bBoldFont = (wstring(L"yes") == (LPCWSTR)GetItemText( ITEM_ID_BOLDFONT, 1 )) ? true : false;

	CRect rt;
	if( bCheck )
        m_pWndGroup->Check();
	pCurEdit->SetWndText( utf16_to_utf8((LPCTSTR)GetItemText( ITEM_ID_CAPTION, 1 )).c_str() );
	pCurEdit->SetStyle( wcstoul( (LPCWSTR)GetItemText( ITEM_ID_STYLE, 1 ), 0, 0 ) );

	rt.left		= _wtoi( GetItemText( ITEM_ID_XPOS, 1 ) );
	rt.top		= _wtoi( GetItemText( ITEM_ID_YPOS, 1 ) );
	rt.right	= rt.left + _wtoi( GetItemText( ITEM_ID_WIDTH, 1 ) );
	rt.bottom	= rt.top + _wtoi( GetItemText( ITEM_ID_HEIGHT, 1 ) );
	pCurEdit->SetWndRect( rt );

	pCurEdit->SetGroupID( _wtoi( GetItemText( ITEM_ID_GROUPID, 1 ) ) );

	uint32 msty = _wtoi(GetItemText(ITEM_ID_STYLE2,1));
	GUI_FONT* pFont = pCurEdit->GetFontInfo(msty);
	if (pFont)
		pCurEdit->SetFont( pFont->m_nFontID, bBoldFont, (float)(pFont->m_FontSize) );
	else
		pCurEdit->SetFont( nFontID, bBoldFont, (float)_wtoi( GetItemText( ITEM_ID_FONTSIZE, 1 ) ) );

	
	pCurEdit->SetMouseOverDesc( utf16_to_utf8((LPCWSTR)GetItemText( ITEM_ID_DESC, 1 )).c_str() );
	pCurEdit->SetLockFlag( m_String2Flag[GetItemText( ITEM_ID_LOCKFLAG, 1 )] );
	Info.szClassName = (LPCWSTR)GetItemText( ITEM_ID_USERCLASS, 1 );
	Info.szCreateInfo = (LPCWSTR)GetItemText( ITEM_ID_USERINFO, 1 );
	pCurEdit->SetMsgFx( eMF_Click, utf16_to_utf8((LPCWSTR)GetItemText( ITEM_ID_CLICKFX, 1 )).c_str() );
	pCurEdit->SetMsgFx( eMF_Open, utf16_to_utf8((LPCWSTR)GetItemText( ITEM_ID_OPENFX, 1 )).c_str() );
	pCurEdit->SetMsgFx( eMF_Close, utf16_to_utf8((LPCWSTR)GetItemText( ITEM_ID_CLOSEFX, 1 )).c_str() );
	pCurEdit->SetStyleIndex( msty );
	pCurEdit->SetszSourceName( utf16_to_utf8((LPCTSTR)GetItemText( ITEM_ID_SOURCE, 1 )).c_str() );

	if (pFont)
	{
		// 更新其他所有字体信息
		IMAGE_PARAM IP1 ( SM_BS_BK, IP_ENABLE );
		pCurEdit->SetWndTextColor( IP1, pFont->m_EventFont[(EventStateMask)(IP1.StateMask)].m_FontColor );
		pCurEdit->SetWndTextBcColor( IP1, pFont->m_EventFont[(EventStateMask)(IP1.StateMask)].m_uFontBkColor );
		pCurEdit->SetFontGradualColor( pFont->m_uFontGradualColor );
		pCurEdit->SetFontEffectMask(pFont->m_uFontEffectMask);
	}
	
	CWndProp* pProp = (CWndProp*)GetParent();
	pProp->m_ImageProp.ResetImageProp( pProp->m_ImageProp.m_pImage, m_pWndGroup );
	
	if( bCheck )
		m_pWndGroup->ReCheck();
	m_pWndGroup->GetRootWnd()->UpdateRect();
}

void CBasePropList::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if( pNMLV->uChanged != LVIF_STATE )
		return;
	if( ( pNMLV->uNewState & LVIS_SELECTED ) == 0 )
		return;

	CString str;
	if( m_Edit.IsWindowVisible() )
	{
		m_Edit.GetWindowText( str );
		SetItemText( m_nEditItem, 1, str );
	}
	if( m_WndFont.IsWindowVisible() )
	{
		m_WndFont.GetWindowText( str );
		SetItemText( m_nEditItem, 1, str );
	}
	if ( m_BoldFont.IsWindowVisible() )
	{
		m_BoldFont.GetWindowText( str );
		SetItemText(m_nEditItem, 1, str);
	}
	if( m_Lock.IsWindowVisible() )
	{
		m_Lock.GetWindowText( str );
		SetItemText( m_nEditItem, 1, str );
	}

	m_WndFont.ShowWindow( SW_HIDE );
	m_BoldFont.ShowWindow(SW_HIDE);
	m_Edit.ShowWindow( SW_HIDE );
	m_Lock.ShowWindow( SW_HIDE );

	CRect rt;
	GetItemRect( pNMLV->iItem, &rt, LVIR_LABEL );
	if( pNMLV->iItem == ITEM_ID_FONTID )
	{
		m_WndFont.SetWindowPos( NULL, WND_PROP_WIDTH + 1, rt.top - 1, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW );
		m_WndFont.SetWindowText( GetItemText( pNMLV->iItem, 1 ) );
		m_WndFont.ShowWindow( SW_SHOW );
	}
	else if( pNMLV->iItem == ITEM_ID_BOLDFONT )
	{
		m_BoldFont.SetWindowPos(NULL, WND_PROP_WIDTH + 1, rt.top - 1, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW );
		m_BoldFont.SetWindowText( GetItemText( ITEM_ID_BOLDFONT, 1 ) );
		m_BoldFont.ShowWindow( SW_SHOW );
	}
	else if( pNMLV->iItem == ITEM_ID_LOCKFLAG )
	{
		m_Lock.SetWindowPos( NULL, WND_PROP_WIDTH + 1, rt.top - 1, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW );
		m_Lock.SetWindowText( GetItemText( pNMLV->iItem, 1 ) );
		m_Lock.ShowWindow( SW_SHOW );
	}
	else
	{
		m_Edit.SetWindowPos( NULL, WND_PROP_WIDTH + 3, rt.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW );
		m_Edit.SetWindowText( GetItemText( pNMLV->iItem, 1 ) );
		m_Edit.SetSel( 0, -1, true );
		m_Edit.ShowWindow( SW_SHOW );
	}
	m_nEditItem = pNMLV->iItem;
	ItemToProp();
	Invalidate();
	m_pWndGroup->GetRootWnd()->UpdateRect();
}

void CBasePropList::OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if( !m_Edit.IsWindowVisible() && !m_WndFont.IsWindowVisible() )
	{
		if( m_nEditItem == ITEM_ID_FONTID )
			m_WndFont.ShowWindow( SW_SHOW );
		else if ( m_nEditItem == ITEM_ID_BOLDFONT )
			m_BoldFont.ShowWindow( SW_SHOW );
		else if( m_nEditItem == ITEM_ID_LOCKFLAG )
			m_Lock.ShowWindow( SW_SHOW );
		else
			m_Edit.ShowWindow( SW_SHOW );
	}

	if( m_nEditItem == ITEM_ID_FONTID )
		m_WndFont.SetFocus();
	else if ( m_nEditItem == ITEM_ID_BOLDFONT )
		m_BoldFont.SetFocus();
	else if( m_nEditItem == ITEM_ID_LOCKFLAG )
		m_Lock.SetFocus();
	else
		m_Edit.SetFocus();
}

void CBasePropList::OnEnKillfocusEdit()
{
	// TODO: Add your control notification handler code here
	CString str;
	if( m_Edit.IsWindowVisible() )
	{
		m_Edit.GetWindowText( str );
		SetItemText( m_nEditItem, 1, str );
	}
	if( m_WndFont.IsWindowVisible() )
	{
		m_WndFont.GetWindowText( str );
		SetItemText( m_nEditItem, 1, str );
	}
	if ( m_BoldFont.IsWindowVisible() )
	{
		m_BoldFont.GetWindowText(str);
		SetItemText(m_nEditItem, 1, str);
	}
	if( m_Lock.IsWindowVisible() )
	{
		m_Lock.GetWindowText( str );
		SetItemText( m_nEditItem, 1, str );
	}

	if( GetFocus() == &m_Edit || GetFocus() == &m_WndFont || GetFocus() == &m_BoldFont || GetFocus() == &m_Lock || GetFocus() == this )
		return;

	m_Lock.ShowWindow( SW_HIDE );
	m_WndFont.ShowWindow( SW_HIDE );
	m_BoldFont.ShowWindow( SW_HIDE );
	m_Edit.ShowWindow( SW_HIDE );

	ItemToProp();
	Invalidate();
}

void CBasePropList::OnPaint()
{
	CListCtrl::OnPaint();

	if( m_nEditItem < 0 )
		return;
	CRect rt;
	GetItemRect( m_nEditItem, &rt, LVIR_LABEL );
	rt.left -=2;
	rt.right = WND_PROP_WIDTH;
	rt.bottom--;
	CDC* pDC = GetDC();
	CFont* pOld = pDC->SelectObject( GetFont() );
	pDC->FillSolidRect( &rt, 0x6A240A );
	rt.left += 4;
	pDC->SetBkColor( 0x6A240A );
	pDC->SetTextColor( 0xffffff );
	pDC->DrawText( GetItemText( m_nEditItem, 0 ), &rt, DT_LEFT );
	pDC->SelectObject( pOld );
	ReleaseDC( pDC );
}

// CBasePropList uMessage handlers


