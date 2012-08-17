#pragma once
#include "stdafx.h"

class COriFileOperate;

string&   replace_all_distinct(string&   str,const   string&   old_value,const   string&   new_value,string::size_type pos);
void ClearEndSpace(string &str);
void ClearSpace(string &str);
string ConverseToUtf8(string &str);
void ReportESHFun(unsigned int, struct _EXCEPTION_POINTERS* pContext);
void ReportInvaidParamentFun(const wchar_t* expression,
						 const wchar_t* function, 
						 const wchar_t* file, 
						 unsigned int line, 
						 uintptr_t pReserved);
void CreateFileDirectory(string strFileName);
string GetFileNameFromPath(const string& strFileName);
