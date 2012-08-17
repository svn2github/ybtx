// DeleteRedundantCfg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "DeleteCfg.h"
#include <cstdlib>
#include "CSplitInfo.h"

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_CTYPE,"");
	_set_invalid_parameter_handler(ReportInvaidParamentFun);
	_set_se_translator(ReportESHFun);
	CSplitInfo::GetInst()->Init(argc, argv);
	CDeleteCfg::GetInst()->StartProcess();
	system("pause");
	return 0;
}

