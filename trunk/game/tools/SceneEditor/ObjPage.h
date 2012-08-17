#pragma once
#include "afxcmn.h"


// CObjPage dialog

class CObjPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CObjPage)

public:
	CObjPage();
	virtual ~CObjPage();

// Dialog Data
	enum { IDD = IDD_OBJ_PAGE };

	bool Init();
	bool loadData();
	PPlaceObj GetSelObj();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int			m_iCount;
	CListCtrl	m_listCtrlObj;
	PlaceObj	m_selectObj;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSearch();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
