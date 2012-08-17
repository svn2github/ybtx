#pragma once

#include "set.h"
#include "afxcmn.h"

// CResSetView form view

class CResSetView : public CFormView
{
//	DECLARE_DYNCREATE(CResSetView)
	CImageList m_Image;

	CImageList m_ImageEffect;
	std::vector< CResSet* >	m_vecFXSets;
	CResSet * m_curFXSet;
				
	std::vector< CResSet* >	m_resSets;
	CResSet * m_curSelResSet;
	
	CTreeCtrl m_SetView;
	CListCtrl m_listResSet;
	
	C3DResSetUnit * m_pActiveUnit;
	C2DResSetUnit * m_pActive2DUnit;
	
	//COperator::EEDIT_STATE m_eEditState;
	
	void ResetResSetView();
	void SetResSetToTree2D( HTREEITEM h2DSetItem );
	void SetResSetToTree3D( HTREEITEM h3DSetItem );
	
	void SetResSetUnitToTree3D( HTREEITEM hGroupItem );
	void SetResSetUnitToTree2D( HTREEITEM hGroupItem );
	void AddSingleUnitForSelectedGroup( TVITEM& TItem );
	void AddComboUnitForSelectedGroup( TVITEM& TItem );
	
	void AddResSetFromFile(std::string strResSetFileName);	

	
public:
	void ResetResSetList();		
	CResSetView();           // protected constructor used by dynamic creation
	virtual ~CResSetView();

	void LoadLog();	
public:
	enum { IDD = IDD_SCENARIO };

protected:
	virtual void PostNcDestroy() { }
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	void InvalidResTree();

	DECLARE_MESSAGE_MAP()

public:
	CTreeCtrl & GetResTree() 
	{
		return m_SetView;
	}

	afx_msg void OnBnClickedCreategroup();
	CString m_groupName;
	afx_msg void OnBnClickedCreateSingleUnit();
	afx_msg void OnBnClickedDeletegroup();
	afx_msg void OnBnClickedDeleteunit();
	afx_msg void OnTvnSelchangedSetview(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	CString m_resSetName;
	afx_msg void OnDestroy();
	afx_msg void OnTvnItemexpandedSetview(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedCreateComboUnit();
	afx_msg void OnNMClickSetview(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCenter();
	afx_msg void OnBnClickedCorner();
	afx_msg void OnBnClickedXMidWay();
//	afx_msg void OnBnHotItemChangeRadio6(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedZMidWay();

	afx_msg void OnBnClickedCreateresset();
	afx_msg void OnBnClickedDeleteresset();
	afx_msg void OnLvnItemchangedRessetlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSv();
	afx_msg void OnBnClickedHeight();
	afx_msg void OnBnClickedBlocktype();
	afx_msg void OnBnClickedBindani();
	afx_msg void OnBnClickedTransvertex();
	afx_msg void OnBnClickedTranstexture();
	CTreeCtrl m_effectView;
	afx_msg void OnNMClickEffectview(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBindeffect();
	
	void AddEffectSetFromFile(std::string strFXFileName);
	void AddFXSetTo3DTree(CResSet * pResSet);
	void AddFXGroupTo3DTree(HTREEITEM hSetItem);
	void AddFXUnitTo3DTree(HTREEITEM hSetItem);

	void OnResetActiveObject();
	afx_msg void OnBnClickedDoshadow();
	afx_msg void OnBnClickedNoshadow();
	//afx_msg void OnBnClickedShadow();
	afx_msg void OnBnClickedLoadresset();
};


