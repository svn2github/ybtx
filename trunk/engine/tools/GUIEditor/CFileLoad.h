#pragma once


// CFileLoad

class CFileLoad : public ::CButton
{
	DECLARE_DYNAMIC(CFileLoad)

	CString m_szFilter;
	bool	m_beTex;

public:
	CFileLoad();
	virtual ~CFileLoad();
	BOOL Create( DWORD dwStyle, const RECT& rect, ::CWnd* pParentWnd, UINT nID, bool beTex = false );

	void SetFilter( const CString& szFilter );

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClicked();
};
