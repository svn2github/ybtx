// EditPan.cpp : implementation file
//

#include "stdafx.h"
#include "EditPan.h"
#include <CTileEditApp.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditPan

CEditPan::CEditPan()
{
	m_ViewCont=NULL;
}

CEditPan::~CEditPan()
{
	m_ViewCont = NULL;
}


BEGIN_MESSAGE_MAP(CEditPan, CTabCtrl)
	//{{AFX_MSG_MAP(CEditPan)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditPan message handlers

BOOL CEditPan::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	m_Font.CreateFont(12,0,0,0,400,0,0,0,134,3,2,2,2,"");
	BOOL re=CTabCtrl::Create(dwStyle, rect, pParentWnd, nID);
	SetFont(&m_Font);
	ShowWindow(SW_SHOW);
	SetPadding(CSize(2,4));
	SetMinTabWidth(12);
	return re;
}

UINT CEditPan::AddToList( CView* view, int Image, LPCTSTR ItemName )
{
	CFormViewContainer** temp=&m_ViewCont;

	while(*temp)
	{
		(*temp)->m_View->ShowWindow(SW_HIDE);
		temp=&((*temp)->m_Next);
	}
	*temp=new CFormViewContainer;
	(*temp)->m_View=view;

	CRect rect;
	GetClientRect(&rect);

	view->Create(NULL, NULL, WS_CHILD, rect, this, 1000, NULL);
	view->UpdateData( FALSE );
	view->ShowWindow(SW_SHOW);

	TCITEM item;
	memset(&item,0,sizeof(TCITEM));
	if( Image>= 0 )
		item.mask |= TCIF_IMAGE;
	if( ItemName )
		item.mask |= TCIF_TEXT;
	item.iImage    = Image;
	item.pszText   = (LPSTR)ItemName;

	UINT n=GetItemCount();
	n=InsertItem(n,&item);
	(*temp)->m_Pos=n;

	SetCurSel(n);
	SendMessage( WM_SIZE );

	return n;
}

BOOL CEditPan::DeleteFormList(UINT Num)
{
	CFormViewContainer** temp=&m_ViewCont;
	int n=0;
	while(*temp)
	{
		(*temp)->m_Pos-=n;
		if((*temp)->m_Pos==Num&&!n)
		{
			CFormViewContainer* del=*temp;
			*temp=(*temp)->m_Next;
			DeleteItem(del->m_Pos);
			delete del;
			n=1;
		}
		else
			temp=&((*temp)->m_Next);
	}

	return 0;
}

BOOL CEditPan::DeleteAll()
{
	CFormViewContainer* temp=m_ViewCont;
	while(temp)
	{
		CFormViewContainer* del=temp;
		temp=temp->m_Next;
		delete del;
	}
	DeleteAllItems();

	return 0;
}

void CEditPan::OnSize(UINT nType, int cx, int cy) 
{
	CTabCtrl::OnSize(nType, cx, cy);
	
	CRect rect;
	if(GetSafeHwnd())
	{
		GetClientRect(&rect);
		CFormViewContainer* temp=m_ViewCont;
		while(temp)
		{
			temp->m_View->SetWindowPos(NULL,5,25,rect.Width()-10,rect.Height()-30,SWP_NOZORDER);
			temp=temp->m_Next;
		}
	}
	
}

BOOL CEditPan::ShowTab(UINT Num)
{
 //   if ( Num == 0 )
	//{
	//	if (  )
	//	CTileEditApp::GetInstance()->GetOperator()->SetEditState(COperator::EES_NONE);
	//}
	//

	//if ( Num == 1 )
	//{
	//	CTileEditApp::GetInstance()->GetOperator()->SetEditState(COperator::EES_SET_TILE_FACE);
	//}
	
	//
	//if ( Num == 2 )
	//{
	//	CTileEditApp::GetInstance()->GetOperator()->SetEditState(COperator::EES_SET_PARAMS);
	//}

	SetCurSel( Num );

	CFormViewContainer * temp = m_ViewCont;

	while(temp)
	{
		if((temp)->m_Pos==Num)
		{			
			temp->m_View->ShowWindow(SW_SHOW);
		}
		else
			temp->m_View->ShowWindow(SW_HIDE);
		temp=temp->m_Next;
	}

	return 0;
}

void CEditPan::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
   	ShowTab(GetCurSel());
	*pResult = 0;
}
