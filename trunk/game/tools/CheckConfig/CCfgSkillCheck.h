#pragma once
#include "CCfgMagicEffCheck.h"

class CCfgSkillCheck
{
public:
	CCfgSkillCheck();
	~CCfgSkillCheck();
	static bool	CheckPlayerSkillCfg(const TCHAR* cfgFile);
	static bool	CheckNPCSkillCfg(const TCHAR* cfgFile);
	static bool	CheckIntObjSkillCfg(const TCHAR* cfgFile);
	static void	EndCheck();

	static bool IsPlayerSkillNameValid(const TCHAR* szName);
	static bool IsNPCSkillNameValid(const TCHAR* szName);
	static bool IsIntObjSkillNameValid(const TCHAR* szName);
	static bool IsNPCSkillDoSkillRule(const TCHAR* szName);

	string GetSkillName() { return m_strSkillName; }
	CCfgMagicEffCheck* GetMagicEff() { return m_pMagicEff; }
	bool CheckRelationAvail();

	typedef map<string, CCfgSkillCheck*> MapSkill;
	static MapSkill				ms_mapSkill;

	static CCfgSkillCheck* GetInst();
	
private:
	string						m_strSkillName;
	bool						m_bIsDoSkillRule;
	CCfgMagicEffCheck*			m_pMagicEff;
	set<string>					m_setSkillAction;
};
