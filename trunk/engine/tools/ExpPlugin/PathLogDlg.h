#pragma once
#include "afxwin.h"


// CPathLogDlg dialog

class CPathLogDlg : public CDialog
{
	DECLARE_DYNAMIC(CPathLogDlg)

public:
	CPathLogDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPathLogDlg();

// Dialog Data
	enum { IDD = IDD_PATHLOGDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_PathListBox;
	vector<string> m_PathList;
	virtual BOOL OnInitDialog();
	void	MyDoModal();
	void	AddPath( const string& path );
};
