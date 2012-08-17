#include "stdafx.h"
#include "CCfgTempVarCheck.h"

CCfgTempVarCheck::SetImmuneTaskAttackName CCfgTempVarCheck::m_setImmuneTaskAttackName;

bool CCfgTempVarCheck::Check()
{
	m_setImmuneTaskAttackName.insert("任务非战斗");
	m_setImmuneTaskAttackName.insert("任务炸弹");
	m_setImmuneTaskAttackName.insert("任务特殊");
	m_setImmuneTaskAttackName.insert("非国战建筑");

	return true;
}

bool CCfgTempVarCheck::IsImmuneTaskAttackName(const string& strName)
{
	return m_setImmuneTaskAttackName.find(strName) != m_setImmuneTaskAttackName.end();
}

void CCfgTempVarCheck::EndCheck()
{
	m_setImmuneTaskAttackName.clear();
}
