// ObjTrapPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NewSceneEditor.h"
#include "ObjTrapPropertiesDlg.h"


// CObjTrapPropertiesDlg dialog

IMPLEMENT_DYNAMIC(CObjTrapPropertiesDlg, CDialog)

CObjTrapPropertiesDlg::CObjTrapPropertiesDlg(const CString& type,const CString& name, const FPos& fPos, const CPoint& dlgPos,CWnd* pParent /*=NULL*/)
	: CDialog(CObjTrapPropertiesDlg::IDD, pParent) ,m_sType(type),m_sName(name),m_fPos(fPos), m_dlgPos(dlgPos)
{

}

CObjTrapPropertiesDlg::~CObjTrapPropertiesDlg()
{
}

void CObjTrapPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CObjTrapPropertiesDlg, CDialog)
END_MESSAGE_MAP()


// CObjTrapPropertiesDlg message handlers

BOOL CObjTrapPropertiesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	GetDlgItem(IDC_NAME)->SetWindowText(m_sName);
	CString pos;
	pos.Format("%.2f", m_fPos.x);
	GetDlgItem(IDC_GRID_X)->SetWindowText(pos);
	pos.Format("%.2f", m_fPos.y);
	GetDlgItem(IDC_GRID_Y)->SetWindowText(pos);
	SetWindowText(m_sType + "  Ù–‘");
	CRect rc;
	GetWindowRect(&rc);
	MoveWindow(m_dlgPos.x,m_dlgPos.y - (rc.bottom - rc.top)/2, rc.right - rc.left, rc.bottom - rc.top);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
