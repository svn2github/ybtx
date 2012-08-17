#pragma once
#include "CTxtTableFile.h"

class CCfgSkillBaseCheck
{
public:
	static void Load(CTxtTableFile& TxtTableFile, const TCHAR* cfgFile, string strTableName);
	static bool CheckOverlap(string strColName, int32 nRowNum);
	static void EndCheckOverlap();
	static bool CheckExist(const string& strMagicName);

	typedef set<string>	SetRowName;
	static SetRowName	ms_setRowName;
	static string		m_strTableName;
};
