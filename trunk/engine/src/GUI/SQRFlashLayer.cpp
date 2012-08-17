#include "stdafx.h"
#include "SQRFlashLayer.h"
#include "SQRRootWnd.h"
#include "CGraphic.h"
#include "SQRGUIManager.h"

SQRFlashLayer::SQRFlashLayer(void){}

SQRFlashLayer::SQRFlashLayer(UIString flashTex, const CFRect& rt, SQRWnd* pChildWnd, 
			uint32 ct, float v, float lx, float ty ,float rx, float by)
: m_texStr(flashTex)
, m_pParentWnd(pChildWnd)
, m_rtTex(rt)
, m_v(v)
, m_count(ct)
, m_lx(lx)
, m_ty(ty)
, m_rx(rx)
, m_by(by)
, m_pTexture(NULL)
, m_bDel(false)
, m_bAdd(true)
, m_color(0xffffffff)
, m_fTransParent(1.0f)
, m_curSysTime((DWORD)0)
, m_flash(NULL)
, m_Amax(1.0f)
, m_Amin(0.0f)
{
	InitTexture(m_texStr);
}

SQRFlashLayer::SQRFlashLayer(FlashWndInfo* flash, SQRWnd* pChildWnd)
: m_flash(flash)
, m_pParentWnd(pChildWnd)
, m_bDel(false)
, m_bAdd(flash->m_bAdd)
, m_color(0xffffffff)
, m_curSysTime((DWORD)0)
, m_pTexture(NULL)
, m_Amax(flash->m_Amax)
, m_Amin(flash->m_Amin)
, m_v((float)(flash->m_time))
, m_curCount(0)
, m_count(flash->m_count)
{
	if (flash)
		flash->CreateTexture();

	m_Amax = m_Amax > m_Amin ? m_Amax : m_Amin;
	limit2(m_Amax, 0.0f, 1.0f);
	limit2(m_Amin, 0.0f, 1.0f);
	if ( flash->m_bAdd )
		m_fTransParent = flash->m_Amax;
	else
		m_fTransParent = flash->m_Amin;
	if ( pChildWnd )
		pChildWnd->SetBGIsDrawed(false);
}

void SQRFlashLayer::InitTexture(UIString stex)
{
	ITexture* pTexture = NULL;

	SafeRelease(m_pTexture);
	if ( stex.c_str() && stex[0] )
	{
		HRESULT hr = CGraphic::GetInst()->CreateTexture( PATH_ALIAS_GUITEX.c_str(), stex.c_str(), &pTexture ); // szTextName
		if(FAILED(hr))
			OutputDebugString(stex.c_str());
		if ( !pTexture )
			return;
	}

	m_pTexture	= pTexture;
}

void SQRFlashLayer::ReInitFlash(FlashWndInfo* flash)
{
	if ( !flash )
	{
		m_bDel = true;
		return;
	}
	if (m_flash)
		m_flash->ReleaseTexture();
	m_flash = flash;
	m_flash->CreateTexture();

	m_bDel = false;
	m_bAdd =flash->m_bAdd;
	m_color= 0xffffffff;

	if ( flash->m_bAdd )
		m_fTransParent = flash->m_Amax;
	else
		m_fTransParent = flash->m_Amin;
	m_curSysTime = (DWORD)0;
	SafeRelease(m_pTexture);

	m_Amax=flash->m_Amax;
	m_Amin=flash->m_Amin;
	m_v=(float)(flash->m_time);

	m_Amax = m_Amax > m_Amin ? m_Amax : m_Amin;
	limit2(m_Amax, 0.0f, 1.0f);
	limit2(m_Amin, 0.0f, 1.0f);
	m_curCount = 0;
	m_count = flash->m_count;
}

SQRFlashLayer::~SQRFlashLayer(void)
{
	SafeRelease(m_pTexture);
	if (m_flash)
		m_flash->ReleaseTexture();
}

void SQRFlashLayer::DrawBackground()
{
	// 更新flash
	if ( RefreshFlash() )
	{
		// 画flash
		if ( m_pParentWnd && m_pParentWnd->IsShow() )
		{
#ifdef UI_HIDE_MODE
			if (m_pParentWnd->GetWndHandle()->m_pGUI->GetHidenMode() 
				&& !(m_pParentWnd->IsModleState()) )
				return;
#endif
			if ( m_pParentWnd->GetCutFlash() )
			{
				if (m_pParentWnd->GetStyle() & WS_CUTBYPARENT)
				{
					CFRect rtParent = m_pParentWnd->GetParentCutRect();
					rtParent.left	+= m_pParentWnd->GetRootWnd()->GetWndHandle()->m_xScreen;
					rtParent.right	+= m_pParentWnd->GetRootWnd()->GetWndHandle()->m_xScreen;
					rtParent.top	+= m_pParentWnd->GetRootWnd()->GetWndHandle()->m_yScreen;
					rtParent.bottom += m_pParentWnd->GetRootWnd()->GetWndHandle()->m_yScreen;

					CFRect m_cutPos = m_curPos.Intersection(rtParent);
					if (m_cutPos.IsEmpty())
						return;
					else
					{
						if ( m_pTexture && m_flash == NULL )
						{
							CColor col = m_color;
							CFRect texRT = m_rtTex;

							texRT.left = texRT.left + texRT.Width() * (m_curPos.left - m_cutPos.left) / m_curPos.Width();
							texRT.right = texRT.right - texRT.Width() * (m_curPos.right - m_cutPos.right) / m_curPos.Width();
							texRT.top = texRT.top + texRT.Height() * (m_curPos.top - m_cutPos.top ) / m_curPos.Height();
							texRT.bottom = texRT.bottom - texRT.Height() * (m_curPos.bottom - m_cutPos.bottom) / m_cutPos.Height();
							col.A = static_cast<uint8>( col.A*m_fTransParent );
							CGraphic::GetInst()->DrawRect(m_cutPos, col, m_pTexture, &texRT);
						}
						else if ( m_flash != NULL && m_flash->m_image.pTexture )
						{
							CColor col = m_color;
							CFRect texRT = m_flash->m_image.rtTexture;

							texRT.left = texRT.left + texRT.Width() * (m_curPos.left - m_cutPos.left) / m_curPos.Width();
							texRT.right = texRT.right - texRT.Width() * (m_curPos.right - m_cutPos.right) / m_curPos.Width();
							texRT.top = texRT.top + texRT.Height() * (m_curPos.top - m_cutPos.top ) / m_curPos.Height();
							texRT.bottom = texRT.bottom - texRT.Height() * (m_curPos.bottom - m_cutPos.bottom) / m_cutPos.Height();
							col.A = static_cast<uint8>( col.A*m_fTransParent );
							CGraphic::GetInst()->DrawRect(m_cutPos, col, m_flash->m_image.pTexture, &texRT);
						}
					}
				}
			}
			else
			{
				if ( m_pTexture && m_flash == NULL )
				{
					CColor col = m_color;
					col.A = static_cast<uint8>( col.A*m_fTransParent );
					CGraphic::GetInst()->DrawRect(m_curPos, col, m_pTexture, &m_rtTex);
				}
				else if ( m_flash != NULL && m_flash->m_image.pTexture )
				{
					CColor col = m_color;
					col.A = static_cast<uint8>( col.A*m_fTransParent );
					CGraphic::GetInst()->DrawRect(m_curPos, col, m_flash->m_image.pTexture, &m_flash->m_image.rtTexture);
				}
			}
		}
	}
}

bool SQRFlashLayer::CheckFlashContShow()
{
	if (m_pParentWnd && m_pParentWnd->IsShow() && m_count != 0 
		&& (m_pTexture && m_flash == NULL || m_flash != NULL && m_flash->m_image.pTexture))
		return true;
	return false;
}

void SQRFlashLayer::SetDrawed()
{
	if (m_pParentWnd)
		m_pParentWnd->SetBGIsDrawed(false);
}

bool SQRFlashLayer::RefreshFlash()
{
	if ( !CheckWillShow() ) return false;
	SQRRefreshTrans();
	if ( !SQRRefreshPos() )	return false;
	if ( !CheckWillShow() ) return false;
	return true;
}

void SQRFlashLayer::SQRRefreshTrans()
{
	int32 det;
	if ( 0 == m_curSysTime )
	{
		m_curSysTime = static_cast<DWORD>( GetProcessTime() );
		det = 0;
	}
	else
	{
		det = static_cast<int32>(GetProcessTime() - m_curSysTime);
		m_curSysTime = static_cast<DWORD>( GetProcessTime() );
	}

	if ( m_bAdd )		/// 透明度增加状态
	{
		float percent  = det * ( m_Amax - m_Amin ) / m_v;
		m_fTransParent -= percent;
		if ( m_fTransParent <= m_Amin )
		{
			m_fTransParent = m_Amin;
			m_bAdd = !m_bAdd;
			++m_curCount;
		}
	}
	else
	{
		float percent = det * ( m_Amax - m_Amin ) / m_v;
		m_fTransParent += percent;
		if ( m_fTransParent >= m_Amax )
		{
			m_fTransParent = m_Amax;
			m_bAdd = !m_bAdd;
			++m_curCount;
		}
	}
	m_curCount = m_curCount < 10000 ? m_curCount : 0;
}

bool SQRFlashLayer::SQRRefreshPos()
{
	// 更新位置 
	if ( m_pParentWnd == NULL )	return false;
	if ( !(m_pParentWnd->GetBGIsDrawed()) )
		return false;

	if ( m_flash )
	{
		m_pParentWnd->GetWndRect( m_curPos );
		float zoom = m_pParentWnd->GetWndZoom();
		m_curPos.left	-= m_flash->m_repos.left * zoom;
		m_curPos.top	-= m_flash->m_repos.top * zoom;
		m_curPos.bottom += m_flash->m_repos.bottom * zoom;
		m_curPos.right	+= m_flash->m_repos.right * zoom;
	}
	else
	{
		if ( m_pParentWnd == NULL )	return false;
		m_pParentWnd->GetWndRect( m_curPos );
		float zoom = m_pParentWnd->GetWndZoom();
		m_curPos.left	-= m_lx * zoom;
		m_curPos.top	-= m_ty * zoom;
		m_curPos.bottom += m_by * zoom;
		m_curPos.right	+= m_rx * zoom;
	}
	m_curPos.left	+= m_pParentWnd->GetRootWnd()->GetWndHandle()->m_xScreen;
	m_curPos.right	+= m_pParentWnd->GetRootWnd()->GetWndHandle()->m_xScreen;
	m_curPos.top	+= m_pParentWnd->GetRootWnd()->GetWndHandle()->m_yScreen;
	m_curPos.bottom += m_pParentWnd->GetRootWnd()->GetWndHandle()->m_yScreen;
	return true;
}

bool SQRFlashLayer::CheckWillShow()
{
	if ( m_curCount >= m_count && m_count != 0 )
	{
		m_bDel = true;
		return false;
	}
	return true;
}
