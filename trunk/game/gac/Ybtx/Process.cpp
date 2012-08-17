#include "stdafx.h"
#include "Process.h"
#include <TlHelp32.h>

const int PATH_SIZE = 4096;
const wchar_t* HOST_FILE = L"uploader.php";
const wchar_t* UP_SERVER = L"UploadServer";

// 检查进程是否在运作
BOOL ProcessActive( const wchar_t* szProcessName )
{
	HANDLE hSnapshot = NULL;
	PROCESSENTRY32 pe;
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	pe.dwSize = sizeof(PROCESSENTRY32);
	int nProcCount = 0;
	Process32First(hSnapshot, &pe);
	do 
	{
		if (_wcsicmp(pe.szExeFile , szProcessName) == 0)
		{
			return TRUE;
		}
	}
	while (Process32Next(hSnapshot, &pe) == TRUE);

	return FALSE;
}

// 启动新进程
BOOL StartupProcessAndQuit( LPCWSTR AppName, LPWSTR szCmdLine, LPCWSTR szDir, DWORD& err_code )
{
	PROCESS_INFORMATION pinf;
	STARTUPINFO sinf;
	memset(&sinf,0,sizeof(sinf));
	sinf.cb = sizeof(sinf);

	BOOL bResult = CreateProcess(AppName, szCmdLine, NULL, NULL, TRUE, 0, NULL, szDir, &sinf, &pinf);
	if (!bResult)
	{
		err_code = GetLastError();
	}

	CloseHandle(pinf.hProcess);
	CloseHandle(pinf.hThread);

	if( bResult )
	{
		::TerminateProcess(::GetCurrentProcess(), 0);
	}

	return bResult;
}

// 启动logUpload上传器
void StartLogUpload( wstring strUrlPath )
{
	wchar_t szBuf[PATH_SIZE];
	GetCurrentDirectory(PATH_SIZE, szBuf);
	wchar_t szCmdLine[PATH_SIZE];
	wcscpy_s(szCmdLine, PATH_SIZE, L"\"");
	wcscat_s(szCmdLine, PATH_SIZE, szBuf);
	wchar_t* ptr = NULL;

	// 拼凑Url.ini路径
	wchar_t szIniPath[PATH_SIZE];
	wcscpy_s(szIniPath, PATH_SIZE, szBuf);
	wcscat_s(szIniPath, PATH_SIZE, L"/");
	wcscat_s(szIniPath, PATH_SIZE, strUrlPath.c_str());

	// 拼凑log目录
	TransferBacklash(szCmdLine);
	CutString(szCmdLine);
	wcscat_s(szCmdLine, PATH_SIZE, L"/var");
	wcscat_s(szCmdLine, PATH_SIZE, L"\"");

	wchar_t szLogHost[PATH_SIZE];
	DWORD dwRet = GetPrivateProfileString(L"Setting", UP_SERVER, NULL, szLogHost, PATH_SIZE, szIniPath);
	if (wcscmp(szLogHost, L"") == 0)
	{
		return;
	}
	if ( dwRet == 0 )
		wprintf(L"Can't read ini file: %s %d\n", szIniPath, GetLastError());
	wcscat_s(szLogHost, PATH_SIZE, L"/");
	wcscat_s(szLogHost, PATH_SIZE, HOST_FILE);

	ptr = szCmdLine + wcslen(szCmdLine);
	*ptr++ = L' ';
	*ptr = L'\0';
	size_t nLeaveSize = PATH_SIZE - wcslen(szCmdLine) - 1;
	wcscat_s(ptr, nLeaveSize, szLogHost);

	ShellExecute(NULL, L"open", L"UploadLog.exe", szCmdLine, NULL, SW_HIDE);
}

void CutString( wchar_t* szBuf )
{
	size_t uSize = wcslen(szBuf);
	size_t uCount = 0;
	wchar_t* ptr = szBuf + uSize;
	size_t i = 0;
	for (i=0; i<uSize; ++i)
	{
		if (*ptr == L'/' || *ptr == L'\\')
			++uCount;
		if (uCount > 1)
		{
			*ptr = L'\0';
			break;
		}
		--ptr;
	}
}

void TransferBacklash( wchar_t* szBuf )
{
	wchar_t* ptr = szBuf;
	while ( *ptr != L'\0' )
	{
		if ( *ptr == L'\\' )
			*ptr = L'/';

		++ptr;
	}
}
