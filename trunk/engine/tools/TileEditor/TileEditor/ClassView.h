#pragma once
#include "afxcmn.h"
#include "classmgr.h"

// CClassView 窗体视图

class CClassView : public CFormView
{
	CImageList m_Image;

public:
	CClassView();   
	virtual ~CClassView();

public:
	enum { IDD = IDD_CLASS };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void PostNcDestroy() { }

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_classTree;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

private:
	CClassMgr m_server;
	CClassMgr m_client;
	HTREEITEM m_hServer;
	HTREEITEM m_hClient;

	void ResetServer(HTREEITEM hSetItem);
	void ResetClient(HTREEITEM hSetItem);
	void AddObject(HTREEITEM hSetItem, void * p);

public:
	afx_msg void OnTvnSelchangedClassview(NMHDR *pNMHDR, LRESULT *pResult);
	
	//void InvalidClassTree()
	//{
	//	m_classTree.Select(0,TVGN_CARET);
	//}
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void SaveMeAsServer(FILE * fp, int nRgnIndex);
	void SaveMeAsClient(FILE * fp, int nRgnIndex);
	void SaveMeAsLocal(FILE * fp);
};


