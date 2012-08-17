#pragma once
#include "CValueMagicOpServer.h"
#include "CFighterAgileInfoServer.h"
#include "CFighterCalInfo.h"
#include "TFighterCtrlInfo.h"


template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
class TChangeAgileInfoValueMOP
	:public CValueMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
};

template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
class TChangeAdderMOP
	:public CValueCancelableMagicOp
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	void ChangeAdder(CSkillInstServer* pSkillInst,int32 iValue,CFighterDictator* pFighter);
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	void CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value);
	void Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo);
};


template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
class TChangeMultiplierMOP
	:public CValueCancelableMagicOp
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	void ChangeMultiplier(CSkillInstServer* pSkillInst,float fValue,CFighterDictator* pFighter);
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	void CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value);
	void Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo);
};


//定义所有AgileValueMOP

#define DefineAgileValueMOP(Name) \
	typedef TChangeAgileInfoValueMOP<C##Name##Server,&CFighterAgileInfoServer::m_##Name>						CChange##Name##MOP;\
	typedef TChangeAdderMOP<C##Name##Server,CFighterAgileInfoServer,&CFighterAgileInfoServer::m_##Name>			CChangeMax##Name##AdderMOP;\
	typedef TChangeMultiplierMOP<C##Name##Server,CFighterAgileInfoServer,&CFighterAgileInfoServer::m_##Name>	CChangeMax##Name##MultiplierMOP

DefineAgileValueMOP(HealthPoint);
DefineAgileValueMOP(ManaPoint);
DefineAgileValueMOP(ComboPoint);
DefineAgileValueMOP(EnergyPoint);
DefineAgileValueMOP(RagePoint);

#undef DefineAgileValueMOP


#define DefineDirectorCalValueMOP(Name) \
	typedef TChangeMultiplierMOP<C##Name,CFighterSyncToDirectorCalInfo,&CFighterSyncToDirectorCalInfo::m_##Name>		CChange##Name##MultiplierMOP

DefineDirectorCalValueMOP(MPConsumeRate);
DefineDirectorCalValueMOP(RPConsumeRate);
DefineDirectorCalValueMOP(EPConsumeRate);

DefineDirectorCalValueMOP(NatureMPConsumeRate);
DefineDirectorCalValueMOP(DestructionMPConsumeRate);
DefineDirectorCalValueMOP(EvilMPConsumeRate);

#undef DefineDirectorCalValueMOP
//定义所有CalValueMOP
#define DefineCalValueMOP(Name) \
	typedef TChangeAdderMOP<C##Name,CFighterCalInfo,&CFighterCalInfo::m_##Name>				CChange##Name##AdderMOP; \
	typedef TChangeMultiplierMOP<C##Name,CFighterCalInfo,&CFighterCalInfo::m_##Name>		CChange##Name##MultiplierMOP

DefineCalValueMOP(Defence);
DefineCalValueMOP(HPUpdateRate);
DefineCalValueMOP(MPUpdateRate);
DefineCalValueMOP(RPProduceRate);
DefineCalValueMOP(RPUpdateValue);
DefineCalValueMOP(EPUpdateValue);
DefineCalValueMOP(RevertPer);

DefineCalValueMOP(StrikeMultiValue);
DefineCalValueMOP(StrikeValue);

DefineCalValueMOP(BlockDamage);
DefineCalValueMOP(PhysicalDodgeValue);
DefineCalValueMOP(MagicDodgeValue);
DefineCalValueMOP(MagicHitValue);
DefineCalValueMOP(RunSpeed);
DefineCalValueMOP(WalkSpeed);
DefineCalValueMOP(ParryValue);
DefineCalValueMOP(ResilienceValue);
DefineCalValueMOP(StrikeResistanceValue);
DefineCalValueMOP(AccuratenessValue);

DefineCalValueMOP(PhysicalDPS);
DefineCalValueMOP(PunctureDamage);
DefineCalValueMOP(ChopDamage);
DefineCalValueMOP(BluntDamage);

DefineCalValueMOP(MagicDamageValue);
DefineCalValueMOP(NatureDamageValue);
DefineCalValueMOP(DestructionDamageValue);
DefineCalValueMOP(EvilDamageValue);

DefineCalValueMOP(NatureResistanceValue);
DefineCalValueMOP(DestructionResistanceValue);
DefineCalValueMOP(EvilResistanceValue);


DefineCalValueMOP(PhysicalDamage);
DefineCalValueMOP(AssistPhysicalDamage);
DefineCalValueMOP(MagicDamage);
DefineCalValueMOP(DOTDamage);

DefineCalValueMOP(PenetrationValue);
DefineCalValueMOP(ProtectionValue);

DefineCalValueMOP(MainHandWeaponInterval);
DefineCalValueMOP(AssistantWeaponInterval);

DefineCalValueMOP(NatureSmashValue);
DefineCalValueMOP(DestructionSmashValue);
DefineCalValueMOP(EvilSmashValue);
DefineCalValueMOP(DefenceSmashValue);

#undef DefineCalValueMOP

#define DefineCalMultiplierValueMOP(Name) \
	typedef TChangeMultiplierMOP<C##Name,CFighterCalInfo,&CFighterCalInfo::m_##Name>		CChange##Name##MultiplierMOP

DefineCalMultiplierValueMOP(MissRate);
DefineCalMultiplierValueMOP(BlockRate);
DefineCalMultiplierValueMOP(ResistCastingInterruptionRate);
DefineCalMultiplierValueMOP(ExtraDOTDamageRate);
DefineCalMultiplierValueMOP(ExtraPunctureDamageRate);
DefineCalMultiplierValueMOP(ExtraChopDamageRate);
DefineCalMultiplierValueMOP(ExtraBluntDamageRate);
DefineCalMultiplierValueMOP(ExtraNatureDamageRate);
DefineCalMultiplierValueMOP(ExtraEvilDamageRate);
DefineCalMultiplierValueMOP(ExtraDestructionDamageRate);
DefineCalMultiplierValueMOP(ExtraBowDamageRate);
DefineCalMultiplierValueMOP(ExtraCrossBowDamageRate);
DefineCalMultiplierValueMOP(ExtraTwohandWeaponDamageRate);
DefineCalMultiplierValueMOP(ExtraPolearmDamageRate);
DefineCalMultiplierValueMOP(ExtraPaladinWeaponDamageRate);
DefineCalMultiplierValueMOP(ExtraAssistWeaponDamageRate);
DefineCalMultiplierValueMOP(ExtraLongDistDamageRate);
DefineCalMultiplierValueMOP(ExtraShortDistDamageRate);
DefineCalMultiplierValueMOP(LongDistWeaponDamageRate);
DefineCalMultiplierValueMOP(DamageDecsRate);
DefineCalMultiplierValueMOP(CastingProcessTimeRatio);
DefineCalMultiplierValueMOP(ExtraCureValueRate);
DefineCalMultiplierValueMOP(ExtraBeCuredValueRate);
DefineCalMultiplierValueMOP(ControlDecreaseResistRate);
DefineCalMultiplierValueMOP(PauseDecreaseResistRate);
DefineCalMultiplierValueMOP(CripplingDecreaseResistRate);
DefineCalMultiplierValueMOP(DebuffDecreaseResistRate);
DefineCalMultiplierValueMOP(DOTDecreaseResistRate);
DefineCalMultiplierValueMOP(SpecialDecreaseResistRate);
DefineCalMultiplierValueMOP(ControlDecreaseTimeRate);
DefineCalMultiplierValueMOP(PauseDecreaseTimeRate);
DefineCalMultiplierValueMOP(CripplingDecreaseTimeRate);
DefineCalMultiplierValueMOP(SpecialDecreaseTimeRate);
DefineCalMultiplierValueMOP(ExtraPhysicDefenceRate);
DefineCalMultiplierValueMOP(ExtraPhysicDodgeRate);
DefineCalMultiplierValueMOP(ExtraParryRate);
DefineCalMultiplierValueMOP(ExtraStrikeRate);
DefineCalMultiplierValueMOP(ExtraMagicDodgeRate);
DefineCalMultiplierValueMOP(ExtraMagicResistanceRate);
DefineCalMultiplierValueMOP(ExtraNatureResistanceRate);
DefineCalMultiplierValueMOP(ExtraDestructionResistanceRate);
DefineCalMultiplierValueMOP(ExtraEvilResistanceRate);
DefineCalMultiplierValueMOP(ExtraCompleteResistanceRate);

DefineCalMultiplierValueMOP(ImmuneRate);
DefineCalMultiplierValueMOP(PunctureDamageImmuneRate);
DefineCalMultiplierValueMOP(ChopDamageImmuneRate);
DefineCalMultiplierValueMOP(BluntDamageImmuneRate);
DefineCalMultiplierValueMOP(NatureDamageImmuneRate);
DefineCalMultiplierValueMOP(DestructionDamageImmuneRate);
DefineCalMultiplierValueMOP(EvilDamageImmuneRate);
DefineCalMultiplierValueMOP(ControlDecreaseImmuneRate);
DefineCalMultiplierValueMOP(PauseDecreaseImmuneRate);
DefineCalMultiplierValueMOP(CripplingDecreaseImmuneRate);
DefineCalMultiplierValueMOP(DebuffDecreaseImmuneRate);
DefineCalMultiplierValueMOP(DOTDecreaseImmuneRate);
DefineCalMultiplierValueMOP(SpecialDecreaseImmuneRate);
DefineCalMultiplierValueMOP(CureImmuneRate);
DefineCalMultiplierValueMOP(MoveMagicImmuneRate);
DefineCalMultiplierValueMOP(NonTypePhysicsDamageImmuneRate);
DefineCalMultiplierValueMOP(NonTypeDamageImmuneRate);
DefineCalMultiplierValueMOP(NonTypeCureImmuneRate);
DefineCalMultiplierValueMOP(InterruptCastingProcessImmuneRate);

#undef DefineCalMultiplierValueMOP

void SetMainHandSpeed(CFighterDictator* pFighter, float fSpeed);
void SetOffHandSpeed(CFighterDictator* pFighter, float fSpeed);
void SetRunSpeed(CFighterDictator* pFighter, float fSpeed);
void SetWalkSpeed(CFighterDictator* pFighter, float fSpeed);

void SetMainHandIntervalPercent(CFighterDictator* pFighter, float fSpeed);
void SetOffHandIntervalPercent(CFighterDictator* pFighter, float fSpeed);
void SetRunSpeedPercent(CFighterDictator* pFighter, float fPercent);
void SetWalkSpeedPercent(CFighterDictator* pFighter, float fPercent);

void SetArmorValueRateMultiplier(CFighterDictator* pFighter,float fBluntRate,float fPunctureRate,float fChopRate);

void SetRPConsumeRate(CFighterDictator* pFighter, float fMulti);

class CChangeSelfMagicDamageAdderMOP
	: public CChangeMagicDamageAdderMOP
{
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	void CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value);
};

