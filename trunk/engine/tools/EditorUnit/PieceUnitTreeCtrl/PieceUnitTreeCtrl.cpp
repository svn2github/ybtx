#include "stdafx.h"
#include "PieceUnitTreeCtrl/PieceUnitTreeCtrlRes.h"
#include "PieceUnitTreeCtrl.h"


// CPieceUnitTreeCtrl
#define   DRAG_DELAY   60
IMPLEMENT_DYNAMIC(CPieceUnitTreeCtrl, CTreeCtrl)
BEGIN_MESSAGE_MAP(CPieceUnitTreeCtrl, CTreeCtrl)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnTvnEndlabeledit)
END_MESSAGE_MAP()

// CPieceUnitTreeCtrl message handlers
void CPieceUnitTreeCtrl::PreSubclassWindow()
{
	m_Image.Create(16,16,ILC_COLOR16|ILC_MASK,0,1);
	m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_SINGLEPIECE ) );
	m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_RENDERSTYLE ) );
	m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_RETEXTURE ) );
	m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_PIECERS ) );
	m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_VERANI ) );
	m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_SINGLEANI ) );
	m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_LODLEVEL ) );
	SetImageList( &m_Image, TVSIL_NORMAL );

	m_bDragging = false;


	CTreeCtrl::PreSubclassWindow();
}

void CPieceUnitTreeCtrl::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: Add your control notification handler code here

	if( !pTVDispInfo->item.hItem || !pTVDispInfo->item.pszText )
		return;
	int nImage, nSelectedImage;
	GetItemImage( pTVDispInfo->item.hItem, nImage, nSelectedImage );
	if( nImage == 0 )
	{
		HTREEITEM hPieceItem = GetChildItem( TVI_ROOT );

		while( hPieceItem )
		{
			CEditPiece* pPiece = ( CEditPiece* )GetItemData( hPieceItem );
			if( pPiece && pPiece->GetPieceName() == pTVDispInfo->item.pszText )
				return;
			hPieceItem = GetNextItem( hPieceItem, TVGN_NEXT );
		}

		CEditPiece* PieceUnit = ( CEditPiece* )GetItemData( pTVDispInfo->item.hItem );
		g_Model.DeleteMeshPiece( PieceUnit->GetPieceName() );
		PieceUnit->SetPieceName( pTVDispInfo->item.pszText );
		SetItemText( pTVDispInfo->item.hItem, pTVDispInfo->item.pszText );
		g_Model.AddMeshPieceEx( PieceUnit,0 );
	}
	if( nImage == 5 )
	{
		HTREEITEM hPiece = GetParentItem( GetParentItem( GetParentItem( pTVDispInfo->item.hItem ) ) );
		CEditPiece* PieceUnit = ( CEditPiece* )GetItemData( hPiece );
#if 0
		CMeshLevel* Level = ( CMeshLevel* )GetItemData( GetParentItem( pTVDispInfo->item.hItem ) );
		for( int i = 0; i < (int)Level->GetAniNum(); i++ )
		{
			if( Level->GetAniName(i) == GetItemText( pTVDispInfo->item.hItem ) )
			{
				Level->SetAniName( i, pTVDispInfo->item.pszText );
				SetItemText( pTVDispInfo->item.hItem, pTVDispInfo->item.pszText );
				break;
			}
		}
#endif
	}
	if( nImage == 6 )
	{
		HTREEITEM hPiece = GetParentItem( GetParentItem( pTVDispInfo->item.hItem ) );
		CEditPiece* PieceUnit = ( CEditPiece* )GetItemData( hPiece );

		CMeshLevel* Level = ( CMeshLevel* )GetItemData( pTVDispInfo->item.hItem );
		float Dist = pTVDispInfo->item.pszText ? (float)atof( pTVDispInfo->item.pszText ) : 0.0f;
		Level->SetDist(Dist);

		CString Val;
		Val.Format( "%g", Dist );
		SetItemText( pTVDispInfo->item.hItem, Val );
	}
	*pResult = 0;
}

void CPieceUnitTreeCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	//如果是无意拖曳，则放弃操作
	if( (timeGetTime() - m_dwDragStart) < DRAG_DELAY )
		return;

	m_hItemDragS = pNMTreeView->itemNew.hItem;
	m_hItemDragD = NULL;

	//得到用于拖动时显示的图象列表
	m_pDragImage = CreateDragImage( m_hItemDragS );
	if( !m_pDragImage )
		return;
	if(m_nImage!=1)
		return;

	m_bDragging = true;
	m_pDragImage->BeginDrag ( 0,CPoint(8,8) );
	CPoint  pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter ( this,pt );  //"this"将拖曳动作限制在该窗口
	SetCapture();

	m_nScrollTimerID = SetTimer( 2,40,NULL );
}




//拷贝条目
HTREEITEM CPieceUnitTreeCtrl::CopyItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter)
{
	TV_INSERTSTRUCT  tvstruct;
	HTREEITEM        hNewItem;
	CString          sText;

	//得到源条目的信息
	tvstruct.item.hItem = hItem;
	tvstruct.item.mask  = TVIF_CHILDREN|TVIF_HANDLE|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
	GetItem( &tvstruct.item );
	sText = GetItemText( hItem );
	tvstruct.item.cchTextMax = sText.GetLength ();
	tvstruct.item.pszText    = sText.LockBuffer ();

	//将条目插入到合适的位置
	tvstruct.hParent         = htiNewParent;
	tvstruct.hInsertAfter    = htiAfter;
	tvstruct.item.mask       = TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_TEXT;
	hNewItem = InsertItem( &tvstruct );
	sText.ReleaseBuffer ();

	//限制拷贝条目数据和条目状态
	SetItemData( hNewItem,GetItemData(hItem) );
	SetItemState( hNewItem,GetItemState(hItem,TVIS_STATEIMAGEMASK),TVIS_STATEIMAGEMASK);

	return hNewItem;
}

//拷贝分支
HTREEITEM CPieceUnitTreeCtrl::CopyBranch(HTREEITEM htiBranch, HTREEITEM htiNewParent, HTREEITEM htiAfter)
{
	HTREEITEM  hChild;
	HTREEITEM  hNewItem = CopyItem( htiBranch,htiNewParent,htiAfter );
	hChild = GetChildItem( htiBranch );

	while( hChild != NULL )
	{
		CopyBranch( hChild,hNewItem,htiAfter );
		hChild = GetNextSiblingItem( hChild );
	}

	return  hNewItem;
}
void CPieceUnitTreeCtrl::OnTimer(UINT nIDEvent) 
{
	//if(!(::GetKeyState(VK_CONTROL)&0x8000))
	//	return;
	//鼠标敏感节点
	if( nIDEvent == m_nHoverTimerID )
	{
		KillTimer( m_nHoverTimerID );
		m_nHoverTimerID = 0;
		HTREEITEM  trItem = 0;
		UINT  uFlag = 0;
		trItem = HitTest( m_HoverPoint,&uFlag );
		if( trItem && m_bDragging )
		{
			SelectItem( trItem );
			Expand( trItem,TVE_EXPAND );
		}
	}
	//处理拖曳过程中的滚动问题
	else if( nIDEvent == m_nScrollTimerID )
	{
		m_TimerTicks++;
		CPoint  pt;
		GetCursorPos( &pt );
		CRect  rect;
		GetClientRect( &rect );
		ClientToScreen( &rect );

		HTREEITEM  hItem = GetFirstVisibleItem();

		if( pt.y < rect.top +10 )
		{
			//向上滚动
			int  slowscroll = 6 - (rect.top + 10 - pt.y )/20;
			if( 0 == (m_TimerTicks % ((slowscroll > 0) ? slowscroll : 1)) )
			{
				CImageList::DragShowNolock ( false );
				SendMessage( WM_VSCROLL,SB_LINEUP );
				SelectDropTarget( hItem );
				m_hItemDragD = hItem;
				CImageList::DragShowNolock ( true );
			}
		}
		else if( pt.y > rect.bottom - 10 )
		{
			//向下滚动
			int  slowscroll = 6 - (pt.y - rect.bottom + 10)/20;
			if( 0 == (m_TimerTicks % ((slowscroll > 0) ? slowscroll : 1)) )
			{
				CImageList::DragShowNolock ( false );
				SendMessage( WM_VSCROLL,SB_LINEDOWN );
				int  nCount = GetVisibleCount();
				for( int i=0 ; i<nCount-1 ; i++ )
					hItem = GetNextVisibleItem( hItem );
				if( hItem )
					SelectDropTarget( hItem );
				m_hItemDragD = hItem;
				CImageList::DragShowNolock ( true );
			}
		}
	}
	else
		CTreeCtrl::OnTimer(nIDEvent);
}
void CPieceUnitTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	HTREEITEM  hItem;
	UINT       flags;

	//检测鼠标敏感定时器是否存在,如果存在则删除,删除后再定时
	//if(!(::GetKeyState(VK_CONTROL)&0x8000))
	//	return;
	if( m_nHoverTimerID )
	{
		KillTimer( m_nHoverTimerID );
		m_nHoverTimerID = 0;
	}
	m_nHoverTimerID = SetTimer( 1,800,NULL );  //定时为 0.8 秒则自动展开
	m_HoverPoint = point;

	if( m_bDragging )
	{
		CPoint  pt = point;
		CImageList::DragMove( pt );

		//鼠标经过时高亮显示
		CImageList::DragShowNolock( false );  //避免鼠标经过时留下难看的痕迹
		if( (hItem = HitTest(point,&flags)) != NULL )
		{
			SelectDropTarget( hItem );
			m_hItemDragD = hItem;
		}
		CImageList::DragShowNolock( true );

		//当条目被拖曳到左边缘时，将条目放在根下
		CRect  rect;
		GetClientRect( &rect );
		if( point.x < rect.left + 20 )
			m_hItemDragD = NULL;
	}

	CTreeCtrl::OnMouseMove(nFlags, point);
}
void CPieceUnitTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_dwDragStart = timeGetTime();
	m_Node.clear();
	m_nCount =0;


	CModelEditorDlg* Dlg = (CModelEditorDlg*)AfxGetMainWnd();
	HTREEITEM hPreSel = GetSelectedItem();

	CTreeCtrl::OnLButtonDown(nFlags, point);

	HTREEITEM hCurSel = GetSelectedItem();
	if( !hCurSel )
		return;
	m_NodeText = GetItemText(hCurSel);
	HTREEITEM   hRoot   =   GetRootItem();   
	TreeNodeVisit(hRoot);

	int nImage, nSelectedImage;
	GetItemImage( hCurSel, nImage, nSelectedImage );
	m_nImage = nImage;

	if( nImage == 0 )
	{
		CEditPiece* PieceUnit = ( CEditPiece* )GetItemData( hCurSel );
		string str = PieceUnit->GetPieceName();
		g_Model.SetSelectedPiece( str );
		TCHAR Buf[256];
		sprintf( Buf, "%f", PieceUnit->GetZBias() );
		m_zBais.SetWindowText( Buf );
	}
	else if(nImage == 1)
	{
		CEditPieceRenderStyle* rs = ( CEditPieceRenderStyle* )GetItemData( hCurSel );
		Dlg->m_PieceEdit.m_rsIndexNameBox.SetWindowText(rs->GetRSIndexName());
		g_Model.SetSelectedPiece( string() );
	}
	else
		g_Model.SetSelectedPiece( string() );
	/*
	else
	{
	HTREEITEM HPiece = hCurSel;
	while( nImage != 0 )
	{
	HPiece = GetParentItem( HPiece );
	GetItemImage( HPiece, nImage, nSelectedImage );
	}
	CEditPiece* PieceUnit = ( CEditPiece* )GetItemData( HPiece );
	string str = PieceUnit->GetPieceClassName();
	if( g_Model.GetSelectedPieceClass() != str )
	g_Model.SetSelectedPieceClass( str );
	}*/
}
void CPieceUnitTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnLButtonUp(nFlags, point);

	if( m_bDragging )
	{
		m_bDragging = FALSE;
		CImageList::DragLeave( this );
		CImageList::EndDrag();
		ReleaseCapture();
		delete m_pDragImage;

		SelectDropTarget( NULL );

		if( m_hItemDragS == m_hItemDragD )
		{
			KillTimer( m_nScrollTimerID );
			return;
		}

		int nImage,nSelectedImage;
		GetItemImage( m_hItemDragD, nImage, nSelectedImage );
		if(nImage!=1)
		{
			KillTimer( m_nScrollTimerID );
			return;
		}
		CString DragS = GetItemText(GetParentItem(GetParentItem(m_hItemDragS)));
		CString DragD = GetItemText(GetParentItem(GetParentItem(m_hItemDragD)));

		if(DragS!=DragD)
		{
			KillTimer( m_nScrollTimerID);
			return;
		}
		Expand( m_hItemDragD,TVE_EXPAND );

		HTREEITEM  htiNew = CopyBranch( m_hItemDragS,GetParentItem(m_hItemDragD),m_hItemDragD );


		int nMark=0;
		int nItemS;
		int nItemD;

		HTREEITEM hElement = GetChildItem( GetParentItem(m_hItemDragD) );
		while( hElement )
		{
			if(hElement==m_hItemDragD)
				nItemD = nMark;
			if(hElement==m_hItemDragS)
				nItemS = nMark;
			nMark++;
			hElement = GetNextItem( hElement, TVGN_NEXT);;
		}
		if(nItemS>nItemD)
			nItemS--;



		CEditPiece* PieceUnit = ( CEditPiece* )GetItemData( GetParentItem(GetParentItem(m_hItemDragD)) );
		PieceUnit->InsertRenderStyle(nItemS,nItemD);

		DeleteItem( m_hItemDragS );
		SelectItem( m_hItemDragD );
		KillTimer( m_nScrollTimerID );
	}
}
void CPieceUnitTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	HTREEITEM hCurSel = GetSelectedItem();
	if( !hCurSel )
		return;
	int nImage, nSelectedImage;
	GetItemImage( hCurSel, nImage, nSelectedImage );
	if( nImage == 1 )
	{
		HTREEITEM hPiece = GetParentItem( GetParentItem( hCurSel ) );
		CEditPiece* PieceUnit = ( CEditPiece* )GetItemData( hPiece );
		CEditPieceRenderStyle* rs = (CEditPieceRenderStyle*)GetItemData( hCurSel );
		for( UINT i = 0; i < PieceUnit->GetRSNum(); i++ )
		{
			if( PieceUnit->GetRS(i) == rs )
			{
				CRenderPiece RP( PieceUnit, i );
				g_Model.AddMeshPieceEx( PieceUnit, i);
				return;
			}
		}
	}
	if( nImage == 5 )
	{
		HTREEITEM hPiece = GetParentItem( GetParentItem( GetParentItem( hCurSel ) ) );
		CEditPiece* PieceUnit = ( CEditPiece* )GetItemData( hPiece );
#if 0
		CMeshLevel* Level = ( CMeshLevel* )GetItemData( GetParentItem( hCurSel ) );
		for( int i = 0; i < (int)Level->GetAniNum(); i++ )
		{
			if( Level->GetAniName(i) == GetItemText( hCurSel ) )
			{
				g_Model.SetNextAnimation( Level->GetAniName(i),FramePair(0,-1),true,0,1.0f );
				SetNextAniAndFrame( , 0 );
				break;
			}
		}
#endif
	}

	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

void CPieceUnitTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if ( nChar == VK_DELETE )
	{
		HTREEITEM hCurSel = GetSelectedItem();
		if( !hCurSel )
			return;
		int nImage, nSelectedImage;
		GetItemImage( hCurSel, nImage, nSelectedImage );
		if( nImage == 1 )
		{
			HTREEITEM hPiece = GetParentItem( GetParentItem( hCurSel ) );
			CEditPiece* PieceUnit = ( CEditPiece* )GetItemData( hPiece );
			CEditPieceRenderStyle* rs = (CEditPieceRenderStyle*)GetItemData( hCurSel );
			for( UINT i = 0; i < PieceUnit->GetRSNum(); i++ )
			{
				if( PieceUnit->GetRS(i) == rs )
				{
					PieceUnit->DeleteRS( i );
					DeleteItem( hCurSel );
					return ;
				}
			}				
		}
		if( nImage == 6 )
		{
			HTREEITEM hPiece = GetParentItem( GetParentItem( hCurSel ) );
			CEditPiece* PieceUnit = ( CEditPiece* )GetItemData( hPiece );

			CMeshLevel* Level = ( CMeshLevel* )GetItemData( hCurSel );
			GVector<CMeshLevel>& MeshLevel = PieceUnit->GetMesh()->GetMeshLevel();

			for( int i = 0; i < (int)MeshLevel.size(); i++ )
			{
				if( &MeshLevel[i] == Level )
				{
					MeshLevel.erase( MeshLevel.begin() + i );
					delete Level;
					DeleteItem( hCurSel );
					return ;
				}
			}		
		}
		if( nImage == 5 )
		{
			HTREEITEM hPiece = GetParentItem( GetParentItem( GetParentItem( hCurSel ) ) );
			CEditPiece* PieceUnit = ( CEditPiece* )GetItemData( hPiece );
#if 0 
			CMeshLevel* Level = ( CMeshLevel* )GetItemData( GetParentItem( hCurSel ) );
			for( int i = 0; i < (int)Level->GetAniNum(); i++ )
			{
				if( Level->GetAniName(i) == GetItemText( hCurSel ) )
				{
					Level->DeleteAni( i );
					g_Model.SetNextAnimation( "",FramePair(0,-1),true,0,1.0f );
					DeleteItem( hCurSel );
					break;
				}
			}
#endif
		}
	}

	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

int	CPieceUnitTreeCtrl::AddPieceUnit( CEditPiece* Piece )
{
	if( !Piece )
		return GERROR;

	int i;
	TVINSERTSTRUCT Item;

	HTREEITEM hPieceItem = GetChildItem( TVI_ROOT );

	for( i = 0; hPieceItem; i++ )
	{
		CEditPiece* OldPiece = ( CEditPiece* )GetItemData( hPieceItem );
		if( OldPiece && OldPiece->GetPieceName() == Piece->GetPieceName() )
		{		
			AddPiece( hPieceItem, Piece );
			return 0;
		}
		hPieceItem = GetNextItem( hPieceItem, TVGN_NEXT );
	}

	Item.hParent = TVI_ROOT;
	Item.hInsertAfter = TVI_LAST;
	Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
	Item.item.iImage = 0;
	Item.item.iSelectedImage = 0;
	Item.item.lParam = (LPARAM)Piece;
	Item.item.pszText = (LPSTR)Piece->GetPieceName();

	AddPiece( InsertItem( &Item ), Piece );

	return 0;
}

int	CPieceUnitTreeCtrl::AddPiece( HTREEITEM hPieceItem, CEditPiece* Piece )
{
	TVINSERTSTRUCT Item;
	CModelEditorDlg* MainDlg = (CModelEditorDlg*)AfxGetMainWnd();
	if(MainDlg->m_PieceEdit.IsPreView())
		g_Model1.AddMeshPieceEx(Piece,0);
	else
		g_Model.AddMeshPieceEx(Piece,0);

	if(g_bUseLastPortait)
		*g_pPortraitInfo = g_PortraitInfoLast;
	else
		g_PortraitInfoLast = *g_pPortraitInfo;

	SetItemData( hPieceItem, (LPARAM)Piece );
	HTREEITEM hRSItem = GetChildItem( hPieceItem );
	while( hRSItem )
	{
		HTREEITEM CurItem = hRSItem;
		hRSItem = GetNextItem( hRSItem, TVGN_NEXT);;
		DeleteItem( CurItem );
	}

	Item.hParent = hPieceItem;
	Item.hInsertAfter = TVI_LAST;
	Item.item.mask = TVIF_IMAGE|TVIF_TEXT|TVIF_SELECTEDIMAGE;
	Item.item.iImage = 3;
	Item.item.iSelectedImage = 3;
	Item.item.pszText = "PieceRS";
	AddPieceRS( InsertItem( &Item ), Piece );

	Item.hParent = hPieceItem;
	Item.hInsertAfter = TVI_LAST;
	Item.item.mask = TVIF_IMAGE|TVIF_TEXT|TVIF_SELECTEDIMAGE;
	Item.item.iImage = 4;
	Item.item.iSelectedImage = 4;
	Item.item.pszText = "VertexAni";
	AddPieceVerAni( InsertItem( &Item ), Piece );
	return 0;
}

int	CPieceUnitTreeCtrl::AddPieceRS( HTREEITEM hRSItem, CEditPiece* Piece )
{
	TVINSERTSTRUCT Item;

	HTREEITEM hChildItem = GetChildItem( hRSItem );
	while( hChildItem )
	{
		HTREEITEM CurItem = hChildItem;
		hChildItem = GetNextItem( hChildItem, TVGN_NEXT);;
		DeleteItem( CurItem );
	}

	for( UINT i = 0; i < Piece->GetRSNum(); i++ )
	{
		CEditPieceRenderStyle* rs = (CEditPieceRenderStyle*)Piece->GetRS(i);
		CString Str ;
		if(rs->GetEditRenderStyle() != NULL)
			Str = rs->GetEditRenderStyle()->GetName();
		else
		{
			CString info;
			info = Piece->GetPieceName();
			info += "这个piece中有没设置的RS";
			MessageBox(info);
		}

		CString RSIndexStr = rs->GetRSIndexName();
		if( Str.IsEmpty() )
			Str = "Default";
		CString tmp =Str + "("+RSIndexStr+")";
		Str = tmp ;
		LPCTSTR Name = (LPCTSTR)Str + Str.GetLength();
		while ( *( Name - 1 ) != '\\' && Name != (LPCTSTR)Str )
			Name--;
		Item.hParent = hRSItem;
		Item.hInsertAfter = TVI_LAST;
		Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
		Item.item.iImage = 1;
		Item.item.iSelectedImage = 1;
		Item.item.lParam = (LPARAM)rs;
		Item.item.pszText = (LPSTR)Name;
		hChildItem = InsertItem( &Item );

		for( int j = 0; j<rs->GetTextureNum(); j++ )
		{
			string TextName = rs->GetTexture( j )->GetName();
			LPCTSTR Name = TextName.c_str() + TextName.size();
			while ( *( Name - 1 ) != '\\' && Name != TextName.c_str() )
				Name--;
			Item.hParent = hChildItem;
			Item.hInsertAfter = TVI_LAST;
			Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
			Item.item.iImage = 2;
			Item.item.iSelectedImage = 2;
			Item.item.lParam = (LPARAM)rs->GetTexture( j );
			Item.item.pszText = (LPSTR)Name;
			InsertItem( &Item );
		}
	}
	return 0;
}

int	CPieceUnitTreeCtrl::AddPieceVerAni( HTREEITEM hVAItem, CEditPiece* Piece )
{
	TVINSERTSTRUCT Item;

	HTREEITEM hChildItem = GetChildItem( hVAItem );
	while( hChildItem )
	{
		HTREEITEM CurItem = hChildItem;
		hChildItem = GetNextItem( hChildItem, TVGN_NEXT);;
		DeleteItem( CurItem );
	}

	GVector<CMeshLevel>& MeshLevel = Piece->GetMesh()->GetMeshLevel();
	for( int Level = 0; Level < (int)MeshLevel.size(); Level++ )
	{
		CString Str;
		Str.Format( "%g", MeshLevel[Level].GetDist() );
		Item.hParent = hVAItem;
		Item.hInsertAfter = TVI_LAST;
		Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
		Item.item.iImage = 6;
		Item.item.iSelectedImage = 6;
		Item.item.lParam = (LPARAM)&MeshLevel[Level];
		Item.item.pszText = (LPSTR)(LPCTSTR)Str;
		HTREEITEM hLevel = InsertItem( &Item );
#if 0
		for( int i = 0; i < (int)MeshLevel[Level]->GetAniNum(); i++ )
		{
			CString Str = (LPCTSTR)( MeshLevel[Level]->GetAniName(i) );
			Item.hParent = hLevel;
			Item.hInsertAfter = TVI_LAST;
			Item.item.mask = TVIF_IMAGE|TVIF_TEXT|TVIF_SELECTEDIMAGE;
			Item.item.iImage = 5;
			Item.item.iSelectedImage = 5;
			Item.item.pszText = (LPSTR)(LPCTSTR)Str;
			InsertItem( &Item );
		}
#endif
	}
	return 0;
}

int	CPieceUnitTreeCtrl::DeletePieceUnit( CEditPiece* Piece )
{
	HTREEITEM hPieceItem = GetChildItem( TVI_ROOT );
	while( hPieceItem )
	{
		HTREEITEM CurItem = hPieceItem;
		hPieceItem = GetNextItem( hPieceItem, TVGN_NEXT);;
		CEditPiece* PieceUnit = ( CEditPiece* )GetItemData( CurItem );
		if( !Piece || PieceUnit == Piece )
		{
			g_Model.DeleteMeshPiece( PieceUnit->GetPieceClassName());
			DeleteItem( CurItem );
		}
	}
	g_Model.ClearMeshPiece();
	g_Model1.ClearMeshPiece();
	return 0;
}

void CPieceUnitTreeCtrl::AddVerAni( CString VerAniName, CEditPieceGroup* PieceGroup )
{
#if 0
	int i,j,k;
	HTREEITEM hPieceItem = GetChildItem( TVI_ROOT );

	for( i = 0; hPieceItem; i++ )
	{
		CEditPiece* DestPiece = ( CEditPiece* )GetItemData( hPieceItem );
		for( j = 0; j < PieceGroup->GetPieceNum(); j++ )
		{
			CEditPiece* SrcPiece = ( CEditPiece* )PieceGroup->GetPiece( j );
			if( SrcPiece->GetPieceName() != DestPiece->GetPieceName() )
				continue;
			CMesh* DestMesh = DestPiece->GetMesh();
			CMesh* SrcMesh = SrcPiece->GetMesh();

			if( DestMesh->GetVertexType() != VT_VERTEXANI || SrcMesh->GetVertexType() != VT_VERTEXANI )
				break;

			vector<CMeshLevel*>& DestLevels = DestMesh->GetLevel();
			vector<CMeshLevel*>& SrcLevels = SrcMesh->GetLevel();
			if( DestLevels.size() != 1 || SrcLevels.size() != 1 )
				break;

			CModelLevel* pSrc = (CModelLevel*)SrcLevels[0];
			CModelLevel* pDes = (CModelLevel*)DestLevels[0];

			if( memcmp( &pDes->m_VertexIndex[0], &pSrc->m_VertexIndex[0], pDes->m_VertexIndex.size()*sizeof( WORD ) ) )
				break;

			int n = (int)DestLevels[0]->GetAniNum();
			sqr::IDNAME Name = (LPCTSTR)VerAniName;
			for( k = 0; k < n; k++ )
			{
				if( pDes->GetAniName(k) == Name )
					break;
			}
			if( k != n )
				break;

			pDes->InserAni( Name, pSrc->m_VerAnimate[0] );

			AddPiece( hPieceItem, DestPiece );
		}
		hPieceItem = GetNextItem( hPieceItem, TVGN_NEXT );
	}
#endif
}

void    CPieceUnitTreeCtrl::FindNodeName(HTREEITEM hItem)
{
	m_nCount++;
	while(hItem)
	{
		CString nodeText;
		nodeText = GetItemText(hItem);
		string nt = nodeText.GetBuffer();
		if(nt.find("default")!=-1||nt.find("Default")!=-1)
		{
			CEditPiece* PieceUnit = ( CEditPiece* )GetItemData(GetParentItem(GetParentItem(hItem)));
			m_CheckNodeInfo = PieceUnit->GetPieceName();
			m_CheckNodeInfo += "中有没有设置的rs(Default)";
		}

		if(ItemHasChildren(hItem))       
		{   
			HTREEITEM   hChildItem   =   GetChildItem(hItem);       
			while(hChildItem!=NULL)       
			{   
				FindNodeName(hChildItem);           //递归遍历孩子节点  
				m_nCount--;
				hChildItem   =   GetNextItem(hChildItem,   TVGN_NEXT);     
			}       
		}       
		else
			return;
		hItem   =   GetNextItem(hItem,   TVGN_NEXT);  
	}  
}

bool	CPieceUnitTreeCtrl::CheckPieceGroup()
{
	FindNodeName(GetRootItem());
	if(m_CheckNodeInfo.IsEmpty())
		return true;
	else
	{
		MessageBox(m_CheckNodeInfo);
		m_CheckNodeInfo.Empty();
		return false;
	}
}

void	CPieceUnitTreeCtrl::TreeNodeVisit(HTREEITEM hItem)
{
	m_nCount++;
	while(hItem)
	{
		CString nodeText;
		nodeText = GetItemText(hItem);
		if(nodeText == m_NodeText && m_nCount == 3)
			m_Node.push_back(hItem);

		if(ItemHasChildren(hItem))       
		{   
			HTREEITEM   hChildItem   =   GetChildItem(hItem);       
			while(hChildItem!=NULL)       
			{   
				TreeNodeVisit(hChildItem);           //递归遍历孩子节点  
				m_nCount--;
				hChildItem   =   GetNextItem(hChildItem,   TVGN_NEXT);     
			}       

		}       
		else
			return;
		hItem   =   GetNextItem(hItem,   TVGN_NEXT);  
	}  
}

int	CPieceUnitTreeCtrl::CreateRS( CString FileName )
{
	HTREEITEM hCurSel = GetSelectedItem();
	if( !hCurSel )
		return GERROR;
	int nImage, nSelectedImage;
	GetItemImage( hCurSel, nImage, nSelectedImage );
	if( nImage == 3 )
	{
		CEditPiece* PieceUnit = ( CEditPiece* )GetItemData( GetParentItem( hCurSel ) );
		CEditRenderStyle* pRs = new CEditRenderStyle( NULL,NULL, FileName.GetBuffer() );
		if( !FileName.IsEmpty() )
			pRs->BinLoad();
		CEditPieceRenderStyle* rs = new CEditPieceRenderStyle(pRs);
		int j = 0;
		HTREEITEM hChildItem = GetChildItem( hCurSel );
		while ( hChildItem != NULL )
		{
			hChildItem = GetNextItem( hChildItem, TVGN_NEXT );
			j++;
		}
		CString strnum;
		strnum.Format("%d",j);
		string rsIndx = "默认";
		rsIndx += strnum.GetBuffer(strnum.GetLength());
		rs->SetRSIndexName(rsIndx);


		PieceUnit->AddRS( rs );

		CString tmpFileName = FileName;
		tmpFileName += "(";
		tmpFileName += rsIndx.c_str();
		tmpFileName += ")";

		LPCTSTR Name = (LPCTSTR)tmpFileName + tmpFileName.GetLength();
		while ( *( Name - 1 ) != '\\' && Name != (LPCTSTR)tmpFileName )
			Name--;

		TVINSERTSTRUCT Item;
		Item.hParent = hCurSel;
		Item.hInsertAfter = TVI_LAST;
		Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
		Item.item.iImage = 1;
		Item.item.iSelectedImage = 1;
		Item.item.lParam = (LPARAM)rs;
		Item.item.pszText = (LPSTR)Name;
		InsertItem( &Item );
		Expand( hCurSel, TVE_EXPAND );
	}
	return 0;
}

void CPieceUnitTreeCtrl::CreateLevel( float Dist, float FacePercent, float DeleteMaxLen, int MinTris )
{
	HTREEITEM hItem = GetSelectedItem();
	if( hItem )
	{
		int Image, SelectImage;
		GetItemImage( hItem, Image, SelectImage );

		if( Image == 4 )
		{
			CEditPiece* Piece = (CEditPiece*)GetItemData( GetParentItem( hItem ) );
			CLODBuilder Buider( Piece->GetMesh(), Dist, FacePercent, DeleteMaxLen, MinTris );
			CMeshLevel* Level = Buider.Build();
			if( Level )
			{
				GVector<CMeshLevel>& MeshLevel = Piece->GetMesh()->GetMeshLevel();
				GVector<CMeshLevel>::iterator it;
				for( it = MeshLevel.begin(); it != MeshLevel.end(); it++ )
				{
					CMeshLevel& Cur = *it;
					if( (*it).GetDist() > Level->GetDist() )
						break;
				}
				MeshLevel.insert( it, *Level );

				CString str;
				str.Format( "%f", Level->GetDist() );
				TVINSERTSTRUCT Item;
				Item.hParent = hItem;
				Item.hInsertAfter = TVI_LAST;
				Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
				Item.item.iImage = 6;
				Item.item.iSelectedImage = 6;
				Item.item.lParam = (LPARAM)Level;
				Item.item.pszText = (LPSTR)( (LPCTSTR)str );
				HTREEITEM hLevel = InsertItem( &Item );
#if 0
				for( int i = 0; i < (int)Level->GetAniNum(); i++ )
				{
					CString Str = (LPCTSTR)( Level->GetAniName(i) );
					Item.hParent = hLevel;
					Item.hInsertAfter = TVI_LAST;
					Item.item.mask = TVIF_IMAGE|TVIF_TEXT|TVIF_SELECTEDIMAGE;
					Item.item.iImage = 5;
					Item.item.iSelectedImage = 5;
					Item.item.pszText = (LPSTR)(LPCTSTR)Str;
					InsertItem( &Item );
				}
#endif
			}
		}
	}
}

//CSoftLevel* CPieceUnitTreeCtrl::GetSelectSoftLevel()
//{
//HTREEITEM hCurSel = GetSelectedItem();
//if( !hCurSel )
//	return NULL;
//int nImage, nSelectedImage;
//GetItemImage( hCurSel, nImage, nSelectedImage );
//if( nImage == 0 )
//{
//	CEditPiece* PieceUnit = ( CEditPiece* )GetItemData( hCurSel );
//	if( PieceUnit->GetMesh()->GetVertexType() == VT_SOFTBODYANI )
//		return ( CSoftLevel*)PieceUnit->GetMesh()->GetLevel()[0];
//}
//	return NULL;
//}

//CSoftClothLevel* CPieceUnitTreeCtrl::GetSelectSoftClothLevel()
//{
//HTREEITEM hCurSel = GetSelectedItem();
//if( !hCurSel )
//    return NULL;
//int nImage, nSelectedImage;
//GetItemImage( hCurSel, nImage, nSelectedImage );
//if( nImage == 0 )
//{
//    CEditPiece* PieceUnit = ( CEditPiece* )GetItemData( hCurSel );
//    if( PieceUnit->GetMesh()->GetVertexType() == VT_SOFTCLOTHANI )
//        return ( CSoftClothLevel*)PieceUnit->GetMesh()->GetLevel()[0];
//}
//   return NULL;
//}

CMesh* CPieceUnitTreeCtrl::GetSelectMesh()
{
	HTREEITEM hCurSel = GetSelectedItem();
	if( !hCurSel )
		return NULL;
	int nImage, nSelectedImage;
	GetItemImage( hCurSel, nImage, nSelectedImage );
	if( nImage == 0 )
		return ( ( CEditPiece* )GetItemData( hCurSel ) )->GetMesh();
	return NULL;
}