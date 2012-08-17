#pragma once
#include "CFunctionMagicCondServer.h"


class CTestMagicStateMCOND
	:public CStateExistInClientFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};

class CNotTestMagicStateAndSelfExistTalentMCOND
	:public CStateExistInClientFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};

class CTestNotInMagicStateMCOND
	:public CStateExistInClientFunctionMagicCondServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};

class CTestNotInRepeatedMagicStateMCOND
	:public CFunctionMagicCondServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};

class CTestNotInDamageChangeStateMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};
class CTestNotInSpecialStateMCOND
	:public CStateExistInClientFunctionMagicCondServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};
class CTestSelfMagicStateMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};

class CTestTriggerStateMCOND
	:public CStateExistInClientFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};

class CTestSelfTriggerStateMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};

class CTestDamageChangeStateMCOND
	:public CStateExistInClientFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};

class CTestSelfDamageChangeStateMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};

class CTestSpecialStateMCOND
	:public CStateExistInClientFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};

class CTestSelfSpecialStateMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};

class CTestNotInStateByDecreaseTypeMCOND
:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
};

class CNotTestInStateByDecreaseTypeAndSelfExistTalentMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};

class CTestMagicStateCascadeGreaterOrEqualMCOND
	:public CStateExistInClientFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};

class CTestAureMagicMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
};

class CTestShieldEquipMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestMainHandEquipMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestAssistHandEquipMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestIsExistPetMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestIsNotExistPetMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestIsExistTotemMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestIsExistHorseMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestIsNotExistHorseMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetCanBeControlledMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetCanBeRavinMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetIsPlayerMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
};

class CTestTargetIsFriendMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetIsFriendsServantMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetIsEnemyMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetClassMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetIsNPCMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetIsSummonMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetLevelLesserOrEqualMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetIsSelfCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestNotInBattleMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestNotOnMissionMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestNotBattleHorseStateMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestWeaponTypeMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

// 骑乘状态检查
class CTestRidingMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

//检测三角路径上是否有障碍
class CTestTrianglePathMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

//检测是否是可用尸体
class CTestIsAvailableDeadBobyMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetLevelIsInScopeMCOND
:public CFunctionMagicCondServer
{
public:
	bool CanExecuteWithTargetIsDead()const
	{
		return true;
	}
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgArg* pArg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};

class CTestTargetDistIsLesserThanMCOND
:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetIsVestInSelfMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetIsRaidmatesMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetInCastingProcess
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetIsMoving
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetIsMoveMagic
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetNotIsMoveMagic
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};


class CTestTargetIsCamp
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

//特殊条件,仅用于流程是检查需要撤销的姿态,撤销这些姿态,用于流程清楚姿态
class CTestRidingAureMagicMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestBurstSoulTimesGreaterOrEqualMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestContinuousBurstSoulTimesEqualMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetHavePositionMagic
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestExpIsVestInSelfAndTriggerStateMCOND
	:public CStateExistInClientFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CCfgArg* InitArg(const string& szArg);
};

class CTestTargetLearnSkillMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetLevelLesserOrEqualToExpOwnerMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetSkillIsCoolDownMCOND
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};

class CTestTargetInScene
	:public CFunctionMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const;
};
