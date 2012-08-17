#pragma once
#include "stdafx.h"
#include <cassert>
#include "TSingleton.h"
#include "CSplitInfo.h"
#include "common.h"

class CDataType2Xml : public TSingleton<CDataType2Xml>
{
public:
	CDataType2Xml();
	~CDataType2Xml();
	void StartProcess();
	static FTW_RESULT WriteToXml(const wchar_t* wszFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void WriteToXml(const char* szFileName, const char* szReplace,string& FileName);
	bool BeInDataType2Xml(const char* szFileName);
	void SaveSingle(const char* szFileName, vector<bool>& vecDataType);
	void SetDataTypeVector(const char* szFileName);
	
	void CreateXml(string& strFileName);
	string GetXmlPath(string& strFileName);
	bool BeMultiRowTable(string& strFileName);
	void	GetColType(string& strFileName,vector<bool>& vecType);
	void WriteToXml(string& strXml, vector<bool>& vecType,string& FileName);

	void ForceDeleteFile(const string& strFile);
	
private:
	vector<bool> m_vecDataType; //trueÎªÊý×Ö£¬falseÎª×Ö·û´®
	CSplitInfo* m_pSplitInfo;
	vector<vector<string>> m_vecElementInfo;
	string m_strCfgTempPath;
	string m_strTgtPath;
	string m_strSrcPath;
	bool m_Convert;
};
