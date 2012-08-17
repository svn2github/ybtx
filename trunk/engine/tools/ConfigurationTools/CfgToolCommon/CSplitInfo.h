#pragma once
#include "TSingleton.h"
#include "CodeCvs.h"
#include <set>
#include <tchar.h>

class CSplitInfo : public TSingleton<CSplitInfo>
{
public:
	CSplitInfo();
	enum ESaveInfor
	{
		eSIF_AppName,
		eSIF_LangName,
		eSIF_AppSavePath,
		eSIF_LangSavePath,
	};
	enum ESplitInfor
	{
		eSTIF_CommonIndex,
		eSTIF_LangIndex,
		eSTIF_LangColName,
		eSTIF_Delete,
		eSTIF_Exception,
		eSTIF_Content,
	};

	struct ExcepIndexData
	{
		vector<map<string,string> > vecExcept;
		vector<string> vecIndex;
		bool CheckCondition(vector<map<string,string> >& vecAttribute);
		string ProduceIndex(vector<map<string,string> >& vecAttribute);
		vector<string> GetExceptCol();
	};

	void Init(int argc, _TCHAR* argv[]);

	bool ReadSplitXML(const string& strCfg);
	bool BeInSplitList(string strName);
	bool BeLangDif(string strName);
	bool BeAppDif(string strName);
	bool BeMakeAppFile(string strName);
	string GetAppSavePath(const string& strFileName);
	string GetLangSavePath(const string& strFileName);
	string GetAppName(const string& strFileName);
	string GetLangName(const string& strFileName);
	void GetSplitInfoArray(const string& strFileName,ESplitInfor eSInfo,vector<string> & vecArg);
	void GetSourceTransCol(const string& strFileName,vector<string> & vecArg);
	void GetDestTransCol(const string& strFileName,vector<string> & vecArg);
	void GetCommonIndex(const string& strFileName,vector<string> & vecArg);
	bool BeHaveExceptInfo(const string& strFileName);

	void CreateExceptMap(ExcepIndexData& referData,vector<string>& vecEleRef,map<int,string>& mapIndexName,vector<map<string,string> >& vecAttribute);
	void CreateExcepIndexAttribute(ExcepIndexData& referData,vector<string>& vecEleRef,map<int,string>& mapIndexName,vector<map<string,string> >& vecAttribute);
	bool CreateExceptIndex(const string& strFileName,vector<string>& vecEleRef,map<int,string>& mapIndexName,string& element);

	bool BeDeleteCol(const string& strFileName,const string& strColName);
	string GetLangIndexName(const string& strFileName);
	string GetSrcDifNameWithLangName(string strLangName);
	string GetDifColNameWithLangName(string strLangName,string lanCol);

	const string& GetLuaCfgExtend() { return m_strLuaExtend;}
	const string& GetCppCfgExtend() { return m_strCppExend;}
	wstring WGetLuaCfgExtend() { return gbk_to_utf16(m_strLuaExtend);}
	wstring WGetCppCfgExtend() { return gbk_to_utf16(m_strCppExend);}

	void tolowerCase(string& str);

	//---------------------------------
	bool GetBeInternational() { return m_bEnableInternational;}
	bool GetBeSplitBaseItemDesc() { return m_bSplitBaseItemDesc;}
	bool GetBeKeepBlankLine(const string& strFileName);
	bool GetCReadTable(const string& strFileName);

	const string& GetTgtPath()	{ return m_strTgtPath;}
	const string& GetSrcPath()	{ return m_strSrcPath;}
	const string& GetTgtFolder()	{ return m_strTgtFolder;}
	const string& GetSrcFolder()	{ return m_strSrcFolder;}

	const string& GetForeLang() { return m_strForeLang;}
	const string& GetOriLang() { return m_strOriLang;}
	const string& GetAutoConvLang() { return m_strAutoConvLang;}

	const string& GetLangPath() { return m_strLangPath;}
	const string& GetLangTablePath() { return m_strLangTablePath;}
	const string& GetLangSrcFolder() { return m_strLangSrcFolder;}
	const string& GetReferFolder() { return m_strReferFolder;}
	const string& GetLangSrcPath() { return m_strLangSrcPath;}
	const string& GetReferPath() { return m_strReferPath;}

	const string& GetCfgTempPath(){return m_strCfgTempPath;}

	const string& GetLanguageTempPath() { return m_strLanguageTempPath;}

	wstring WGetTransImportPath() { return gbk_to_utf16(m_strTransForeignPath);}
	const string& GetTransExportPath() { return m_strTransChinesePath;}
	const string& GetTransExportFolder() { return m_sTransExportFolder;}
	const string& GetQuestFolder() { return m_strQuestFolder;}
	const string& GetQuestPath() { return m_strQuestPath;}
	const string& GetQuestTempPath() { return m_strQuestTempPath;}
	const string& GetItemFolder() { return m_strItemFolder;}
	const string& GetItemPath() { return m_strItemPath;}
	const string& GetItemTempPath() { return m_strItemTempPath;}
	const string& GetScenePath() { return m_strScenePath;}
	const string& GetGuiFolder() { return m_strGuiFolder;}
	const string& GetProjectStructerFileName()		{ return m_strProjectStructerFileName;}
	const string& GetGuiStaticText() { return m_strGuiStaticText;}
	bool GetLogToFile() { return m_bLogToFile;}
	bool GetNeedLuaFiles() { return m_bNeedLuaFiles; }


private:
	static string GetLangIndexConnSymbol();

private:
	map<string,map<ESaveInfor,string> > m_mapFileSaveInfo;
	map<string,map<ESplitInfor,vector<string> > > m_mapMainIndex;
	map<string,vector<ExcepIndexData> > m_mapExceptInfo;
	map<string,map<string,string> > m_mapNoteInfo;
	set<string> m_setFileKeepBlankLine;
	set<string> m_setCReadTable;
	string m_strSrcPath;
	string m_strTgtPath;
	string m_strCfgTempPath;
	string m_strSrcFolder;
	string m_strTgtFolder;
	string m_strForeLang;
	string m_strOriLang;
	string m_strLangPath;
	string m_strLangTablePath;
	string m_strLuaExtend;
	string m_strCppExend;
	string m_strScenePath;
	string m_strGuiFolder;
	//添加是否启用生成国际化表及是否拆分物品描述信息及德语表位置
	bool m_bEnableInternational;
	bool m_bSplitBaseItemDesc;
	bool m_bLogToFile;
	bool m_bNeedLuaFiles;
	string m_strTransForeignPath;
	string m_strTransChinesePath;
	string m_strLanguageTempPath;
	string m_strQuestFolder;
	string m_strQuestPath;
	string m_strQuestTempPath;
	string m_strItemFolder;
	string m_strItemPath;
	string m_strItemTempPath;
	string m_strProjectStructerFileName;
	string m_strLangSrcFolder;
	string m_strReferFolder;
	string m_strLangSrcPath;
	string m_strReferPath;
	string m_strGuiStaticText;
	string m_sTransExportFolder;
	string m_strAutoConvLang;
};
