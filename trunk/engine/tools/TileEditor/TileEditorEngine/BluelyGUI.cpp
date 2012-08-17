//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "bluelygui.h"
#include "render.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace bg
{
namespace ui
{

//////////////////////////////////////////////////////////////////////////
// window base
//////////////////////////////////////////////////////////////////////////

CWnd::CWnd()
{
	m_pParent = NULL;
	ZeroMemory(&m_wndInfo,sizeof(SWndInfo));

	m_wndInfo.colorBack =  BWC_BACK;
	m_wndInfo.colorText =	BWC_TEXT;	
				
	m_wndInfo.colorLow  =   BWC_LOW;
	m_wndInfo.colorMiddle = BWC_MIDDLE;	
	m_wndInfo.colorHigh   = BWC_HIGH; 
}

CWnd::~CWnd()
{
	assert((int)m_listChild.size() == 0);
}

void CWnd::Update(ip::CInputDevice::SEvent & event)
{
	UpdateDefault();

	DWORD & dwStyle = m_wndInfo.dwStyle;
	BOOL  & bActive = m_wndInfo.bActive;

	if ( (dwStyle & BWS_VISIBLE) && !(dwStyle & BWS_DISABLE) )
	{
		UpdateChild(event);
	}
}

void CWnd::Render()
{
	DWORD &dwStyle = m_wndInfo.dwStyle;
	if (dwStyle & BWS_VISIBLE)
	{
		RenderDefault();
		RenderChild();
	}
}

void CWnd::UpdateDefault()
{
//	DWORD & dwStyle = m_wndInfo.dwStyle;
//	BOOL  & bActive = m_wndInfo.bActive;
//	RECT  & rcWnd   = m_wndInfo.rcRect;
//	
//	POINT ptMousePos;
//	POINT ptMouseDelta;
//
//	ip::CInputDevice::GetInstance()->GetMousePosition(ptMousePos.x,ptMousePos.y);
//	ip::CInputDevice::GetInstance()->GetMouseDelta(ptMouseDelta.x,ptMouseDelta.y);
//
//	CUIManager * pMgr = CUIManager::GetInstance();
//	
//	assert(pMgr);
//
//	if ( (dwStyle & BWS_VISIBLE) && !(dwStyle & BWS_DISABLE) )
//	{
//		switch( event->eType )
//		{ 
//		case EUIMT_MS_BTNUP:
//		break;
//
//		case EUIMT_MS_BTNDOWN:
//			switch( event->dwData )
//			{
//			case MK_LBUTTON:
//			break;
//			
//			case MK_RBUTTON:
//				if ( bActive )
//				{
//					int iPtResult = PtInRect(&ptMousePos);
//					if ( iPtResult == BWH_INSIDE && (dwStyle & BWS_DRAG || dwStyle & BWS_DEBUG) ) 
//					{
//						m_wndInfo.bDrag = TRUE;
//					}	
//				
//					if ( iPtResult == BWH_INTERSECT && (dwStyle & BWS_SIZE || dwStyle & BWS_DEBUG) )
//					{
//						WORD & wSize = m_wndInfo.wSize;
//
//						if ( ptMousePos.x >= rcWnd.left   &&  ptMousePos.x <= rcWnd.left + BWA_FRAMESIZE )  wSize |= 0x01;
//						if ( ptMousePos.x <= rcWnd.right  &&  ptMousePos.x >= rcWnd.right - BWA_FRAMESIZE ) wSize |= 0x02;
//						if ( ptMousePos.y >= rcWnd.top    &&  ptMousePos.y <= rcWnd.top + BWA_FRAMESIZE )   wSize |= 0x04;
//						if ( ptMousePos.y <= rcWnd.bottom &&  ptMousePos.y >= rcWnd.bottom - BWA_FRAMESIZE ) wSize |= 0x08;
//					}
//				}
//			break;
//
//			case MK_MBUTTON:
//			break;
//			}
//		break;
//
//		default: break;
//		}
//
//		if ( m_wndInfo.wSize )
//		{
//			RECT & rc = m_wndInfo.rcRect;
//			
//			WORD & wSize = m_wndInfo.wSize;
//			
//			int x = ptMouseDelta.x;
//			int y = ptMouseDelta.y;
//
//			if ( wSize & 0x01 ) 
//			{
//				SizeWnd(-x,0);
//				MoveWnd(x,0);
//			}
//			if ( wSize & 0x02 ) 
//			{
//				SizeWnd(x,0);
//			}
//			if ( wSize & 0x04 ) 
//			{
//				SizeWnd(0,-y);
//				MoveWnd(0,y);
//			}
//			if ( wSize & 0x08 ) 
//			{
//				SizeWnd(0,y);
//			}						
//			
//////@			if ( !(pInputInfo->dwMouseButtonState & MK_RBUTTON) ) m_wndInfo.wSize = 0;
////		}
////
////		if ( m_wndInfo.bDrag )
////		{
////			int x = pInputInfo->ptMouse.x - pInputInfo->ptMouseOld.x;
////			int y = pInputInfo->ptMouse.y - pInputInfo->ptMouseOld.y;
////
////			MoveWnd(x,y);
////			
//////@			if ( !(pInputInfo->dwMouseButtonState & MK_RBUTTON) ) m_wndInfo.bDrag = FALSE;
//		}
//	}
}

void CWnd::SizeWnd(int iCX, int iCY)
{
	RECT & rc = m_wndInfo.rcRect;
	rc.right  += iCX;
	rc.bottom += iCY;
}

void CWnd::UpdateChild(ip::CInputDevice::SEvent & event)
{
	for ( std::list<CWnd *>::iterator it = m_listChild.begin(); it != m_listChild.end(); ++it )
	{
		CWnd * pWnd = *it; 
		pWnd->Update(event);
	}
}
				
void CWnd::AddChild(CWnd * pWnd)
{
	if ( pWnd->GetStyle() & BWS_TOPMOST )
		m_listChild.push_front(pWnd);
	else
	{
		std::list<CWnd *>::iterator it = m_listChild.begin();
		for ( ; it != m_listChild.end(); ++it )
		{
			CWnd * p = *it; 
			if ( !(p->GetStyle() & BWS_TOPMOST) )
			{
				m_listChild.insert(it,pWnd);
				break;
			}
		}
		if ( it == m_listChild.end() )
		{
			m_listChild.push_back(pWnd);
		}
	}
}

void CWnd::RemoveChild(CWnd * pWnd)
{
	m_listChild.remove(pWnd);
}

void CWnd::HitTest(const POINT * pPoint, CWnd ** ppWnd)
{
	if ( m_wndInfo.dwStyle & BWS_DISABLE )
	{
		return;
	}

	if ( *ppWnd ) return ;
	if ( m_wndInfo.dwStyle & BWS_VISIBLE )
	{
		for ( std::list<CWnd *>::iterator it = m_listChild.begin(); it != m_listChild.end(); ++it )
		{
			CWnd * pWnd = *it; 
			pWnd->HitTest(pPoint,ppWnd);
			if (*ppWnd) return;
		}

		if (PtInRect(pPoint) != BWH_OUTSIDE)
		{
			*ppWnd = this;
		}
	}
}

BOOL CWnd::Create(const char * pszName, const RECT * pRect, DWORD dwStyle, CWnd * pParent)
{
	assert(pszName);
	assert(pRect);

	strncpy(m_wndInfo.szName, pszName, 255);
	m_wndInfo.szName[255] = 0;

	m_wndInfo.dwStyle = dwStyle; 

	m_pParent = pParent;

	if ( pParent )
	{
		RECT  rc;
		pParent->GetWndRect(&rc);
		
		m_wndInfo.rcRect.left = rc.left + pRect->left;
		m_wndInfo.rcRect.top  = rc.top  + pRect->top;
		m_wndInfo.rcRect.right= m_wndInfo.rcRect.left + (pRect->right-pRect->left);
		m_wndInfo.rcRect.bottom = m_wndInfo.rcRect.top  + (pRect->bottom-pRect->top);

		pParent->AddChild(this);
	}
	else
		memcpy(&m_wndInfo.rcRect,pRect,sizeof(RECT));

	return TRUE;
}


void CWnd::GetWndRect(RECT * pRect)
{
	memcpy(pRect, &m_wndInfo.rcRect, sizeof(RECT));
}

void CWnd::SetWndRect(RECT *pRect)
{
	memcpy(&m_wndInfo.rcRect, pRect, sizeof(RECT));
}

void CWnd::Destroy()
{
	if (m_pParent)
		m_pParent->RemoveChild(this);
}

int CWnd::PtInRect(const POINT * pPoint)
{
	if ( !(m_wndInfo.dwStyle & BWS_VISIBLE) ) return BWH_OUTSIDE;

	RECT & rc = m_wndInfo.rcRect;
	int x = pPoint->x;
	int y = pPoint->y;
	if ( x < rc.left || y < rc.top || x > rc.right || y > rc.bottom ) return BWH_OUTSIDE; 
	if ( x > rc.left + BWA_FRAMESIZE && x < rc.right - BWA_FRAMESIZE && y > rc.top + BWA_FRAMESIZE && y < rc.bottom - BWA_FRAMESIZE ) return BWH_INSIDE;
	return BWH_INTERSECT;
}

void CWnd::MoveWnd(int iOX, int iOY)
{
	RECT & rc = m_wndInfo.rcRect;
	rc.left += iOX;
	rc.top  += iOY;
	rc.right += iOX;
	rc.bottom += iOY;	

	for ( std::list<CWnd *>::iterator it = m_listChild.begin(); it != m_listChild.end(); ++it )
	{
		CWnd * p = *it; 
		p->MoveWnd(iOX,iOY);
	}
}

void CWnd::ShowWnd(BOOL bShow)
{
	if ( bShow )
		m_wndInfo.dwStyle |= BWS_VISIBLE;
	else
		m_wndInfo.dwStyle &= ~BWS_VISIBLE;
}

void CWnd::MoveWndTo(int iX, int iY)
{
	int iOX = iX - m_wndInfo.rcRect.left;
	int iOY = iY - m_wndInfo.rcRect.top;

	MoveWnd(iOX,iOY);
}

void CWnd::CenterWndTo(int iX, int iY)
{
	DWORD iW = m_wndInfo.rcRect.right - m_wndInfo.rcRect.left + 1;
	DWORD iH = m_wndInfo.rcRect.bottom- m_wndInfo.rcRect.top + 1;
	
	int iOX = iX - m_wndInfo.rcRect.left - iW / 2;
	int iOY = iY - m_wndInfo.rcRect.top  - iH / 2; 

	MoveWnd(iOX,iOY);
}

void CWnd::RenderDefault(void)
{
	DWORD & dwStyle = m_wndInfo.dwStyle;

	rd::CRenderDevice::Layer2D * pLayer2D = rd::CRenderDevice::GetInstance()->GetLayer2D();

	RECT & rc = m_wndInfo.rcRect;

	int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;

	if ( dwStyle & BWS_SHADOW )
	{
		pLayer2D->DrawSolidQuad(iX+4, iY+4, iW, iH, m_wndInfo.colorLow);
	}

	if ( dwStyle & BWS_FILLBKG )
	{
		pLayer2D->DrawSolidQuad(iX,iY,iW,iH,m_wndInfo.colorBack);
	}	

	if ( dwStyle & BWS_DEBUG && m_wndInfo.bActive )
	{
		char szTemp[256];
		
		// relative to parent
		RECT rcRelative = m_wndInfo.rcRect;
		
		if ( m_pParent )
		{
			RECT rcParent;
			m_pParent->GetWndRect(&rcParent);
			rcRelative.left  -= rcParent.left;
			rcRelative.right  = rcRelative.left + iW;
			rcRelative.top   -= rcParent.top;
			rcRelative.bottom = rcRelative.top + iH;
		}

		sprintf(szTemp,"%s [%d %d %d %d][%d,%d]",m_wndInfo.szName,rcRelative.left,rcRelative.top,rcRelative.right,rcRelative.bottom,iW,iH);

		int iStartX = iX;
		int iStartY = iY; 
	}

	if ( dwStyle & BWS_OUTLINE )
	{
		if ( m_wndInfo.bActive )
			pLayer2D->DrawWireQuad(iX,iY,iW,iH,m_wndInfo.colorHigh);
		else
			pLayer2D->DrawWireQuad(iX,iY,iW,iH,m_wndInfo.colorMiddle);			
	}
}

void CWnd::RenderChild(void)
{
	DWORD & dwStyle = m_wndInfo.dwStyle;
	if ( !(dwStyle & BWS_VISIBLE) ) return ;

	for ( std::list<CWnd *>::reverse_iterator it = m_listChild.rbegin(); it != m_listChild.rend(); ++it )
	{
		CWnd * pWnd = *it; 
		pWnd ->Render();
	}
}

void CWnd::ModifyStyle(DWORD dwAdd, DWORD dwRemove)
{
	DWORD & dwStyle = m_wndInfo.dwStyle;
	dwStyle |= dwAdd;
	dwStyle &= ~dwRemove;
}

//////////////////////////////////////////////////////////////////////////
// static
//////////////////////////////////////////////////////////////////////////

void CStatic::Update()
{
	CWnd::UpdateDefault();
}

void CStatic::Render()
{
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	assert(pRender);

	rd::CRenderDevice::Layer2D * pLayer2D = pRender->GetLayer2D();
	assert(pLayer2D);

	static RECT & rc = m_wndInfo.rcRect;

	static int iX = rc.left;
	static int iY = rc.top;
	static int iW = rc.right - rc.left + 1;
	static int iH = rc.bottom - rc.top + 1;
	
	pLayer2D->DrawSolidQuad(iX,iY,iW,iH,m_wndInfo.colorBack);
}

//////////////////////////////////////////////////////////////////////////
// button
//////////////////////////////////////////////////////////////////////////
//
//CButton::CButton()
//{
//	m_eBtnState = EUBS_NORMAL;
//	m_bClicked = false;
//	m_dwTextureHandle = INVALID;
//}
//
//CButton::~CButton()
//{
//
//}
//
//void CButton::Update()
//{
//	CWnd::UpdateDefault();
//
//	DWORD & dwStyle = m_wndInfo.dwStyle;
//	BOOL  & bActive = m_wndInfo.bActive;
//
//	if ( (dwStyle & BWS_VISIBLE) && !(dwStyle & BWS_DISABLE) )
//	{
//		CUIManager::SInputInfo * pInputInfo = CUIManager::GetInstance()->GetInputInfoBuffer();
//
//		BOOL bMouseOver = ( PtInRect(&pInputInfo->ptMouse) > 0 );
//
//		if ( m_pParent->IsActive() || bActive )
//		{
//			switch( pInputInfo->eType )
//			{
//			// handle mouse input
//			case EUIMT_MS_BTNUP:
//				if ( m_eBtnState == EUBS_DOWN && pInputInfo->dwData == MK_LBUTTON )
//				{
//					m_eBtnState = bMouseOver ? EUBS_OVER : EUBS_NORMAL;
//					if ( bMouseOver )
//						m_bClicked = true;
//				}
//			break;
//			
//			case EUIMT_MS_BTNDOWN:
//				if ( bMouseOver && m_eBtnState != EUBS_DOWN && pInputInfo->dwData == MK_LBUTTON)
//				{
//					m_eBtnState = EUBS_DOWN; 
//				}
//			break;
//			
//			default:break;
//			}
//
//			if ( m_eBtnState != EUBS_DOWN )
//			{
//				m_eBtnState = bMouseOver ? EUBS_OVER : EUBS_NORMAL;
//			}
//		}
//	}
//}
//
//void CButton::Render()
//{
//	CWnd::RenderDefault();
//
//	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
//	assert(pRender);
//
//	rd::CRenderDevice::Layer2D * pLayer2D = pRender->GetLayer2D();
//	assert(pLayer2D);
//	
//	DWORD & dwStyle = m_wndInfo.dwStyle;
//	
//	if ( dwStyle & BWS_VISIBLE )
//	{
//		RECT & rc = m_wndInfo.rcRect;
//		
//		int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;
//		int iLen = (int)strlen(m_wndInfo.szName);
//
//		//@ int iTextStartX = iX + iW/2 - pLayer2D->CalcTextWidth(m_wndInfo.szName,iLen)/2;
//		//@ int iTextStartY = iY + iH/2 - pLayer2D->GetFontHeight()/2;
//	
//		// shadow
//		// pLayer2D->DrawSolidQuad(iX+BWA_FRAMESIZE,iY+BWA_FRAMESIZE,iW,iH,m_wndInfo.colorLow);
//	
//		if ( dwStyle & BWS_DISABLE )
//		{
//			if ( m_dwTextureHandle != INVALID )
//			{
//				pLayer2D->DrawTexture(iX,iY,iW,iH,m_dwTextureHandle);
//			//	pLayer2D->DrawWireQuad(iX,iY,iW,iH,m_wndInfo.colorLow);
//			}
//			else
//			{
//				pLayer2D->DrawSolidQuad(iX,iY,iW,iH,m_wndInfo.colorHigh);
//			//	pLayer2D->DrawWireQuad(iX,iY,iW,iH,m_wndInfo.colorLow);
//			}
//		//@	pLayer2D->TextOut(iTextStartX,iTextStartY,m_wndInfo.szName,iLen,cLow);	
//		}
//		else
//		{
//			switch(m_eBtnState)
//			{
//			case EUBS_NORMAL:
//				if ( m_dwTextureHandle != INVALID )
//				{
//					pLayer2D->DrawTexture(iX,iY,iW,iH,m_dwTextureHandle);
//				//	pLayer2D->DrawWireQuad(iX,iY,iW,iH,m_wndInfo.colorMiddle);
//				}
//				else
//				{
//					pLayer2D->DrawSolidQuad(iX,iY,iW,iH,m_wndInfo.colorHigh);
//				//	pLayer2D->DrawWireQuad(iX,iY,iW,iH,m_wndInfo.colorMiddle);
//				}
//			break;
//			
//			case EUBS_DOWN:
//				if ( m_dwTextureHandle != INVALID )
//				{
//					pLayer2D->DrawTexture(++iX,++iY,iW,iH,m_dwTextureHandle);
//				//	pLayer2D->DrawWireQuad(iX,iY,iW,iH,m_wndInfo.colorHigh);
//				}
//				else
//				{
//					pLayer2D->DrawSolidQuad(++iX,++iY,iW,iH,m_wndInfo.colorHigh);
//				//	pLayer2D->DrawWireQuad(iX,iY,iW,iH,m_wndInfo.colorHigh);
//				}
//				//@ iTextStartX++;iTextStartY++;
//			break;
//			
//			case EUBS_OVER:
//				if ( m_dwTextureHandle != INVALID )
//				{
//					pLayer2D->DrawTexture(iX,iY,iW,iH,m_dwTextureHandle);
//				//	pLayer2D->DrawWireQuad(iX,iY,iW,iH,m_wndInfo.colorHigh);
//				}
//				else
//				{
//					pLayer2D->DrawSolidQuad(iX,iY,iW,iH,m_wndInfo.colorHigh);
//				//	pLayer2D->DrawWireQuad(iX,iY,iW,iH,m_wndInfo.colorHigh);
//				}
//			break;
//			}
//			
//			//// default to center
//			//pLayer2D->TextOut(iTextStartX + 1,iTextStartY + 1,m_wndInfo.szName,iLen,BWC_BLACK);	
//			//pLayer2D->TextOut(iTextStartX - 1,iTextStartY - 1,m_wndInfo.szName,iLen,BWC_BLACK);	
//			//pLayer2D->TextOut(iTextStartX + 1,iTextStartY - 1,m_wndInfo.szName,iLen,BWC_BLACK);	
//			//pLayer2D->TextOut(iTextStartX - 1,iTextStartY + 1,m_wndInfo.szName,iLen,BWC_BLACK);		
//			//pLayer2D->TextOut(iTextStartX + 1,iTextStartY,m_wndInfo.szName,iLen,BWC_BLACK);	
//			//pLayer2D->TextOut(iTextStartX - 1,iTextStartY,m_wndInfo.szName,iLen,BWC_BLACK);	
//			//pLayer2D->TextOut(iTextStartX,iTextStartY - 1,m_wndInfo.szName,iLen,BWC_BLACK);	
//			//pLayer2D->TextOut(iTextStartX,iTextStartY + 1,m_wndInfo.szName,iLen,BWC_BLACK);	
//			//pLayer2D->TextOut(iTextStartX,iTextStartY,m_wndInfo.szName,iLen,cText);	
//		}
//	}
//	// NO CHILD
//}
//
//bool CButton::IsClicked()
//{
//	bool bClicked = m_bClicked;
//	m_bClicked = false;
//	return bClicked;
//}

//////////////////////////////////////////////////////////////////////////
// checkbox
//////////////////////////////////////////////////////////////////////////

int CCheckBox::ms_iIDMother = 0;
int CCheckBox::ms_iHotID = INVALID;

CCheckBox::CCheckBox()
{
	m_bChecked = false;
	m_iID = ms_iIDMother++;
	m_dwTextureHandle = INVALID;
	m_bHover = false;
//	m_rc.top = m_rc.bottom = m_rc.left = m_rc.right = 0;
}

CCheckBox::~CCheckBox()
{
}

void CCheckBox::Render()
{
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	assert(pRender);
	rd::CRenderDevice::Layer2D * pLayer2D = pRender->GetLayer2D();
	assert(pLayer2D);

	DWORD & dwStyle = m_wndInfo.dwStyle;
	if ( dwStyle & BWS_VISIBLE )
	{
		CWnd::RenderDefault();

		RECT & rc = m_wndInfo.rcRect;

		int iX = rc.left, iY = rc.top, iW = rc.right - rc.left + 1, iH = rc.bottom - rc.top + 1;
		
		// int iLen = (int)strlen(m_wndInfo.szName);
		// int iTextStartX = iX + iW/2 - pLayer2D->CalcTextWidth(m_tWindowInfo.szName,iLen)/2;
		// int iTextStartY = iY + iH/2 - pLayer2D->GetFontHeight()/2;

		int iTextStartX = iX + iW/2;
		int iTextStartY = iY + iH/2;

		// pLayer2D->DrawWireQuad(iX,iY,iW,iH,m_wndInfo.colorHigh);

		// shadow
		if ( ms_iHotID != INVALID && m_iID == ms_iHotID )
		{
			m_bChecked = true;
			if ( m_dwTextureHandle != INVALID )
			{
				pLayer2D->DrawTexture(iX-2, iY - 2, iW + 4, iH + 4, m_dwTextureHandle);
			}
			pLayer2D->DrawWireQuad(iX-3, iY - 3, iW + 6, iH + 6, m_wndInfo.colorHigh);
			pLayer2D->DrawWireQuad(iX-2, iY - 2, iW + 4, iH + 4, m_wndInfo.colorHigh);
			pLayer2D->DrawWireQuad(iX-1, iY - 1, iW + 2, iH + 2, m_wndInfo.colorHigh);
			m_bHover = false;
		}
		else
		{
			m_bChecked = false;
			if ( m_dwTextureHandle != INVALID )
			{
				pLayer2D->DrawTexture(iX,iY,iW,iH,m_dwTextureHandle);
			}
		}

		if ( m_bHover )
			pLayer2D->DrawWireQuad(iX - 2, iY - 2, iW + 4, iH + 4,m_wndInfo.colorHigh);
	}
}

void CCheckBox::Update(ip::CInputDevice::SEvent & event)
{
	CWnd::UpdateDefault();
	
	POINT ptMousePos;
	ip::CInputDevice::GetInstance()->GetMousePosition((int&)ptMousePos.x,(int&)ptMousePos.y);
	
	DWORD & dwStyle = m_wndInfo.dwStyle;
	BOOL  & bActive = m_wndInfo.bActive;

	if ( (dwStyle & BWS_VISIBLE) && !(dwStyle & BWS_DISABLE))
	{
		BOOL bMouseOver = ( PtInRect(&ptMousePos) > 0 );

		if ( bMouseOver ) 
			m_bHover = true;
		else
			m_bHover = false;

		if ( bMouseOver && m_pParent->IsActive() || bActive )
		{
			switch ( event.eType )
			{
			case EUIMT_MS_BTNDOWN:
				if ( event.dwData == MK_LBUTTON )
				{
					ms_iHotID = m_iID;
					m_bClicked = true;
				}
			break;
			
			default:break;
			}
		}
	}
}	

bool CCheckBox::IsClicked()
{
	bool bClicked = m_bClicked;
	m_bClicked = false;
	return bClicked;
}

void CCheckBox::SetChecked(bool bFlag)
{
	m_bChecked = bFlag;
}

void CCheckBox::SetTextureHandle(DWORD dwTextureHandle)
{
	m_dwTextureHandle = dwTextureHandle;
}

//////////////////////////////////////////////////////////////////////////
// manager
//////////////////////////////////////////////////////////////////////////

CUIManager * CUIManager::one = NULL;

CUIManager::CUIManager()
{
	m_pActiveWnd = NULL;
	m_pMainWnd = NULL;
	m_PopupWindowState.m_ePopupWindowState = EUPS_NONE;
	m_PopupWindowState.pTargetWnd = NULL;
	m_PopupWindowState.pOldActive = NULL;
	m_bEdit = FALSE;
}

void CUIManager::Update(ip::CInputDevice::SEvent & event)
{
	POINT ptMousePos;
	ip::CInputDevice::GetInstance()->GetMousePosition((int&)ptMousePos.x,(int&)ptMousePos.y);

	if ( m_PopupWindowState.m_ePopupWindowState != EUPS_NONE )
	{
		switch ( m_PopupWindowState.m_ePopupWindowState )
		{
		case EUPS_BEGIN:
			{
				m_PopupWindowState.pOldActive = m_pActiveWnd;
				ActiveWindow(m_PopupWindowState.pTargetWnd);
				m_PopupWindowState.m_ePopupWindowState = EUPS_RUNNING;
			}
		break;
		
		case EUPS_RUNNING:
		break;
		
		case EUPS_END:
			{
				ActiveWindow(m_PopupWindowState.pOldActive);
				m_PopupWindowState.m_ePopupWindowState = EUPS_NONE;
			}
		break;
		}
	}
	
	// default messag handle
	switch( event.eType )
	{ 
	case EUIMT_MS_BTNUP:
	
	break;
	
	case EUIMT_MS_BTNDOWN:
		{
			switch ( event.dwData )
			{
			case MK_LBUTTON:
			case MK_MBUTTON:
			case MK_RBUTTON:
				{
					if ( m_PopupWindowState.m_ePopupWindowState == EUPS_NONE )
					{
						CWnd * pWndOver = NULL;
						m_pMainWnd->HitTest(&ptMousePos, &pWndOver);

						if ( pWndOver )
							ActiveWindow(pWndOver);
					}	
					else
					{
						CWnd * pWndOver = NULL;
						m_PopupWindowState.pTargetWnd->HitTest(&ptMousePos,&pWndOver);
						if ( pWndOver )
							ActiveWindow(pWndOver);
					}
				}
			break;
			}
		}
	break;
	
	//case EUIMT_KB_KEYDOWN:
	//	{	
	//		// put in debug mode
	//		if ( event.dwData == DIK_U )
	//		{
	//		//	if ( m_InputInfoBuffer.byKeyBuffer[DIK_LCONTROL] & 0x80
	//		//		&& m_InputInfoBuffer.byKeyBuffer[DIK_LSHIFT] & 0x80
	//		//		&& m_InputInfoBuffer.byKeyBuffer[DIK_LALT] & 0x80 )
	//			{
	//				if ( m_bEdit )
	//				{
	//					m_bEdit = FALSE;
	//					ModifyStyle(m_pMainWnd, 0, BWS_DEBUG);
	//				}
	//				else
	//				{
	//					m_bEdit = TRUE;
	//					ModifyStyle(m_pMainWnd, BWS_DEBUG, 0);
	//				}
	//			}
	//		}
	//		
	//		if ( m_bEdit )
	//		{
	//			switch( event.dwData )
	//			{
	//			case DIK_S:
	//				if ( m_InputInfoBuffer.byKeyBuffer[DIK_LCONTROL] & 0x80 )
	//				{

	//				}
	//			break;
	//			
	//			case DIK_R:
	//				if ( event.byKeyBuffer[DIK_LCONTROL] & 0x80 )
	//				{
	//				} 
	//			break;
	//			
	//			case DIK_UP:
	//				if ( m_pActiveWnd )
	//					if ( m_InputInfoBuffer.byKeyBuffer[DIK_LSHIFT] & 0x80 )
	//					{
	//						m_pActiveWnd->MoveWnd(0,-1);
	//						m_pActiveWnd->SizeWnd(0,1);	
	//					}
	//					else
	//						if ( m_InputInfoBuffer.byKeyBuffer[DIK_LCONTROL] & 0x80 )
	//						{
	//							m_pActiveWnd->SizeWnd(0,-1);	
	//						}
	//						else
	//							m_pActiveWnd->MoveWnd(0,-1);
	//			break;
	//			
	//			case DIK_DOWN:
	//				if ( m_pActiveWnd )
	//					if ( m_InputInfoBuffer.byKeyBuffer[DIK_LSHIFT] & 0x80 )
	//					{
	//						m_pActiveWnd->SizeWnd(0,1);	
	//					}
	//					else if ( m_InputInfoBuffer.byKeyBuffer[DIK_LCONTROL] & 0x80 )
	//					{
	//						m_pActiveWnd->MoveWnd(0,1);
	//						m_pActiveWnd->SizeWnd(0,-1);	
	//					}
	//					else
	//						m_pActiveWnd->MoveWnd(0,1);			
	//			break;	  
	//			
	//			case DIK_LEFT:
	//				if (m_pActiveWnd)
	//					if ( m_InputInfoBuffer.byKeyBuffer[DIK_LSHIFT] & 0x80 )
	//					{
	//						m_pActiveWnd->MoveWnd(-1,0);
	//						m_pActiveWnd->SizeWnd(1,0);	
	//					}
	//					else if ( m_InputInfoBuffer.byKeyBuffer[DIK_LCONTROL] & 0x80 )
	//					{
	//						m_pActiveWnd->SizeWnd(-1,0);	
	//					}
	//					else
	//						m_pActiveWnd->MoveWnd(-1,0);
	//			break;

	//			case DIK_RIGHT:
	//				if ( m_pActiveWnd )
	//					if ( m_InputInfoBuffer.byKeyBuffer[DIK_LSHIFT] & 0x80 )
	//					{
	//						m_pActiveWnd->MoveWnd(0,0);
	//						m_pActiveWnd->SizeWnd(1,0);	
	//					}
	//					else if ( m_InputInfoBuffer.byKeyBuffer[DIK_LCONTROL] & 0x80 )
	//					{
	//						m_pActiveWnd->MoveWnd(1,0);
	//						m_pActiveWnd->SizeWnd(-1,0);	
	//					}
	//					else
	//						m_pActiveWnd->MoveWnd(1,0);
	//			break;
	//			}			
	//		}
	//	}
	//break;
	
	default:break;
	}
	
	if ( m_pMainWnd )
		m_pMainWnd->Update(event);
}

void CUIManager::Render()
{
	if ( m_pMainWnd )
		m_pMainWnd->Render();
}

void CUIManager::InactiveWindow(CWnd * pWnd)
{
	for ( std::list<CWnd *>::iterator it = pWnd->m_listChild.begin(); it != pWnd->m_listChild.end(); ++it )
	{
		CWnd * pWnd = *it; 
		InactiveWindow(pWnd);
	}
	pWnd->m_wndInfo.bActive = FALSE;
}

void CUIManager::BeginPopupWnd(CWnd * pWnd)
{
	// set a window as pop up
	m_PopupWindowState.m_ePopupWindowState = EUPS_BEGIN;
	m_PopupWindowState.pTargetWnd = pWnd;	
}

void CUIManager::EndPopupWnd()
{
	m_PopupWindowState.m_ePopupWindowState = EUPS_END;
	m_PopupWindowState.pTargetWnd = NULL;	
}

void CUIManager::ActiveWindow(CWnd * pWnd)
{
	InactiveWindow(m_pMainWnd);
	m_pActiveWnd = pWnd;	
	if ( pWnd )
	{
		pWnd->m_wndInfo.bActive = TRUE;
		CWnd * pChild = pWnd;
		CWnd * pParent = pChild->GetParent();
		CWnd * pTemp = NULL;
		//modify order
		
		while ( pChild )
		{
			pTemp = pParent;

			if ( pParent )
			{
				pParent->RemoveChild(pChild);
				pParent->AddChild(pChild);
				pParent = pParent->GetParent();
			}
			pChild = pTemp;
		}
	}
}

BOOL CUIManager::Create()
{
	return true;
}

void CUIManager::Destroy(void)
{
	
}

void CUIManager::SetMainWnd(CWnd * pWnd)
{
	m_pMainWnd = pWnd;
	if ( pWnd )
		ActiveWindow(pWnd);
}

void CUIManager::ModifyStyle(CWnd * pWnd, DWORD dwAdd, DWORD dwRemove)
{
	pWnd->ModifyStyle(dwAdd,dwRemove);
	for ( std::list<CWnd *>::iterator it = pWnd->m_listChild.begin(); it != pWnd->m_listChild.end(); ++it )
	{
		CWnd * pWnd = *it; 
		ModifyStyle(pWnd,dwAdd,dwRemove);
	}
}

}
}