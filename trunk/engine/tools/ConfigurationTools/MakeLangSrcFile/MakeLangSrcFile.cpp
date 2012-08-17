// MakeLangSrcFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "CSplitCfg.h"
#include "CExpLog.h"
#include "CSplitInfo.h"

int _tmain(int argc, _TCHAR* argv[])
{
	_set_invalid_parameter_handler(ReportInvaidParamentFun);
	_set_se_translator(ReportESHFun);
	CSplitInfo::GetInst()->Init(argc, argv);
	BeginToPrint("MakeLangSrcFile");
	CSplitCfg::GetInst()->StartProcess();
	EndToPrint();
	return 0;
}

