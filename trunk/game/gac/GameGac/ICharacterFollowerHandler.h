#pragma once
#include "GameDef.h"
#include "ICoreObjectFollowerHandler.h"

class ICharacterFollowerHandler : public ICoreObjectFollowerHandler
{
public:
	virtual void* GetTag() const { return (void*)eCOHT_Character; }
	virtual ~ICharacterFollowerHandler() {}
};
