#include "stdafx.h"
#include "ConvertUtf8ToMbcs.h"
#include "CBaseScriptApp.h"
#include "CodeCvs.h"

const char* sqr::MemH64ToMbcs(const char *src)
{
	if ( NULL == src )
		return "";
	string *pString = CBaseScriptApp::GetCfgTableTlsData();
	(*pString)  = MemH64_to_mbcs(src);
	return (*pString).c_str();
}

