#pragma once
#include "TSingleton.h"

class COriFileOperate;

class CTransTradition :public TSingleton<CTransTradition>
{
public:
	//~CTransTradition();
	CTransTradition();
	static FTW_RESULT EnumTreeFile( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void CreateTraditionFileByItem(string& strFile);
	void ReadUtf8Txt(string& strFileName,vector<vector<string> >& vecRef);
	void CreateTraditionFileByLine(string& strFile);
	void ReadUtf8Txt(string& strFileName,vector<string>& vecRef);
	void CreateTraditionFile(string& strFile);
	void ReadUtf8Txt(string& strFileName, string& vecRef);
	string MakeSavePath(string& strFileName);

};

