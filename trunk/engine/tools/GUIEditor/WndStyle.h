#pragma once

#include "afxcmn.h"
#include "afxwin.h"

class CWnd;

// CWndStyle form view

class CWndStyle : public CFormView
{
	DECLARE_DYNCREATE(CWndStyle)
	CGUIEditorDlg*	m_pMainDlg;
	CWndProp*		m_pWndProp;
	CWndGroup*		m_pWndGroup;
	
public:
	enum { IDD = IDD_STYLE };

public:
	CWndStyle();           // protected constructor used by dynamic creation
	virtual ~CWndStyle();
	BOOL DestroyWindow();
	void Init(CGUIEditorDlg *pMain, CWndGroup* pGroup, CWndProp* m_pProp);

	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
		const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	void Update( uint32 nFlag );
		
protected:
	virtual void PostNcDestroy(){};
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void		 Clear();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};


