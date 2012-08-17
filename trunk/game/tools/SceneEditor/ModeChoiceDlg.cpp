// ModeChoiceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptMgr.h"
#include "NewSceneEditor.h"
#include "ModeChoiceDlg.h"


// CModeChoiceDlg dialog

IMPLEMENT_DYNAMIC(CModeChoiceDlg, CDialog)

CModeChoiceDlg::CModeChoiceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModeChoiceDlg::IDD, pParent), m_eMode(eMT_NORMAL)
{

}

CModeChoiceDlg::~CModeChoiceDlg()
{
}

void CModeChoiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CModeChoiceDlg, CDialog)
	ON_BN_CLICKED(IDC_NORMAL, &CModeChoiceDlg::OnBnClickedNormal)
	ON_BN_CLICKED(IDC_AREA_FB, &CModeChoiceDlg::OnBnClickedAreaFb)
	ON_BN_CLICKED(IDC_DYNAMIC, &CModeChoiceDlg::OnBnClickedDynamic)
	ON_BN_CLICKED(IDC_PATH_EDIT, &CModeChoiceDlg::OnBnClickedPathEdit)
END_MESSAGE_MAP()


// CModeChoiceDlg message handlers

void CModeChoiceDlg::OnBnClickedNormal()
{
	m_eMode = eMT_NORMAL;
	OnOK();
}

void CModeChoiceDlg::OnBnClickedAreaFb()
{
	m_eMode = eMT_AREA_FB;
	OnOK();
}

void CModeChoiceDlg::OnBnClickedDynamic()
{
	m_eMode = eMT_DYNAMIC;
	OnOK();
}

void CModeChoiceDlg::OnBnClickedPathEdit()
{
	m_eMode = eMT_PATH_EDIT;
	OnOK();
}
