#pragma once

#include "CDynamicObject.h"
#include "CallBackX.h"
#include "CNpcAIMallocObject.h"

class CCharacterDictator;

class INpcEnmityQueryCallback
	: virtual public CDynamicObject
	, public CNpcAIMallocObject
{
public:
	void QueryEnemyJob(CCharacterDictator* pCharacter);
	virtual void Exec(uint32 uEntityID, CCharacterDictator* pServant)
	{
		CALL_CLASS_CALLBACK_2(uEntityID, pServant)
	}
};
