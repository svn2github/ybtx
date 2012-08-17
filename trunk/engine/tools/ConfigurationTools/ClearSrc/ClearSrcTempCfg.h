#pragma once
#include "stdafx.h"
#include "TSingleton.h"
#include <iterator>
#include "CSplitInfo.h"
#include "common.h"
class ClearSrcCfg:public TSingleton<ClearSrcCfg>
{
public:
	ClearSrcCfg();
	~ClearSrcCfg();
	void StartProcess();
	static FTW_RESULT MakeSrc( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void DeleteSrc(string & strFileName);
	bool ForceDeleteFile(const string& strFile);
private:
	CSplitInfo * m_pSplitInfo;
	string m_strSrc;
};