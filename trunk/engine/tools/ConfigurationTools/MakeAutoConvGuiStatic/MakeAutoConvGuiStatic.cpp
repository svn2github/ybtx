// MakeAutoConvGuiStatic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CCopyGUIStaticText.h"
#include "ConsoleInfo.h"
#include "common.h"
#include "CSplitInfo.h"
#include "CExpLog.h"



int _tmain(int argc, _TCHAR* argv[])
{
	_set_invalid_parameter_handler(ReportInvaidParamentFun);
	_set_se_translator(ReportESHFun);
	CSplitInfo::GetInst()->Init(argc, argv);
	if(!CSplitInfo::GetInst()->GetBeInternational())
		return 0;
	BeginToPrint("MakeAutoConvGuiStatic");
	CConsoleInfo::GetInst()->PrintFunction("繁体GUIStaticText表生成");
	CCopyGUIStaticText::GetInst()->StartProcess();
	EndToPrint();
	return 0;
}

