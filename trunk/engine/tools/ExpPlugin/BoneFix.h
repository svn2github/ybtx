#pragma once
#include "afxwin.h"
#include "resource.h"
#include <map>
#include <string>

// CBoneFix dialog

extern std::map< string, string >	BoneFixMap;
extern bool							HaveBoneFix;

class CBoneFix : public CDialog
{
	DECLARE_DYNAMIC(CBoneFix)

public:
	CBoneFix(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBoneFix();

// Dialog Data
	enum { IDD = IDD_BONEFIX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_BoneRadio1;
	CButton m_BoneRadio2;
	CButton m_BoneCheck;
	afx_msg void OnBnClickedBoneRadio1();
	afx_msg void OnBnClickedBoneRadio2();
	afx_msg void OnBnClickedBoneCheck3();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
