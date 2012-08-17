// CopyLackFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CLackFileCopy.h"
#include "CExpLog.h"
#include "CSplitInfo.h"


int _tmain(int argc, _TCHAR* argv[])
{
	CSplitInfo::GetInst()->Init(argc, argv);
	BeginToPrint("CopyLackFile");
	CLackFileCopy::GetInst()->StartProcess();
	EndToPrint();
	return 0;
}

