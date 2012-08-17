// WndStyle.cpp : implementation file
//

#include "stdafx.h"
#include "GUIEditor.h"
#include "WndStyle.h"


// CWndStyle

IMPLEMENT_DYNCREATE(CWndStyle, CFormView)

CWndStyle::CWndStyle()
	: CFormView(CWndStyle::IDD)
	, m_pMainDlg( NULL )
	, m_pWndGroup( NULL )
{

}

CWndStyle::~CWndStyle()
{
}

void CWndStyle::Init(CGUIEditorDlg *pMain, CWndGroup* pGroup, CWndProp* m_pProp)
{
	m_pMainDlg  = pMain;
	m_pWndGroup = pGroup; 
	m_pWndProp  = m_pProp;
}

BOOL CWndStyle::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	BOOL re = CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	UpdateData( FALSE );

	return re;
}

BOOL CWndStyle::DestroyWindow()
{
	return CFormView::DestroyWindow();
}

void CWndStyle::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWndStyle, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CWndStyle::OnBnClickedButton1)
END_MESSAGE_MAP()

void CWndStyle::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	SQRWnd* pCurEdit = m_pWndGroup->GetEditWnd();
	uint32 value = 0x00000000;
	if( TRUE == ((::CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() )
	{
		value |= 0x40000000;
	}
	if( TRUE == ((::CButton*)GetDlgItem(IDC_CHECK2))->GetCheck() )
	{
		value |= 0x20000000;
	}
	if( TRUE == ((::CButton*)GetDlgItem(IDC_CHECK3))->GetCheck() )
	{
		value |= 0x00100000;
	}
	if( TRUE == ((::CButton*)GetDlgItem(IDC_CHECK4))->GetCheck() )
	{
		value |= 0x08000000;
	}
	if( TRUE == ((::CButton*)GetDlgItem(IDC_CHECK5))->GetCheck() )
	{
		value |= 0x04000000;
	}
	if( TRUE == ((::CButton*)GetDlgItem(IDC_CHECK6))->GetCheck() )
	{
		value |= 0x02000000L;
	}
	if( TRUE == ((::CButton*)GetDlgItem(IDC_CHECK7))->GetCheck() )
	{
		value |= 0x01000000;
	}
	if( TRUE == ((::CButton*)GetDlgItem(IDC_CHECK8))->GetCheck() )
	{
		value |= 0x00080000;
	}
	if( TRUE == ((::CButton*)GetDlgItem(IDC_CHECK9))->GetCheck() )
	{
		value |= 0x00400000;
	}
	if( TRUE == ((::CButton*)GetDlgItem(IDC_CHECK10))->GetCheck() )
	{
		value |= 0x00200000;
	}
	if( TRUE == ((::CButton*)GetDlgItem(IDC_CHECK11))->GetCheck() )
	{
		value |= 0x80000000;
	}
	if( TRUE == ((::CButton*)GetDlgItem(IDC_CHECK12))->GetCheck() )
	{
		value |= 0x00001000;
	}
	//if( TRUE == ((::CButton*)GetDlgItem(IDC_CHECK13))->GetCheck() )
	//{
	//	value |= 0x00000100;
	//}
	//if( TRUE == ((::CButton*)GetDlgItem(IDC_CHECK14))->GetCheck() )
	//{
	//	value |= 0x00001000;
	//}
	if( NULL != pCurEdit )
	{
		pCurEdit->SetStyle( value );
	}
	m_pWndProp->m_PropList.SetToItem();
	m_pMainDlg->m_EditTab.ShowTab(1);
}

void CWndStyle::Clear()
{
	((::CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(FALSE);
	((::CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(FALSE);
	((::CButton*)GetDlgItem(IDC_CHECK3))->SetCheck(FALSE);
	((::CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(FALSE);
	((::CButton*)GetDlgItem(IDC_CHECK5))->SetCheck(FALSE);
	((::CButton*)GetDlgItem(IDC_CHECK6))->SetCheck(FALSE);
	((::CButton*)GetDlgItem(IDC_CHECK7))->SetCheck(FALSE);
	((::CButton*)GetDlgItem(IDC_CHECK8))->SetCheck(FALSE);
	((::CButton*)GetDlgItem(IDC_CHECK9))->SetCheck(FALSE);
	((::CButton*)GetDlgItem(IDC_CHECK10))->SetCheck(FALSE);
	((::CButton*)GetDlgItem(IDC_CHECK11))->SetCheck(FALSE);
	((::CButton*)GetDlgItem(IDC_CHECK12))->SetCheck(FALSE);
	//((::CButton*)GetDlgItem(IDC_CHECK13))->SetCheck(FALSE);
	//((::CButton*)GetDlgItem(IDC_CHECK14))->SetCheck(FALSE);
}

void CWndStyle::Update( uint32 nFlag )
{
	if( nFlag & EDIT_POSITION )
	{
		Clear();
		SQRWnd* pCurEdit = m_pWndGroup->GetEditWnd();
		if( !pCurEdit )
			return;
		uint32 value = pCurEdit->GetStyle();
			
		if( value & 0x40000000 )
		{
			((::CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(TRUE);
		}
		if( value & 0x20000000 )
		{
			((::CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(TRUE);
		}
		if( value & 0x00100000 )
		{
			((::CButton*)GetDlgItem(IDC_CHECK3))->SetCheck(TRUE);
		}
		if( value & 0x08000000 )
		{
			((::CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(TRUE);
		}
		if( value & 0x04000000 )
		{
			((::CButton*)GetDlgItem(IDC_CHECK5))->SetCheck(TRUE);
		}
		if( value & 0x02000000L )
		{
			((::CButton*)GetDlgItem(IDC_CHECK6))->SetCheck(TRUE);
		}
		if( value & 0x01000000 )
		{
			((::CButton*)GetDlgItem(IDC_CHECK7))->SetCheck(TRUE);
		}
		if( value & 0x00080000 )
		{
			((::CButton*)GetDlgItem(IDC_CHECK8))->SetCheck(TRUE);
		}
		if( value & 0x00400000 )
		{
			((::CButton*)GetDlgItem(IDC_CHECK9))->SetCheck(TRUE);
		}
		if( value & 0x00200000 )
		{
			((::CButton*)GetDlgItem(IDC_CHECK10))->SetCheck(TRUE);
		}
		if( value & 0x80000000 )
		{
			((::CButton*)GetDlgItem(IDC_CHECK11))->SetCheck(TRUE);
		}
		if( value & 0x00001000 )
		{
			((::CButton*)GetDlgItem(IDC_CHECK12))->SetCheck(TRUE);
		}
		//if( value & 0x00000100 )
		//{
		//	((::CButton*)GetDlgItem(IDC_CHECK13))->SetCheck(TRUE);
		//}
		//if( value & 0x00001000 )
		//{
		//	((::CButton*)GetDlgItem(IDC_CHECK14))->SetCheck(TRUE);
		//}
	}
}
