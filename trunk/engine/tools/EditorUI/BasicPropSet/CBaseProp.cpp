#include "stdafx.h"
#include "CBaseProp.h"
#include "CCtrlBase.h"

CBaseProp::CBaseProp(void* ctrl)
{
	m_pCtrl = (CCtrlBase*)ctrl;
}

CBaseProp::~CBaseProp(void)
{
}

void* CBaseProp::GetCoreCtrl()
{
	return m_pCtrl;
}


float CBaseProp::ShowModify(float value)
{
	if(value>0)
	{
		if(value<=0.0000001f)
			value=0.0f;
	}
	else
	{
		if(value>=-0.0000001f)
			value=0.0f;
	}
	return value;
}
