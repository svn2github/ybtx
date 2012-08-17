#pragma once

#include <string>


// CGameSet dialog
class CGameSet : public CDialog
{
	DECLARE_DYNAMIC(CGameSet)

public:
	CGameSet(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGameSet();

// Dialog Data
	enum { IDD = IDD_SET_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CComboBox* m_ResolveComb;
	CComboBox* m_ColorBitComb;
	CComboBox* m_ShowModeComb;
public:
	afx_msg void OnBnClickedOk();

private:
	std::wstring  m_strIniFileDir;
};
