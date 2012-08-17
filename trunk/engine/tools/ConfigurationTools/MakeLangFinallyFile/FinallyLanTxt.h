#pragma once
#include "TSingleton.h"
#include "CDifFile.h"

class CSplitInfo;

class CFinallyLanTxt : public TSingleton<CFinallyLanTxt>
{
public:
	CFinallyLanTxt();
	~CFinallyLanTxt();
	enum ECharacterType
	{
		eCT_Normal,
		eCT_Enter,
		eCT_Table,
	};
	void StartProcess();
	static FTW_RESULT Txt2TgtTxt( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void CreateFinallyTxt(string& strFileName);
	void CreateFinallyLua(string& strFileName);
	void ReadUtf16Txt(string& strFileName,vector<vector<wstring> >& vecRef);
	void ReadUtf8Txt(string& strFileName,vector<vector<string> >& vecRef);
	void FormatIndex(vector<vector<string> >& vecRef);
	ECharacterType AdjustCharacterType(char c[2]);
	void CleanExcelTxtQuotation(string& str);
	void CleanExcelFormat(vector<vector<string> >& vecRef);
	bool IsNumberString(string& str);
	string MakeSavePath(string& strFileName);
	bool BeExistLangPath(string& strFileName);
	string GetLanguage(string& strFileName);
	void CleanExcelDifQuotationToSimpleTxt(string& str);
	string GetRequireName(string& strIncName);
	void CleanOnePairQuotation(string& str);
	void Change_toLuaFormat(string& str);
	void Write_toLua(string strLuaName,string strLuaIncName,string strRequireName,vector<vector<string> >& vecRef);
private:
	static CSplitInfo* m_pSplitInfo;
	vector<string> m_vecLanguage;
};
