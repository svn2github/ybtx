#pragma once



// CPlaceDataSheet

class CPlaceDataSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPlaceDataSheet)

public:
	CPlaceDataSheet();
	CPlaceDataSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPlaceDataSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CPlaceDataSheet();
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
};


