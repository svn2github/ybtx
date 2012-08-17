#pragma once
#include "CCfgBaseCheck.h"

class CCfgBloodCheck : public CCfgBaseCheck
{
public:
	static bool Check(const TCHAR* cfgFile);
	static void	EndCheck();
};