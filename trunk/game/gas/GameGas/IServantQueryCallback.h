#pragma once

#include "CDynamicObject.h"
#include "CallBackX.h"
#include "CNpcAIMallocObject.h"

class CCharacterDictator;

class IServantQueryCallback
	: virtual public CDynamicObject
	, public CNpcAIMallocObject
{
public:
	void QueryServantJob(CCharacterDictator* pCharacter);
	virtual void Exec(uint32 uEntityID, CCharacterDictator* pServant)
	{
		CALL_CLASS_CALLBACK_2(uEntityID, pServant)
	}
};
