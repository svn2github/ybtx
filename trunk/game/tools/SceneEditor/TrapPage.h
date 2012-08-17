#pragma once
#include "afxcmn.h"


// CTrapPage dialog

class CTrapPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CTrapPage)

public:
	CTrapPage();
	virtual ~CTrapPage();

// Dialog Data
	enum { IDD = IDD_TRAP_PAGE };


	bool Init();
	bool loadData();
	PPlaceTrap GetSelTrap();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int			m_iCount;
	CListCtrl	m_listCtrlTrap;
	PlaceTrap	m_selectTrap;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSearch();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
