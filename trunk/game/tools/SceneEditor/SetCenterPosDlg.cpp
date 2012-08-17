// SetCenterPosDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptMgr.h"
#include "SetCenterPosDlg.h"


// CSetCenterPosDlg dialog

IMPLEMENT_DYNAMIC(CSetCenterPosDlg, CDialog)

CSetCenterPosDlg::CSetCenterPosDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetCenterPosDlg::IDD, pParent)
{

}

CSetCenterPosDlg::~CSetCenterPosDlg()
{
}

void CSetCenterPosDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetCenterPosDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CSetCenterPosDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CSetCenterPosDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CSetCenterPosDlg message handlers

void CSetCenterPosDlg::OnBnClickedButton1()
{
	CString s;
	GetDlgItem(IDC_X)->GetWindowText(s);
	if(s != "")
	{
		m_CenterPos.x = (float)atof(s.GetBuffer());
	}
	else
	{
		MessageBox("X 不能为空");
		return;
	}

	GetDlgItem(IDC_Y)->GetWindowText(s);
	if(s != "")
	{
		m_CenterPos.y = (float)atof(s.GetBuffer());
	}
	else
	{
		MessageBox("Y 不能为空");
		return;
	}
	OnOK();
}

BOOL CSetCenterPosDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	UpdatePos();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetCenterPosDlg::UpdatePos()
{
	CString s;
	FPos pos = g_pCache->GetCenterPos();
	s.Format("%0.2f", pos.x);
	GetDlgItem(IDC_X)->SetWindowText(s);
	s.Format("%0.2f", pos.y);
	GetDlgItem(IDC_Y)->SetWindowText(s);
}

void CSetCenterPosDlg::OnBnClickedButton2()
{
	g_pCache->ComputeCenterPos();
	UpdatePos();
}
