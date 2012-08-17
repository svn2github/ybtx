#pragma once
#include "CMagicCondServer.h"

class CSkillInstServer;
class CMagicOpArg;
class CFighterDictator;
class CCfgCalc;
class CCfgArg;

//***********************Ä§·¨Ìõ¼þ********************************
class CFunctionMagicCondServer
	:public CMagicCondServer
{
public:
	virtual EMagicCondClassType GetMagicCondType()const
	{
		return eMCT_Function;
	}
	virtual uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	virtual uint32 Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	virtual bool GetStateNeedSync()const
	{
		return false;
	}
};

class CStateExistInClientFunctionMagicCondServer
	:public CFunctionMagicCondServer
{
	virtual bool GetStateNeedSync()const
	{
		return true;
	}
};

