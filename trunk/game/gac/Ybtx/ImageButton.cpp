
#include "stdafx.h"
#include "ImageButton.h"
#include "resource.h"


BEGIN_MESSAGE_MAP(CImageButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, &CImageButton::OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, &CImageButton::OnMouseLeave)
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


CImageButton::CImageButton()
	: m_state(eBS_NORMAL)
	, m_enable(true)
{
}

CImageButton::~CImageButton()
{
}


void CImageButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch (m_state)
	{
	case eBS_NORMAL:
		SelectObject(m_NormalImage.GetMemDC(), m_NormalImage.GetBitmapHandle());
		BitBlt(lpDrawItemStruct->hDC, 0, 0, m_NormalImage.GetBitmapInfo().bmWidth,
			m_NormalImage.GetBitmapInfo().bmHeight, m_NormalImage.GetMemDC(), 0, 0, SRCCOPY);
		break;
	case eBS_HOVER:
		SelectObject(m_HoverImage.GetMemDC(), m_HoverImage.GetBitmapHandle());
		BitBlt(lpDrawItemStruct->hDC, 0, 0, m_HoverImage.GetBitmapInfo().bmWidth,
			m_HoverImage.GetBitmapInfo().bmHeight, m_HoverImage.GetMemDC(), 0, 0, SRCCOPY);
		break;
	case eBS_DISABLE:
		SelectObject(m_DisableImage.GetMemDC(), m_DisableImage.GetBitmapHandle());
		BitBlt(lpDrawItemStruct->hDC, 0, 0, m_DisableImage.GetBitmapInfo().bmWidth,
			m_DisableImage.GetBitmapInfo().bmHeight, m_DisableImage.GetMemDC(), 0, 0, SRCCOPY);
		break;
	case eBS_CLICK:
		SelectObject(m_ClickImage.GetMemDC(), m_ClickImage.GetBitmapHandle());
		BitBlt(lpDrawItemStruct->hDC, 0, 0, m_ClickImage.GetBitmapInfo().bmWidth,
			m_ClickImage.GetBitmapInfo().bmHeight, m_ClickImage.GetMemDC(), 0, 0, SRCCOPY);
		break;
	}
}

void CImageButton::LoadButtonImage(int ImageFile)
{
	m_NormalImage.LoadImage(ImageFile);
}

void CImageButton::LoadButtonImage( wstring strImageFile )
{
	m_NormalImage.LoadImage(strImageFile.c_str());
}

void CImageButton::LoadHoverImage(int ImageFile)
{
	m_HoverImage.LoadImage(ImageFile);
}

void CImageButton::LoadHoverImage( wstring strImageFile )
{
	m_HoverImage.LoadImage(strImageFile.c_str());
}

void CImageButton::LoadDisableImage(int ImageFile)
{
	m_DisableImage.LoadImage(ImageFile);
}

void CImageButton::LoadDisableImage( wstring strImageFile )
{
	m_DisableImage.LoadImage(strImageFile.c_str());
}

void CImageButton::LoadClickImage(int ImageFile)
{
	m_ClickImage.LoadImage(ImageFile);
}

void CImageButton::LoadClickImage( wstring strImageFile )
{
	m_ClickImage.LoadImage(strImageFile.c_str());
}

void CImageButton::SetState(BtnState state)
{
	m_state = state;
	InvalidateRect(NULL);
}

BtnState CImageButton::GetState()
{
	return m_state;
}

void CImageButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CButton::OnMouseMove(nFlags, point);
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_HOVER | TME_LEAVE;
	tme.dwHoverTime = 30; // HOVER_DEFAULT
	tme.hwndTrack = GetSafeHwnd();
	_TrackMouseEvent(&tme);
}

LRESULT CImageButton::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	if ( m_state == eBS_NORMAL )
		SetState(eBS_HOVER);

	return 0;
}

LRESULT CImageButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (m_state != eBS_NORMAL && m_state != eBS_DISABLE)
		SetState(eBS_NORMAL);

	return 0;
}

BOOL CImageButton::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}


void CImageButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if ( m_state != eBS_DISABLE )
		SetState(eBS_NORMAL);

	CButton::OnLButtonUp(nFlags, point);
}

void CImageButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if ( m_state != eBS_DISABLE )
		SetState(eBS_CLICK);

	CButton::OnLButtonDown(nFlags, point);
}
