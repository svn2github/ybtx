// PathLogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ExpPlugin.h"
#include "PathLogDlg.h"


// CPathLogDlg dialog

IMPLEMENT_DYNAMIC(CPathLogDlg, CDialog)

CPathLogDlg::CPathLogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPathLogDlg::IDD, pParent)
{

}

CPathLogDlg::~CPathLogDlg()
{
}

void CPathLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PATHLOGLIST, m_PathListBox);
}


BEGIN_MESSAGE_MAP(CPathLogDlg, CDialog)
END_MESSAGE_MAP()


// CPathLogDlg message handlers

void CPathLogDlg::MyDoModal()
{
	if(CDialog::DoModal()==IDOK)
	{
		CString err;
		for (int i = 0;i<m_PathList.size();i++)
		{
			string s = m_PathList[i];
			if(s.find(":")!=string::npos)
			{
				err+=s.c_str();
				err+= "\n";
			}
		}
		if(err!="")
		{
			err += "这些纹理有绝对路径,存在问题请检查!";
			MessageBox(err);
		}
	}
}

BOOL CPathLogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText("自动匹配上的纹理有");
	for(int i =0;i<m_PathList.size();i++)
	{
		CString s = m_PathList[i].c_str();
		m_PathListBox.AddString(s);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CPathLogDlg::AddPath( const string& path )
{	
	m_PathList.push_back(path);
}