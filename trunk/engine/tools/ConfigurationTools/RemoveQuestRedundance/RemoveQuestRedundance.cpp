// RemoveQuestRedundance.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "QuestRedundancyResolver.h"
#include "ConsoleInfo.h"
#include "CExpLog.h"

int _tmain(int argc, _TCHAR* argv[])
{
	_set_invalid_parameter_handler(ReportInvaidParamentFun);
	_set_se_translator(ReportESHFun);
	CSplitInfo::GetInst()->Init(argc, argv);
	if(!CSplitInfo::GetInst()->GetBeInternational())
		return 0;
	BeginToPrint("RemoveQuestRedundance");
	FileTreeWalk( gbk_to_utf16(CSplitInfo::GetInst()->GetQuestPath()).c_str(), CQuestRedundancyResolver::ResolveDesc, NULL );
	FileTreeWalk( gbk_to_utf16(CSplitInfo::GetInst()->GetQuestTempPath()).c_str(), CQuestRedundancyResolver::ResolveDesc, NULL );
	EndToPrint();
	return 0;
}

