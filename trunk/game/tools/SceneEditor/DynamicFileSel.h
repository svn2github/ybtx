#pragma once
#include "afxwin.h"


// CDynamicFileSel dialog

class CDynamicFileSel : public CDialog
{
	DECLARE_DYNAMIC(CDynamicFileSel)

public:
	CDynamicFileSel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDynamicFileSel();

// Dialog Data
	enum { IDD = IDD_DYNAMIC_FILE_SEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listName;
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	CString GetSelName();
	afx_msg void OnBnClickedButton2();
	void UpdateNames();
private:
	CString m_strSelName;
public:
	afx_msg void OnLbnDblclkList3();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
//	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
};
