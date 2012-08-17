#pragma once
#include "CCfgMagicEffCheck.h"

class CCfgNormalAttackCheck
{
public:
	static bool	CheckPlayerNormalAttackCfg(const TCHAR* cfgFile);
	static bool	CheckNPCNormalAttackCfg(const TCHAR* cfgFile);
	static void	EndCheck();

	static bool IsPlayerNormalAttackNameValid(const string& strName);
	static bool IsNPCNormalAttackNameValid(const string& strName);

	string GetName() { return m_strName; }
	CCfgMagicEffCheck* GetMagicEff() { return m_pMagicEff; }
	bool CheckRelationAvail();

	typedef set<string>			SetPlayerNAName;
	typedef set<string>			SetNPCNAName;
	static SetPlayerNAName		ms_setPlayerNAName;
	static SetNPCNAName			ms_setNPCNAName;

	typedef map<string, CCfgNormalAttackCheck*> MapNormalAttack;
	static MapNormalAttack		ms_mapNormalAttack;

private:
	string				m_strName;
	CCfgMagicEffCheck*	m_pMagicEff;
};
