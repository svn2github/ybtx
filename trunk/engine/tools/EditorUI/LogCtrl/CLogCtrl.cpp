#include "StdAfx.h"
#include "CLogCtrl.h"
#include "LogImp/CLogImp.h"

char* CLogCtrl::GetImpTypeName(void)
{
	switch(m_LogType)
	{
	case ELogType::PUBLIC_LOG:
		return GET_IMP_NAME(CSystemLogImp);
		break;
	case ELogType::PRIVATE_LOG:
		return GET_IMP_NAME(CLogCtrlImp);
		break;
	default:
		return NULL;
	}
}

void	 CLogCtrl::Update(void)
{
	CLogCtrlImp* pLog = (CLogCtrlImp*)GetCoreCtrl();
	if(pLog)
	{
		LogCtrlText->Text = UnmagStringToMagString(pLog->GetLog().c_str());
		LogCtrlText->SelectionStart = LogCtrlText->TextLength;
		LogCtrlText->ScrollToCaret();
	}
}