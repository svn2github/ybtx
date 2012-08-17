#pragma once

#include "CSkillRuleDef.h"

class TypeName2SRTarget
{
	typedef map<string, ESRTargetType> Name2SRTarget_T;
	typedef map<string, ERCondType> Name2RCondType_T;
	typedef Name2SRTarget_T::const_iterator  mapName2SRTarget_itr;
public:
	TypeName2SRTarget();
	Name2SRTarget_T mapTargetType;
	Name2RCondType_T mapCondType;
private:
	void InitTargetTypeMap();
	void InitCondTypeMap();
};
