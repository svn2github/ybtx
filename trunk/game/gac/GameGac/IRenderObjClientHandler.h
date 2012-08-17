#pragma once
#include "IModelHandler.h"
#include "ISpaceNodeHandler.h"

class IRenderObjClientHandler : public IModelHandler ,public ISpaceNodeHandler
{
public:
	virtual ~IRenderObjClientHandler(){}
};