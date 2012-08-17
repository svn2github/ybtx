#pragma once
#include "CMagicCondServer.h"
#include "CValueData.h"

class CSkillInstServer;
class CMagicOpArg;
class CFighterDictator;
class CCfgCalc;
class CCfgArg;

//***********************Ä§·¨Ìõ¼þ********************************
class CValueMagicCondServer
	:public CMagicCondServer
{
public:
	virtual EMagicCondClassType GetMagicCondType()const
	{
		return eMCT_Value;
	}
	virtual uint32 Test(CSkillInstServer* pSkillInst,CCfgArg* Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	virtual uint32 Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const=0;
	virtual CValueData MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const=0;
	virtual bool MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const=0;
};

