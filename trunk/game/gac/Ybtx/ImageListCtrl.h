#pragma once

#include "ImageManage.h"
#include "ImageScrollbar.h"
#include <map>

using namespace std;
// CImageListCtrl

class CImageListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CImageListCtrl)

public:
	CImageListCtrl();
	virtual ~CImageListCtrl();

	void LoadNormalImage(wstring ImageFile);
	void LoadSelectImage(wstring ImageFile);
	void InvalidateGrid(int nRow, int nCol);
	void SetItemHeight(int nItemHeight);
	void LoadBkImage(wstring ImageFile);
	void SelectSubItem(wstring strSubItem);
	void SelectSubItem(int nRow, int nCol);
	void InitScrollbar(CRect rt,wstring ArrowUpImageFile,wstring ArrowDownImageFile,wstring ThumbImageFile,wstring BkgImageFile );
	void SetNormalColor(COLORREF color);
	void SetSelectColor(COLORREF color);
	COLORREF GetItemColor(int Row,int Col);
	void SetItemColor(int Row,int Col,COLORREF ItemColor);
	void DeleteAllItems();
	CImageManage* GetItemSelectImage(int Row,int Col);
	CImageManage* GetItemNormalImage(int Row,int Col);
	void SetItemImage(int ImageID,CImageManage* NormalImage,CImageManage* SelectImage);
	void SetItemImageID(int Row,int Col,int ImageID);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void SetItemToolTipText( int nItem, int nSubItem, LPCTSTR lpszToolTipText );
private:
	CImageManage    m_NormalImage;
	CImageManage    m_SelectImage;
	CImageManage	m_BkgndImage;
	CImageScrollbar m_ImageVertScrollbar;
	int				m_SelRow;
	int				m_SelCol;
	int				m_nItemHeight;
	CFont			m_Font;
	COLORREF		m_SelectColor;
	COLORREF		m_NormalColor;
	map< pair<int,int>, COLORREF>	m_ItemColorMap;
	map< pair<int,int>, int> m_ItemImageIDMap;
	map< int, pair<CImageManage*,CImageManage*> > m_ItemImageMap;
	CMapStringToString m_ToolTipMap;
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
};


