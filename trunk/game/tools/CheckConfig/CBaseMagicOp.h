#pragma once
#include "FightDef.h"

class CCfgCalc;

enum EMagicOpBaseType
{
	eMOPBT_MagicCondition,
	eMOPBT_MagicOP,
};

enum EMagicOpType
{
	eMOT_Value,
	eMOT_Function,
	eMOT_PosFunction,
};

enum EValueMagicOpType
{
	eVMOT_None,
	eVMOT_Physical,
	eVMOT_Magic,
	eVMOT_End,
};

class CBaseMagicOp
{
public:
	virtual EMagicOpBaseType GetBaseType() const = 0;
	virtual bool Cancelable() const = 0;
	virtual bool CheckObject2MOP(EObjFilterType eObjFilterType);//检测再次筛选目标对于魔法操作的有效性
	virtual bool CheckArg2MOP(CCfgCalc* pMagicOpArg) { return true;}//检测魔法操作参数对于魔法操作的有效性
	virtual bool CheckIsCancelEffOfState() const { return false; }
};

class CMagicCond : public CBaseMagicOp
{
public:
	virtual EMagicOpBaseType GetBaseType() const { return eMOPBT_MagicCondition; }
	virtual bool Cancelable() const { return false; }
};

class CMagicOp : public CBaseMagicOp
{
public:
	virtual EMagicOpBaseType GetBaseType() const{ return eMOPBT_MagicOP; }
	virtual EMagicOpType GetMagicOpType() const = 0;
	virtual bool Cancelable() const { return false; }
};

class CValueMagicCond : public CMagicCond
{
public:
	virtual bool CheckArg2MOP(CCfgCalc* pMagicOpArg);
};


class CMagicCondWithEmptyArg
	: public CMagicCond
{
	bool CheckArg2MOP(CCfgCalc* pMagicOpArg);
};

class CValueMagicOp : public CMagicOp
{
public:
	virtual EMagicOpType GetMagicOpType() const { return eMOT_Value; }
	virtual bool CheckArg2MOP(CCfgCalc* pMagicOpArg);
	virtual bool CalcNeedTarget(CCfgCalc* pArg) const;
	virtual EValueMagicOpType GetValueOpType() const { return eVMOT_End; }
};

class CFunctionMagicOp : public CMagicOp
{
public:
	virtual EMagicOpType GetMagicOpType() const { return eMOT_Function; }
};

class CPosMagicOp : public CMagicOp
{
public:
	virtual EMagicOpType GetMagicOpType() const { return eMOT_PosFunction; }
	virtual bool CheckObject2MOP(EObjFilterType eObjFilterType);
};

class CValueCancelableMagicOp : public CValueMagicOp
{
public:
	virtual bool Cancelable() const { return true; }
};

class CFunctionCancelableMagicOp : public CFunctionMagicOp
{
public:
	virtual bool Cancelable() const { return true; }
	virtual bool IsSetupNestedObj() const { return false; }
};

class CFunctionMagicOpWithEmptyArg
	: public CFunctionMagicOp
{
	bool CheckArg2MOP(CCfgCalc* pMagicOpArg);
};

// CBaseMagicOp -- CMagicOp -- CValueMagicOp
class CCalInstantHurtMop
	: public CValueMagicOp
{
public:
	bool CheckArg2MOP(CCfgCalc* pMagicOpArt);
};

// CBaseMagicOp -- CMagicOp -- CPosMagicOp
class CRelPositionMagicByDirAndDistanceMOP
	: public CPosMagicOp
{
public:
	bool CheckObject2MOP(EObjFilterType eObjFilterType);
	bool CheckArg2MOP(CCfgCalc* pMagicOpArg);
};

class CRelPositionMagicRandomInAreaMOP
	: public CPosMagicOp
{
public:
	bool CheckObject2MOP(EObjFilterType eObjFilterType);
	bool CheckArg2MOP(CCfgCalc* pMagicOpArg);
};

// CBaseMagicOp -- CMagicOp -- CPosMagicOp
class CCreateTotemByDirAndDistanceMOP
	: public CPosMagicOp
{
public:
	bool CheckObject2MOP(EObjFilterType eObjFilterType);
	bool CheckArg2MOP(CCfgCalc* pMagicOpArg);
};

class CCPosMagicOpWithEmptyArg
	: public CPosMagicOp
{
	bool CheckArg2MOP(CCfgCalc* pMagicOpArg);
};

class CFunctionCancelableMagicOpWithEmptyArg
	: public CFunctionCancelableMagicOp
{
	bool CheckArg2MOP(CCfgCalc* pMagicOpArg);
};

class CValueCancelableMagicOpWithEmptyArg
	: public CValueCancelableMagicOp
{
	bool CheckArg2MOP(CCfgCalc* pMagicOpArg);
};

#define DefineCondMOP(Name) \
class Name : public CMagicCond \
{ \
public: \
	bool CheckArg2MOP(CCfgCalc* pMagicOpArg);\
};

DefineCondMOP(CTestMagicStateMCOND);
DefineCondMOP(CNotTestMagicStateAndSelfExistTalentMCOND);
DefineCondMOP(CTestNotInDamageChangeStateMCOND);
DefineCondMOP(CTestNotInMagicStateMCOND);
DefineCondMOP(CTestNotInRepeatedMagicStateMCOND);
DefineCondMOP(CTestNotInSpecialStateMCOND);
DefineCondMOP(CTestSelfMagicStateMCOND);
DefineCondMOP(CTestTriggerStateMCOND);
DefineCondMOP(CTestSelfTriggerStateMCOND);
DefineCondMOP(CTestDamageChangeStateMCOND);
DefineCondMOP(CTestSelfDamageChangeStateMCOND);
DefineCondMOP(CTestSpecialStateMCOND);
DefineCondMOP(CTestSelfSpecialStateMCOND);
DefineCondMOP(CTestNotInStateByDecreaseTypeMCOND);
DefineCondMOP(CNotTestInStateByDecreaseTypeAndSelfExistTalentMCOND);
DefineCondMOP(CTestMagicStateCascadeGreaterOrEqualMCOND);
DefineCondMOP(CTestAureMagicMCOND);
DefineCondMOP(CProbabilityLesserCond);
DefineCondMOP(CTestExpIsVestInSelfAndTriggerStateMCOND);
DefineCondMOP(CTestTargetLearnSkillMCOND);
DefineCondMOP(CTestTargetLevelLesserOrEqualToExpOwnerMCOND);
DefineCondMOP(CTestWeaponTypeMCOND);
DefineCondMOP(CTestIsAvailableDeadBobyMCOND);
DefineCondMOP(CTestTargetLevelIsInScopeMCOND);
DefineCondMOP(CTestTargetInCastingProcess);
DefineCondMOP(CTestRidingAureMagicMCOND);
DefineCondMOP(CTestBurstSoulTimesGreaterOrEqualMCOND);
DefineCondMOP(CTestContinuousBurstSoulTimesEqualMCOND);
DefineCondMOP(CTestTargetHavePositionMagic);
DefineCondMOP(CTestMainHandEquipMCOND);
DefineCondMOP(CTestAssistHandEquipMCOND);
DefineCondMOP(CTestIsExistPetMCOND);
DefineCondMOP(CTestIsNotExistPetMCOND);
DefineCondMOP(CTestTargetClassMCOND);
DefineCondMOP(CTestTargetIsNPCMCOND);
DefineCondMOP(CTestRidingMCOND);
DefineCondMOP(CTestTargetDistIsLesserThanMCOND);
DefineCondMOP(CTestTargetIsCamp);
DefineCondMOP(CTestTargetSkillIsCoolDownMCOND);
DefineCondMOP(CTestTargetInScene);
DefineCondMOP(CTestDebaseEnmityMOP);
DefineCondMOP(CTestTargetLevelLesserOrEqualMCOND);


#define DefineCondDefaultMOP(Name) class Name : public CMagicCondWithEmptyArg {};

DefineCondDefaultMOP(CTestShieldEquipMCOND);
DefineCondDefaultMOP(CTestIsExistTotemMCOND);
DefineCondDefaultMOP(CTestIsExistHorseMCOND);
DefineCondDefaultMOP(CTestIsNotExistHorseMCOND);
DefineCondDefaultMOP(CTestTargetCanBeControlledMCOND);
DefineCondDefaultMOP(CTestTargetCanBeRavinMCOND);
DefineCondDefaultMOP(CTestTargetIsPlayerMCOND);
DefineCondDefaultMOP(CTestTargetIsFriendMCOND);
DefineCondDefaultMOP(CTestTargetIsFriendsServantMCOND);
DefineCondDefaultMOP(CTestTargetIsEnemyMCOND);
DefineCondDefaultMOP(CTestTargetIsSummonMCOND);
DefineCondDefaultMOP(CTestTargetIsSelfCOND);
DefineCondDefaultMOP(CTestNotInBattleMCOND);
DefineCondDefaultMOP(CTestNotOnMissionMCOND);
DefineCondDefaultMOP(CTestNotBattleHorseStateMCOND);
DefineCondDefaultMOP(CTestTrianglePathMCOND);
DefineCondDefaultMOP(CTestTargetIsVestInSelfMCOND);
DefineCondDefaultMOP(CTestTargetIsRaidmatesMCOND);
DefineCondDefaultMOP(CTestTargetIsMoving);
DefineCondDefaultMOP(CTestTargetIsMoveMagic);
DefineCondDefaultMOP(CTestTargetNotIsMoveMagic);

#define DefineCondDefaultMOP1(Name) class Name : public CValueMagicCond {};
DefineCondDefaultMOP1(CHealthPointLesserMCOND);
DefineCondDefaultMOP1(CManaPointLesserMCOND);
DefineCondDefaultMOP1(CHealthPointGreaterOrEqualMCOND);
DefineCondDefaultMOP1(CManaPointGreaterOrEqualMCOND);
DefineCondDefaultMOP1(CMaxManaPointGreaterOrEqualMCOND);
DefineCondDefaultMOP1(CEnergyPointGreaterOrEqualMCOND);
DefineCondDefaultMOP1(CRagePointGreaterOrEqualMCOND);
DefineCondDefaultMOP1(CComboPointGreaterOrEqualMCOND);
DefineCondDefaultMOP1(CDefenceLesserMCOND);
DefineCondDefaultMOP1(CDefenceGreaterOrEqualMCOND);
DefineCondDefaultMOP1(CNatureResistanceValueLesserMCOND);
DefineCondDefaultMOP1(CNatureResistanceValueGreaterOrEqualMCOND);
DefineCondDefaultMOP1(CDestructionResistanceValueLesserMCOND);
DefineCondDefaultMOP1(CDestructionResistanceValueGreaterOrEqualMCOND);
DefineCondDefaultMOP1(CEvilResistanceValueLesserMCOND);
DefineCondDefaultMOP1(CEvilResistanceValueGreaterOrEqualMCOND);
DefineCondDefaultMOP1(CDistOfAttackerAndTargetLesserCond);
DefineCondDefaultMOP1(CDistOfAttackerAndTargetGreaterOrEqualCond);
DefineCondDefaultMOP1(CDistOfMagicAndTargetLesserCond);
DefineCondDefaultMOP1(CDistOfMagicAndTargetGreaterOrEqualCond);

#define DefineCancelableValueMOP(Name) \
class Name : public CValueCancelableMagicOp \
{ \
public: \
	virtual bool Cancelable() const { return true; } \
};

DefineCancelableValueMOP(CChangeMaxHealthPointAdderMOP);
DefineCancelableValueMOP(CChangeMaxHealthPointMultiplierMOP);
DefineCancelableValueMOP(CChangeMaxManaPointAdderMOP);
DefineCancelableValueMOP(CChangeMaxManaPointMultiplierMOP);
DefineCancelableValueMOP(CChangeHPUpdateRateMultiplierMOP);
DefineCancelableValueMOP(CChangeMPUpdateRateMultiplierMOP);
DefineCancelableValueMOP(CChangeRPUpdateValueAdderMOP);
DefineCancelableValueMOP(CChangeRPUpdateValueMultiplierMOP);
DefineCancelableValueMOP(CChangeEPUpdateValueAdderMOP);
DefineCancelableValueMOP(CChangeEPUpdateValueMultiplierMOP);
DefineCancelableValueMOP(CChangeMPConsumeRateMultiplierMOP);
DefineCancelableValueMOP(CChangeRPConsumeRateMultiplierMOP);
DefineCancelableValueMOP(CChangeEPConsumeRateMultiplierMOP);
DefineCancelableValueMOP(CChangeNatureMPConsumeRateMultiplierMOP);
DefineCancelableValueMOP(CChangeDestructionMPConsumeRateMultiplierMOP);
DefineCancelableValueMOP(CChangeEvilMPConsumeRateMultiplierMOP);
DefineCancelableValueMOP(CChangeRPProduceRateMultiplierMOP);
DefineCancelableValueMOP(CChangeDefenceAdderMOP);
DefineCancelableValueMOP(CChangeDefenceMultiplierMOP);
DefineCancelableValueMOP(CChangePhysicalDPSAdderMOP);
DefineCancelableValueMOP(CChangePhysicalDPSMultiplierMOP);
DefineCancelableValueMOP(CChangeResilienceValueAdderMOP);
DefineCancelableValueMOP(CChangeResilienceValueMultiplierMOP);
DefineCancelableValueMOP(CChangeStrikeResistanceValueAdderMOP);
DefineCancelableValueMOP(CChangeStrikeResistanceValueMultiplierMOP);
DefineCancelableValueMOP(CChangeAccuratenessValueAdderMOP);
DefineCancelableValueMOP(CChangeAccuratenessValueMultiplierMOP);
DefineCancelableValueMOP(CChangeStrikeValueAdderMOP);
DefineCancelableValueMOP(CChangeStrikeValueMultiplierMOP);
DefineCancelableValueMOP(CChangeStrikeMultiValueAdderMOP);
DefineCancelableValueMOP(CChangeStrikeMultiValueMultiplierMOP);
DefineCancelableValueMOP(CChangeBlockRateMultiplierMOP);
DefineCancelableValueMOP(CChangeBlockDamageAdderMOP);
DefineCancelableValueMOP(CChangeBlockDamageMultiplierMOP);
DefineCancelableValueMOP(CChangePhysicalDodgeValueAdderMOP);
DefineCancelableValueMOP(CChangePhysicalDodgeValueMultiplierMOP);
DefineCancelableValueMOP(CChangeParryValueAdderMOP);
DefineCancelableValueMOP(CChangeParryValueMultiplierMOP);
DefineCancelableValueMOP(CChangeMagicDodgeValueAdderMOP);
DefineCancelableValueMOP(CChangeMagicDodgeValueMultiplierMOP);
DefineCancelableValueMOP(CChangeMagicHitValueAdderMOP);
DefineCancelableValueMOP(CChangeMagicHitValueMultiplierMOP);
DefineCancelableValueMOP(CChangePunctureDamageAdderMOP);
DefineCancelableValueMOP(CChangePunctureDamageMultiplierMOP);
DefineCancelableValueMOP(CChangeChopDamageAdderMOP);
DefineCancelableValueMOP(CChangeChopDamageMultiplierMOP);
DefineCancelableValueMOP(CChangeBluntDamageAdderMOP);
DefineCancelableValueMOP(CChangeBluntDamageMultiplierMOP);
DefineCancelableValueMOP(CChangeMagicDamageValueAdderMOP);
DefineCancelableValueMOP(CChangeMagicDamageValueMultiplierMOP);
DefineCancelableValueMOP(CChangeNatureDamageValueAdderMOP);
DefineCancelableValueMOP(CChangeNatureDamageValueMultiplierMOP);
DefineCancelableValueMOP(CChangeDestructionDamageValueAdderMOP);
DefineCancelableValueMOP(CChangeDestructionDamageValueMultiplierMOP);
DefineCancelableValueMOP(CChangeEvilDamageValueAdderMOP);
DefineCancelableValueMOP(CChangeEvilDamageValueMultiplierMOP);
DefineCancelableValueMOP(CChangeNatureResistanceValueAdderMOP);
DefineCancelableValueMOP(CChangeNatureResistanceValueMultiplierMOP);
DefineCancelableValueMOP(CChangeEvilResistanceValueAdderMOP);
DefineCancelableValueMOP(CChangeEvilResistanceValueMultiplierMOP);
DefineCancelableValueMOP(CChangeDestructionResistanceValueAdderMOP);
DefineCancelableValueMOP(CChangeDestructionResistanceValueMultiplierMOP);
DefineCancelableValueMOP(CChangePhysicalDamageAdderMOP);
DefineCancelableValueMOP(CChangeAssistPhysicalDamageAdderMOP);
DefineCancelableValueMOP(CChangePhysicalDamageMultiplierMOP);
DefineCancelableValueMOP(CChangeAssistPhysicalDamageMultiplierMOP);
DefineCancelableValueMOP(CChangeMagicDamageAdderMOP);
DefineCancelableValueMOP(CChangeSelfMagicDamageAdderMOP);
DefineCancelableValueMOP(CChangeMagicDamageMultiplierMOP);
DefineCancelableValueMOP(CChangeDOTDamageAdderMOP);
DefineCancelableValueMOP(CChangeDOTDamageMultiplierMOP);
DefineCancelableValueMOP(CChangeResistCastingInterruptionRateMultiplierMOP);
DefineCancelableValueMOP(CChangePenetrationValueAdderMOP);
DefineCancelableValueMOP(CChangePenetrationValueMultiplierMOP);
DefineCancelableValueMOP(CChangeProtectionValueAdderMOP);
DefineCancelableValueMOP(CChangeProtectionValueMultiplierMOP);
DefineCancelableValueMOP(CChangeStealthMOP);
DefineCancelableValueMOP(CChangeKeennessMOP);
DefineCancelableValueMOP(CChangeNatureSmashValueAdderMOP);
DefineCancelableValueMOP(CChangeNatureSmashValueMultiplierMOP);
DefineCancelableValueMOP(CChangeDestructionSmashValueAdderMOP);
DefineCancelableValueMOP(CChangeEvilSmashValueAdderMOP);
DefineCancelableValueMOP(CChangeEvilSmashValueMultiplierMOP);
DefineCancelableValueMOP(CChangeDefenceSmashValueAdderMOP);
DefineCancelableValueMOP(CChangeDefenceSmashValueMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraDOTDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraPunctureDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraChopDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraBluntDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraNatureDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraEvilDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraDestructionDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraBowDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraCrossBowDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraTwohandWeaponDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraPolearmDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraPaladinWeaponDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraAssistWeaponDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraLongDistDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraShortDistDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeLongDistWeaponDamageRateMultiplierMOP);
DefineCancelableValueMOP(CChangeDamageDecsRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraCureValueRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraBeCuredValueRateMultiplierMOP);
DefineCancelableValueMOP(CChangeControlDecreaseResistRateMultiplierMOP);
DefineCancelableValueMOP(CChangePauseDecreaseResistRateMultiplierMOP);
DefineCancelableValueMOP(CChangeCripplingDecreaseResistRateMultiplierMOP);
DefineCancelableValueMOP(CChangeDebuffDecreaseResistRateMultiplierMOP);
DefineCancelableValueMOP(CChangeDOTDecreaseResistRateMultiplierMOP);
DefineCancelableValueMOP(CChangeSpecialDecreaseResistRateMultiplierMOP);
DefineCancelableValueMOP(CChangeControlDecreaseTimeRateMultiplierMOP);
DefineCancelableValueMOP(CChangePauseDecreaseTimeRateMultiplierMOP);
DefineCancelableValueMOP(CChangeCripplingDecreaseTimeRateMultiplierMOP);
DefineCancelableValueMOP(CChangeSpecialDecreaseTimeRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraPhysicDefenceRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraPhysicDodgeRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraParryRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraStrikeRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraMagicDodgeRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraMagicResistanceRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraNatureResistanceRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraDestructionResistanceRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraEvilResistanceRateMultiplierMOP);
DefineCancelableValueMOP(CChangeExtraCompleteResistanceRateMultiplierMOP);
DefineCancelableValueMOP(CChangeMissRateMultiplierMOP);
DefineCancelableValueMOP(CChangeCastingProcessTimeRatioMultiplierMOP);
DefineCancelableValueMOP(CChangeNpcAttackScopeToMOP);
DefineCancelableValueMOP(CChangeImmuneRateMultiplierMOP);

#define DefineCancelableValueMOP1(Name) \
class Name : public CValueCancelableMagicOpWithEmptyArg \
{ \
public: \
	virtual bool Cancelable() const { return true; } \
};

DefineCancelableValueMOP1(CDisableStealthMOP);
DefineCancelableValueMOP1(CDisableKeennessMOP);

#define DefineCancelableValueMOP2(Name) \
class Name : public CValueCancelableMagicOp \
{ \
public: \
	bool CheckArg2MOP(CCfgCalc* pMagicOpArg);\
	virtual bool Cancelable() const { return true; } \
};

DefineCancelableValueMOP2(CChangePercentMoveSpeedMOP);
DefineCancelableValueMOP2(CChangePercentWalkSpeedMOP);
DefineCancelableValueMOP2(CChangePercentNormalAttackSpeedMOP);

class CCalValueMagicOp : public CValueMagicOp
{
public:
	virtual bool CheckArg2MOP(CCfgCalc* pMagicOpArg);
};

#define DefineCalValueMOP(Name) class Name : public CCalValueMagicOp {};

DefineCalValueMOP(CCalNonTypeHurtMop);

class CCalcNeedTargetValueMagicOp : public CCalValueMagicOp
{
public:
	virtual bool CalcNeedTarget(CCfgCalc* pArg) const
	{
		return true;
	}
};


#define DefineValueDefaultMOP(Name) class Name : public CValueMagicOp {};
DefineValueDefaultMOP(CTaskNonBattleHurtMOP);
DefineValueDefaultMOP(CTaskBombHurtMOP);
DefineValueDefaultMOP(CTaskSpecialHurtMOP);
DefineValueDefaultMOP(CTaskNonNationBattleBuildHurtMOP);
DefineValueDefaultMOP(CChangeHPMOP);
DefineValueDefaultMOP(CChangeManaPointMOP);
DefineValueDefaultMOP(CChangeRagePointMOP);
DefineValueDefaultMOP(CChangeEnergyPointMOP);
DefineValueDefaultMOP(CChangeComboPointMOP);
DefineValueDefaultMOP(CTargetChangeHPMOP);
DefineValueDefaultMOP(CSuckBloodMOP);
DefineValueDefaultMOP(CNoEventChangeHPMOP);
DefineValueDefaultMOP(CAbsorbManaPointMOP);
DefineValueDefaultMOP(CAbsorbRagePointMOP);
DefineValueDefaultMOP(CDrugChangeHPMOP);
DefineValueDefaultMOP(CIgnoreImmuneChangeHPMOP);

#define DefineCalcNeedTargetValueMOP(Name) \
class Name : public CCalcNeedTargetValueMagicOp \
{ \
public: \
	virtual EValueMagicOpType GetValueOpType() const; \
};

DefineCalcNeedTargetValueMOP(CCureMop);
DefineCalcNeedTargetValueMOP(CCalculateMainHandPhysicsHurtMop);
DefineCalcNeedTargetValueMOP(CCalculateAssistantPhyicsHurtMop);
DefineCalcNeedTargetValueMOP(CCalculateMainHandPhysicsStrikeMop);
DefineCalcNeedTargetValueMOP(CCalculateMainHandPhysicsNoStrikeHurtMop);
DefineCalcNeedTargetValueMOP(CCalculateMainHandPhysicsOnlyHitAndStrikeHurtMop);
DefineCalcNeedTargetValueMOP(CCalNatureMagHurtMop);
DefineCalcNeedTargetValueMOP(CCalDestructionMagHurtMop);
DefineCalcNeedTargetValueMOP(CCalEvilMagHurtMop);
DefineCalcNeedTargetValueMOP(CCalNatureMagNoStrikeHurtMop);
DefineCalcNeedTargetValueMOP(CCalDestructionMagNoStrikeHurtMop);
DefineCalcNeedTargetValueMOP(CCalDestructionMagNoResistHurtMop);
DefineCalcNeedTargetValueMOP(CCalEvilMagNoStrikeHurtMop);
DefineCalcNeedTargetValueMOP(CCalDOTHurtMop);
DefineCalcNeedTargetValueMOP(CCalDOTNoStrikeHurtMop);

#define DefineCancelableFunctionMOP(Name) \
class Name : public CFunctionCancelableMagicOpWithEmptyArg \
{ \
public: \
	virtual bool Cancelable() const { return true; } \
};

DefineCancelableFunctionMOP(CResistControlMOP);
DefineCancelableFunctionMOP(CResistPauseMOP);
DefineCancelableFunctionMOP(CResistCripplingMOP);
DefineCancelableFunctionMOP(CResistDOTMOP);
DefineCancelableFunctionMOP(CTargetForbitMoveMOP);
DefineCancelableFunctionMOP(CTargetForbitTurnAroundMOP);
DefineCancelableFunctionMOP(CTargetSuspendNAMOP);
DefineCancelableFunctionMOP(CTargetForbitUseNatureSkillMOP);
DefineCancelableFunctionMOP(CTargetForbitUseDestructionSkillMOP);
DefineCancelableFunctionMOP(CTargetForbitUseEvilSkillMOP);
DefineCancelableFunctionMOP(CTargetForbitClientOperateMOP);
DefineCancelableFunctionMOP(CTargetForbitUseClassSkillMOP);
DefineCancelableFunctionMOP(CTargetForbitUseDrugItemSkillMOP);
DefineCancelableFunctionMOP(CTargetForbitUseNotFightSkillMOP);
DefineCancelableFunctionMOP(CTargetForbitUseMissionItemSkillMOP);
DefineCancelableFunctionMOP(CFeignDeathMOP);
DefineCancelableFunctionMOP(CReflectMagicMOP);
DefineCancelableFunctionMOP(CContinueAddMPMOP);
DefineCancelableFunctionMOP(CStopMoveMOP);
DefineCancelableFunctionMOP(CBreakOutMOP);
DefineCancelableFunctionMOP(CForbitUseWeaponMOP);
DefineCancelableFunctionMOP(CTargetSetHiddenMOP);
DefineCancelableFunctionMOP(CSetNoSingTimeForever);
DefineCancelableFunctionMOP(CChangeTargetToBarrierMOP);
DefineCancelableFunctionMOP(CChangeNAActionRhythmMOP);
DefineCancelableFunctionMOP(CDisTriggerEventMOP);
DefineCancelableFunctionMOP(CReplaceSkillMOP);
DefineCancelableFunctionMOP(CImmunePunctureDamageMOP);
DefineCancelableFunctionMOP(CImmuneChopDamageMOP);
DefineCancelableFunctionMOP(CImmuneBluntDamageMOP);
DefineCancelableFunctionMOP(CImmuneNatureDamageMOP);
DefineCancelableFunctionMOP(CImmuneDestructionDamageMOP);
DefineCancelableFunctionMOP(CImmuneEvilDamageMOP);
DefineCancelableFunctionMOP(CImmuneControlDecreaseMOP);
DefineCancelableFunctionMOP(CImmunePauseDecreaseMOP);
DefineCancelableFunctionMOP(CImmuneCripplingDecreaseMOP);
DefineCancelableFunctionMOP(CImmuneDebuffDecreaseMOP);
DefineCancelableFunctionMOP(CImmuneDOTDecreaseMOP);
DefineCancelableFunctionMOP(CImmuneSpecialDecreaseMOP);
DefineCancelableFunctionMOP(CImmuneCureMOP);
DefineCancelableFunctionMOP(CImmuneMoveMagicMOP);
DefineCancelableFunctionMOP(CImmuneNonTypePhysicsDamageMOP);
DefineCancelableFunctionMOP(CImmuneNonTypeDamageMOP);
DefineCancelableFunctionMOP(CImmuneNonTypeCureMOP);
DefineCancelableFunctionMOP(CImmuneInterruptCastingProcessMOP);
DefineCancelableFunctionMOP(CClosePrintInfoMOP);


#define DefineCancelableFunctionMOP2(Name) \
class Name : public CFunctionCancelableMagicOp \
{ \
public: \
	virtual bool Cancelable() const { return true; } \
	virtual bool CheckArg2MOP(CCfgCalc* pMagicOpArg);\
};

DefineCancelableFunctionMOP2(CSetupCumuliTriggerStateMOP);
DefineCancelableFunctionMOP2(CSetupSpecialStateMOP);
DefineCancelableFunctionMOP2(CCancelableRelMoveMagicMOP);
DefineCancelableFunctionMOP2(CEraseStateOnCancellingMOP);
DefineCancelableFunctionMOP2(CSetNoSingTimeAboutNextSkill);
DefineCancelableFunctionMOP2(CImmuneTaskTypeDamageMOP);
DefineCancelableFunctionMOP2(CChangeExpRateMOP);
DefineCancelableFunctionMOP2(CChangeExpRateInFBMOP);
DefineCancelableFunctionMOP2(CChangeFetchRateMOP);
DefineCancelableFunctionMOP2(CNonCombatStateMagicOp);
DefineCancelableFunctionMOP2(CDoAllBodyActionMOP);
DefineCancelableFunctionMOP2(CUseNormalHorseMOP);
DefineCancelableFunctionMOP2(CUseBattleHorseMOP);
DefineCancelableFunctionMOP2(CCallBattleHorseMOP);
DefineCancelableFunctionMOP2(CChangeCampMOP);
DefineCancelableFunctionMOP2(COnlyPlayFX);
DefineCancelableFunctionMOP2(CLockAgileValueMOP);
DefineCancelableFunctionMOP2(CSoulLinkMOP);


#define DefineCancelableFunctionMOP3(Name) \
class Name : public CFunctionCancelableMagicOp \
{ \
public: \
	virtual bool Cancelable() const { return true; } \
	virtual bool CheckArg2MOP(CCfgCalc* pMagicOpArg);\
	virtual bool IsSetupNestedObj() const { return true; } \
};

DefineCancelableFunctionMOP3(CSetupAureMagicMOP);
DefineCancelableFunctionMOP3(CIgnoreImmuneSetupSpecialStateMOP);
DefineCancelableFunctionMOP3(CPassiveSetupMagicStateMOP);
DefineCancelableFunctionMOP3(CSetupMagicStateMOP);
DefineCancelableFunctionMOP3(CSetupTriggerStateMOP);
DefineCancelableFunctionMOP3(CSetupDamageChangeStateMOP);
DefineCancelableFunctionMOP3(CSetupMultiMagicStateMOP);
DefineCancelableFunctionMOP3(CSetupOrEraseTriggerStateMOP);
DefineCancelableFunctionMOP3(CSetupOrEraseMagicStateMOP);
DefineCancelableFunctionMOP3(CSetupMagicStateByMaxSkillLevelMOP);

#define DefineCancelableFunctionMOP4(Name) \
class Name : public CFunctionCancelableMagicOp \
{ \
public: \
	virtual bool Cancelable() const { return true; } \
	virtual bool CheckIsCancelEffOfState() const { return true; }\
};

DefineCancelableFunctionMOP4(CTargetForbitNormalAttackMOP);
DefineCancelableFunctionMOP4(CTargetForbitUseSkillMOP);

#define DefineFunctionMOP(Name) \
class Name : public CFunctionMagicOp \
{ \
public: \
	bool CheckArg2MOP(CCfgCalc* pMagicOpArg);\
};

DefineFunctionMOP(CEraseAureMagicMOP);
DefineFunctionMOP(CEraseAureMagicOnCancellingMOP);
DefineFunctionMOP(CCancelMagicStateCascadeMOP);
DefineFunctionMOP(CCancelMagicStateCascadeByEffectMOP);
DefineFunctionMOP(CResetMagicStateTimeMOP);
DefineFunctionMOP(CAddMagicStateTimeMOP);
DefineFunctionMOP(CSetDamageChangeStateAccumulateMOP);
DefineFunctionMOP(CSetCumuliTriggerStateAccumulateMOP);
DefineFunctionMOP(CTriggerStateMessageEventMOP);
DefineFunctionMOP(CRelMoveMagicMOP);
DefineFunctionMOP(CRelTransferMagicMOP);
DefineFunctionMOP(CRelBulletMagicMOP);
DefineFunctionMOP(CDelayRelBulletMagicMOP);
DefineFunctionMOP(CEraseMoveMagicMOP);
DefineFunctionMOP(CErasePositionMagicMOP);
DefineFunctionMOP(CEraseAllPositionMagicMOP);
DefineFunctionMOP(CReplacePositionMagicMOP);
DefineFunctionMOP(CEraseStateMOP);
DefineFunctionMOP(CEraseSelfStateMOP);
DefineFunctionMOP(CEraseStateByDecreaseTypeMOP);
DefineFunctionMOP(CRelPassiveSkillMOP);
DefineFunctionMOP(CRelNPCSkillMOP);
DefineFunctionMOP(CBurstSoulPrizeToExpOwnerMOP);
DefineFunctionMOP(CCBurstSoulExtraPrizeToExpOwnerMOP);
DefineFunctionMOP(CAddBurstSoulTimesMOP);
DefineFunctionMOP(CAddContinuousBurstTimesMOP);
DefineFunctionMOP(CAddBurstSoulTimesToExpOwnerMOP);
DefineFunctionMOP(CConsumeBurstSoulTimesMOP);
DefineFunctionMOP(CTalentChangeManaPointMOP);
DefineFunctionMOP(CRelMoveMagicAtShockwaveMagicPosMOP);
DefineFunctionMOP(CRelPositionMagicAtTargetPosMOP);
DefineFunctionMOP(CRelShockWaveMagicAtTargetPosMOP);
DefineFunctionMOP(CRelTickMOP);
DefineFunctionMOP(CCreateNPCMOP);
DefineFunctionMOP(CDestoryNPCMOP);
DefineFunctionMOP(CSetupUncancellableMagicStateMOP);
DefineFunctionMOP(CSetupOrEraseAureMagicMOP);
DefineFunctionMOP(CChangeAureMagicMOP);
DefineFunctionMOP(CCreateServantMOP);
DefineFunctionMOP(CCreateServantWithTargetLevelFollowMasterMOP);
DefineFunctionMOP(CCreatePassiveNPCMOP);
DefineFunctionMOP(CCreateNPCOnTargetPosMOP);
DefineFunctionMOP(CSetServantLeftTimeMOP);
DefineFunctionMOP(CTauntMOP);
DefineFunctionMOP(CNormalAttackProduceRagePoint);
DefineFunctionMOP(CBeDamagePreduceRagePoint);
DefineFunctionMOP(CRelNPCDirSkillMOP);
DefineFunctionMOP(CTargetSetupMagicStateToSelfMOP);
DefineFunctionMOP(CEraseIncreaseStateMOP);
DefineFunctionMOP(CEraseAllDecreaseStateMOP);
DefineFunctionMOP(CReflectStateByDecreaseTypeMOP);
DefineFunctionMOP(CResetSingleCoolDownTimeMOP);
DefineFunctionMOP(CTargetRelBulletMagicMOP);
DefineFunctionMOP(CTargetBurstSoulFXMOP);
DefineFunctionMOP(CBurstSoulFXMOP);
DefineFunctionMOP(CBurstSoulBallFXMOP);
DefineFunctionMOP(CNonCombatBehaviorMOP);
DefineFunctionMOP(CChangeSceneMOP);
DefineFunctionMOP(CDisplayMsgMOP);
DefineFunctionMOP(CDestroyServantOnCancellingMOP);
DefineFunctionMOP(CDoWorldSkillMOP);
DefineFunctionMOP(CDoCampSkillMOP);
DefineFunctionMOP(CDoArmyCorpsSkillMOP);
DefineFunctionMOP(CDoTongSkillMOP);


#define DefineFunctionDefaultMOP(Name) class Name : public CFunctionMagicOpWithEmptyArg {};

DefineFunctionDefaultMOP(CFillHPMPMOP);
DefineFunctionDefaultMOP(CDestroyServantMOP);
DefineFunctionDefaultMOP(CSetMonsterCardSkillLevelByMasterSkillLevelMOP);
DefineFunctionDefaultMOP(CControlServantMOP);
DefineFunctionDefaultMOP(CBecomeTargetShadowMOP);
DefineFunctionDefaultMOP(CRemoveServantEnemyMOP);
DefineFunctionDefaultMOP(CTakeBackRidingHorseMOP);
DefineFunctionDefaultMOP(CSetServantTargetMOP);
DefineFunctionDefaultMOP(CLetServantAttackTargetMOP);
DefineFunctionDefaultMOP(CRemoveEnemyFromServantEnmityListMOP);
DefineFunctionDefaultMOP(CSetSameTargetMOP);
DefineFunctionDefaultMOP(CSetTargetSelfMOP);
DefineFunctionDefaultMOP(CSetTargetNULLMOP);
DefineFunctionDefaultMOP(CTargetLeftBattleStateMOP);
DefineFunctionDefaultMOP(CTargetCancelNAMOP);
DefineFunctionDefaultMOP(CRenascenceMOP);
DefineFunctionDefaultMOP(CRebornWhereYouBeMOP);
DefineFunctionDefaultMOP(CPermitRebornMOP);
DefineFunctionDefaultMOP(CSwapPositionMOP);
DefineFunctionDefaultMOP(CTakeOverTRuckMop);
DefineFunctionDefaultMOP(CMoveSelfToTargetPosMop);
DefineFunctionDefaultMOP(CEraseAllBufferMOP);
DefineFunctionDefaultMOP(CEraseMoveDecreaseStateMOP);
DefineFunctionDefaultMOP(CEraseSpeedDecreaseStateMOP);
DefineFunctionDefaultMOP(CRandEraseAllDecreaseStateMOP);
DefineFunctionDefaultMOP(CRandEraseIncreaseStateMOP);
DefineFunctionDefaultMOP(CEraseRidingStateMOP);
DefineFunctionDefaultMOP(CAllSkillReadyMOP);
DefineFunctionDefaultMOP(CAllFightSkillReadyMOP);
DefineFunctionDefaultMOP(CHandActionMOP);
DefineFunctionDefaultMOP(CFootActionMOP);
DefineFunctionDefaultMOP(CReturnBattleArrayMOP);
DefineFunctionDefaultMOP(CInterruptCastingProcessMagicMOP);
DefineFunctionDefaultMOP(CForceSetDOTDamageMOP);
DefineFunctionDefaultMOP(CBindTargetObjToSelfMOP);
DefineFunctionDefaultMOP(CSelfMurderBlastMOP);


#define DefinePosMOP(Name) \
class Name : public CPosMagicOp \
{ \
public: \
	bool CheckArg2MOP(CCfgCalc* pMagicOpArg);\
};

DefinePosMOP(CRelShockWaveMagicMOP);
DefinePosMOP(CRelPositionMagicMOP);
DefinePosMOP(CRelPosBulletMagicMOP);
DefinePosMOP(CRelPosMoveMagicMOP);
DefinePosMOP(CRelBattleArrayMagicMOP);
DefinePosMOP(CEraseEnemyPositionMagicMOP);
DefinePosMOP(CPursueKillMOP);
DefinePosMOP(CSetupDynamicBarrierMOP);
DefinePosMOP(CCreateTotemMOP);
DefinePosMOP(CPosCreateNPCMOP);
DefinePosMOP(CCreateShadowMOP);
DefinePosMOP(CPosCreateNPCLevelSameMOP);
DefinePosMOP(CTargetPosCreateNPCMOP);
DefinePosMOP(CTargetPosCreateNPCLevelSameMOP);
DefinePosMOP(CDelayRelShockWaveMagicMOP);
DefinePosMOP(CPosCreateNpcWithMasterMOP);
DefinePosMOP(CPosDelayCreateNPCMOP);
DefinePosMOP(CDelayRelPositionMagicMOP);


#define DefinePosDefaultMOP(Name) class Name : public CCPosMagicOpWithEmptyArg {};
DefinePosDefaultMOP(CUseDeadBobyMOP);
DefinePosDefaultMOP(CPosCreateDeadNPCMOP);
DefinePosDefaultMOP(CPostionPlayFXMOP);
