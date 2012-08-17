#pragma once
#include "TypeName2SRTarget.h"

class CCfgSkillRule
{
public:
	CCfgSkillRule(string strName, string strSkillName, ESRTargetType eSRTargetType, string strSkillArg);

	string			m_strSkillName;
	ESRTargetType	m_eSRTargetType;
	typedef list<string>	SkillNameLst;
	SkillNameLst	m_lstMultiArgs;
	static TypeName2SRTarget	ms_mapSRTargetType;
};

class CCfgSkillRuleCheck
{
public:
	~CCfgSkillRuleCheck();
	static bool Check(const TCHAR* cfgFile);
	static void	EndCheck();
	static bool CheckRuleSkill();
	static uint32 GetArgNum(ESRTargetType eSRTargetType);
	static bool CreateRuleDistance(string& strName, string& strSkillName, string& strDistace);
	static bool CreateRuleCondData(string &strName,string &strBegin,string &strContiune,string &strSkillName, int32 iSkillPage);
	static bool ParseRuleCond(string &strName,string& strCond,ERuleCondKind eRuleKind,string &strSkillName, int32 iSkillPage);
	static bool ParseRuleCondArg(string &strName,ERCondType eRCType,string& strArgTemp,string &strSkillName, int32 iSkillPage);
	static bool CheckRuleSkillTargetType(const string sRuleName, string sSkillName, ESRTargetType eTargetType);
	static CCfgSkillRule* CreateCfgSkillRule(string strName, string strSkillName, string strTarget, string strSkillArg);
	static bool CheckExist(string& strSkillRuleName);

	typedef map<string, CCfgSkillRuleCheck*> MapSkillRuleName;
	static MapSkillRuleName		ms_mapSkillRuleName;

	typedef vector<CCfgSkillRule*>	VecChild;
	VecChild				m_vecChild;
};

