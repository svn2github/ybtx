#pragma once
#include "CCfgBaseCheck.h"

class CCfgServantDataCheck : public CCfgBaseCheck
{
public:
	static bool Check(const TCHAR* cfgFile);
	static void	EndCheck();
};
