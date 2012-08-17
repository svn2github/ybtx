#include "stdafx.h"
#include "SQRGUITransparentEffect.h"

SQRGUITransparentEffect::SQRGUITransparentEffect(SQRWnd* wnd)
: m_wnd(wnd)
, m_systime(0)
, m_runtime(0)
, m_TransparentValue(0.0f)
, m_OldTransparentValue(0.0f)
, m_bValue(TRUE)
{}

SQRGUITransparentEffect::~SQRGUITransparentEffect()
{

}

void SQRGUITransparentEffect::ExecuteEffect()
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
			fvalue *= m_TransparentValue;

			_SetTransparent(m_OldTransparentValue + fvalue);
		}
		else
		{
			_EndTransparent(m_bValue);
		}
	}
	else
		m_die = true;
}

void SQRGUITransparentEffect::SetTransparentObj(int32 runtime, bool bvalue)
{
	if ( !m_wnd )
	{
		m_die = true;
		return;
	}
	if ( 0 != runtime)
	{
		m_systime             = static_cast<DWORD>(GetProcessTime());
		m_runtime             = runtime;
		m_OldTransparentValue = m_wnd->GetTransparent();
		m_TransparentValue    = static_cast<FLOAT>(bvalue ? -m_OldTransparentValue : 1.0 - m_OldTransparentValue);
		m_bValue = bvalue;
		m_die = false;
		if ( FALSE == bvalue )
		{
			m_wnd->ShowWnd(true);
		}
	}
	else if ( 0 == runtime )
	{
		m_bValue = bvalue;
		if ( FALSE == bvalue )
		{
			m_wnd->ShowWnd(true);
		}
		bvalue ? _SetTransparent(0.0f) : _SetTransparent(1.0f);
		_EndTransparent(m_bValue);
	}
}

void SQRGUITransparentEffect::EndEffect()
{
	_EndTransparent(m_bValue);
}

void SQRGUITransparentEffect::EndTransparent( bool bvalue )
{
	_EndTransparent(bvalue);
}

void SQRGUITransparentEffect::_EndTransparent( bool bvalue)
{
	m_systime = 0;
	m_runtime = 0;
	m_TransparentValue = 0.0f;
	m_OldTransparentValue = 0.0f;

	if ( bvalue == FALSE )
	{
		_SetTransparent(1.0f);
		m_wnd->ShowWnd(true);
	}
	else
		m_wnd->ShowWnd(false);
	m_die = true;
}

void  SQRGUITransparentEffect::_SetTransparent( float fTrans )
{
	m_wnd->SetTransparent(fTrans);
}

EGUIEffect SQRGUITransparentEffect::GetEffectType()
{
	return eEff_Transparent;
}
