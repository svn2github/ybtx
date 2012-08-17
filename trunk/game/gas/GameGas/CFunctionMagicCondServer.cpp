#include "stdafx.h"
#include "CFunctionMagicCondServer.h"
#include "CCfgArg.h"

uint32 CFunctionMagicCondServer::Test(CSkillInstServer *pSkillInst, const CCfgArg* pArg, const CFighterDictator *pFrom, const CFighterDictator *pTo) const
{
	if (!pArg)
	{
		return Test(pSkillInst,*ms_pMagicOpCalcArg->m_pArg,pFrom,pTo);
	}
	CMagicOpCalcArg* pMagicOpCalcArg = static_cast<CMagicOpCalcArg*>(const_cast<CCfgArg*>(pArg));
	return Test(pSkillInst,*pMagicOpCalcArg->m_pArg,pFrom,pTo);
}

uint32 CFunctionMagicCondServer::Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	return eDSR_Fail;
}
