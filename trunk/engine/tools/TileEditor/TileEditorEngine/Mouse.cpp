//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Mouse.h"
#include "Render.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace bg
{
namespace sc
{

void CMouse::OnEvent(ip::CInputDevice::SEvent & event)
{
	switch ( event.eType )
	{
	case ip::EIET_MS_MOVE :
		{
			m_vPt.x = LOWORD(event.lParam);
			m_vPt.y = HIWORD(event.lParam);
		}
	break;

	case ip::EIET_MS_BTNDOWN :
		{
			if ( event.dwData == MK_LBUTTON )
				m_bClicked = true;
		}
	break;

	case ip::EIET_MS_BTNUP :
		{
			if ( event.dwData == MK_LBUTTON )
				m_bClicked = false;
		}
	break;
	}
}

void CMouse::Render()
{
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();
	rd::CRenderDevice::Layer2D * pLayer2D = pRender->GetLayer2D();
	if ( !m_bClicked )
		pLayer2D->DrawTexture(static_cast<int>(m_vPt.x),static_cast<int>(m_vPt.y),16,16,m_dwTextureHandleIdle);
	else
		pLayer2D->DrawTexture(static_cast<int>(m_vPt.x),static_cast<int>(m_vPt.y),16,16,m_dwTextureHandleClicked);
}

void CMouse::SetMouseTexture(DWORD dwTextureHandleIdle,DWORD dwTextureHandleClicked)
{
	m_dwTextureHandleIdle = dwTextureHandleIdle;
	m_dwTextureHandleClicked = dwTextureHandleClicked;
}

}
}
