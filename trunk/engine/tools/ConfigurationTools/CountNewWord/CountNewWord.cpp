// CountNewWord.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CCountNewWord.h"

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_CTYPE,"");
	_set_invalid_parameter_handler(ReportInvaidParamentFun);
	_set_se_translator(ReportESHFun);
	CCount::GetInst()->start();
	return 0;
}