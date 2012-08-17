#pragma once
#include "CTxtTableFile.h"
#include "CStaticObject.h"

class CCfgBaseCheck
	:public virtual CStaticObject
{
public:
	static void Load(CTxtTableFile& TxtTableFile, const TCHAR* cfgFile, string strTableName);
	static bool	CheckOverlap(string strColName, int32 nRowNum);
	static void	EndCheckOverlap();

private:
	typedef set<string>	SetRowName;
	static SetRowName	ms_setRowName;
};
