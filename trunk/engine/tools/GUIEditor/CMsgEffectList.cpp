// CMsgEffectList.cpp : implementation file
//

#include "stdafx.h"
#include "GUIEditor.h"
#include "CMsgEffectList.h"
#include "CWndGroup.h"

#define WND_MSGEFFECT_WIDTH			60

// CMsgEffectList


IMPLEMENT_DYNAMIC(CMsgEffectList, CListCtrl)
CMsgEffectList::CMsgEffectList()
{
}

CMsgEffectList::~CMsgEffectList()
{
}

BEGIN_MESSAGE_MAP(CMsgEffectList, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
	ON_NOTIFY_REFLECT(NM_SETFOCUS, OnNMSetfocus)
	ON_WM_PAINT()
	ON_EN_KILLFOCUS( 1000, OnEnKillfocusEdit)
	ON_CBN_KILLFOCUS( 1001, OnEnKillfocusEdit)
END_MESSAGE_MAP()

void CMsgEffectList::Init( CWndGroup* pWndGroup )
{
	m_pWndGroup = pWndGroup;
}

void CMsgEffectList::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	CRect rt;
	GetClientRect( &rt );
	InsertColumn( 0, L"Prop", LVCFMT_LEFT, WND_MSGEFFECT_WIDTH );
	InsertColumn( 1, L"Data", LVCFMT_LEFT, rt.Width() - WND_MSGEFFECT_WIDTH );
	SetExtendedStyle( GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT );

	InsertItem( 0, L"LBtnDown" );
	InsertItem( 1, L"RBtnDown" );
	InsertItem( 2, L"Open" );
	InsertItem( 3, L"Close" );

	rt.left = WND_MSGEFFECT_WIDTH;
	rt.top = 0;
	rt.bottom = 13;
	m_Edit.Create( 0, rt, this, 1000 );
	m_Edit.SetFont( GetFont() );

	m_nEditItem = -1;

	CListCtrl::PreSubclassWindow();
}

void CMsgEffectList::OnUpdate()
{
}

void CMsgEffectList::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
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

	m_Edit.ShowWindow( SW_HIDE );

	CRect rt;
	GetItemRect( pNMLV->iItem, &rt, LVIR_LABEL );
	m_Edit.SetWindowPos( NULL, WND_MSGEFFECT_WIDTH + 3, rt.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW );
	m_Edit.SetWindowText( GetItemText( pNMLV->iItem, 1 ) );
	m_Edit.SetSel( 0, -1, true );
	m_Edit.ShowWindow( SW_SHOW );

	m_nEditItem = pNMLV->iItem;
	Invalidate();
}

void CMsgEffectList::OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if( !m_Edit.IsWindowVisible() )
	{
		m_Edit.ShowWindow( SW_SHOW );
	}

	m_Edit.SetFocus();
}

void CMsgEffectList::OnEnKillfocusEdit()
{
	// TODO: Add your control notification handler code here
	if( GetFocus() == &m_Edit || GetFocus() == this )
		return;
	CString str;
	if( m_Edit.IsWindowVisible() )
	{
		m_Edit.GetWindowText( str );
		SetItemText( m_nEditItem, 1, str );
	}
	m_Edit.ShowWindow( SW_HIDE );
}

void CMsgEffectList::OnPaint()
{
	CListCtrl::OnPaint();

	if( m_nEditItem < 0 )
		return;
	CRect rt;
	GetItemRect( m_nEditItem, &rt, LVIR_LABEL );
	rt.left -=2;
	rt.right = WND_MSGEFFECT_WIDTH;
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


// CMsgEffectList uMessage handlers

