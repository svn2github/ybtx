// NewMapDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "NewMapDialog.h"
#include ".\newmapdialog.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CNewMapDialog 对话框

IMPLEMENT_DYNAMIC(CNewMapDialog, CDialog)
CNewMapDialog::CNewMapDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNewMapDialog::IDD, pParent)
	, m_dwWidth(4)
	, m_dwDepth(4)
{
}

CNewMapDialog::~CNewMapDialog()
{
}

void CNewMapDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, m_dwWidth);
	DDX_Text(pDX, IDC_EDIT2, m_dwDepth);
}

BEGIN_MESSAGE_MAP(CNewMapDialog, CDialog)
	ON_BN_CLICKED(IDOK2, OnBnClickedOk2)
END_MESSAGE_MAP()


// CNewMapDialog 消息处理程序

void CNewMapDialog::OnBnClickedOk2()
{
	UpdateData();
	((CMainFrame*)(AfxGetApp()->GetMainWnd()))->InitMapEditor(m_dwWidth,m_dwDepth);
	CDialog::OnOK();
}
