#include "stdafx.h"
#include "ConvertMbcsToUtf8.h"
#include "CBaseScriptApp.h"
#include "ThreadHelper.h"
#include "CodeCvs.h"

#define MAX_BUFFER_SIZE 100000;
const char* sqr::GB2U8( const char *src )
{
	string *pString = CBaseScriptApp::GetCfgTableTlsData();
	(*pString) = gbk_to_utf8(src);

	return (*pString).c_str();
}

const char* sqr::MemH16(const char *src)
{
	string *pString = CBaseScriptApp::GetStrIndexTlsData();
	(*pString) = mbcs_to_MemH16(src);

	return (*pString).c_str();
}

const char* sqr::MemH64(const char *src)
{
	if (!src)
		return "";
	string *pString = CBaseScriptApp::GetStr64IndexTlsData();
	(*pString) = mbcs_to_MemH64(src);

	return (*pString).c_str();
}

