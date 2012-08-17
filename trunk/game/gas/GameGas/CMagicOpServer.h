#pragma once
#include "CBaseMagicOpServer.h"

enum	EMagicOpClassType
{
	eMOT_Value,
	eMOT_Function,
	eMOT_PosFunction,
};

class CMagicOpServer
	: public CBaseMagicOpServer
{
public:
	virtual EMagicOPClassType GetType()const
	{
		return eMOPCT_MagicOP;
	}
	virtual EMagicOpClassType GetMagicOpType()const=0;
	virtual bool Cancelable()const
	{
		return false;
	}
	virtual ~CMagicOpServer(){}
};


