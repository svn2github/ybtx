// ConvDifToSvs.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "CDif2Svs.h"
#include "CSplitInfo.h"

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_CTYPE,"");
	_set_invalid_parameter_handler(ReportInvaidParamentFun);
	_set_se_translator(ReportESHFun);

	uint64 uStartTime = GetCurrentTime();
	CSplitInfo::GetInst()->Init(argc, argv);
	FileTreeWalk( gbk_to_utf16(CSplitInfo::GetInst()->GetSrcPath()).c_str(), CDif2Svs::Convert, NULL );
	uint64 uDif2SvsTime = GetCurrentTime() - uStartTime;
	printf("CDif2Svs完成,共用时间 %d 毫秒\n", uDif2SvsTime);
	return 0;
}

