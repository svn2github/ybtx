#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "MyEdit.h"
#include "D3dd.h"
#include "Texture.h"
// AutoBatDlg dialog
#define WM_DROP_MESSAGE (WM_USER+100)
#define WM_DROP_MESSAGE1 (WM_USER+101)
class CAutoBatDlg : public CDialog
{
	DECLARE_DYNAMIC(CAutoBatDlg)

public:
	CAutoBatDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAutoBatDlg();

// Dialog Data
	enum { IDD = IDD_AUTOBAT };
	WORD m_IDAlpha;
	WORD m_IDNoAlpha;
	BOOL m_bMipMap;

	CD3d* m_D3d;

	BOOL  m_IsCancel;
	CString m_CurPath;
	
	CTexture::ESaveType	m_SaveType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAutoBatDlg)
	afx_msg LRESULT OnDropMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDropMessage1(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnFileAutobat();
	afx_msg void OnBnClickedAtargb32();
	afx_msg void OnBnClickedAtdx5();
	afx_msg void OnBnClickedAtdxt1();
	afx_msg void OnBnClickedAtrgb24();
	CStatic m_FileSaveProc;
	CProgressCtrl m_Progress;

protected:
	virtual void OnOK();
public:
	virtual INT_PTR DoModal(CD3d* d3d);
	int     Run();
	CMyEdit m_PicPath;
	CMyEdit m_SavePath;
	afx_msg void OnBnClickedPicbrown();
	afx_msg void OnBnClickedSavebrown();
	
protected:
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedEnablemip();
	afx_msg void OnBnClickedAtr5g6b5();
	afx_msg void OnBnClickedAta4r4g4b4();
	CComboBox m_comboSaveType;

public:
	BOOL	OnInitDialog();
	afx_msg void OnCbnSelchangeCombosavetype();
	afx_msg void OnBnClickedOk();
};
