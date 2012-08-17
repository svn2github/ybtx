#if !defined(AFX_EDITPAN_H__BB1E7E3A_DF48_4A47_8163_7E5DA30A5B1E__INCLUDED_)
#define AFX_EDITPAN_H__BB1E7E3A_DF48_4A47_8163_7E5DA30A5B1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditPan.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditPan window

struct CFormViewContainer
{
	CFormViewContainer()
	{
		m_View=NULL;
		m_Next=NULL;
	}
	UINT                m_Pos;
	CView*              m_View;
	CFormViewContainer* m_Next;
};

class CEditPan : public CTabCtrl
{
// Construction
public:
	CEditPan();
	UINT AddToList( CView* view, int Image, LPCTSTR ItemName );
	BOOL DeleteFormList(UINT Num);
	BOOL DeleteAll();
	BOOL ShowTab(UINT Num);

private:	
	CFont               m_Font;
	CFormViewContainer* m_ViewCont;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditPan)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditPan();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditPan)
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITPAN_H__BB1E7E3A_DF48_4A47_8163_7E5DA30A5B1E__INCLUDED_)
