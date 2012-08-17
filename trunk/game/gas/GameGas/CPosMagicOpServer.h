#pragma once
#include "CMagicOpServer.h"
#include "CPos.h"

class CSkillInstServer;
class CFighterDictator;
class CMagicOpArg;
class CCfgCalc;
class CCfgArg;

class CPosMagicOpServer
	:public CMagicOpServer
{
public:
	virtual uint32 Do(CSkillInstServer* pSkillInst,const CCfgArg* pArg,CFighterDictator* pFrom,const CFPos& pos);
	virtual uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,const CFPos& pos);
	virtual ~CPosMagicOpServer(){}
	virtual EMagicOpClassType GetMagicOpType()const
	{
		return eMOT_PosFunction;
	}
};
