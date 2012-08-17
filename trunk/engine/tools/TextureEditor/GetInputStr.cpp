// GetInputStr.cpp : implementation file
//

#include "stdafx.h"
#include "TextureEditor.h"
#include "GetInputStr.h"
#include ".\getinputstr.h"


// CGetInputStr dialog

IMPLEMENT_DYNAMIC(CGetInputStr, CDialog)
CGetInputStr::CGetInputStr(CWnd* pParent /*=NULL*/)
	: CDialog(CGetInputStr::IDD, pParent)
	, m_String(_T(""))
{
}

CGetInputStr::~CGetInputStr()
{
}

void CGetInputStr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_String);
	SetWindowText( m_Title );
}


BEGIN_MESSAGE_MAP(CGetInputStr, CDialog)
END_MESSAGE_MAP()


// CGetInputStr message handlers

CString& CGetInputStr::DoModal( CString Title, CString Init )
{
	m_Title  = Title;
	m_String = Init;
	// TODO: Add your specialized code here and/or call the base class
	if( CDialog::DoModal() == IDCANCEL )
		m_String.Empty();

	return m_String;
}

void CGetInputStr::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData();
	CDialog::OnOK();
}
