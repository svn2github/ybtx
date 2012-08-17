#include "stdafx.h"
#include "CEventDelegate.h"
#include "CEventOwner.h"

CEventDelegate::CEventDelegate(void)
:m_pOwner(NULL)
{

}

CEventDelegate::~CEventDelegate(void)
{

}

bool CEventDelegate::MsgProc(const CMsg& msg)
{
	switch( msg.msgSource )
	{
	case MGS_SYSMSG:
		switch( msg.msgType.sys_type )
		{
		case MGT_SYS_INIT:
			if(m_pOwner)_Init(m_pOwner);
			break;
		case MGT_SYS_RENDER:
			return false;
		}
		break;
	}
	return true;
}

void CEventDelegate::UnBind(void)
{
	if(m_pOwner)
	{
		m_pOwner->UnRegisterDelegate(this);
		m_pOwner = NULL;
	}
}

void CEventDelegate::Active(void)
{
	if(m_pOwner)
	{
		m_pOwner->ActiveDelegate(this);
	}
}

void CEventDelegate::_Init(CEventOwner* pOwner)
{
	
}

void sqr::CEventDelegate::Render( void )
{

}

void sqr::CEventDelegate::_UnActive( void )
{
}