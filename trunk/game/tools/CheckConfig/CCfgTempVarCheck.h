#pragma once

class CCfgTempVarCheck
{
public:
	static bool Check();
	static void EndCheck();
	static bool IsImmuneTaskAttackName(const string& strName);

	typedef set<string> SetImmuneTaskAttackName;
	static SetImmuneTaskAttackName m_setImmuneTaskAttackName;
};