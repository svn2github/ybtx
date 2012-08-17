// MakeGermanGuiStatic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CGUIStaticText.h"
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
	BeginToPrint("MakeGermanGuiStatic");
	CConsoleInfo::GetInst()->PrintFunction("德语GUIStaticText表生成");
	CGUIStaticText::GetInst()->ConverseToGermanLangSrc();
	EndToPrint();
	return 0;
}

