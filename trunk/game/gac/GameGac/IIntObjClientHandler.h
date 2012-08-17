#pragma once
#include "GameDef.h"
#include "ICoreObjectFollowerHandler.h"

class IIntObjClientHandler : public ICoreObjectFollowerHandler
{
public:
	virtual void* GetTag() const { return (void*)eCOHT_Obj; }
};
