// MakeGermanReferTxt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "CGermanRefer.h"
#include "ConsoleInfo.h"
#include "CSplitInfo.h"
#include "CExpLog.h"

int _tmain(int argc, _TCHAR* argv[])
{
	_set_invalid_parameter_handler(ReportInvaidParamentFun);
	_set_se_translator(ReportESHFun);
	CSplitInfo::GetInst()->Init(argc, argv);
	if(!CSplitInfo::GetInst()->GetBeInternational())
		return 0;
	BeginToPrint("MakeGermanReferTxt");
	CConsoleInfo::GetInst()->PrintFunction("德语任务索引关系表生成");
	FileTreeWalk( gbk_to_utf16(CSplitInfo::GetInst()->GetReferPath()).c_str(), CGermanRefer::CreateRefer, NULL );
	CGermanRefer::GetInst()->WriteToGermanRefer();
	EndToPrint();
	return 0;
}

