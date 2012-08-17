#include "stdafx.h"
#include "Exp.h"
#include "ExpPlugin.h"
#include "CSkeInfoDlg.h"

// CBoneFix dialog

IMPLEMENT_DYNAMIC(CSkeInfoDlg, CDialog)
CSkeInfoDlg::CSkeInfoDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSkeInfoDlg::IDD, pParent)
, m_SkeName(_T(""))
{
}

CSkeInfoDlg::~CSkeInfoDlg()
{
}

void CSkeInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SKENAMEEDIT, m_SkeName);
}


BEGIN_MESSAGE_MAP(CSkeInfoDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSkeInfoDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

BOOL CSkeInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem( IDC_SKENAMEEDIT )->SetWindowText( m_SkeName );
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSkeInfoDlg::DoSkeModal(CString& str)
{
	m_SkeName = str;
	this->DoModal();
}

void CSkeInfoDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_SkeName==""||m_SkeName.Find(" ")!=-1)
	{
		MessageBox("请填写骨架ske名字或者名字中有空格","提示",MB_OK);
		return;
	}
	OnOK();
}
void CSkeInfoDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//OnCancel();
}