#pragma once


#include "ImageManage.h"


class CImageLabel : public CStatic
{
public:
	CImageLabel();
	~CImageLabel();
	void SetTextColor(COLORREF color);
	void SetTextStyle(int nStyle);
private:
	COLORREF m_TextColor;
	int		 m_TextStyle;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

