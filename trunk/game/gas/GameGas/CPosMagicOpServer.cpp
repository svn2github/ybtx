#include "stdafx.h"
#include "CPosMagicOpServer.h"
#include "CCfgArg.h"

uint32 CPosMagicOpServer::Do(CSkillInstServer* pSkillInst,const CCfgArg* pArg,CFighterDictator* pFrom,const CFPos& pos)
{
	if (!pArg)
	{
		return Do(pSkillInst,*ms_pMagicOpCalcArg->m_pArg,pFrom,pos);
	}
	CMagicOpCalcArg* pMagicOpCalcArg = static_cast<CMagicOpCalcArg*>(const_cast<CCfgArg*>(pArg));
	return Do(pSkillInst,*pMagicOpCalcArg->m_pArg,pFrom,pos);
}

uint32 CPosMagicOpServer::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,const CFPos& pos)
{
	return eHR_Fail;
}
