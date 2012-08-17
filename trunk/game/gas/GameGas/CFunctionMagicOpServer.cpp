#include "stdafx.h"
#include "CFunctionMagicOpServer.h"
#include "CCfgArg.h"

uint32 CFunctionMagicOpServer::Do(CSkillInstServer* pSkillInst, const CCfgArg* Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!Arg)
	{
		return Do(pSkillInst,*ms_pMagicOpCalcArg->m_pArg,pFrom,pTo);
	}
	CMagicOpCalcArg* pMagicOpCalcArg = static_cast<CMagicOpCalcArg*>(const_cast<CCfgArg*>(Arg));
	return Do(pSkillInst,*pMagicOpCalcArg->m_pArg,pFrom,pTo);
}

uint32 CFunctionMagicOpServer::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	return eHR_Fail;
}

void CFunctionCancelableMagicOp::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!Arg)
	{
		return Cancel(pSkillInst,*ms_pMagicOpCalcArg->m_pArg,pFrom,pTo);
	}
	CMagicOpCalcArg* pMagicOpCalcArg = static_cast<CMagicOpCalcArg*>(const_cast<CCfgArg*>(Arg));
	return Cancel(pSkillInst,*pMagicOpCalcArg->m_pArg,pFrom,pTo);
}
