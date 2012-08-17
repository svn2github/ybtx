
#include "stdafx.h"
#include "ImageProgress.h"


CImageProgress::CImageProgress()
	: m_Pos(0)
{
}

CImageProgress::~CImageProgress()
{
}

BEGIN_MESSAGE_MAP(CImageProgress, CButton)
	ON_WM_DRAWITEM()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


BOOL CImageProgress::LoadProgressImage(int BackImage, int ForeImage)
{
	m_ForeImage.LoadImage(ForeImage);
	m_BackImage.LoadImage(BackImage);

	return TRUE;
}

BOOL CImageProgress::LoadProgressImage( wstring strBackImage, wstring strForeImage )
{
	m_ForeImage.LoadImage(strForeImage.c_str());
	m_BackImage.LoadImage(strBackImage.c_str());
	return TRUE;
}

void CImageProgress::SetPos(int percent)
{
	if (m_Pos != percent)
	{
		m_Pos = percent;
		InvalidateRect(NULL);
	}
}

void CImageProgress::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	HBITMAP holdBmp = (HBITMAP)SelectObject(m_ForeImage.GetMemDC(), m_ForeImage.GetBitmapHandle()); // 选择对象到指定的HDC设备
	int SrcWidth = int( m_ForeImage.GetBitmapInfo().bmWidth * (m_Pos / 100.0f) );
	if ( SrcWidth != 0 )
		BitBlt(lpDrawItemStruct->hDC, 0, 0, SrcWidth, m_ForeImage.GetBitmapInfo().bmHeight, m_ForeImage.GetMemDC(), 0, 0, SRCCOPY);
	SelectObject(m_BackImage.GetMemDC(), m_BackImage.GetBitmapHandle());
	int DstWidth = m_BackImage.GetBitmapInfo().bmWidth - SrcWidth;
	if ( DstWidth != 0 )
		BitBlt(lpDrawItemStruct->hDC, SrcWidth, 0, DstWidth, m_BackImage.GetBitmapInfo().bmHeight, m_BackImage.GetMemDC(), SrcWidth, 0, SRCCOPY);
}

 

BOOL CImageProgress::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}
