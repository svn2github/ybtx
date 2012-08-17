#pragma once
#include "ICoreObjectDictatorHandler.h"

//class ICharacterDictatorHandler : public ICoreObjectDictatorForEntityHandler
class ICharacterDictatorHandler : public ICoreObjectDictatorHandler
{
public:
	virtual void* GetTag() const { return (void*)eCOHT_Character; }
	virtual ~ICharacterDictatorHandler() {}
};
