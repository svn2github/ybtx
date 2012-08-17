#pragma once
#include "CCfgBaseCheck.h"

class CCfgNPCResCommonCheck
{
public:
	static void Check(const TCHAR* sNpcName, float fBaseSize, bool bAdhereTerrain, const TCHAR* sAniFileName);
	static void	EndCheck();
	static bool	BeExist(string sNpcName);

	typedef set<string>	SetNpcName;
	static SetNpcName	ms_setNpcName;
};
