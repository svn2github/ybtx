// SimilarWordReferTxt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "CommonSeqReference.h"
#include "ConsoleInfo.h"
#include "CExpLog.h"

int _tmain(int argc, _TCHAR* argv[])
{
	_set_invalid_parameter_handler(ReportInvaidParamentFun);
	_set_se_translator(ReportESHFun);
	CSplitInfo::GetInst()->Init(argc, argv);
	if(!CSplitInfo::GetInst()->GetBeInternational())
		return 0;
	BeginToPrint("SimilarWordReferTxt");
	CCommonSeqReference::GetInst()->BeginItemDesc();
	CCommonSeqReference::GetInst()->BeginQuestDesc();
	EndToPrint();
	return 0;
}

