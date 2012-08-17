#pragma once

#include "ImageManage.h"
// CImageScrollbar

class CImageScrollbar : public CStatic
{
	DECLARE_DYNAMIC(CImageScrollbar)

public:
	CImageScrollbar();
	virtual ~CImageScrollbar();

public:
	void LimitThumbPosition();
	void UpdateThumbPosition();
	void Draw();
	void ScrollDown();
	void ScrollUp();
	void PageUp();
	void PageDown();
	void LoadThumbImage(wstring ImageFile);
	void LoadArrowUpImage(wstring ImageFile);
	void LoadArrowDownImage(wstring ImageFile);
	void LoadBkgImage(wstring ImageFile);
	void SetList(CListCtrl * pList);

private:
	bool	m_MouseDownArrowUp;
	bool	m_MouseDownArrowDown;
	bool	m_MouseDown;

	int		m_ThumbHeight;	//滑块的高度
	int		m_ThumbTop;		//滑块Top边坐标
	int		m_ArrowHeight;	//箭头的高度

	CImageManage		m_ThumbImage;
	CImageManage		m_ArrowUpImage;
	CImageManage		m_ArrowDownImage;
	CImageManage		m_BkgImage;
	CListCtrl*			m_pList;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


