// NotBase64CoderDlg.h : header file
//

#pragma once


// CNotBase64CoderDlg dialog
class CNotBase64CoderDlg : public CDialog
{
// Construction
public:
	CNotBase64CoderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_NotBase64Coder_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	void ConvTextToBase64();
	void ConvBase64ToText();

	bool m_bConverting;
public:
	afx_msg void OnEnChangeTomem();
	afx_msg void OnOK();
	afx_msg void OnBnClickedBtnB2tconv();
	afx_msg void OnEnChangeDisp();
	afx_msg void OnEnSetfocusDisp();
	afx_msg void OnEnSetfocusTomem();
};
