#pragma once
// CNpcSelectView form view

class CNpcSelectView : public CFormView
{
	DECLARE_DYNCREATE(CNpcSelectView)

public:
	CNpcSelectView();           // protected constructor used by dynamic creation
	virtual ~CNpcSelectView();

public:
	enum { IDD = IDD_NPC_TOOLS };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	CEdit				m_EditFind;
	CTreeCtrl			m_CharacterList;
	CTreeCtrl			m_IntObjList;
	HTREEITEM			m_LastChatSelectItem;
	HTREEITEM			m_LastObjSelectItem;
	BOOL				m_isInitialized;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			void DeleteChildrenItem(CTreeCtrl& tree, HTREEITEM item);
	DECLARE_MESSAGE_MAP()

public:
	virtual void PostNcDestroy(){};
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnTvnSelchangedTree2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedLoadtable();
	afx_msg void OnBnClickedFindNpc();
	afx_msg void OnCheckAnimate();
	void ShowCharacterList(bool isShow = true);
	void ShowIntObjList(bool isShow = true);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


