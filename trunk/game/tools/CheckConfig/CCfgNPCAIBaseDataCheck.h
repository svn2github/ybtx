#pragma once
#include "CCfgBaseCheck.h"

class CCfgNPCAIBaseDataCheck : public CCfgBaseCheck
{
public:
	static bool Check(const TCHAR* cfgFile);
	static void	EndCheck();
	static bool BeExist(string sAIDataName);
	static uint32 GetEyeSizeByName(const TCHAR* szName);

	typedef map<string, CCfgNPCAIBaseDataCheck*> MapAIBaseData;
	static MapAIBaseData	ms_mapAIBaseData;

private:
	uint32 m_uEyeSize;
};
