#pragma once
#include "GameDef.h"
#include "ICoreObjectDirectorHandler.h"

class ICharacterDirectorHandler : public ICoreObjectDirectorHandler
{
public:
	virtual void OnPosChanged(){}
	virtual void* GetTag() const { return (void*)eCOHT_Character; }
	virtual ~ICharacterDirectorHandler() {}
};
