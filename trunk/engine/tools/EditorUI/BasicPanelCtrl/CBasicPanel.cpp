#include "StdAfx.h"
#include "CBasicPanel.h"
#include "RecordCtrl/CRecordCtrlImp.h"

void CBasicPanel::Update(void)
{
}

char* CBasicPanel::GetImpTypeName(void)
{
	return GET_IMP_NAME(CRecordCtrlImp);
}

