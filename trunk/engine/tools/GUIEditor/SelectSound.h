#pragma once
#include "afxcmn.h"


// CSelectSound dialog

class CSelectSound : public ::CDialog
{
	DECLARE_DYNAMIC(CSelectSound)
	bool m_bCancle;
public:
	CSelectSound(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectSound();

// Dialog Data
	enum { IDD = IDD_SELECT_SOUND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
public:
	::CTreeCtrl m_treeCues;
	wstring m_strLastCueName;
	wstring m_strCurCueName;
	wstring m_strOriginCue;
	afx_msg void OnTvnSelchangedTreeCues(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	wstring  GetSelectSoundName() const;
};
