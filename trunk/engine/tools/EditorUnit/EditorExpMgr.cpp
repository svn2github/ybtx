#include "stdafx.h"
#include <dbghelp.h>
#include <string>
#include "StringHelper.h"
#include "ErrLogHelper.h"
#include "CLogOwner.h"

#pragma comment(lib, "dbghelp.lib")

static _invalid_parameter_handler	ls_PreParamHandler;
static _purecall_handler			ls_PrePurecallHandler;
static _se_translator_function		ls_PreSeTransFunction;
static wchar_t						ls_CurPath[1024];
static string strDumpFilePath = "";

void TellMsg(const TCHAR* szError,CLogOwner* pConn)
{
	MessageBox( NULL, szError, "错误提示", 0 );
}

static void CreateEditorCoreDumpAndTell(PEXCEPTION_POINTERS pExceptionInfo, const char* error_msg)
{
	SYSTEMTIME ti;
	GetLocalTime(&ti);

	string path("");
	path = format("%s_%d_%d_%d_%d.dmp", strDumpFilePath.c_str(), ti.wMonth, ti.wDay, ti.wHour, ti.wMinute);
	HANDLE hReportFile = CreateFile( path.c_str(), GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, 0 );
	if( !hReportFile )
		return;

	_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

	ExInfo.ThreadId = ::GetCurrentThreadId();
	ExInfo.ExceptionPointers = pExceptionInfo;
	ExInfo.ClientPointers = NULL;

	MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hReportFile, MiniDumpNormal, &ExInfo, NULL, NULL );

	CloseHandle( hReportFile );

	MessageBox(NULL, "工具执行程序所在路径下产生dump文件，请发给相关工作人员", "提示", MB_OK);
}

static void EditorTransESHFun( unsigned int u, EXCEPTION_POINTERS* pContext )
{
	(void)u;

	CreateEditorCoreDumpAndTell(pContext, "window 结构化异常");

	exit(1);
}

static void EditorInvalidParameterHandler(const wchar_t* , const wchar_t* , const wchar_t* , 
									unsigned int , uintptr_t )
{
	__try{
		RaiseException(0, 0, 0, 0);
	}
	__except(CreateEditorCoreDumpAndTell(GetExceptionInformation(), "不正确的参数类型"), EXCEPTION_EXECUTE_HANDLER)
	{
	}

	exit(1);
}

static void EditorPurecallHandler()
{
	__try{
		RaiseException(0, 0, 0, 0);
	}
	__except(CreateEditorCoreDumpAndTell(GetExceptionInformation(), "纯虚函数调用"), EXCEPTION_EXECUTE_HANDLER)
	{
	}

	exit(1);
}

void InitExpHandler(const string& strDumppatch, bool bGenerateDmpFile, bool bErrorMsgShow)
{
	strDumpFilePath = strDumppatch;

	if( bErrorMsgShow )
		SetLogCallBackFunc(&TellMsg);

#ifndef _DEBUG
	// 引擎是开coredmp直接爆掉，不开则容错+产生coredump；工具希望宕掉后产生coredump文件
	// GenErr
	EnableCatchError( bGenerateDmpFile );
	if( bGenerateDmpFile )
	{
		ls_PreParamHandler = _set_invalid_parameter_handler(EditorInvalidParameterHandler);
		ls_PreSeTransFunction = _set_se_translator(EditorTransESHFun);
		ls_PrePurecallHandler = _set_purecall_handler(EditorPurecallHandler);
	}

#else
	EnableCatchError(false);

#endif

	_wgetcwd(ls_CurPath, _countof(ls_CurPath));
}


void UnitExpHandler()
{
	_set_purecall_handler(ls_PrePurecallHandler);
	_set_se_translator(ls_PreSeTransFunction);
	_set_invalid_parameter_handler(ls_PreParamHandler);

}
