#pragma once
#include "TNpcAIDataAllocator.h"

class CNormalSkillServerSharedPtr;

enum ESpecialSkillName
{
	eSSN_EraseDecreaseState,
	eSSN_SetupImmune,
	eSSN_CancelImmune,
	eSSN_DieBurstSoul,
	eSSN_NormalDieBurstSoul,
	eSSN_Exclaim,
	eSSN_ByExclaim,
};

class CNpcSpecialSkillCfg
{
public:
	typedef vector<const CNormalSkillServerSharedPtr*, TNpcAIDataAllocator<const CNormalSkillServerSharedPtr*> > VecNpcSpecialSkill;
	static VecNpcSpecialSkill ms_vecNpcSpecialSkill;
	static bool LoadNpcSpecialSkill();
	static void UnLoadNpcSpecialSkill();
};

