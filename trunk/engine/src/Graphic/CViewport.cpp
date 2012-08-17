#include "stdafx.h"
#include "CViewport.h"
#include "CWindowTarget.h"

CViewport::CViewport(CWindowTarget* pParentWindow)
		: m_pParentWindow(pParentWindow)
{
	m_cmtClear = CMT_CLEAR_ZBUFFER_STENCI;
}

CViewport::~CViewport()
{
	m_pParentWindow->DeleteViewport(this);
}

void CViewport::Render(void)
{
	if(m_rcViewPort.right <= m_rcViewPort.left + 1 || 
		m_rcViewPort.bottom <= m_rcViewPort.top + 1 )
		return;

	UpdateViewport();
	Clear();
	CRenderTarget::Render();
}

void	CViewport::SetRect( const CIRect& rect )
{
	CIRect tRect;
	if(m_pParentWindow)
	{		
		m_pParentWindow->GetRect(tRect);
		tRect.top	= max(rect.top,tRect.top);
		tRect.left	= max(rect.left,tRect.left);
		tRect.right	= min(rect.right,tRect.right);
		tRect.bottom= min(rect.bottom,tRect.bottom);
	}
	else
		tRect = rect;
	tRect.right = max(tRect.left,tRect.right);
	tRect.bottom= max(tRect.top,tRect.bottom);
	CRenderTarget::SetRect(tRect);
}