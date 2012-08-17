#pragma once
#include "afxwin.h"
#include "Resource.h"


// CElementTabPage dialog

enum ETabPageType
{
	eTPT_NPC,
	eTPT_OBJ,
	eTPT_TRAP,
	eTPT_COLONY,
	
	eTPT_COUNT
};




class CElementTabPage : public CDialog
{
	DECLARE_DYNAMIC(CElementTabPage)

public:
	CElementTabPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CElementTabPage();

// Dialog Data
	enum { IDD = IDD_ELEMENT_TAB };
	void SetType(ETabPageType eType);
	void AddElement(CElement* pElement);
	void DelElement(CElement* pElement);
	void UpdateElement(CElement* pElement);
	void SetCount(int count);
	void ClearElement();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedFind();
private:
	CListBox m_listboxElement;
	ETabPageType	m_eType;
public:
	afx_msg void OnLbnDblclkElementList();
protected:
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedNewElement();
	afx_msg void OnBnClickedEditElement();
	afx_msg void OnBnClickedDelElement();
};
