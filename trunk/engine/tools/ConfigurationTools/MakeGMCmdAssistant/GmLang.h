#pragma once
#include "stdafx.h"
#include "TSingleton.h"
#include "COriFileOperate.h"

class CGmLang : public TSingleton<CGmLang>
{
public:
	CGmLang();
	~CGmLang();
	static void GmLang();

private:
	bool ReadGmConflictXml();
	void InitMapGmElementInfo();
	void ParseElementCol( const string& strDisplayColName,string& strFile);
	void MergeElementCol( string& strGroup,string& strFile);
	void ParseMultiElementCol( const string& strGroupName );

	bool BeGmChange(string& strFile);
	string FormatFileFullPath(const string& strFile);
	void AddNewData_toGm(string& strIndex,string& strFileName,string& strDisplayName);
	void DeleteOldData(string& strIndex,string& strFileName);
	void CopyNewData_toOld();
	bool BeExistSameData(string& strIndex,string& strFileName,string& strDisplayName);
	bool BeExistSameIndexDiffName(string& strIndex,string& strFileName,string& strDisplayName);
	void PrintLogInfo(string strMsg);

private:
	enum EGmInfo
	{
		eGIF_Path,
		eGIF_DisplayCol
	};
	struct RowInfo
	{
		int nRowOffset;
		bool bFlag;
	};
	struct GmGroupInfo 
	{
		vector<hash_map<string, string> > m_vecFileInfo;
		vector<hash_map<string, string> > m_vecIndexInfo;
		vector<string> GetIndexCol();
		bool CheckCondition( vector<hash_map<string,string> >& vecAttribute );
		string m_strDisplayCol;
	};
	
	string m_strGmPath;
	string m_strSrcPath;
	hash_map<string, hash_map<EGmInfo, char*> > m_hmapGmInfo;
	hash_map<string, GmGroupInfo> m_hmapGmGroupInfo;
	COriFileOperate m_destFile;
	COriFileOperate m_ElementColFile;
	hash_map<string,COriFileOperate*> m_hmapElementCol;
	
	COriFileOperate* m_pOldFile;
	bool	m_bXmlRight;
	bool m_bNeedSave;
	vector<string> m_vecFileChange;
};
