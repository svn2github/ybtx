
#include "stdafx.h"
#include "ImageLabel.h"


CImageLabel::CImageLabel()
:m_TextColor(0x7a7a7a)
,m_TextStyle(DT_LEFT)
{
}

CImageLabel::~CImageLabel()
{
}

BEGIN_MESSAGE_MAP(CImageLabel, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

BOOL CImageLabel::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CImageLabel::OnPaint()
{
	CPaintDC dc(this); 
	CString strText;
	GetWindowText(strText);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(m_TextColor);	
	CRect ClientRt;
	GetClientRect(&ClientRt);

	dc.DrawText(strText,&ClientRt,m_TextStyle);
}

HBRUSH CImageLabel::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetTextColor(m_TextColor);
	pDC->SetBkMode(TRANSPARENT);
	return (HBRUSH)::GetStockObject(NULL_BRUSH);
}

LRESULT CImageLabel::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_SETTEXT)
	{
		CRect rect;
		GetWindowRect(&rect);
		CWnd* pParent = GetParent();
		if (pParent)
		{
			pParent->ScreenToClient(&rect);
			pParent->InvalidateRect(&rect);
		}
	}
	return CStatic::WindowProc(message, wParam, lParam);
}

void CImageLabel::SetTextColor(COLORREF color)
{
	m_TextColor = color;
}

void CImageLabel::SetTextStyle(int nStyle)
{
	m_TextStyle = nStyle;
}