#include "stdafx.h"
#include "CTick.h"
#include "CTickImp.h"
#include "CallBackX.h"
#include "ExpHelper.h"

CTick::CTick()
{
	m_pImp=NULL;
}

CTick::~CTick(void)
{
	UnRegister();
}

void CTick::UnRegister()
{
	if( !m_pImp)
		return;
	m_pImp->UnRegister();
}

bool CTick::Registered()const
{
	return m_pImp != NULL;
}

void CTick::OnTick()
{
	CALL_CLASS_CALLBACK()
}

const char* CTick::GetTickClassName()const
{
	return typeid(*this).name();
}

uint32 CTick::GetInterval()const
{
	if( !m_pImp )
		GenErr("tick not registered yet.");
	return m_pImp->GetInterval();
}

void CTick::SetTickName(const char* szName)
{
	if( !m_pImp )
		GenErr("tick not registered yet.");
	return m_pImp->SetTickName( szName );
}

const char* CTick::GetTickName()const
{
	if( !m_pImp )
		GenErr("tick not registered yet.");
	return m_pImp->GetTickName();
}

uint32 CTick::GetLeftTime()const
{
	if( !m_pImp )
		GenErr("tick not registered yet.");
	return m_pImp->GetLeftTime();
}

