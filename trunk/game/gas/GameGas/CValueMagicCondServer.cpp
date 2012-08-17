#include "stdafx.h"
#include "CValueMagicCondServer.h"
#include "CCfgArg.h"

uint32 CValueMagicCondServer::Test(CSkillInstServer* pSkillInst,CCfgArg* Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	if (!Arg)
	{
		return Test(pSkillInst,*ms_pMagicOpCalcArg->m_pArg,pFrom,pTo);
	}
	CMagicOpCalcArg* pMagicOpCalcArg = static_cast<CMagicOpCalcArg*>(Arg);
	return Test(pSkillInst,*pMagicOpCalcArg->m_pArg,pFrom,pTo);
}
