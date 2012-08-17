#pragma once
#include "TSingleton.h"


class CAdjustGermanLan : public TSingleton<CAdjustGermanLan>
{
public:
	CAdjustGermanLan();
	~CAdjustGermanLan();
	void StartProcess();
	static FTW_RESULT CleanLangFile( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void ReadUtf8Txt(string& strFileName,vector<vector<string> >& vecRef);
	void ChangeLanFormat(string& strFileName);
	bool ReadMappedXML();
	void ReplaceKeyName(string& str);
	void RemoveNoteCol(const string& strFileName, vector<vector<string> >& vecRef);
	string GetRelativePath(string& str);
	bool BeUtf8Format(string& strFileName);
	bool BeUtf16Format(string& strFileName);
	int UtfFormatNumber(string& strFileName);
	void ReadUtf16Txt(string& strFileName,vector<vector<wstring> >& vecRef);
	void ReadUtf16Txt(string& strFileName,vector<vector<string> >& vecRef);

	static FTW_RESULT WalkXml( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void CopyXml_Geramn(string& strFileName);
	void ForceDeleteFile(const string& strFile);

private:
	map<string,string> m_mapName;
	string m_strNote;
	string m_strPath;
	wstring m_wstrFilePath;
};