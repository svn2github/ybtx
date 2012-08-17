#pragma once
#include "TSingleton.h"

class CSplitInfo;

class CLanDif2LanTxt : public TSingleton<CLanDif2LanTxt>
{
public:
	CLanDif2LanTxt();
	~CLanDif2LanTxt();
	void StartProcess();
	static FTW_RESULT LanDif2LanTxt( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void Change2LanTxt(string& strFileName);
	void AddExcelTxtDoubleQuotation(string& str);
	string gbk_to_utf8( const string& src );
	bool BeLangDif(string& strFileName);

private:
	CSplitInfo* m_pSplitInfo;
};
