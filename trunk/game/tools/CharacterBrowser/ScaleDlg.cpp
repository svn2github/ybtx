// ScaleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScaleDlg.h"


// CScaleDlg dialog

IMPLEMENT_DYNAMIC(CScaleDlg, CDialog)

CScaleDlg::CScaleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScaleDlg::IDD, pParent)
	, m_fScale(0)
{

}

CScaleDlg::~CScaleDlg()
{
}

void CScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_fScale);
}


BEGIN_MESSAGE_MAP(CScaleDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CScaleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CScaleDlg message handlers

void CScaleDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	OnOK();
}
