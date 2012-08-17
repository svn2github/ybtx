#include "stdafx.h"
#include "common.h"
#include "CodeCvs.h"
#include "COriFileOperate.h"
#include <dbghelp.h>

#pragma comment(lib, "dbghelp.lib")

string&   replace_all_distinct(string&   str,const   string&   old_value,const   string&   new_value,string::size_type pos)   
{   
	for( pos;   pos!=string::npos;   pos+=new_value.length())  
	{   
		if( (pos=str.find(old_value,pos)) != string::npos )   
			str.replace(pos,old_value.length(),new_value);   
		else
			break;   
	}   
	return   str;   
}

void ClearEndSpace(string &str)
{
	int iPos = str.find("\"",1);
	if (iPos != -1)
	{
		string strEnd = str.substr(iPos);
		int iLen = str.find_last_not_of(" ",iPos-1); 
		str = str.substr(0,iLen+1) + strEnd;
	}
}

void ClearSpace(string &str)
{
	str.erase(remove_if(str.begin(), str.end(), bind2nd(equal_to<char>(), ' ')), str.end());
}

string ConverseToUtf8(string &str)
{
	//将Ansi转成unicode
	int length = str.size();
	wstring unicode(str.size(), 0);
	setlocale(LC_ALL,"");
	int unicodeLength = mbstowcs(const_cast<wchar_t*>(unicode.data()),str.c_str(),unicode.size());
	//if (unicodeLength == -1)
	//	return str;
	unicode.resize(unicodeLength);

	//unicode转成utf8
	string utf8code;
	UCS2ToUtf8( utf8code,unicode );
	return utf8code;
}

static void CreateCoreDumpAndTell(PEXCEPTION_POINTERS pExceptionInfo, const wchar_t* error_msg)
{
	wchar_t szFileName[1024];
	wchar_t szBaseName[100];
	wchar_t szCurPath[1024];
	_wgetcwd(szCurPath,_countof(szCurPath));
	swprintf_s(szBaseName, _countof(szBaseName), L"Ybtx_%u.dmp", (unsigned int)GetCurrentProcessId());
	szBaseName[_countof(szBaseName) - 1] = L'\0';

	swprintf_s(szFileName, _countof(szFileName), L"%s/%s", szCurPath, szBaseName);
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
	strm << error_msg << endl << L"程序将会在 " << szCurPath << L"/ 路径下面生成错误文件\n"
		<< L"生成的文件为: " << szBaseName << endl;

	MessageBoxW(NULL, strm.str().c_str(), L"Error", MB_OK);
}


void ReportESHFun(unsigned int, struct _EXCEPTION_POINTERS* pContext)
{
	CreateCoreDumpAndTell(pContext, L"window 结构化异常");
	exit(1);
}

void ReportInvaidParamentFun(const wchar_t* expression,
						 const wchar_t* function, 
						 const wchar_t* file, 
						 unsigned int line, 
						 uintptr_t pReserved)
{
	__try{
		RaiseException(0, 0, 0, 0);
	}
	__except(CreateCoreDumpAndTell(GetExceptionInformation(), L"不正确的参数类型"), EXCEPTION_EXECUTE_HANDLER)
	{
		SQR_THROW("函数参数错误");
	}
}

void CreateFileDirectory(string strFileName)
{
	int slashPos = strFileName.rfind("/");
	string strPath = strFileName.substr(0,slashPos);
	replace_all_distinct(strPath,"//","/",0);
	MakeDirectory( gbk_to_utf16(strPath.c_str()).c_str());
}

string GetFileNameFromPath(const string& strFileName)
{
	string strName = strFileName;
	const TCHAR* szSperate = max( strrchr( strFileName.c_str(), '\\' ), strrchr( strFileName.c_str(), '/' ) );
	if( szSperate )
	{
		int subPos = static_cast<int>(szSperate-strFileName.c_str());
		strName = strFileName.substr(subPos+1,strFileName.length()-subPos-1);
	}
	return strName;
}


