#pragma once
#include "CMagicOpServer.h"
#include "CCfgCalc.h"

class CSkillInstServer;
class CMagicOpArg;
class CFighterDictator;
class CCfgArg;

class CFunctionMagicOpServer
	:public CMagicOpServer
{
public:	
	virtual uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	virtual uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	virtual bool Cancelable()const
	{
		return false;
	}
	virtual ~CFunctionMagicOpServer(){}
	virtual EMagicOpClassType GetMagicOpType()const
	{
		return eMOT_Function;
	}
};

//可撤销功能型魔法操作必须满足pFrom不影响pTo的原则，即pFrom为空时与pFrom为空时造成的效果（包括Do和Cancel）对pTo没有区别。
//并且这些魔法操作里对pFrom的操作在pFrom为空时不执行。
class CFunctionCancelableMagicOp
	:public CFunctionMagicOpServer
{
public:
	virtual void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	virtual void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo){};
	virtual bool Cancelable()const
	{ 
		return true;
	}
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	virtual bool IsSetupNestedObj() const
	{
		return false;
	}
};
