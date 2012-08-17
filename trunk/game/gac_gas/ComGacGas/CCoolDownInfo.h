#pragma once
#include "CDynamicObject.h"
#include "FightDef.h"
#include "CFighterMallocObject.h"

class CCoolDownInfo
	:public virtual CDynamicObject
	,public CFighterMallocObject
{
public:
	uint64	m_uBeginTime;
	uint64	m_uEndTime;
	uint64  m_uCoolDownTime;
	ESkillCoolDownType m_eCoolDownType;
	bool	m_bSwitchEquipSkill;
};

