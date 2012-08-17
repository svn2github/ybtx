// CheckConfigurationFiles.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <conio.h>
#include <ctype.h>
#include "CRecordManager.h"
#include "CPkgFile.h"
#include "CScriptMgr.h"

int main(int argc, TCHAR* argv[])
{
	//wchar_t szCurPath[MAX_PATH];
	//_wgetcwd(szCurPath, _countof(szCurPath));
	//CPkgFile::AddLoadPath(L"", szCurPath);

	CScriptMgr Runner;
	CRecordManager::GetInst()->CalculateState();
	Runner.Run();

	//cout<<"\n\n检测完成，按任意键退出!"<<endl;
	//_getch();
	return 0;
}

