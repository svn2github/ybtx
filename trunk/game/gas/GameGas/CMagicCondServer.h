#pragma once
#include "CBaseMagicOpServer.h"
//***********************Ä§·¨Ìõ¼þ********************************
enum	EMagicCondClassType
{
	eMCT_Value,
	eMCT_Function,
};

class CMagicCondServer
	:public CBaseMagicOpServer
{
public:
	virtual EMagicCondClassType GetMagicCondType()const=0;
	virtual EMagicOPClassType GetType()const
	{
		return eMOPCT_MagicCondition;
	}
};

