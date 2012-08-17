#include "stdafx.h"
#include "SQRGUIShapeEffect.h"

SQRGUIShapeEffect::SQRGUIShapeEffect(SQRWnd* wnd)
: m_wnd(wnd)
{
	if (m_wnd)
		m_wndZoom = m_wnd->GetWndZoom();
	else
		m_wndZoom = 1.0f;
	
}

SQRGUIShapeEffect::~SQRGUIShapeEffect()
{
}

EGUIEffect SQRGUIShapeEffect::GetEffectType()
{
	return eEff_Shape;
}

void SQRGUIShapeEffect::ExecuteEffect()
{
	if (!m_wnd)
	{
		m_die = true;
		return;
	}

	if ( 0 != m_systime )
	{
		DWORD value = static_cast<DWORD>(GetProcessTime() - m_systime);
		if ( value < static_cast<DWORD>(m_runtime) )
		{
			FLOAT fvalue = (FLOAT)value / m_runtime;

			_SetShape( fvalue );
		}
		else
		{
			_EndShape();
		}
	}
	else
		m_die = true;
}

void SQRGUIShapeEffect::EndEffect()
{
	_EndShape();
}

void SQRGUIShapeEffect::_SetShape(float percent)
{
	// µ±Ç°±ÈÀý
	percent = percent < 1.0f ? percent : 1.0f;
	CFRect temp_rect;
	CFPos temp_pos = m_souPos + (m_desPos - m_souPos) * percent;
	float cur_with = (m_souRt.right - m_souRt.left) 
				   + (m_desRt.right - m_desRt.left - m_souRt.right + m_souRt.left) * percent;
	float cur_height = (m_souRt.bottom - m_souRt.top) 
				   + (m_desRt.bottom - m_desRt.top - m_souRt.bottom + m_souRt.top) * percent;

	temp_rect.left = temp_pos.x - cur_with/2.0f;
	temp_rect.right = temp_pos.x + cur_with/2.0f;
	temp_rect.top = temp_pos.y - cur_height/2.0f;
	temp_rect.bottom = temp_pos.y + cur_height/2.0f;
	
	float per = percent;
	if (m_wndRt.Height() != 0.0f && m_wndRt.Width() != 0.0f)
	{
		per = (temp_rect.Height()/m_wndRt.Height() + temp_rect.Width()/m_wndRt.Width())/2.0f;
	}
	
	float temp_zoom = m_wndZoom * per;
	m_wnd->SetWndZoom(temp_zoom);
	m_wnd->ResizeRect(temp_rect, false);

	if (percent == 1.0f )
		m_die = true;
}

void SQRGUIShapeEffect::_EndShape()
{
	_SetShape(1.0f);
	if (m_wnd)
	{
		m_wnd->SetWndZoom(m_wndZoom);
		m_wnd->ResizeRect(m_wndRt, false);
		m_wnd->OnShapeEffectEnded();
	}
	m_die = true;
}

void SQRGUIShapeEffect::SetShapeEffect(int32 runtime, CFRect src, CFRect des)
{
	if ( !m_wnd || runtime < 0 )
	{
		m_die = true;
		return;
	}
	m_souRt	= src;
	m_desRt = des;

	m_souPos.x	= (m_souRt.left + m_souRt.right) / 2.0f;
	m_souPos.y	= (m_souRt.top + m_souRt.bottom) / 2.0f;
	m_desPos.x	= (m_desRt.left + m_desRt.right) / 2.0f;
	m_desPos.y	= (m_desRt.top + m_desRt.bottom) / 2.0f;

	if (m_die == true)
		m_wndZoom = m_wnd->GetWndZoom();
	m_wnd->GetWndRectBak(m_wndRt);

	if ( 0 != runtime)
	{
		m_systime             = static_cast<DWORD>(GetProcessTime());
		m_runtime             = runtime;
		m_die = false;
	}
	else if ( 0 == runtime )
	{
		m_wnd->ResizeRect(m_desRt, false);
		m_die = true;
	}
}

