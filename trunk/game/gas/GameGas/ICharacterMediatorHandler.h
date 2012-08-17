#pragma once
#include "ICoreObjectMediatorHandler.h"

class ICharacterMediatorHandler : public ICoreObjectMediatorHandler
{
public:
	virtual void* GetTag() const { return (void*)eCOHT_Character; }
	virtual ~ICharacterMediatorHandler() {}
};
