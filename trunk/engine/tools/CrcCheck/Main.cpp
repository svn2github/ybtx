// CrcCheck.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CrcCheck.h"

using namespace sqr;

int _tmain(int argc, _TCHAR* argv[])
{
	CCrcChecking *pInst = new CCrcChecking;
	bool bCheckResult = pInst->Check();
	if (bCheckResult)
	{
		cout << "校验结果：没有错误" << endl;
	}
	cout << "请按任意键结束";
	getchar();
	return 0;
}

