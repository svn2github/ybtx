// CWndImageProp.cpp : implementation file
//

#include "stdafx.h"
#include "GUIEditor.h"
#include "CWndImageProp.h"

#define WND_IMAGEPROP_WIDTH 40

// CWndImageProp


IMPLEMENT_DYNAMIC(CWndImageProp, CListCtrl)
CWndImageProp::CWndImageProp()
: m_pImage(NULL)
, m_ImageList(NULL)
, m_pWndGroup(NULL)
{
	m_CurID = IDC_ARROW;
}

CWndImageProp::~CWndImageProp()
{
}


BEGIN_MESSAGE_MAP(CWndImageProp, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
	ON_NOTIFY_REFLECT(NM_SETFOCUS, OnNMSetfocus)
	ON_WM_PAINT()
	ON_EN_KILLFOCUS( 1000, OnEnKillfocusEdit)
	ON_CBN_KILLFOCUS( 1001, OnEnKillfocusEdit)
END_MESSAGE_MAP()


void CWndImageProp::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	CRect rt;
	GetClientRect( &rt );
	InsertColumn( 0, L"Prop", LVCFMT_LEFT, WND_IMAGEPROP_WIDTH );
	InsertColumn( 1, L"Data", LVCFMT_LEFT, rt.Width() - WND_IMAGEPROP_WIDTH );
	SetExtendedStyle( GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT );

	InsertItem( 0, L"xWnd" );
	InsertItem( 1, L"yWnd" );
	InsertItem( 2, L"Left" );
	InsertItem( 3, L"Right" );
	InsertItem( 4, L"Top" );
	InsertItem( 5, L"Bottom" );
	InsertItem( 6, L"Color" );
	InsertItem( 7, L"Lock" );

	rt.left = WND_IMAGEPROP_WIDTH;
	rt.top = 0;
	rt.bottom = 13;
	m_Edit.Create( 0, rt, this, 1000 );
	m_Edit.SetFont( GetFont() );

	rt.bottom += 300;
	m_Lock.Create( CBS_DROPDOWN|WS_VSCROLL, rt, this, 1001 );
	m_Lock.SetFont( GetFont() );
	m_Lock.SetItemHeight( -1, 12 );

	m_Flag2String[0]											= "居中"; 
	m_Flag2String[LOCK_LEFT]									= "左居中"; 
	m_Flag2String[LOCK_RIGHT]									= "右居中"; 
	m_Flag2String[LOCK_TOP]										= "上居中"; 
	m_Flag2String[LOCK_BOTTOM]									= "下居中"; 
	m_Flag2String[LOCK_LEFT|LOCK_TOP]							= "左上"; 
	m_Flag2String[LOCK_TOP|LOCK_RIGHT]							= "右上";
	m_Flag2String[LOCK_RIGHT|LOCK_BOTTOM]						= "右下"; 
	m_Flag2String[LOCK_LEFT|LOCK_BOTTOM]						= "左下";
	m_Flag2String[LOCK_LEFT|LOCK_TOP|LOCK_RIGHT]				= "上锁定";  
	m_Flag2String[LOCK_TOP|LOCK_RIGHT|LOCK_BOTTOM]				= "右锁定"; 
	m_Flag2String[LOCK_LEFT|LOCK_RIGHT|LOCK_BOTTOM]				= "下锁定";  
	m_Flag2String[LOCK_LEFT|LOCK_BOTTOM|LOCK_TOP]				= "左锁定"; 
	m_Flag2String[LOCK_LEFT|LOCK_TOP|LOCK_RIGHT|LOCK_BOTTOM]	= "四边锁定";
	m_Flag2String[LOCK_LEFT|LOCK_TOP|LOCK_RIGHT|LOCK_UNZOOM]				= "非缩放上锁定";  
	m_Flag2String[LOCK_TOP|LOCK_RIGHT|LOCK_BOTTOM|LOCK_UNZOOM]				= "非缩放右锁定"; 
	m_Flag2String[LOCK_LEFT|LOCK_RIGHT|LOCK_BOTTOM|LOCK_UNZOOM]				= "非缩放下锁定";  
	m_Flag2String[LOCK_LEFT|LOCK_BOTTOM|LOCK_TOP|LOCK_UNZOOM]				= "非缩放左锁定"; 
	m_Flag2String[LOCK_LEFT|LOCK_TOP|LOCK_RIGHT|LOCK_BOTTOM|LOCK_UNZOOM]	= "非缩放四边锁定";

	for( map<unsigned,CString>::iterator it = m_Flag2String.begin(); it != m_Flag2String.end(); it++ )
	{
		m_String2Flag[it->second] = it->first;
		m_Lock.AddString( it->second );
	}

	m_nEditItem = -1;

	CListCtrl::PreSubclassWindow();
}
void CWndImageProp::SetImageList( WND_IMAGE_LIST* imageList )
{
	m_ImageList = imageList;
}

void CWndImageProp::SetChooseImgList(std::vector<int> chooseImgList)
{
	m_vChooseImgList = chooseImgList;
}

void CWndImageProp::ResetImageProp( IMAGE* pImage, CWndGroup* pWndGroup )
{
	if( NULL == pImage )
		return;

	m_Edit.ShowWindow( SW_HIDE );
	m_pImage = pImage;
	m_pWndGroup = pWndGroup;
	CString str = m_Flag2String[LOCK_LEFT|LOCK_TOP];
	if( m_Flag2String.find( pImage->dwLockFlag ) != m_Flag2String.end() )
		str = m_Flag2String[pImage->dwLockFlag];

	wchar_t Buf[256];
	SetItemText( 0, 1, _itow( (int)pImage->posWnd.x, Buf, 10 ) );
	SetItemText( 1, 1, _itow( (int)pImage->posWnd.y, Buf, 10 ) );
	SetItemText( 2, 1, _itow( (int)pImage->rtTexture.left, Buf, 10 ) );
	SetItemText( 3, 1, _itow( (int)pImage->rtTexture.right, Buf, 10 ) );
	SetItemText( 4, 1, _itow( (int)pImage->rtTexture.top, Buf, 10 ) );
	SetItemText( 5, 1, _itow( (int)pImage->rtTexture.bottom, Buf, 10 ) );
	SetItemText( 6, 1, _ultow( pImage->dwColor, Buf, 16 ) );
	SetItemText( 7, 1, str );

	m_Edit.SetWindowText( GetItemText( m_nEditItem, 1 ) );
	m_Lock.SetWindowText( str );
}

void CWndImageProp::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CString str;
	if( m_Edit.IsWindowVisible() )
	{
		m_Edit.GetWindowText( str );
		SetItemText( m_nEditItem, 1, str );
		UpdateProp();
	}
	if( m_Lock.IsWindowVisible() )
	{
		m_Lock.GetWindowText( str );
		SetItemText( m_nEditItem, 1, str );
		UpdateProp();
	}

	m_Edit.ShowWindow( SW_HIDE );
	m_Lock.ShowWindow( SW_HIDE );

	CRect rt;
	GetItemRect( pNMLV->iItem, &rt, LVIR_LABEL );
	if( pNMLV->iItem == 7 )
	{
		m_Lock.SetWindowPos( NULL, WND_IMAGEPROP_WIDTH + 1, rt.top - 1, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW );
		m_Lock.SetWindowText( GetItemText( pNMLV->iItem, 1 ) );
		m_Lock.ShowWindow( SW_SHOW );
	}
	else
	{
		m_Edit.SetWindowPos( NULL, WND_IMAGEPROP_WIDTH + 3, rt.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW );
		m_Edit.SetWindowText( GetItemText( pNMLV->iItem, 1 ) );
		m_Edit.SetSel( 0, -1, true );
		m_Edit.ShowWindow( SW_SHOW );
	}

	m_nEditItem = pNMLV->iItem;
	Invalidate();
}

void CWndImageProp::OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if( !m_Edit.IsWindowVisible() && !m_Lock.IsWindowVisible() )
	{
		if( m_nEditItem == 7 )
			m_Lock.ShowWindow( SW_SHOW );
		else
			m_Edit.ShowWindow( SW_SHOW );
	}

	if( m_nEditItem == 7 )
		m_Lock.SetFocus();
	else
		m_Edit.SetFocus();
}

void CWndImageProp::OnEnKillfocusEdit()
{
	// TODO: Add your control notification handler code here
	if( GetFocus() == &m_Edit || GetFocus() == &m_Lock || GetFocus() == this )
		return;
	CString str;
	if( m_Edit.IsWindowVisible() )
	{
		m_Edit.GetWindowText( str );
		SetItemText( m_nEditItem, 1, str );
		UpdateProp();
	}
	if( m_Lock.IsWindowVisible() )
	{
		m_Lock.GetWindowText( str );
		SetItemText( m_nEditItem, 1, str );
		UpdateProp();
	}
	m_Edit.ShowWindow( SW_HIDE );
	m_Lock.ShowWindow( SW_HIDE );
}

void CWndImageProp::UpdateProp()
{
	if( m_pImage == NULL )
		return;

	CString str;
	if( m_Edit.IsWindowVisible() )
	{
		m_Edit.GetWindowText( str );
		SetItemText( m_nEditItem, 1, str );
	}
	if( m_Lock.IsWindowVisible() )
	{
		m_Lock.GetWindowText( str );
		SetItemText( m_nEditItem, 1, str );
	}

	m_pImage ->posWnd.x			= (float)_wtoi( GetItemText( 0, 1 ) );
	m_pImage ->posWnd.y			= (float)_wtoi( GetItemText( 1, 1 ) );
	m_pImage ->rtTexture.left	= (float)_wtoi( GetItemText( 2, 1 ) );
	m_pImage ->rtTexture.right	= (float)_wtoi( GetItemText( 3, 1 ) );
	m_pImage ->rtTexture.top	= (float)_wtoi( GetItemText( 4, 1 ) );
	m_pImage ->rtTexture.bottom = (float)_wtoi( GetItemText( 5, 1 ) );
	m_pImage ->dwColor			= wcstoul( L"0x" + GetItemText( 6, 1 ), 0, 0 );
	m_pImage ->dwLockFlag		= m_String2Flag[ GetItemText( 7, 1 ) ];

	// 更新控件显示
	if ( m_pWndGroup->m_pCurEditWnd->GetClassName() == string( "CListCtrl" ))
	{
		SQRListCtrl* pListCtrl = (SQRListCtrl*)(m_pWndGroup->m_pCurEditWnd);
		pListCtrl->UpdateItemImage();
	}
}

void CWndImageProp::Flip( bool Vert )
{
	m_pWndGroup->Check();
	if( Vert )
	{
		if( m_pImage->rtTexture.top < m_pImage->rtTexture.bottom )
		{
			m_pImage->rtTexture.top--;
			m_pImage->rtTexture.bottom--;
		}
		else
		{
			m_pImage->rtTexture.top++;
			m_pImage->rtTexture.bottom++;
		}
		swap( m_pImage->rtTexture.top, m_pImage->rtTexture.bottom );
	}
	else
	{
		if( m_pImage->rtTexture.left < m_pImage->rtTexture.right )
		{
			m_pImage->rtTexture.left--;
			m_pImage->rtTexture.right--;
		}
		else
		{
			m_pImage->rtTexture.left++;
			m_pImage->rtTexture.right++;
		}
		swap( m_pImage->rtTexture.left, m_pImage->rtTexture.right );
	}
	m_pWndGroup->ReCheck();
	ResetImageProp( m_pImage, m_pWndGroup );
}

void CWndImageProp::MoveText( UINT uMessage, WPARAM wParam, LPARAM lParam, SQRWnd* pEditWnd )
{
	//if( abs( Cur.x - rtWnd.left ) < 2 )
	//	m_pImage->rtTexture.left += Cur.x - Pre.x;
	//if( abs( Cur.x - rtWnd.left ) < 2 )
	//	m_pImage->rtTexture.left += Cur.x - Pre.x;
	//if( abs( Cur.x - rtWnd.left ) < 2 )
	//	m_pImage->rtTexture.left += Cur.x - Pre.x;
	//m_pImage ->posWnd.x += Pos.x;
	//m_pImage ->posWnd.y += Pos.y;

	CFRect rt;
	pEditWnd->GetWndRect( rt );
	CPoint point( LOWORD(lParam), HIWORD(lParam) );
	wchar_t Buf[256];

	switch( uMessage ) 
	{

	case WM_MOUSEMOVE:
		if( abs( m_CurPos.x - ( rt.left + m_pImage->posWnd.x ) ) < 3 )
		{
			m_CurID = IDC_SIZEWE;
			if( wParam&MK_LBUTTON )
			{
				m_pImage->posWnd.x += point.x - m_CurPos.x; 
				m_pImage->rtTexture.left += m_pImage->rtTexture.right > m_pImage->rtTexture.left ? point.x - m_CurPos.x : m_CurPos.x - point.x;
				SetItemText( 0, 1, _itow( (int)m_pImage->posWnd.x, Buf, 10 ) );
				SetItemText( 2, 1, _itow( (int)m_pImage->rtTexture.left, Buf, 10 ) );
			}
		}
		else if( abs( m_CurPos.x - ( rt.left + m_pImage->posWnd.x + abs( m_pImage->rtTexture.Width() ) ) ) < 3 )
		{
			m_CurID = IDC_SIZEWE;
			if( wParam&MK_LBUTTON )
			{
				m_pImage->rtTexture.right += m_pImage->rtTexture.right > m_pImage->rtTexture.left ? point.x - m_CurPos.x : m_CurPos.x - point.x;
				SetItemText( 3, 1, _itow( (int)m_pImage->rtTexture.right, Buf, 10 ) );
			}
		}
		else if( abs( m_CurPos.y - ( rt.top + m_pImage->posWnd.y ) ) < 3 )
		{
			m_CurID = IDC_SIZENS;
			if( wParam&MK_LBUTTON )
			{
				m_pImage->posWnd.y += point.y - m_CurPos.y; 
				m_pImage->rtTexture.top += m_pImage->rtTexture.bottom > m_pImage->rtTexture.top ? point.y - m_CurPos.y : m_CurPos.y - point.y;
				SetItemText( 1, 1, _itow( (int)m_pImage->posWnd.y, Buf, 10 ) );
				SetItemText( 4, 1, _itow( (int)m_pImage->rtTexture.top, Buf, 10 ) );
			}
		}
		else if( abs( m_CurPos.y - ( rt.top + m_pImage->posWnd.y + abs( m_pImage->rtTexture.Height() ) ) ) < 3 )
		{
			m_CurID = IDC_SIZENS;
			if( wParam&MK_LBUTTON )
			{
				m_pImage->rtTexture.bottom += m_pImage->rtTexture.bottom > m_pImage->rtTexture.top ? point.y - m_CurPos.y : m_CurPos.y - point.y;
				SetItemText( 5, 1, _itow( (int)m_pImage->rtTexture.bottom, Buf, 10 ) );
			}
		}
		else
		{
			if( wParam&MK_LBUTTON )//用鼠标移动贴图，已经实现多贴图移动
			{
				m_CurID = IDC_ARROW;
				for(int i = 0; i< int(m_vChooseImgList.size()); i++)
				{
					int tmp = m_vChooseImgList[i];
					m_ImageList->GetImage(tmp);
					m_ImageList->GetImage(tmp).posWnd.x += point.x - m_CurPos.x;
					m_ImageList->GetImage(tmp).posWnd.y += point.y - m_CurPos.y;
					SetItemText( 0, 1, _itow( int32(m_ImageList->GetImage(tmp).posWnd.x), Buf, 10 ) );
					SetItemText( 1, 1, _itow( int32(m_ImageList->GetImage(tmp).posWnd.y), Buf, 10 ) );
				}
			}
		}
		m_CurPos = point;
		break;
	}
	::SetCursor( ::LoadCursor( NULL, m_CurID ) );
}

void CWndImageProp::DrawEditLine( CGraphic* pGraphic, SQRWnd* pEditWnd, int n )
{
	if( pEditWnd == NULL || m_vChooseImgList.empty())
		return;

	for(int i = 0; i< int(m_vChooseImgList.size()); i++)
	{
		CFRect rt;
		pEditWnd->GetWndRect( rt);
		int tmp = m_vChooseImgList[i];

		rt.left += m_ImageList->GetImage(tmp).posWnd.x;
		rt.top  += m_ImageList->GetImage(tmp).posWnd.y;
		rt.right = rt.left + abs( m_ImageList->GetImage(tmp).rtTexture.Width() );
		rt.bottom = rt.top + abs( m_ImageList->GetImage(tmp).rtTexture.Height() );

		rt.left *= n;
		rt.right *= n;
		rt.top *= n;
		rt.bottom *= n;
		rt.left--;
		rt.top--;
		VerColor2D v[5]=
		{
			VerColor2D( PixelAligned(rt.left), PixelAligned(rt.top),     0,	0xffff0000 ),
			VerColor2D( PixelAligned(rt.right), PixelAligned(rt.top),    0,	0xffff0000 ),
			VerColor2D( PixelAligned(rt.right), PixelAligned(rt.bottom), 0,	0xffff0000 ),
			VerColor2D( PixelAligned(rt.left),  PixelAligned(rt.bottom), 0,	0xffff0000 ),
			VerColor2D( PixelAligned(rt.left),  PixelAligned(rt.top),    0,	0xffff0000 ),
		};
		VerColor2D* pVB;
		pGraphic->GetRenderStack( RS_FONT, NULL, NULL, PT_LINESTRIP, 5, 4, 
			VerColor2D::Format, sizeof(VerColor2D), (void**)&pVB, NULL );
		memcpy( pVB, v, sizeof( VerColor2D )*5 );
	}
	
}

void CWndImageProp::OnPaint()
{
	CListCtrl::OnPaint();

	if( m_nEditItem < 0 )
		return;
	CRect rt;
	GetItemRect( m_nEditItem, &rt, LVIR_LABEL );
	rt.left -=2;
	rt.right = WND_IMAGEPROP_WIDTH;
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


// CWndImageProp uMessage handlers

