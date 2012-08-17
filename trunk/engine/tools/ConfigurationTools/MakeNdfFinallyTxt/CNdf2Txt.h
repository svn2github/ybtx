#pragma once
#include "TSingleton.h"

class CSplitInfo;

class CNdf2Txt : public TSingleton<CNdf2Txt>
{
public:
	CNdf2Txt();
	~CNdf2Txt();
	void StartProcess();
	static FTW_RESULT Ndf2Txt( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void Change2Txt(string& strFileName);
	void AddExcelTxtDoubleQuotation(string& str);

private:
	CSplitInfo* m_pSplitInfo;
};
