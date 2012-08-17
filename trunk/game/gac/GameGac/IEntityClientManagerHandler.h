#pragma once
#include "CallBackX.h"
#include "CEntityClient.h"
#include "CStaticObject.h"

class IEntityClientManagerHandler : public virtual CStaticObject
{
public:
	virtual ~IEntityClientManagerHandler(){}
	virtual void OnGameEntityAdded(CEntityClient* pObject)  {
		CALL_CLASS_CALLBACK_1(pObject)
	};
};