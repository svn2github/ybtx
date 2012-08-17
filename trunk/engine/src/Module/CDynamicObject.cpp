#include "stdafx.h"
#include "CDynamicObject.h"
#include "BaseHelper.h"

CDynamicObject::CDynamicObject()
{

}

CDynamicObject::~CDynamicObject()
{
	AdvDisbind(this);
}
