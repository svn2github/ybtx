#pragma once
#include "TSingleton.h"

class CSplitInfo;

class CSplitCfg : public TSingleton<CSplitCfg>
{
public:
	CSplitCfg();
	~CSplitCfg();
	void StartProcess();
	static FTW_RESULT SplitDif( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void SplitDif(string& strFileName);
	string GetAppWholePath(const string& strFileName);
	string GetLangWholePath(const string& strFileName);
	void GetSplitColNumber(vector<string>& vecRef,vector<string>& vecSrc,vector<int>& vecArg);
	void CleanUpFileName(string& strFileName);
	void CreateIndexToNameMap(vector<string>& vecRef,map<int,string>& mapIndexName);
	
	string CreateCommonIndex(vector<string>& vecRef,vector<int>& vecIndexNumber);
	void ReplaceWildcard(string& str);

	static string GetLangIndexConnSymbol();
	static string GetFileNameFromPath(const string& strFileName);

	bool BeInSplitList(const string& strFileName);

	//²ð·ÖNpc_Common±í
	bool SplitNpc_Common(string& strFileName);
private:
	CSplitInfo* m_pSplitInfo;
};
