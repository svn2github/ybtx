#pragma once

// CScaleDlg dialog
#include "Resource.h"

class CScaleDlg : public ::CDialog
{
	DECLARE_DYNAMIC(CScaleDlg)

public:
	CScaleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CScaleDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_fScale;
	afx_msg void OnBnClickedOk();
};
