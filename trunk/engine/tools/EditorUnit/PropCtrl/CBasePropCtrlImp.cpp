#include "StdAfx.h"
#include "PropCtrl\CBasePropCtrlImp.h"

CBasePropCtrlImp::CBasePropCtrlImp(CEditContext* context)
:m_pOwnContext(context)
{
	m_PropType=PROP_BASE;
}

CBasePropCtrlImp::~CBasePropCtrlImp(void)
{
}

void CBasePropCtrlImp::SetContext(CEditContext* context)
{
	m_pOwnContext = context;
}
