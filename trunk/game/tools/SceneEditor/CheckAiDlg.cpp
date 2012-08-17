// CheckAiDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptMgr.h"
#include "NewSceneEditor.h"
#include "CheckAiDlg.h"


// CCheckAiDlg dialog

IMPLEMENT_DYNAMIC(CCheckAiDlg, CDialog)

CCheckAiDlg::CCheckAiDlg(const CString& name, const CString& cfgAI, const CString& placeAi, CWnd* pParent /*=NULL*/)
	: CDialog(CCheckAiDlg::IDD, pParent),m_strName(name),m_strCfgAI(cfgAI),m_strPlaceAi(placeAi)
	, m_bNoCheck(FALSE)
{
	
}

CCheckAiDlg::~CCheckAiDlg()
{
}

void CCheckAiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_NO_CHECK, m_bNoCheck);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Text(pDX, IDC_CFG_AI, m_strCfgAI);
	DDX_Text(pDX, IDC_PLACE_AI, m_strPlaceAi);
}


BEGIN_MESSAGE_MAP(CCheckAiDlg, CDialog)
	ON_BN_CLICKED(IDC_DEFAULT_AI, &CCheckAiDlg::OnBnClickedDefaultAi)
	ON_BN_CLICKED(IDC_SKIP, &CCheckAiDlg::OnBnClickedSkip)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CCheckAiDlg message handlers

void CCheckAiDlg::OnBnClickedDefaultAi()
{
	UpdateData();
	OnOK();
	// TODO: Add your control notification handler code here
}

void CCheckAiDlg::OnBnClickedSkip()
{
	UpdateData();
	OnCancel();
	// TODO: Add your control notification handler code here
}

void CCheckAiDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	UpdateData();
	CDialog::OnClose();
}

BOOL CCheckAiDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
