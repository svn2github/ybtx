// MakeGMCmdAssistant.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "GmLang.h"
#include "CExpLog.h"
#include "CSplitInfo.h"

int _tmain(int argc, _TCHAR* argv[])
{
	_set_invalid_parameter_handler(ReportInvaidParamentFun);
	//_set_se_translator(ReportESHFun);
	CSplitInfo::GetInst()->Init(argc, argv);
	BeginToPrint("MakeGMCmdAssistant");
	CGmLang::GmLang();
	EndToPrint();
	return 0;
}

