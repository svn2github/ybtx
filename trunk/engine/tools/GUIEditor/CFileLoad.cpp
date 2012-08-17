// CFileLoad.cpp : implementation file
//

#include "stdafx.h"
#include "GUIEditor.h"
#include "CFileLoad.h"
#include ".\cfileload.h"
#include "GUIEditorHelper.h"


// CFileLoad

IMPLEMENT_DYNAMIC(CFileLoad, CButton)
CFileLoad::CFileLoad()
{
}

CFileLoad::~CFileLoad()
{
}


BEGIN_MESSAGE_MAP(CFileLoad, CButton)
	ON_CONTROL_REFLECT(BN_CLICKED, OnBnClicked)
END_MESSAGE_MAP()


// CFileLoad uMessage handlers

BOOL CFileLoad::Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, bool beTex )
{
	BOOL re = ::CButton::Create( L"", BS_FLAT|BS_RIGHT, rect, pParentWnd, nID );

	m_beTex = beTex;
	ShowWindow( SW_SHOW );
	return re;
}

void CFileLoad::SetFilter( const CString& szFilter )
{
	m_szFilter = szFilter;
}

void CFileLoad::OnBnClicked()
{
	// TODO: Add your control notification handler code here
	vector<CString> vecFiles;
	if ( m_beTex )
	{
		vecFiles = GetMutiFileNameTex( m_szFilter );
	}
	else
	{
		vecFiles = GetMutiFileName( m_szFilter );
	}
	
	if( vecFiles.size() && !vecFiles[0].IsEmpty() )
	{
		SetWindowText( vecFiles[0] );
		WPARAM wParam = GetDlgCtrlID() | 0x2000000;
		LPARAM lParam = (LPARAM)GetSafeHwnd();
		GetParent()->SendMessage( WM_COMMAND, wParam, lParam );
	}
}
