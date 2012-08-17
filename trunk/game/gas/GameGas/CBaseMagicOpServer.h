#pragma once
#include "FightDef.h"
#include "CConfigMallocObject.h"

class CCfgArg;
class CMagicOpCalcArg;

enum	EMagicOPClassType
{
	eMOPCT_MagicCondition,
	eMOPCT_MagicOP,
};

class CBaseMagicOpServer
	:public CConfigMallocObject
{
public:
	virtual bool CanExecuteWithAttackerIsDead()const
	{
		return false;
	}
	virtual bool CanExecuteWithTargetIsDead()const
	{
		return false;
	}
	virtual EMagicOPClassType GetType()const=0;
	virtual CCfgArg* InitArg(const string& szArg);
	virtual ~CBaseMagicOpServer(){}
	static void Release();
	static CMagicOpCalcArg* ms_pMagicOpCalcArg;
};

