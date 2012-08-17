// MakeGermanLanFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GermanLang.h"
#include "CSplitInfo.h"
#include "common.h"
#include "ConsoleInfo.h"
#include "CExpLog.h"
#include "CSplitInfo.h"

int _tmain(int argc, _TCHAR* argv[])
{
	_set_invalid_parameter_handler(ReportInvaidParamentFun);
	_set_se_translator(ReportESHFun);
	CSplitInfo::GetInst()->Init(argc, argv);
	if(!CSplitInfo::GetInst()->GetBeInternational())
		return 0;
	BeginToPrint("MakeGermanLanFile");
	CConsoleInfo::GetInst()->PrintFunction("德语语言翻译表生成");
	FileTreeWalk( gbk_to_utf16(CSplitInfo::GetInst()->GetLangSrcPath()).c_str(), CGermanLang::FormatUtf2Utf16, NULL );
	EndToPrint();
	return 0;
}

