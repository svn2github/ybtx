// MakeAppFinallyLua.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "CAppSrc2Lua.h"
#include "CSplitInfo.h"

int _tmain(int argc, _TCHAR* argv[])
{
	_set_invalid_parameter_handler(ReportInvaidParamentFun);
	_set_se_translator(ReportESHFun);
	CSplitInfo::GetInst()->Init(argc, argv);
	CDif2Lua::GetInst()->StartProcess();
	return 0;
}

