#include "stdafx.h"

#ifdef _WIN32
#include <dbghelp.h>

#pragma comment(lib, "dbghelp.lib")

static _invalid_parameter_handler	ls_PreParamHandler;
static _purecall_handler			ls_PrePurecallHandler;
static _se_translator_function		ls_PreSeTransFunction;
static wchar_t						ls_CurPath[1024];

static void CreateCoreDumpAndTell(PEXCEPTION_POINTERS pExceptionInfo, const wchar_t* error_msg)
{
	wchar_t szFileName[1024];
	wchar_t szBaseName[100];
	swprintf_s(szBaseName, _countof(szBaseName), L"Ybtx_%u.dmp", (unsigned int)GetCurrentProcessId());
	szBaseName[_countof(szBaseName) - 1] = L'\0';

	swprintf_s(szFileName, _countof(szFileName), L"%s/%s", ls_CurPath, szBaseName);
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

	wostringstream strm;
	strm << error_msg << endl << L"程序将会在 " << ls_CurPath << L"/ 路径下面生成错误文件\n"
		<< L"生成的文件为: " << szBaseName << endl;

	MessageBoxW(NULL, strm.str().c_str(), L"Error", MB_OK);
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
	//ls_PreParamHandler = _set_invalid_parameter_handler(InvalidParameterHandler);
	//ls_PreSeTransFunction = _set_se_translator(TransESHFun);
	//ls_PrePurecallHandler = _set_purecall_handler(PurecallHandler);

	_wgetcwd(ls_CurPath, _countof(ls_CurPath));
}

void UnitExpHandler()
{
	//_set_purecall_handler(ls_PrePurecallHandler);
	//_set_se_translator(ls_PreSeTransFunction);
	//_set_invalid_parameter_handler(ls_PreParamHandler);

}

#endif