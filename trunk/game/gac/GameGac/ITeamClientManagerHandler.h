#pragma once
#include "CStaticObject.h"

class ITeamClientManagerHandler : public virtual CStaticObject
{
public:
	virtual void UpdateTeamInfo(){CALL_CLASS_CALLBACK()}
};