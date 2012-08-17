#pragma once
#include "afxwin.h"
#include "resource.h"

class CSkeInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CSkeInfoDlg)
public:
	CString					m_SkeName;
public:
	CSkeInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSkeInfoDlg();

	// Dialog Data
	enum { IDD = IDD_SKEINFODLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	void DoSkeModal(CString& str);
	afx_msg void OnBnClickedCancel();
};