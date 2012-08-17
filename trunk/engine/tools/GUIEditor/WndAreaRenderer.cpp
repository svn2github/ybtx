#include "stdafx.h"
#include "WndAreaRenderer.h"
#include "CGraphic.h"
#include "CWndGroup.h"

namespace sqr
{
	CWndAreaRendere::CWndAreaRendere() 
		: m_bRender(true)
		, m_bRootRender(false)
		, m_renderWndList(NULL)
	{		 
		if(CMainWindowTarget::HasInst())
			CMainWindowTarget::GetInst()->InsertRenderer(this);
	}


	CWndAreaRendere::~CWndAreaRendere()
	{
		if(CMainWindowTarget::HasInst())
			CMainWindowTarget::GetInst()->DeleteRenderer(this);
	}

	void CWndAreaRendere::Render()
	{
		// first render the root rect
		RenderRootRect();

		// then render the wnd rect selected
		if( !m_bRender )
			return;

		CGraphic* pGraphic = CGraphic::GetInst();
		VerColor2D v[5]=
		{
			VerColor2D( (float)m_curRect.left,  (float)m_curRect.top,    0,	0xff0000ff ),
			VerColor2D( (float)m_curRect.right, (float)m_curRect.top,    0,	0xff0000ff ),
			VerColor2D( (float)m_curRect.right, (float)m_curRect.bottom, 0,	0xff0000ff ),
			VerColor2D( (float)m_curRect.left,  (float)m_curRect.bottom, 0,	0xff0000ff ),
			VerColor2D( (float)m_curRect.left,  (float)m_curRect.top,    0,	0xff0000ff ),
		};
		VerColor2D* pVB;
		RenderState * pRS;
		pGraphic->GetRenderStack( RS_FONT, NULL, NULL, PT_LINESTRIP, 5, 4, 
			VerColor2D::Format, sizeof(VerColor2D), (void**)&pVB, NULL, (void**)&pRS );
		pRS->m_ZTestEnable = false;
		pRS->m_ZTestFun    = CMP_ALWAYS;
		memcpy( pVB, v, sizeof( VerColor2D )*5 );

		// render wnd list
		RenderSelWndList();
	}

	void CWndAreaRendere::RenderRootRect()
	{
		if( !m_bRootRender )
			return;

		CGraphic* pGraphic = CGraphic::GetInst();
		VerColor2D v[5]=
		{
			VerColor2D( (float)m_rootRect.left,  (float)m_rootRect.top,    0,	0xffff0000 ),
			VerColor2D( (float)m_rootRect.right, (float)m_rootRect.top,    0,	0xffff0000 ),
			VerColor2D( (float)m_rootRect.right, (float)m_rootRect.bottom, 0,	0xffff0000 ),
			VerColor2D( (float)m_rootRect.left,  (float)m_rootRect.bottom, 0,	0xffff0000 ),
			VerColor2D( (float)m_rootRect.left,  (float)m_rootRect.top,    0,	0xffff0000 ),
		};
		VerColor2D* pVB;
		RenderState * pRS;
		pGraphic->GetRenderStack( RS_FONT, NULL, NULL, PT_LINESTRIP, 5, 4, 
			VerColor2D::Format, sizeof(VerColor2D), (void**)&pVB, NULL, (void**)&pRS );
		pRS->m_ZTestEnable = false;
		pRS->m_ZTestFun    = CMP_ALWAYS;
		memcpy( pVB, v, sizeof( VerColor2D )*5 );
	}
	
	void CWndAreaRendere::RenderSelWndList()
	{
		if( !m_renderWndList || m_renderWndList->size() <= 1 )
			return;
		
		CGraphic* pGraphic = CGraphic::GetInst();
		vector<SQRWnd*>::iterator it = m_renderWndList->begin();
		vector<SQRWnd*>::iterator eit = m_renderWndList->end();
		while(it != eit)
		{
			SQRWnd* pwnd = *it;
			if (pwnd)
			{
				CFRect rt; pwnd->GetWndRect(rt);
				VerColor2D v[5]=
				{
					VerColor2D( (float)rt.left,  (float)rt.top,    0,	0xffff00ff ),
					VerColor2D( (float)rt.right, (float)rt.top,    0,	0xffff00ff ),
					VerColor2D( (float)rt.right, (float)rt.bottom, 0,	0xffff00ff ),
					VerColor2D( (float)rt.left,  (float)rt.bottom, 0,	0xffff00ff ),
					VerColor2D( (float)rt.left,  (float)rt.top,    0,	0xffff00ff ),
				};
				VerColor2D* pVB;
				RenderState * pRS;
				pGraphic->GetRenderStack( RS_FONT, NULL, NULL, PT_LINESTRIP, 5, 4, 
					VerColor2D::Format, sizeof(VerColor2D), (void**)&pVB, NULL, (void**)&pRS );
				pRS->m_ZTestEnable = false;
				pRS->m_ZTestFun    = CMP_ALWAYS;
				memcpy( pVB, v, sizeof( VerColor2D )*5 );
			}
			
			++it;
		}
		
	}

	

	void CWndAreaRendere::SetCurRect( const CRect &rt )
	{
		m_curRect = rt;
	}

	void CWndAreaRendere::SetIsRender( const bool b )
	{
		m_bRender = b;
	}

	void CWndAreaRendere::SetRootRect( const CRect &rt )
	{
		m_rootRect = rt;
	}

	void CWndAreaRendere::SetIsRootRender( const bool b )
	{
		m_bRootRender = b;
	}

	void CWndAreaRendere::SetWndRenderList(vector<SQRWnd*>* plist)
	{
		m_renderWndList = plist;
	}
}