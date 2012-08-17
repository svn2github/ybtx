// ImageListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Ybtx.h"
#include "ImageListCtrl.h"
#include "SelectServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CImageListCtrl

IMPLEMENT_DYNAMIC(CImageListCtrl, CListCtrl)

CImageListCtrl::CImageListCtrl()
{
	m_SelRow = -1;
	m_SelCol = -1;
	
	//BOOL temp = m_Font.CreateFont( 20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
	//	OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY | DRAFT_QUALITY | PROOF_QUALITY,
	//	FF_DONTCARE | DEFAULT_PITCH, L"方正准圆_GBK" );
	m_nItemHeight = 30;
	m_NormalColor = ::GetSysColor(COLOR_WINDOWFRAME);
	m_SelectColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	EnableToolTips(true);
}

CImageListCtrl::~CImageListCtrl()
{
	map< int, pair<CImageManage*,CImageManage*> >::iterator it;
	for (it = m_ItemImageMap.begin(); it != m_ItemImageMap.end(); ++it )
	{
		delete it->second.first;
		delete it->second.second;
	}
	m_ItemImageMap.clear();
}

BEGIN_MESSAGE_MAP(CImageListCtrl, CListCtrl)
	ON_WM_MEASUREITEM()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_NCCALCSIZE()
	ON_WM_ERASEBKGND()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA,0,0xFFFF,OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW,0,0xFFFF,OnToolTipText)
END_MESSAGE_MAP()

// CImageListCtrl message handlers

void CImageListCtrl::LoadNormalImage(wstring ImageFile)
{
	m_NormalImage.LoadImage(ImageFile.c_str());
}

void CImageListCtrl::LoadSelectImage(wstring ImageFile)
{
	m_SelectImage.LoadImage(ImageFile.c_str());
}

void CImageListCtrl::LoadBkImage(wstring ImageFile)
{
	m_BkgndImage.LoadImage(ImageFile.c_str());
}

void CImageListCtrl::SelectSubItem(int nRow, int nCol)
{
	InvalidateGrid(m_SelRow,m_SelCol);
	m_SelRow =	nRow;
	m_SelCol =	nCol;
	InvalidateGrid(m_SelRow,m_SelCol);
}

void CImageListCtrl::SelectSubItem(wstring strSubItem)
{
	if ( strSubItem.empty() )
		return ;

	CString strTemp;
	for ( int nRow = 0; nRow < GetItemCount(); nRow++)
	{
		LV_COLUMN lvc;
		lvc.mask = LVCF_FMT|LVCF_WIDTH|LVCF_SUBITEM;
		for ( int nCol = 0; GetColumn(nCol,&lvc); nCol++ )
		{
			strTemp = GetItemText(nRow,nCol);
			if (wstring(strTemp) == strSubItem)
			{
				SelectSubItem(nRow, nCol);
				return;
			}
		}
	}
}

void CImageListCtrl::InvalidateGrid(int nRow, int nCol)
{
	CRect r;
	if( nCol==0 )
		GetItemRect(nRow,&r,LVIR_LABEL);
	else
		GetSubItemRect(nRow,nCol,LVIR_BOUNDS,r);

	InvalidateRect(&r);
}

void CImageListCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CImageListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = m_nItemHeight;
}

void CImageListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int nItem = lpDrawItemStruct->itemID;
	if (nItem == -1)
		return;

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	int nOldDCMode = pDC->SaveDC();

	//Set Font Color
	CString sText;
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT|LVCF_WIDTH|LVCF_SUBITEM;

	CRect rcCol = lpDrawItemStruct->rcItem;
	rcCol.right = rcCol.left;
	for ( int nCol = 0; GetColumn(nCol,&lvc); nCol++ )
	{
		sText = GetItemText(nItem,nCol);
		if(sText.IsEmpty())
			return;

		rcCol.left = rcCol.right;
		rcCol.right = rcCol.left + GetColumnWidth(nCol);
		if (  m_SelRow == nItem &&  m_SelCol == nCol )
		{
			CImageManage* pImage = GetItemSelectImage(nItem,nCol);
			int offX = (rcCol.Width() -  pImage->GetBitmapInfo().bmWidth)/2;
			int offY = (rcCol.Height() - pImage->GetBitmapInfo().bmHeight)/2;
			pDC->SetTextColor(m_SelectColor);
			SelectObject(pImage->GetMemDC(), pImage->GetBitmapHandle());
			BitBlt(lpDrawItemStruct->hDC, rcCol.left + offX, rcCol.top + offY, pImage->GetBitmapInfo().bmWidth,
				pImage->GetBitmapInfo().bmHeight, pImage->GetMemDC(), 0, 0, SRCCOPY);
		}
		else
		{
			CImageManage* pImage = GetItemNormalImage(nItem,nCol);
			int offX = (rcCol.Width() -  pImage->GetBitmapInfo().bmWidth)/2;
			int offY = (rcCol.Height() - pImage->GetBitmapInfo().bmHeight)/2;
			pDC->SetTextColor(GetItemColor(nItem,nCol));
			SelectObject(pImage->GetMemDC(), pImage->GetBitmapHandle());
			BitBlt(lpDrawItemStruct->hDC, rcCol.left + offX, rcCol.top + offY, pImage->GetBitmapInfo().bmWidth,
				pImage->GetBitmapInfo().bmHeight, pImage->GetMemDC(), 0, 0, SRCCOPY);
		}
		//pDC->SelectObject( &m_Font );
		
		pDC->DrawText(sText,-1,CRect::CRect(rcCol.left,rcCol.top,rcCol.right,rcCol.bottom),DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}
	pDC->RestoreDC(nOldDCMode);
	m_ImageVertScrollbar.Draw();
}

void CImageListCtrl::SetItemHeight(int nItemHeight)
{
	m_nItemHeight = nItemHeight;
	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED,0,(LPARAM)&wp);
}

void CImageListCtrl::InitScrollbar(CRect rt,wstring ArrowUpImageFile,wstring ArrowDownImageFile,wstring ThumbImageFile,wstring BkgImageFile )
{
	m_ImageVertScrollbar.Create(NULL,WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE|WS_GROUP,rt,this->GetParent());
	m_ImageVertScrollbar.LoadArrowUpImage(ArrowUpImageFile);
	m_ImageVertScrollbar.LoadArrowDownImage(ArrowDownImageFile);
	m_ImageVertScrollbar.LoadBkgImage(BkgImageFile);
	m_ImageVertScrollbar.LoadThumbImage(ThumbImageFile);
	m_ImageVertScrollbar.SetList(this);
}
void CImageListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	//去掉ScrollBar
	ModifyStyle(WS_HSCROLL|WS_VSCROLL,0);
	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void CImageListCtrl::SetNormalColor(COLORREF color)
{
	m_NormalColor = color;
}

void CImageListCtrl::SetSelectColor(COLORREF color)
{
	m_SelectColor = color;
}

BOOL CImageListCtrl::OnEraseBkgnd(CDC* pDC)
{
	CRect clientRect;
	GetClientRect(&clientRect);

	int offX = (clientRect.Width() -  m_BkgndImage.GetBitmapInfo().bmWidth)/2;
	int offY = (clientRect.Height() - m_BkgndImage.GetBitmapInfo().bmHeight)/2;	
	
	SelectObject(m_BkgndImage.GetMemDC(), m_BkgndImage.GetBitmapHandle());
	BitBlt(pDC->m_hDC, clientRect.left + offX,  clientRect.top + offY , m_BkgndImage.GetBitmapInfo().bmWidth,
		m_BkgndImage.GetBitmapInfo().bmHeight, m_BkgndImage.GetMemDC(), 0, 0, SRCCOPY);	
	
	return true;
}

COLORREF CImageListCtrl::GetItemColor(int Row,int Col)
{
	map<pair<int,int>, COLORREF>::iterator it;
	it = m_ItemColorMap.find( pair<int,int>(Row,Col) );
	if ( it == m_ItemColorMap.end() )
		return m_NormalColor;
	else
		return it->second;
}

void CImageListCtrl::SetItemColor(int Row,int Col,COLORREF ItemColor)
{
	m_ItemColorMap[make_pair(Row,Col)] = ItemColor;
}

void CImageListCtrl::DeleteAllItems()
{
	m_ItemImageIDMap.clear();
	m_ItemColorMap.clear();
	m_ToolTipMap.RemoveAll();
	CListCtrl::DeleteAllItems();
}

CImageManage* CImageListCtrl::GetItemNormalImage(int Row,int Col)
{
	map< pair<int,int>, int>::iterator itID = m_ItemImageIDMap.find(pair<int,int>(Row,Col));
	if (itID == m_ItemImageIDMap.end())
		return &m_NormalImage;
	else
	{
		map< int, pair<CImageManage*,CImageManage*> >::iterator itImage  = m_ItemImageMap.find(itID->second);
		if ( itImage == m_ItemImageMap.end() )
			return &m_NormalImage;
		else
			return itImage->second.first;
	}
}

CImageManage* CImageListCtrl::GetItemSelectImage(int Row,int Col)
{
	map< pair<int,int>, int>::iterator itID = m_ItemImageIDMap.find(pair<int,int>(Row,Col));
	if ( itID == m_ItemImageIDMap.end())
		return &m_SelectImage;
	else
	{
		map< int, pair<CImageManage*,CImageManage*> >::iterator itImage  = m_ItemImageMap.find(itID->second);
		if ( itImage == m_ItemImageMap.end() )
			return &m_SelectImage;
		else
			return itImage->second.second;
	}
}

void CImageListCtrl::SetItemImage(int ImageID,CImageManage* pNormalImage,CImageManage* pSelectImage)
{
	m_ItemImageMap[ImageID] = make_pair(pNormalImage,pSelectImage);
}

void CImageListCtrl::SetItemImageID(int Row,int Col,int ImageID)
{
	m_ItemImageIDMap[make_pair(Row,Col)] = ImageID;
}

BOOL CImageListCtrl::OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{

	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;

	if( (pNMHDR->hwndFrom == m_hWnd) && ( ((pNMHDR->code == TTN_NEEDTEXTA) && (pTTTA->uFlags & TTF_IDISHWND)) || 
		((pNMHDR->code == TTN_NEEDTEXTW) && (pTTTW->uFlags & TTF_IDISHWND)) ) )
		return FALSE;

	AFX_MODULE_THREAD_STATE* pThreadState = AfxGetModuleThreadState();
	CToolTipCtrl* pToolTip = pThreadState->m_pToolTip;
	pToolTip->SetTipBkColor(0x202020);
	pToolTip->SetTipTextColor(0x618aa2);
	pToolTip->SetMaxTipWidth(200);
	pToolTip->SetDelayTime(TTDT_AUTOPOP,15000);
	pToolTip->SetDelayTime(TTDT_INITIAL,0);

	*pResult = 0;
	const MSG* pMessage;
	pMessage = GetCurrentMessage();
	ASSERT ( pMessage );
	CPoint pt;
	pt = pMessage->pt; 
	ScreenToClient( &pt );

	LVHITTESTINFO lvhitTestInfo;
	lvhitTestInfo.pt	= pt;
	int nItem = SubItemHitTest(&lvhitTestInfo);
	int nSubItem = lvhitTestInfo.iSubItem;
	UINT nFlags =   lvhitTestInfo.flags;
	CString strTipText;
	if( nFlags & LVHT_ONITEMLABEL )
	{
		CString strKey;
		strKey.Format(_T("%d"),  nItem * 100 + nSubItem);
		if( m_ToolTipMap.Lookup(strKey, strTipText ) )
		{
			#ifndef _UNICODE
				if (pNMHDR->code == TTN_NEEDTEXTA)
					lstrcpyn(pTTTA->szText, strTipText, 80);
				else
					_mbstowcsz(pTTTW->szText, strTipText, 80);
			#else
				if (pNMHDR->code == TTN_NEEDTEXTA)
					_wcstombsz(pTTTA->szText, strTipText, 80);
				else
					lstrcpyn(pTTTW->szText, strTipText, 80);
			#endif
			return FALSE;
		}
	}
	return FALSE;
}

INT_PTR CImageListCtrl::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	LVHITTESTINFO lvhitTestInfo;
	lvhitTestInfo.pt	= point;
	int nItem = ListView_SubItemHitTest(this->m_hWnd,&lvhitTestInfo);
	int nSubItem = lvhitTestInfo.iSubItem;
	UINT nFlags =   lvhitTestInfo.flags;
	if (nFlags & LVHT_ONITEMLABEL)
	{
		RECT rcClient;
		GetClientRect( &rcClient );
		pTI->hwnd = m_hWnd;
		pTI->uId = (UINT) (nItem * 100 + nSubItem);
		pTI->lpszText = LPSTR_TEXTCALLBACK;
		pTI->rect = rcClient;
		return pTI->uId;
	}
	else
		return CListCtrl::OnToolHitTest(point, pTI);
}

void CImageListCtrl::SetItemToolTipText( int nItem, int nSubItem, LPCTSTR lpszToolTipText )
{
	CString strKey;
	strKey.Format(_T("%d"),  nItem * 100 + nSubItem);
	m_ToolTipMap.SetAt( strKey, lpszToolTipText );
}