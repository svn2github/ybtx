#pragma once
#include "CPathMgr.h"

class CRecordManager
{
public:
	CRecordManager()
		: m_bSave(false),m_strLanguage("chinese_simple"),m_bFastCheck(false) {}

	struct RecordData
	{
		string m_strFunction;
		map<string,FILETIME> m_mapFileTime;
		map<string,string> m_mapFilePath;
		vector<string> m_vecRefresh;
		bool m_bOK;
	};
	void ReadRecordDoc();
	bool GetFileAmendTime(string strFile,FILETIME* time);
	void CreateRecord(string strFile);
	void SaveRecordEntry();
	bool BeNeedCheck(string strFunName);
	void SetRecordState(string strFunName,bool bState);
	bool BeInRecordList(string& strFunName);
	bool BeNoneRecord();
	void ReadXmlCfg();
	void CalculateState();
	void UpdateFilePath(string& strFileName);
	void CheckFileTime();
	void UpdateRecordData();
	void FindRecordData(string& strFunction,int& index);
	void CheckAllFileExist();
	void SaveCurrentTime(string strFunName);
	void CheckDependenceFun();
	void SetDependeceState(string strFunName);
	void SetCommonDependece();

	static CRecordManager* GetInst();
	static FTW_RESULT CheckFilePath( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );

private:
	vector<RecordData> m_vecRecord;
	map<string,vector<string> > m_mapFunction;
	map<string,vector<string> > m_mapDepedence;
	bool m_bSave;
	string	m_strLanguage;
	bool m_bFastCheck;
	wstring m_strCfgPath;
};