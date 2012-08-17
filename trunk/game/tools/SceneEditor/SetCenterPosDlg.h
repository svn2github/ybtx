#pragma once
#include "resource.h"
#include "DataType.h"

// CSetCenterPosDlg dialog

class CSetCenterPosDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetCenterPosDlg)

public:
	CSetCenterPosDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetCenterPosDlg();

// Dialog Data
	enum { IDD = IDD_SET_CENTER_POS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

	FPos GetCenterPos(){return m_CenterPos;};
private:
	FPos m_CenterPos;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton2();
	void UpdatePos();
};
