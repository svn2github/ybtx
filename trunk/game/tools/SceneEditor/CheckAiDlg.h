#pragma once
#include "afxwin.h"
#include "Resource.h"


// CCheckAiDlg dialog

class CCheckAiDlg : public CDialog
{
	DECLARE_DYNAMIC(CCheckAiDlg)

public:
	CCheckAiDlg(const CString& name, const CString& cfgAI, const CString& placeAi, CWnd* pParent = NULL);   // standard constructor
	virtual ~CCheckAiDlg();

// Dialog Data
	enum { IDD = IDD_AI_CHECK_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bNoCheck;
	afx_msg void OnBnClickedDefaultAi();
	afx_msg void OnBnClickedSkip();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	CString m_strName;
	CString m_strCfgAI;
	CString m_strPlaceAi;
};
