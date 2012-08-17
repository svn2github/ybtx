#pragma once
#include "TSingleton.h"

class CSplitInfo;

class CDif2Lua : public TSingleton<CDif2Lua>
{
public:
	CDif2Lua();
	~CDif2Lua();
	void StartProcess();
	static FTW_RESULT Dif2Lua( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void Change2Lua(string& strFileName);
	void ChangeWithLuaFormat(string&str);

	string GetLuaFileName(string& strFileName);
	string GetLuaIncFileName(string& strFileName);
	string GetRequireStr(string& strFileName);

	bool BeRequireToLua(const string& strFileName);
	void SetBeLangState(bool bState) { m_bLang = bState;}

private:
	CSplitInfo* m_pSplitInfo;
	bool m_bLang;
};
