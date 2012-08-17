#pragma once
#include "CCfgSkillBaseCheck.h"

class CCfgTalentEffCheck : public CCfgSkillBaseCheck
{
public:
	static bool Check(const TCHAR* cfgFile);
	static void	EndCheck();
};
