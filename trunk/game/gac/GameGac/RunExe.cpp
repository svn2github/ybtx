#include "stdafx.h"
#include "RunExe.h"
#include <shellapi.h>

bool RunExe(const TCHAR* dir, const TCHAR* name, const TCHAR* cmd)
{
	DWORD	dwProcessId;
	dwProcessId = ::GetCurrentProcessId();

	PROCESS_INFORMATION pinf;
	STARTUPINFO sinf;
	memset(&sinf,0,sizeof(sinf));
	sinf.cb = sizeof(sinf);

	string strAppName = string(dir) + string(name);
	if(CreateProcess(
		strAppName.c_str(),
		(LPSTR)cmd,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		dir,
		&sinf,
		&pinf) == FALSE)
	{
		//MessageBox(NULL,"Æô¶¯ÓÎÏ·Æô¶¯Æ÷Ê§°Ü£¡","´íÎó",MB_ICONSTOP|MB_OK);
		return false;	
	}

	CloseHandle(pinf.hProcess);
	CloseHandle(pinf.hThread);

	return true;
}

void RunUrl(const TCHAR* rul)
{
	ShellExecute(NULL,"open",rul,NULL,NULL,SW_SHOWNORMAL);
}