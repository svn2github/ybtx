#pragma once
#include "afxwin.h"

// CNpcProPertiesDlg dialog

class CNpcProPertiesDlg : public CDialog
{
	DECLARE_DYNAMIC(CNpcProPertiesDlg)

public:
	CNpcProPertiesDlg(PPlaceNpc pNpc,const CPoint& dlgPos,CWnd* pParent = NULL);   // standard constructor
	virtual ~CNpcProPertiesDlg();

// Dialog Data
	enum { IDD = IDD_NPC_RDLG };

	void UpdateAIComboBox(CString sAIType);
	void SetAi(CString sAIType);
	void UpdateMoveComboBox(CString sAIType);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	PPlaceNpc m_pNpc;
	CPoint m_dlgPos;
public:
	CComboBox m_ComboAISet;
	afx_msg void OnCbnSelchangeCombo3();
	CComboBox m_ComboMoveType;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	CComboBox m_ComboMoveState;
	CComboBox m_ComboColonyType;
	CComboBox m_ComboIdleAct;
	afx_msg void OnBnClickedIgnoreClog();
};
