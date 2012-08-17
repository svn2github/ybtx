#pragma once

#include "TSingleton.h"

class CSplitInfo;

class CGermanRefer : public TSingleton<CGermanRefer>
{
public:
	CGermanRefer();
	static FTW_RESULT MakeDifList( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	static FTW_RESULT CreateRefer( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void RecordDifPath(string& str);
	void CombinationQuestRefer(string strFileName);
	void CombinationIntObjRefer(string strFileName);
	void CombinationQuestDesc(string strFileName);
	void ReadUtf8Txt(const string& strFileName,vector<vector<string> >& vecRef);
	bool AdjustBeQuestTbl(string& strFileName);
	void SplitFileName(string strFileName,vector<string>& vecResult);
	void ReadReferneceTxt();
	void ReadReferDescriptTxt();
	string GetProjekt(string& strFileName);
	string GetKeyPrefix(string& strFileName);
	void ReplaceKeyword(string& strIndex);
	string GetQuestDescription(const string& str);
	bool ReadMappedXML();
	void CreateFileMapData(string& strFileName);
	void WriteToGermanRefer();
	bool BeQuestLangFile(string& strLang);
private:
	struct ReferenceInfor 
	{
		string m_strProjekt;
		string m_strPreFix;
	};
	map<string,ReferenceInfor> m_mapReference;
	map<string,string> m_mapReferDescript;
	map<string,string> m_mapName;
	CSplitInfo* m_pSplitInfo;
	string	m_strSavePath;
	map<string,vector<vector<wstring> > > m_referData;
	map<string,string> m_repalceDescript;
	map<string,string> m_mapFileList;
};
