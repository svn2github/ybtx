#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CAreaEditDlg dialog

class CAreaEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CAreaEditDlg)

public:
	CAreaEditDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAreaEditDlg();

// Dialog Data
	enum { IDD = IDD_AREA_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CComboBox			m_comboMusic;
	CEdit				m_editAreaName;
	CListCtrl			m_listCtrlAreaState;
	CComboBox m_comboxIsFb;
	CEdit m_editMaxPlayerNum;
	CEdit m_editRatingPlayerNum;
public:
	afx_msg void OnLvnItemchangedAreaState(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedModify();
	afx_msg void OnBnClickedDeleteArea();
	afx_msg void OnCbnSelchangeIsFb();

};
