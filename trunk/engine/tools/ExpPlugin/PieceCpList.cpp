// PieceCpList.cpp : implementation file
//
#include "stdafx.h"
#include "Exp.h"

#include "PieceCpList.h"
#include "CPluginMeshBase.h"

// PieceCpList

IMPLEMENT_DYNAMIC(CPieceCpList, CListCtrl)
CPieceCpList::CPieceCpList()
{
}

CPieceCpList::~CPieceCpList()
{
}


BEGIN_MESSAGE_MAP(CPieceCpList, CListCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnLvnColumnclick)
END_MESSAGE_MAP()


// CPieceCpList message handlers
void CPieceCpList::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CListCtrl::OnPaint() for painting messages

	//创建需要的画笔
	CFont* Font = GetFont( );
	CFont* OldFont = dc.SelectObject( Font );
	CPen PenTiny,PenThick;
	PenTiny.CreatePen( PS_SOLID, 1, RGB( 0xc0, 0xc0, 0xc0 ) );
	PenThick.CreatePen( PS_SOLID, 1, RGB( 0, 0, 0 ) );
	CPen* OldPen = dc.SelectObject( &PenThick );

	//获得客户区大小
	CRect ClientRect;
	GetClientRect( &ClientRect );
	int Vert = ClientRect.bottom - ClientRect.top;

	int ItemCount = GetItemCount( );
	if( ItemCount == 0 )
		return;

	CRect ItemRect;
	GetItemRect( 0, &ItemRect, LVIR_BOUNDS );
	int Pos = GetScrollPos( SB_VERT );
	int End = min( Pos + Vert/( ItemRect.bottom - ItemRect.top ), ItemCount );

	//跟踪每一行绘制其中文字和内容
	for( int i=Pos; i<End; i++ )
	{
		GetItemRect( i, &ItemRect, LVIR_BOUNDS );
		int sx = ItemRect.left + 3;

		LVCOLUMN Lv;
		Lv.mask = LVCF_WIDTH ;
		int r = GetColumn( 0, &Lv );
		if( !r )
			break;

		RECT DrawRect = { sx, ItemRect.top, sx + Lv.cx - 4 , ItemRect.bottom };
		dc.SetBkColor( RGB( 255, 255, 255 ) );
		dc.DrawText( GetItemText( i, 0 ), &DrawRect, DT_LEFT );
		sx += Lv.cx;

		dc.SelectObject( &PenThick );
		for( int j = 1; ; j++ )
		{
			LVCOLUMN Lv;
			Lv.mask = LVCF_WIDTH;
			int r = GetColumn( j, &Lv );
			if( !r )
				break;

			//边框绘制
			int EndCheck = sx + Lv.cx - 4;
			if( Lv.cx >= ItemRect.Height() )
			{
				EndCheck = sx + ItemRect.Height() - 4;
				dc.MoveTo( EndCheck, ItemRect.bottom - 3 );
				dc.LineTo( EndCheck, ItemRect.top + 1 );
			}
			if( EndCheck > sx )
			{
				dc.MoveTo( EndCheck, ItemRect.top + 1 );
				dc.LineTo( sx, ItemRect.top + 1 );
				dc.LineTo( sx, ItemRect.bottom - 3 );
				dc.LineTo( EndCheck, ItemRect.bottom - 3 );
			}

			//选中绘制
			BOOL Draw = FALSE;
			ListNodeCp* Node = (ListNodeCp*)GetItemData( i );			
			if( j == 1 && Node->m_ExpType == enumPT_COMPRESS )
				Draw = TRUE;		
			if( j == 2 && Node->m_ExpType == enumPT_UNCOMPRESS )
				Draw = TRUE;		

			if( EndCheck - 1 > sx + 2 && Draw )
				dc.FillSolidRect( &CRect( sx + 2, ItemRect.top + 3, EndCheck - 1, ItemRect.bottom - 4 ), RGB( 128, 128, 128 ) ); 
			sx += Lv.cx;
		}
		dc.SelectObject( &PenTiny );
		dc.MoveTo( ClientRect.left, ItemRect.bottom-1 );
		dc.LineTo( ClientRect.right, ItemRect.bottom-1 );
	}

	dc.SelectObject( OldFont );
	dc.SelectObject( OldPen );
}

void CPieceCpList::OnLButtonDown(UINT nFlags, CPoint point)
{
	// 获得点选行列

	int Vert = GetScrollPos( SB_VERT );
	int Horz = GetScrollPos( SB_HORZ );

	RECT ItemRect;
	GetItemRect( Vert, &ItemRect, LVIR_BOUNDS );

	int RowNum = GetItemCount();
	if( RowNum == 0 )
		return ;
	//获得点选行
	int SelRow = Vert + ( point.y - ItemRect.top )/( ItemRect.bottom - ItemRect.top );
	int SelColumn = -1;
	if(SelRow>=RowNum)
		return;
	int sx = ItemRect.left;
	for( int i = 0; ; i++ )
	{
		sx += GetColumnWidth( i );
		if( sx > ItemRect.right )
			break;
		if( sx > point.x && SelColumn < 0 )
		{
			//点选列
			SelColumn = i;
			break;
		}
	}

	ListNodeCp* Node = (ListNodeCp*)GetItemData( SelRow );
	//if( Node->m_NodeType != enumNT_COMPRESS || SelColumn != 2 )
	//{
	if( SelColumn == 1 )
	{
		Node->m_Mesh->SetIsCompress(true);
		Node->m_ExpType = enumPT_COMPRESS;
	}
	if( SelColumn == 2 )
	{
		Node->m_Mesh->SetIsCompress(false);
		Node->m_ExpType = enumPT_UNCOMPRESS;
	}	
	//}

	if( SelColumn >= 0 )
	{
		//刷新
		GetItemRect( SelRow, &ItemRect, LVIR_BOUNDS );
		InvalidateRect( &ItemRect );
	}

	CListCtrl::OnLButtonDown(nFlags, point);
}

void CPieceCpList::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	int ItemCount = GetItemCount( );
	if( ItemCount == 0 )
		return;	

	//跟踪每一行绘制其中文字和内容
	for( int i = 0; i < ItemCount; i++ )
	{
		ListNodeCp* Node = (ListNodeCp*)GetItemData( i );
		if( pNMLV->iSubItem == 1 )
		{
			Node->m_Mesh->SetIsCompress(true);
			Node->m_ExpType = enumPT_COMPRESS;
		}
		if( pNMLV->iSubItem == 2/* && Node->m_NodeType == enumNT_UNCOMPRESS*/ )
		{
			Node->m_Mesh->SetIsCompress(false);
			Node->m_ExpType = enumPT_UNCOMPRESS;
		}	
	}
	Invalidate();
}
