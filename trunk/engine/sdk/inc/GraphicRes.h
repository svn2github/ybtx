#pragma once
#include "CRefObject.h"
#include "CGraphicMallocObject.h"

namespace sqr
{

class GraphicRes 
	: public CRefObject
{
public:
	virtual bool	IsValid() = 0;
};

}