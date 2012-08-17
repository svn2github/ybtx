#include "StdAfx.h"
#include "CLightEffectPanel.h"
#include "GridCtrl\CGridCtrlImp.h"

void CLightEffectPanel::Update(void)
{
	CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
	if(!pImp)
		return ;
}

char* CLightEffectPanel::GetImpTypeName(void)
{
	return GET_IMP_NAME(CGridCtrlImp);
}
