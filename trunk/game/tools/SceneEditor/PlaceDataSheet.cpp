// PlaceDataSheet.cpp : implementation file
//

#include "stdafx.h"
#include "NewSceneEditor.h"
#include "PlaceDataSheet.h"


// CPlaceDataSheet

IMPLEMENT_DYNAMIC(CPlaceDataSheet, CPropertySheet)


CPlaceDataSheet::CPlaceDataSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CPlaceDataSheet::CPlaceDataSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

}

CPlaceDataSheet::~CPlaceDataSheet()
{
}


BEGIN_MESSAGE_MAP(CPlaceDataSheet, CPropertySheet)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CPlaceDataSheet message handlers

void CPlaceDataSheet::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	//CPropertySheet::OnClose();
	ShowWindow(false);
}
