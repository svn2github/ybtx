// setupBash.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "RegBash.h"
#include "RemoveReg.h"

int _tmain(int argc, _TCHAR* argv[])
{
	RegBash(); //注册文件关联
	//RemoveReg(); //删除注册表关联信息

	getchar();
	return 0;
}

