// LangWordStatistic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "ChineseWordStatistic.h"
#include "CSplitInfo.h"

int _tmain(int argc, _TCHAR* argv[])
{
	_set_invalid_parameter_handler(ReportInvaidParamentFun);
	_set_se_translator(ReportESHFun);
	setlocale(LC_CTYPE, "");
	CSplitInfo::GetInst()->Init(argc, argv);
	CChineseWordStatistic::GetInst()->StartProcess();
	//string strTestPath = "E:/ybtxlangfiles/German/chinese_simplified1152363_2011-03-03b";
	//string strTestTwoPath = "E:/ybtx/designer/tools/配置表工具集/germanfolder";
	//CChineseWordStatistic::GetInst()->FolderCompare(strTestPath,strTestTwoPath);
	return 0;
}

