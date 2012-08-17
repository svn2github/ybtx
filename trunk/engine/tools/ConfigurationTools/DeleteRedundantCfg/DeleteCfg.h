#pragma once
#include "stdafx.h"
#include "TSingleton.h"
#include <iterator>
#include "common.h"
class CSplitInfo;
class CDeleteCfg : public TSingleton<CDeleteCfg>
{
public:
	CDeleteCfg();
	~CDeleteCfg();

	void StartProcess();
	//遍历src将所有要拆分的表记录下来
	static FTW_RESULT MakeFilePath( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	//遍历temp和src进行比对
	static FTW_RESULT CheckTemp( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	//遍历src将所有表记录下来
	static FTW_RESULT MakeAllFilePath( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	//遍历tgt和src进行比对
	static FTW_RESULT CheckTgt( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	//遍历lang和src比对
	static FTW_RESULT CheckLanguage( const wchar_t* szFileName, const struct _stat *,FTW_FLAG eFlag, void* );
	//将src目录下要拆分的表加入map
	void Add2Map(string& strFileName);
	//将src目录下所有表加入map
	void AddAll2Map(string& strFileName);
	//检查temp文件夹
	void CheckTempFile(string& strFileName);
	//检查tgt文件夹
	void CheckTgtFile(string& strFileName);
	//检查lang文件夹
	void CheckLanguageFile(string& strFileName);
	//强制删除文件
	bool ForceDeleteFile(const string& strFile);
	//删除冗余配置表
	void DeleteCfg(vector<string> vec);

private:
	CSplitInfo* m_pSplitInfo;
	string strSrc;
	string strTgt;
	string strTemp;
	string strLangFile;
    map<string,string> m_mFilePath;
	map<string,string> m_mAllFilePath;
	vector<string> m_vFile;
	vector<string> m_vAllFile;
	vector<string> m_vLangFile;

};
