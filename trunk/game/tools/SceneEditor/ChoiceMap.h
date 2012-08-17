#pragma once
#include "afxwin.h"


// CChoiceMap dialog

class CChoiceMap : public CDialog
{
	DECLARE_DYNAMIC(CChoiceMap)

public:
	CChoiceMap(CWnd* pParent = NULL);   // standard constructor
	virtual ~CChoiceMap();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	bool IsAutoOpenFile();
	bool IsAutoOpenAreaFile();
	CString GetSceneName();
private:
	CString  m_strSceneName;
	bool m_bIsAutoOpenScene;
	bool m_bIsAutoOpenArea;
public:
	afx_msg void OnBnClickedButton1();
	CListBox m_listMapName;
	afx_msg void OnLbnDblclkMapList();
	afx_msg void OnBnClickedAutoOpenFile();
	afx_msg void OnBnClickedSearch();
};
