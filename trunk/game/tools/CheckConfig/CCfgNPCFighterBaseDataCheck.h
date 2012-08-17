#pragma once
#include "CCfgBaseCheck.h"

class CCfgNPCFighterBaseDataCheck : public CCfgBaseCheck
{
	typedef set<string> LstSkillRuleName;
	typedef map<string, LstSkillRuleName> MapName2SkillRule;
	typedef map<string, CCfgNPCFighterBaseDataCheck*> MapNpcFightBaseData;
public:
	static bool Check(const TCHAR* cfgFile);
	static void CheckNpcBornSkill(string& strSkill, uint32 uLineNum);
	static void CheckNpcNormalAttack(string& sAttackName, uint32 uLineNum);
	static uint32 GetAttackDistByName(const TCHAR* szName);
	static void	EndCheck();
	static bool BeExist(string sNpcName);
	static bool MakeRuleMap(string& strNpcName, string& strSkillRule);
	static bool CheckSkillRule(string& strNpcName, string& strSkillRuleName );

	static MapName2SkillRule			ms_mapName2SkillRule;
	static MapNpcFightBaseData			ms_mapNpcFightBaseData;
private:
	uint32 m_uAttackDist;
};
