#include "stdafx.h"
#include "UtfCvs.h"

#ifdef _WIN32
#include <dbghelp.h>

#pragma comment(lib, "dbghelp.lib")

static _invalid_parameter_handler	ls_PreParamHandler;
static _purecall_handler			ls_PrePurecallHandler;
static _se_translator_function		ls_PreSeTransFunction;
static LPTOP_LEVEL_EXCEPTION_FILTER ls_pPrevSEFilter;
static wchar_t						ls_CurPath[1024];

static void CreateCoreDumpAndTell(PEXCEPTION_POINTERS pExceptionInfo, const wchar_t* error_msg)
{
	wchar_t szFileName[1024];
	wchar_t szBaseName[100];
	swprintf_s(szBaseName, _countof(szBaseName), L"RunModule_%u.dmp", (uint32)GetCurrentProcessId());
	szBaseName[_countof(szBaseName) - 1] = L'\0';
	
	swprintf_s(szFileName, _countof(szFileName), L"%s/../../var/%s", ls_CurPath, szBaseName);
	szFileName[ _countof(szFileName) - 1 ] = L'\0';	


	HANDLE hReportFile = CreateFileW( szFileName, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, 0 );
	if( !hReportFile )
		return;

	_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

	ExInfo.ThreadId = ::GetCurrentThreadId();
	ExInfo.ExceptionPointers = pExceptionInfo;
	ExInfo.ClientPointers = NULL;

	MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hReportFile, MiniDumpNormal, &ExInfo, NULL, NULL );

	CloseHandle( hReportFile );

	wchar_t* wzError=L"非常遗憾地通知您，程序内部发生了错误，导致游戏无法继续运行。";

	MessageBoxW(NULL,wzError, L"提示信息：",MB_OK);
}

static LONG WINAPI  SEFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	CreateCoreDumpAndTell(pExceptionInfo,L"程序发生未捕获异常");

	TerminateProcess(GetCurrentProcess(), 1);

	return EXCEPTION_EXECUTE_HANDLER;
}

static void TransESHFun( unsigned int u, EXCEPTION_POINTERS* pContext )
{
	(void)u;

	CreateCoreDumpAndTell(pContext, L"window 结构化异常");

	exit(1);
}

static void InvalidParameterHandler(const wchar_t* , const wchar_t* , const wchar_t* , 
							 unsigned int , uintptr_t )
{
	__try{
		RaiseException(0, 0, 0, 0);
	}
	__except(CreateCoreDumpAndTell(GetExceptionInformation(), L"不正确的参数类型"), EXCEPTION_EXECUTE_HANDLER)
	{
	}

	exit(1);
}

static void PurecallHandler()
{
	__try{
		RaiseException(0, 0, 0, 0);
	}
	__except(CreateCoreDumpAndTell(GetExceptionInformation(), L"纯虚函数调用"), EXCEPTION_EXECUTE_HANDLER)
	{
	}

	exit(1);
}

void InitExpHandler()
{
	ls_pPrevSEFilter=SetUnhandledExceptionFilter( &SEFilter );
	ls_PreParamHandler = _set_invalid_parameter_handler(InvalidParameterHandler);
	ls_PreSeTransFunction = _set_se_translator(TransESHFun);
	ls_PrePurecallHandler = _set_purecall_handler(PurecallHandler);

	_wgetcwd(ls_CurPath, _countof(ls_CurPath));
}

void UnitExpHandler()
{
	_set_purecall_handler(ls_PrePurecallHandler);
	_set_se_translator(ls_PreSeTransFunction);
	_set_invalid_parameter_handler(ls_PreParamHandler);
	SetUnhandledExceptionFilter( ls_pPrevSEFilter );
}

void LogExpMsg(const wchar_t* msg)
{
	wchar_t szFileName[1024];

	swprintf_s(szFileName, _countof(szFileName), L"%s/../../var/RunModule_%u.log", ls_CurPath, (uint32)GetCurrentProcessId());
	szFileName[ _countof(szFileName) - 1 ] = L'\0';	

	FILE* fd = _wfopen(szFileName, L"a");


	char* str = U16_to_MB(msg);

	fprintf(fd, "%s\n", str);

	FreeUtfCvsDest(str);

	fclose(fd);
}
#endif

