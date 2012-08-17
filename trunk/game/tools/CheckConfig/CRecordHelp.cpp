#include "stdafx.h"
#include "CRecordHelp.h"
#include "CRecordManager.h"

bool BeNeededToRun(TCHAR* szFunName)
{
	return CRecordManager::GetInst()->BeNeedCheck(szFunName);
}

void UpdateRecord(TCHAR* szFunName,bool bOK)
{
	CRecordManager::GetInst()->SetRecordState(szFunName,bOK);
}