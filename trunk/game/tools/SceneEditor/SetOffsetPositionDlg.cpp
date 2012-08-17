// SetOffsetPositionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptMgr.h"
#include "SetOffsetPositionDlg.h"


// CSetOffsetPositionDlg dialog

IMPLEMENT_DYNAMIC(CSetOffsetPositionDlg, CDialog)

CSetOffsetPositionDlg::CSetOffsetPositionDlg(float x, float y, CWnd* pParent /*=NULL*/)
	: CDialog(CSetOffsetPositionDlg::IDD, pParent)
	, m_fOffsetX(x)
	, m_fOffsetY(y)
{

}

CSetOffsetPositionDlg::~CSetOffsetPositionDlg()
{
}

void CSetOffsetPositionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetOffsetPositionDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CSetOffsetPositionDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CSetOffsetPositionDlg message handlers

void CSetOffsetPositionDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	CString s;
	GetDlgItem(IDC_X)->GetWindowText(s);
	if(s != "")
	{
		m_fOffsetX = (float)atof(s.GetBuffer());
	}
	else
	{
		MessageBox("X 不能为空");
		return;
	}

	GetDlgItem(IDC_Y)->GetWindowText(s);
	if(s != "")
	{
		m_fOffsetY = (float)atof(s.GetBuffer());
	}
	else
	{
		MessageBox("Y 不能为空");
		return;
	}
	OnOK();
}

BOOL CSetOffsetPositionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	UpdatePos();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetOffsetPositionDlg::UpdatePos()
{
	CString s;
	FPos pos = g_pCache->GetOffsetPos();
	s.Format("%0.2f", pos.x);
	GetDlgItem(IDC_X)->SetWindowText(s);
	s.Format("%0.2f", pos.y);
	GetDlgItem(IDC_Y)->SetWindowText(s);
}

void CSetOffsetPositionDlg::GetOffsetPos(FPos & pos)
{
	pos.x = m_fOffsetX;
	pos.y = m_fOffsetY;
}
