#include "stdafx.h"
#include "CStaticObject.h"
#include "BaseHelper.h"

CStaticObject::CStaticObject()
{

}

CStaticObject::~CStaticObject()
{
	AdvDisbind(this);
}
