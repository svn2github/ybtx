#pragma once
#include "CFileLoad.h"

// CWndImageList
class CWndGroup;
class CWndImageProp;

class CWndImageList : public ::CListCtrl
{
	DECLARE_DYNAMIC(CWndImageList)

	CFileLoad			m_FileLoader;
	int					m_nEditItem;
	vector<int>			m_vecSelectItem;

	CWndImageProp*		m_ImageProp;
	CWndGroup*			m_pWndGroup;
	int					m_nImageType;

	CImageList			m_ImageList;

	WND_IMAGE_LIST&		m_ImageCopy;
	CFRect				m_rtWndCopy;

	WND_IMAGE_LIST*		tmpImageList;
public:
	CWndImageList();
	virtual ~CWndImageList();
	void Init( CWndGroup* pWndGroup,CWndImageProp* pImageProp );
	void SetEditImageType( int nImageType, bool bSavePreWnd = true );
	void UpdateImageList();
	void DetectImage( UINT uMessage, WPARAM wParam, LPARAM lParam );
	void DrawEditLine();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusEdit();
	afx_msg void OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
};
