// ImageScrollbar.cpp : implementation file
//

#include "stdafx.h"
#include "Ybtx.h"
#include "ImageScrollbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CImageScrollbar

IMPLEMENT_DYNAMIC(CImageScrollbar, CStatic)

CImageScrollbar::CImageScrollbar()
	: m_ArrowHeight(0)
	, m_MouseDownArrowUp(false)
	, m_MouseDownArrowDown(false)
	, m_MouseDown(false)
	, m_ThumbTop(36)
	, m_pList(NULL)
{
}

CImageScrollbar::~CImageScrollbar()
{
}


BEGIN_MESSAGE_MAP(CImageScrollbar, CStatic)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CImageScrollbar::PageDown()
{
	m_pList->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEDOWN,0),NULL);
	UpdateThumbPosition();
}

void CImageScrollbar::PageUp()
{
	m_pList->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEUP,0),NULL);
	UpdateThumbPosition();
}

void CImageScrollbar::ScrollUp()
{
	m_pList->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP,0),NULL);
	UpdateThumbPosition();
}

void CImageScrollbar::ScrollDown()
{
	m_pList->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN,0),NULL);
	UpdateThumbPosition();
}

void CImageScrollbar::LoadThumbImage(wstring ImageFile)
{
	if ( !ImageFile.empty())
		m_ThumbImage.LoadImage(ImageFile.c_str());
}

void CImageScrollbar::LoadArrowDownImage(wstring ImageFile)
{
	if ( !ImageFile.empty())
		m_ArrowDownImage.LoadImage(ImageFile.c_str());
}

void CImageScrollbar::LoadArrowUpImage(wstring ImageFile)
{
	if ( !ImageFile.empty())
		m_ArrowUpImage.LoadImage(ImageFile.c_str());
}

void CImageScrollbar::LoadBkgImage(wstring ImageFile)
{
	if ( !ImageFile.empty())
		m_BkgImage.LoadImage(ImageFile.c_str());
}

void CImageScrollbar::UpdateThumbPosition()
{
	CRect ClientRt;
	GetClientRect(&ClientRt);
	double Pos = m_pList->GetScrollPos(SB_VERT);
	double Max = m_pList->GetScrollLimit(SB_VERT);
	double Height = ClientRt.Height() -  m_ArrowHeight*2 - m_ThumbHeight;
	
	double ThumbInterval  = Height/Max; 
	int NewValue = (int)(ThumbInterval * Pos);
	
	m_ThumbTop = NewValue + m_ArrowHeight;

	LimitThumbPosition();
	Draw();
}

void CImageScrollbar::Draw()
{
	//是否显示滚动条
	CRect clientRect;
	GetClientRect(&clientRect);

	CDC *pDC = GetDC();
	int nHeight = clientRect.Height() - m_ArrowHeight;
	
	//draw the background (span)
	if (m_BkgImage.GetMemDC())
	{
		SelectObject(m_BkgImage.GetMemDC(), m_BkgImage.GetBitmapHandle());
		BitBlt(pDC->m_hDC, 0, 0, m_BkgImage.GetBitmapInfo().bmWidth,
			m_BkgImage.GetBitmapInfo().bmHeight, m_BkgImage.GetMemDC(), 0, 0, SRCCOPY);
	}
	
	//draw the down arrow of the scrollbar
	if (m_ArrowDownImage.GetMemDC())
	{
		SelectObject(m_ArrowDownImage.GetMemDC(), m_ArrowDownImage.GetBitmapHandle());
		BitBlt(pDC->m_hDC, clientRect.left, nHeight, m_ArrowDownImage.GetBitmapInfo().bmWidth,
			m_ArrowDownImage.GetBitmapInfo().bmHeight, m_ArrowDownImage.GetMemDC(), 0, 0, SRCCOPY);
	}

	//draw the up arrow of the scrollbar
	if (m_ArrowUpImage.GetMemDC())
	{
		SelectObject(m_ArrowUpImage.GetMemDC(), m_ArrowUpImage.GetBitmapHandle());
		BitBlt(pDC->m_hDC, clientRect.left, 0, m_ArrowUpImage.GetBitmapInfo().bmWidth,
			m_ArrowUpImage.GetBitmapInfo().bmHeight, m_ArrowUpImage.GetMemDC(), 0, 0, SRCCOPY);
	}

	int	ItemCount = m_pList->GetItemCount();
	if ( ItemCount > 0 )
	{
		CRect ItemRect;
		m_pList->GetItemRect(0,&ItemRect,LVIR_LABEL);
		if ( ItemRect.Height() * ItemCount > clientRect.Height())
		{
			if (m_ThumbImage.GetMemDC())
			{
				SelectObject(m_ThumbImage.GetMemDC(), m_ThumbImage.GetBitmapHandle());
				BitBlt(pDC->m_hDC, clientRect.left, clientRect.top +m_ThumbTop, m_ThumbImage.GetBitmapInfo().bmWidth,
					m_ThumbImage.GetBitmapInfo().bmHeight, m_ThumbImage.GetMemDC(), 0, 0, SRCCOPY);
			}				
		}
	}
}

void CImageScrollbar::LimitThumbPosition()
{
	CRect clientRect;
	GetClientRect(&clientRect);
	int clientHeight = clientRect.Height();

	//判断Thumb的Bottom是否越界
	if (m_ThumbTop + m_ThumbHeight > clientHeight - m_ArrowHeight) 
	{
		m_ThumbTop = clientHeight - m_ArrowHeight - m_ThumbHeight;
	}

	//判断Thumb的Top是否越界
	if ( m_ThumbTop < clientRect.top + m_ArrowHeight)	
	{
		m_ThumbTop = clientRect.top + m_ArrowHeight;
	}
}

void CImageScrollbar::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) 
	{
		if ( m_MouseDownArrowDown)
		{
			ScrollDown();
		}

		if (m_MouseDownArrowUp)
		{
			ScrollUp();
		}
	}
	else if( nIDEvent == 2 ) 
	{
		if ( m_MouseDownArrowDown)
		{
			KillTimer(2);
			SetTimer(1,50,NULL);
		}

		if ( m_MouseDownArrowUp)
		{
			KillTimer(2);
			SetTimer(1,50,NULL);
		}
	}
	CStatic::OnTimer(nIDEvent);
}

void CImageScrollbar::OnPaint()
{
	CPaintDC dc(this); 
	Draw();
}

void CImageScrollbar::OnMouseMove(UINT nFlags, CPoint point)
{
	
	CRect ClientRect;
	GetClientRect(&ClientRect);
	CRect ThumbRt(0,m_ThumbTop,ClientRect.Width(),m_ThumbTop+m_ThumbHeight);

	if (m_MouseDown /*&& ThumbRt.PtInRect(point)*/)
	{
		double Max = m_pList->GetScrollLimit(SB_VERT);
		int Pos = m_pList->GetScrollPos(SB_VERT);
		
		double Height = ClientRect.Height() - m_ThumbHeight - m_ArrowHeight * 2;
		double ThumbInterval = Height/Max;

		m_ThumbTop = point.y - 	m_ThumbHeight/2;
		int ScrollTimes = (int)((m_ThumbTop - m_ArrowHeight)/ThumbInterval) - Pos;

		CRect ItemRect;
		m_pList->GetItemRect(0,&ItemRect,LVIR_LABEL);

		CSize Size;
		Size.cx = 0;
		Size.cy = ScrollTimes *	ItemRect.Height();
		
		if ( ScrollTimes != 0 )
			m_pList->Scroll(Size);
		LimitThumbPosition();
		Draw();
	}
	else
		m_MouseDown = false;

	CStatic::OnMouseMove(nFlags, point);
}

void CImageScrollbar::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	CRect ClientRect;
	GetClientRect(&ClientRect);
	int Height = ClientRect.Height() - m_ArrowHeight;

	CRect UpArrowRt( 0,0,ClientRect.Width(),m_ArrowHeight);
	CRect DownArrowRt(0,Height,ClientRect.Width(),Height+m_ArrowHeight);
	CRect ThumbRt(0,m_ThumbTop,ClientRect.Width(),m_ThumbTop+m_ThumbHeight);

	//鼠标在Thumb 上
	if (ThumbRt.PtInRect(point))	
	{
		m_MouseDown = true;
	}

	//按向下滚动箭头
	if (DownArrowRt.PtInRect(point)) 
	{
		m_MouseDownArrowDown = true;
		SetTimer(2,250,NULL);
	}

	//按向上滚动箭头
	if (UpArrowRt.PtInRect(point))  
	{
		m_MouseDownArrowUp = true;
		SetTimer(2,250,NULL);
	}
	CStatic::OnLButtonDown(nFlags, point);
}

void CImageScrollbar::OnLButtonUp(UINT nFlags, CPoint point)
{
	//UpdateThumbPosition();
	KillTimer(1);
	ReleaseCapture();

	CRect ClientRect;
	GetClientRect(&ClientRect);
	int Height = ClientRect.Height() - m_ArrowHeight;

	CRect UpArrowRt( 0,0,ClientRect.Width(),m_ArrowHeight);
	CRect DownArrowRt(0,Height,ClientRect.Width(),Height+m_ArrowHeight);
	CRect ThumbRt(0,m_ThumbTop,ClientRect.Width(),m_ThumbTop+m_ThumbHeight);

	bool bInChannel = true;
	if (UpArrowRt.PtInRect(point) && m_MouseDownArrowUp)
	{
		ScrollUp();
		bInChannel = false;
	}

	if (DownArrowRt.PtInRect(point) && m_MouseDownArrowDown)
	{
		ScrollDown();
		bInChannel = false;
	}

	if (ThumbRt.PtInRect(point))
	{
		bInChannel = false;
	}

	if (ClientRect.PtInRect(point))
	{
		if (bInChannel)
		{
			if (point.y > m_ThumbTop)
				PageDown();
			else
				PageUp();
		}
	}

	m_MouseDown = false;
	m_MouseDownArrowDown = false;
	m_MouseDownArrowUp = false;
	CStatic::OnLButtonUp(nFlags, point);
}

void CImageScrollbar::SetList(CListCtrl* pList)
{
	m_pList = pList;
	m_ThumbHeight = m_ThumbImage.GetBitmapInfo().bmHeight;
	if(m_ArrowUpImage.GetMemDC())
		m_ArrowHeight = m_ArrowUpImage.GetBitmapInfo().bmHeight;

	m_ThumbTop = m_ArrowHeight;
}
BOOL CImageScrollbar::OnEraseBkgnd(CDC* pDC)
{
	return CStatic::OnEraseBkgnd(pDC);
}
