#pragma once
#include "CMagicCondClient.h"

// 处于魔法状态
class CTestMagicStateMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CTestNotInMagicStateMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CTestNotInRepeatedMagicStateMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};


// 处于触发器状态
class CTestTriggerStateMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

// 处于伤害变更状态
class CTestDamageChangeStateMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CTestNotInDamageChangeStateMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

// 处于特殊状态
class CTestSpecialStateMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CTestMagicStateCascadeGreaterOrEqualMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};


// 处于姿态技能
class CTestAureMagicMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

// 装备了盾牌
class CTestShieldEquipMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

// 主手装备了
class CTestMainHandEquipMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CTestAssistHandEquipMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CTestIsExistHorseMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};
class CTestIsNotExistHorseMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CTestTargetCanBeControlledMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CTestBurstSoulTimesGreaterOrEqualMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CTestTargetIsPlayerMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CTestTargetIsNPCMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CTestTargetIsSummonMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CTestTargetLevelLesserOrEqualMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CTestNotInBattleMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CTestNotOnMissionMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CTestIsExistPetMCOND
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

