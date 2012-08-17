#pragma once
#include "FighterProperty.h"
#include "CStaticAttribs.h"
#include "FightDef.h"


#define DefineBaseCalProperty(Name,Type) \
class C##Name : public TAMBProperty<ePID_##Name,Type,&CStaticAttribs::m_##Name> {}; 

DefineBaseCalProperty(HPUpdateRate, float);
DefineBaseCalProperty(MPUpdateRate, float);
DefineBaseCalProperty(RPUpdateValue, uint32);
DefineBaseCalProperty(RPProduceRate, float);
DefineBaseCalProperty(EPUpdateValue, uint32);
DefineBaseCalProperty(RevertPer, float);
DefineBaseCalProperty(RunSpeed,float);
DefineBaseCalProperty(WalkSpeed,float);
DefineBaseCalProperty(PhysicalDodgeValue,int32);
DefineBaseCalProperty(ParryValue,int32);
DefineBaseCalProperty(StrikeValue,uint32);
DefineBaseCalProperty(AccuratenessValue,uint32);
DefineBaseCalProperty(MagicDodgeValue,int32);
DefineBaseCalProperty(MagicHitValue,uint32);
DefineBaseCalProperty(PhysicalDPS,uint32);
DefineBaseCalProperty(Defence,int32);
DefineBaseCalProperty(StrikeMultiValue,uint32);
DefineBaseCalProperty(ResilienceValue,int32);
DefineBaseCalProperty(StrikeResistanceValue,int32);
DefineBaseCalProperty(MagicDamageValue,uint32);
DefineBaseCalProperty(NatureResistanceValue,int32);
DefineBaseCalProperty(DestructionResistanceValue,int32);
DefineBaseCalProperty(EvilResistanceValue,int32);
DefineBaseCalProperty(ValidityCoefficient,uint32);
DefineBaseCalProperty(NatureSmashValue,uint32);
DefineBaseCalProperty(DestructionSmashValue,uint32);
DefineBaseCalProperty(EvilSmashValue,uint32);
DefineBaseCalProperty(DefenceSmashValue,uint32);

#undef DefineBaseCalProperty


#define DefineCalProperty(Name, type) \
class C##Name : public TAMProperty<ePID_##Name,type> {}; 

DefineCalProperty(BlockDamage,uint32);
DefineCalProperty(PunctureDamage,uint32);
DefineCalProperty(ChopDamage,uint32);
DefineCalProperty(BluntDamage,uint32);
DefineCalProperty(NatureDamageValue,uint32);
DefineCalProperty(DestructionDamageValue,uint32);
DefineCalProperty(EvilDamageValue,uint32);

DefineCalProperty(PhysicalDamage,uint32);
DefineCalProperty(AssistPhysicalDamage,uint32);
DefineCalProperty(MagicDamage,uint32);
DefineCalProperty(DOTDamage,uint32);
DefineCalProperty(MainHandMinWeaponDamage,uint32);
DefineCalProperty(MainHandMaxWeaponDamage,uint32);
DefineCalProperty(AssistantMinWeaponDamage,uint32);
DefineCalProperty(AssistantMaxWeaponDamage,uint32);
DefineCalProperty(PenetrationValue,uint32);
DefineCalProperty(ProtectionValue,uint32);
DefineCalProperty(NatureDecreaseResistanceValue,uint32);
DefineCalProperty(DestructionDecreaseResistanceValue,uint32);
DefineCalProperty(EvilDecreaseResistanceValue,uint32);
DefineCalProperty(DefencePenetrationValue,uint32);

DefineCalProperty(MainHandWeaponInterval,float);
DefineCalProperty(AssistantWeaponInterval,float);
DefineCalProperty(MHWeaponIntervalExtraDamageRate,float);
DefineCalProperty(AHWeaponIntervalExtraDamageRate,float);

#undef DefineCalProperty

#define DefineMultiplierProperty(Name) \
class C##Name : public TMProperty<ePID_##Name,float> {}; 

DefineMultiplierProperty(MPConsumeRate);
DefineMultiplierProperty(RPConsumeRate);
DefineMultiplierProperty(EPConsumeRate);
DefineMultiplierProperty(NatureMPConsumeRate);
DefineMultiplierProperty(DestructionMPConsumeRate);
DefineMultiplierProperty(EvilMPConsumeRate);

DefineMultiplierProperty(ExtraDOTDamageRate);
DefineMultiplierProperty(ExtraPunctureDamageRate);
DefineMultiplierProperty(ExtraChopDamageRate);
DefineMultiplierProperty(ExtraBluntDamageRate);
DefineMultiplierProperty(ExtraNatureDamageRate);
DefineMultiplierProperty(ExtraEvilDamageRate);
DefineMultiplierProperty(ExtraDestructionDamageRate);
DefineMultiplierProperty(ExtraBowDamageRate);
DefineMultiplierProperty(ExtraCrossBowDamageRate);
DefineMultiplierProperty(ExtraTwohandWeaponDamageRate);
DefineMultiplierProperty(ExtraPolearmDamageRate);
DefineMultiplierProperty(ExtraPaladinWeaponDamageRate);
DefineMultiplierProperty(LongDistWeaponDamageRate);
DefineMultiplierProperty(DamageDecsRate);
DefineMultiplierProperty(CastingProcessTimeRatio);
DefineMultiplierProperty(ExtraCureValueRate);
DefineMultiplierProperty(ExtraBeCuredValueRate);
DefineMultiplierProperty(ControlDecreaseTimeRate);
DefineMultiplierProperty(PauseDecreaseTimeRate);
DefineMultiplierProperty(CripplingDecreaseTimeRate);
DefineMultiplierProperty(SpecialDecreaseTimeRate);
#undef DefineMultiplierProperty

#define DefineMultiplierPropertyInitValue(Name,InitValue) \
class C##Name : public TMProperty<ePID_##Name,float> { \
public:\
	C##Name(){this->SetProperty(float(InitValue));} }; 

DefineMultiplierPropertyInitValue(BlockRate,0);
DefineMultiplierPropertyInitValue(MissRate,0);
DefineMultiplierPropertyInitValue(ResistCastingInterruptionRate,0);
DefineMultiplierPropertyInitValue(PunctureDamageResistanceRate,0);
DefineMultiplierPropertyInitValue(ChopDamageResistanceRate,0);
DefineMultiplierPropertyInitValue(BluntDamageResistanceRate,0);
DefineMultiplierPropertyInitValue(ControlDecreaseResistRate, 0);
DefineMultiplierPropertyInitValue(PauseDecreaseResistRate, 0);
DefineMultiplierPropertyInitValue(CripplingDecreaseResistRate, 0);
DefineMultiplierPropertyInitValue(DebuffDecreaseResistRate, 0);
DefineMultiplierPropertyInitValue(DOTDecreaseResistRate, 0);
DefineMultiplierPropertyInitValue(SpecialDecreaseResistRate, 0);
DefineMultiplierPropertyInitValue(ExtraAssistWeaponDamageRate, 0);
DefineMultiplierPropertyInitValue(ExtraLongDistDamageRate, 0);
DefineMultiplierPropertyInitValue(ExtraShortDistDamageRate, 0);
DefineMultiplierPropertyInitValue(ExtraPhysicDefenceRate,0);
DefineMultiplierPropertyInitValue(ExtraPhysicDodgeRate,0);
DefineMultiplierPropertyInitValue(ExtraParryRate,0);
DefineMultiplierPropertyInitValue(ExtraStrikeRate,0);
DefineMultiplierPropertyInitValue(ExtraMagicDodgeRate,0);
DefineMultiplierPropertyInitValue(ExtraMagicResistanceRate,0);
DefineMultiplierPropertyInitValue(ExtraNatureResistanceRate,0);
DefineMultiplierPropertyInitValue(ExtraDestructionResistanceRate,0);
DefineMultiplierPropertyInitValue(ExtraEvilResistanceRate,0);
DefineMultiplierPropertyInitValue(ExtraCompleteResistanceRate,0);
DefineMultiplierPropertyInitValue(ImmuneRate, 0);
DefineMultiplierPropertyInitValue(PunctureDamageImmuneRate, 0);
DefineMultiplierPropertyInitValue(ChopDamageImmuneRate, 0);
DefineMultiplierPropertyInitValue(BluntDamageImmuneRate, 0);
DefineMultiplierPropertyInitValue(NatureDamageImmuneRate, 0);
DefineMultiplierPropertyInitValue(DestructionDamageImmuneRate, 0);
DefineMultiplierPropertyInitValue(EvilDamageImmuneRate, 0);
DefineMultiplierPropertyInitValue(ControlDecreaseImmuneRate, 0);
DefineMultiplierPropertyInitValue(PauseDecreaseImmuneRate, 0);
DefineMultiplierPropertyInitValue(CripplingDecreaseImmuneRate, 0);
DefineMultiplierPropertyInitValue(DebuffDecreaseImmuneRate, 0);
DefineMultiplierPropertyInitValue(DOTDecreaseImmuneRate, 0);
DefineMultiplierPropertyInitValue(SpecialDecreaseImmuneRate, 0);
DefineMultiplierPropertyInitValue(CureImmuneRate, 0);
DefineMultiplierPropertyInitValue(MoveMagicImmuneRate, 0);
DefineMultiplierPropertyInitValue(NonTypePhysicsDamageImmuneRate, 0);
DefineMultiplierPropertyInitValue(NonTypeDamageImmuneRate, 0);
DefineMultiplierPropertyInitValue(NonTypeCureImmuneRate, 0);
DefineMultiplierPropertyInitValue(InterruptCastingProcessImmuneRate, 0);
#undef DefineMultiplierPropertyInitValue

class CFighterSyncToDirectorCalInfo
{
public:
	CFighterSyncToDirectorCalInfo(void){};
	~CFighterSyncToDirectorCalInfo(void){};

	CMPConsumeRate					m_MPConsumeRate;				// 魔法消耗系数
	CRPConsumeRate					m_RPConsumeRate;				// 怒气消耗系数
	CEPConsumeRate					m_EPConsumeRate;				// 能量消耗系数

	CNatureMPConsumeRate			m_NatureMPConsumeRate;			// 自然系魔法消耗系数		
	CDestructionMPConsumeRate		m_DestructionMPConsumeRate;		// 破坏系魔法消耗系数
	CEvilMPConsumeRate				m_EvilMPConsumeRate;			// 黑暗系魔法消耗系数
};
  
class CFighterCalInfo
{
public:
	CFighterCalInfo(void);
	~CFighterCalInfo(void);

	CDefence						m_Defence;				

	CHPUpdateRate						m_HPUpdateRate;					// 回血速度
	CMPUpdateRate						m_MPUpdateRate;					// 回蓝速度
	CRPUpdateValue						m_RPUpdateValue;					// 减怒速度
	CEPUpdateValue						m_EPUpdateValue;					// 回能量速度
	CRevertPer						m_RevertPer;					// 战斗状态回复速度 = m_RevertPer * 非战斗状态回复速度
	CRPProduceRate					m_RPProduceRate;				// 怒气产生系数

	CStrikeMultiValue				m_StrikeMultiValue;	
	CStrikeValue					m_StrikeValue;		

	CBlockRate						m_BlockRate;			
	CBlockDamage					m_BlockDamage;			
	CPhysicalDodgeValue				m_PhysicalDodgeValue;	
	CMagicDodgeValue				m_MagicDodgeValue;

	CImmuneRate						m_ImmuneRate;
	CPunctureDamageImmuneRate		m_PunctureDamageImmuneRate;
	CChopDamageImmuneRate			m_ChopDamageImmuneRate;
	CBluntDamageImmuneRate			m_BluntDamageImmuneRate;
	CNatureDamageImmuneRate			m_NatureDamageImmuneRate;
	CDestructionDamageImmuneRate	m_DestructionDamageImmuneRate;
	CEvilDamageImmuneRate			m_EvilDamageImmuneRate;
	CControlDecreaseImmuneRate		m_ControlDecreaseImmuneRate;
	CPauseDecreaseImmuneRate		m_PauseDecreaseImmuneRate;
	CCripplingDecreaseImmuneRate	m_CripplingDecreaseImmuneRate;
	CDebuffDecreaseImmuneRate		m_DebuffDecreaseImmuneRate;
	CDOTDecreaseImmuneRate			m_DOTDecreaseImmuneRate;
	CSpecialDecreaseImmuneRate		m_SpecialDecreaseImmuneRate;
	CCureImmuneRate					m_CureImmuneRate;
	CMoveMagicImmuneRate			m_MoveMagicImmuneRate;
	CNonTypePhysicsDamageImmuneRate	m_NonTypePhysicsDamageImmuneRate;
	CNonTypeDamageImmuneRate		m_NonTypeDamageImmuneRate;
	CNonTypeCureImmuneRate			m_NonTypeCureImmuneRate;
	CInterruptCastingProcessImmuneRate m_InterruptCastingProcessImmuneRate;
	CMissRate						m_MissRate;			

	CMagicHitValue					m_MagicHitValue;		
	CRunSpeed						m_RunSpeed;		//跑动速度
	CWalkSpeed						m_WalkSpeed;		//走动速度
	CParryValue						m_ParryValue;			
	CResilienceValue				m_ResilienceValue;
	CStrikeResistanceValue				m_StrikeResistanceValue;
	CAccuratenessValue				m_AccuratenessValue;			// 精准值

	CPhysicalDPS					m_PhysicalDPS;					// 攻击力
	CPunctureDamage					m_PunctureDamage;		
	CChopDamage						m_ChopDamage;			
	CBluntDamage					m_BluntDamage;			

	CMagicDamageValue				m_MagicDamageValue;				
	CNatureDamageValue				m_NatureDamageValue;			
	CDestructionDamageValue			m_DestructionDamageValue;		
	CEvilDamageValue				m_EvilDamageValue;				

	CNatureResistanceValue			m_NatureResistanceValue;		
	CDestructionResistanceValue		m_DestructionResistanceValue;	
	CEvilResistanceValue			m_EvilResistanceValue;		

	CNatureDecreaseResistanceValue	m_NatureDecreaseResistanceValue;
	CDestructionDecreaseResistanceValue	m_DestructionDecreaseResistanceValue;
	CEvilDecreaseResistanceValue	m_EvilDecreaseResistanceValue;
	CDefencePenetrationValue		m_DefencePenetrationValue;
	CValidityCoefficient			m_ValidityCoefficient;

	//下列5个数值暂时先放在这里，将来都要放到临时修改器里面
	CPhysicalDamage					m_PhysicalDamage;
	CAssistPhysicalDamage			m_AssistPhysicalDamage;
	CMagicDamage					m_MagicDamage;
	CDOTDamage						m_DOTDamage;

	CMainHandMinWeaponDamage		m_MainHandMinWeaponDamage;
	CMainHandMaxWeaponDamage		m_MainHandMaxWeaponDamage;
	CAssistantMinWeaponDamage		m_AssistantMinWeaponDamage;
	CAssistantMaxWeaponDamage		m_AssistantMaxWeaponDamage;
	CMainHandWeaponInterval			m_MainHandWeaponInterval;
	CAssistantWeaponInterval		m_AssistantWeaponInterval;
	CMHWeaponIntervalExtraDamageRate	m_MHWeaponIntervalExtraDamageRate;
	CAHWeaponIntervalExtraDamageRate	m_AHWeaponIntervalExtraDamageRate;

	CResistCastingInterruptionRate	m_ResistCastingInterruptionRate;
	CPenetrationValue				m_PenetrationValue;
	CProtectionValue				m_ProtectionValue;

	CExtraDOTDamageRate				m_ExtraDOTDamageRate;
	CExtraPunctureDamageRate		m_ExtraPunctureDamageRate;
	CExtraChopDamageRate			m_ExtraChopDamageRate;
	CExtraBluntDamageRate			m_ExtraBluntDamageRate;
	CExtraNatureDamageRate			m_ExtraNatureDamageRate;
	CExtraEvilDamageRate			m_ExtraEvilDamageRate;
	CExtraDestructionDamageRate		m_ExtraDestructionDamageRate;
	CExtraBowDamageRate				m_ExtraBowDamageRate;
	CExtraCrossBowDamageRate		m_ExtraCrossBowDamageRate;
	CExtraTwohandWeaponDamageRate	m_ExtraTwohandWeaponDamageRate;
	CExtraPolearmDamageRate			m_ExtraPolearmDamageRate;
	CExtraPaladinWeaponDamageRate	m_ExtraPaladinWeaponDamageRate;
	CExtraAssistWeaponDamageRate	m_ExtraAssistWeaponDamageRate;
	CExtraLongDistDamageRate		m_ExtraLongDistDamageRate;
	CExtraShortDistDamageRate		m_ExtraShortDistDamageRate;
	CLongDistWeaponDamageRate		m_LongDistWeaponDamageRate;
	CDamageDecsRate					m_DamageDecsRate;
	CExtraCureValueRate				m_ExtraCureValueRate;
	CExtraBeCuredValueRate			m_ExtraBeCuredValueRate;

	CControlDecreaseResistRate		m_ControlDecreaseResistRate;
	CPauseDecreaseResistRate		m_PauseDecreaseResistRate;
	CCripplingDecreaseResistRate	m_CripplingDecreaseResistRate;
	CDebuffDecreaseResistRate		m_DebuffDecreaseResistRate;
	CDOTDecreaseResistRate			m_DOTDecreaseResistRate;
	CSpecialDecreaseResistRate		m_SpecialDecreaseResistRate;

	CControlDecreaseTimeRate		m_ControlDecreaseTimeRate;
	CPauseDecreaseTimeRate			m_PauseDecreaseTimeRate;
	CCripplingDecreaseTimeRate		m_CripplingDecreaseTimeRate;
	CSpecialDecreaseTimeRate		m_SpecialDecreaseTimeRate;

	CExtraPhysicDefenceRate			m_ExtraPhysicDefenceRate;
	CExtraPhysicDodgeRate			m_ExtraPhysicDodgeRate;
	CExtraParryRate					m_ExtraParryRate;
	CExtraStrikeRate				m_ExtraStrikeRate;
	CExtraMagicDodgeRate			m_ExtraMagicDodgeRate;

	CExtraMagicResistanceRate		m_ExtraMagicResistanceRate;
	CExtraNatureResistanceRate		m_ExtraNatureResistanceRate;
	CExtraDestructionResistanceRate	m_ExtraDestructionResistanceRate;
	CExtraEvilResistanceRate		m_ExtraEvilResistanceRate;
	CExtraCompleteResistanceRate	m_ExtraCompleteResistanceRate;
	CPunctureDamageResistanceRate	m_PunctureDamageResistanceRate;
	CChopDamageResistanceRate		m_ChopDamageResistanceRate;
	CBluntDamageResistanceRate		m_BluntDamageResistanceRate;
	CCastingProcessTimeRatio		m_CastingProcessTimeRatio;

	CNatureSmashValue				m_NatureSmashValue;
	CDestructionSmashValue			m_DestructionSmashValue;
	CEvilSmashValue					m_EvilSmashValue;
	CDefenceSmashValue				m_DefenceSmashValue;

	float CalStrikeRate(const CFighterBaseInfo* pFighter)const;
	float CalNatureResistanceRate(const CFighterBaseInfo* pFighter)const;
	float CalDestructionResistanceRate(const CFighterBaseInfo* pFighter)const;
	float CalEvilResistanceRate(const CFighterBaseInfo* pFighter)const;
	float CalPhysicalDefenceRate(const CFighterBaseInfo* pFighter)const;
	float CalcSmashRate(const CFighterBaseInfo* pSelfBaseInfo, const CFighterCalInfo* pTargetCalInfo, const CFighterBaseInfo* pTargetBaseInfo, EAttackType attackType)const;
};
