// MakeAppFinallyTxt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "CAppSrc2Txt.h"
#include "CExpLog.h"
#include "CSplitInfo.h"

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_CTYPE,"");
	_set_invalid_parameter_handler(ReportInvaidParamentFun);
	_set_se_translator(ReportESHFun);
	uint64 StratTime = GetCurrentTime();
	CSplitInfo::GetInst()->Init(argc, argv);
	BeginToPrint("MakeAppFinallyTxt");
	CDif2Txt::GetInst()->StartProcess();
	uint64 TotalTime = GetCurrentTime()-StratTime;
	printf("dif转txt全部完成，共计耗时%d\n",TotalTime);
	EndToPrint();
	return 0;
}

