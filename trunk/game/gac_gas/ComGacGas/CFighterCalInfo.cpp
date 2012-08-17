#include "stdafx.h"
#include "CFighterCalInfo.h"
#include "CFighterBaseInfo.h"
#include "FighterProperty.inl"


#define InstantiateBaseCalProperty(Name, type) \
	template class TAMBProperty<ePID_##Name,type,&CStaticAttribs::m_##Name>;

InstantiateBaseCalProperty(HPUpdateRate, float);
InstantiateBaseCalProperty(MPUpdateRate, float);
InstantiateBaseCalProperty(RPUpdateValue, uint32);
InstantiateBaseCalProperty(RPProduceRate, float);
InstantiateBaseCalProperty(EPUpdateValue, uint32);
InstantiateBaseCalProperty(RevertPer, float);
InstantiateBaseCalProperty(RunSpeed,float);
InstantiateBaseCalProperty(WalkSpeed,float);
InstantiateBaseCalProperty(PhysicalDodgeValue,int32);
InstantiateBaseCalProperty(ParryValue,int32);
InstantiateBaseCalProperty(StrikeValue,uint32);
InstantiateBaseCalProperty(AccuratenessValue,uint32);
InstantiateBaseCalProperty(MagicDodgeValue,int32);
InstantiateBaseCalProperty(MagicHitValue,uint32);
InstantiateBaseCalProperty(PhysicalDPS,uint32);
InstantiateBaseCalProperty(Defence,int32);
InstantiateBaseCalProperty(StrikeMultiValue,uint32);
InstantiateBaseCalProperty(ResilienceValue,int32);
InstantiateBaseCalProperty(StrikeResistanceValue,int32);
InstantiateBaseCalProperty(MagicDamageValue,uint32);
InstantiateBaseCalProperty(NatureResistanceValue,int32);
InstantiateBaseCalProperty(DestructionResistanceValue,int32);
InstantiateBaseCalProperty(EvilResistanceValue,int32);
InstantiateBaseCalProperty(ValidityCoefficient,uint32);
InstantiateBaseCalProperty(NatureSmashValue,uint32);
InstantiateBaseCalProperty(DestructionSmashValue,uint32);
InstantiateBaseCalProperty(EvilSmashValue,uint32);
InstantiateBaseCalProperty(DefenceSmashValue,uint32);

#undef InstantiateBaseCalProperty

#define InstantiateCalProperty(Name, type) \
	template class TAMProperty<ePID_##Name,type>;

InstantiateCalProperty(BlockDamage,uint32);
InstantiateCalProperty(PunctureDamage,uint32);
InstantiateCalProperty(ChopDamage,uint32);
InstantiateCalProperty(BluntDamage,uint32);
InstantiateCalProperty(NatureDamageValue,uint32);
InstantiateCalProperty(DestructionDamageValue,uint32);
InstantiateCalProperty(EvilDamageValue,uint32);

InstantiateCalProperty(PhysicalDamage,uint32);
InstantiateCalProperty(AssistPhysicalDamage,uint32);
InstantiateCalProperty(MagicDamage,uint32);
InstantiateCalProperty(DOTDamage,uint32);
InstantiateCalProperty(MainHandMinWeaponDamage,uint32);
InstantiateCalProperty(MainHandMaxWeaponDamage,uint32);
InstantiateCalProperty(AssistantMinWeaponDamage,uint32);
InstantiateCalProperty(AssistantMaxWeaponDamage,uint32);
InstantiateCalProperty(PenetrationValue,uint32);
InstantiateCalProperty(ProtectionValue,uint32);
InstantiateCalProperty(NatureDecreaseResistanceValue,uint32);
InstantiateCalProperty(DestructionDecreaseResistanceValue,uint32);
InstantiateCalProperty(EvilDecreaseResistanceValue,uint32);
InstantiateCalProperty(DefencePenetrationValue,uint32);

InstantiateCalProperty(MainHandWeaponInterval,float);
InstantiateCalProperty(AssistantWeaponInterval,float);
InstantiateCalProperty(MHWeaponIntervalExtraDamageRate,float);
InstantiateCalProperty(AHWeaponIntervalExtraDamageRate,float);

#undef InstantiateCalProperty

#define InstantiateMultiplierCalProperty(Name) \
	template class TMProperty<ePID_##Name,float>;

InstantiateMultiplierCalProperty(MPConsumeRate);
InstantiateMultiplierCalProperty(RPConsumeRate);
InstantiateMultiplierCalProperty(EPConsumeRate);
InstantiateMultiplierCalProperty(NatureMPConsumeRate);
InstantiateMultiplierCalProperty(DestructionMPConsumeRate);
InstantiateMultiplierCalProperty(EvilMPConsumeRate);

InstantiateMultiplierCalProperty(ExtraDOTDamageRate);
InstantiateMultiplierCalProperty(ExtraPunctureDamageRate);
InstantiateMultiplierCalProperty(ExtraChopDamageRate);
InstantiateMultiplierCalProperty(ExtraBluntDamageRate);
InstantiateMultiplierCalProperty(ExtraNatureDamageRate);
InstantiateMultiplierCalProperty(ExtraEvilDamageRate);
InstantiateMultiplierCalProperty(ExtraDestructionDamageRate);
InstantiateMultiplierCalProperty(ExtraBowDamageRate);
InstantiateMultiplierCalProperty(ExtraCrossBowDamageRate);
InstantiateMultiplierCalProperty(ExtraTwohandWeaponDamageRate);
InstantiateMultiplierCalProperty(ExtraPolearmDamageRate);
InstantiateMultiplierCalProperty(ExtraPaladinWeaponDamageRate);
InstantiateMultiplierCalProperty(ExtraAssistWeaponDamageRate);
InstantiateMultiplierCalProperty(ExtraLongDistDamageRate);
InstantiateMultiplierCalProperty(ExtraShortDistDamageRate);
InstantiateMultiplierCalProperty(LongDistWeaponDamageRate);
InstantiateMultiplierCalProperty(DamageDecsRate);
InstantiateMultiplierCalProperty(CastingProcessTimeRatio);
InstantiateMultiplierCalProperty(ExtraCureValueRate);
InstantiateMultiplierCalProperty(ExtraBeCuredValueRate);
InstantiateMultiplierCalProperty(BlockRate);
InstantiateMultiplierCalProperty(MissRate);
InstantiateMultiplierCalProperty(ResistCastingInterruptionRate);
InstantiateMultiplierCalProperty(PunctureDamageResistanceRate);
InstantiateMultiplierCalProperty(ChopDamageResistanceRate);
InstantiateMultiplierCalProperty(BluntDamageResistanceRate);
InstantiateMultiplierCalProperty(ControlDecreaseResistRate);
InstantiateMultiplierCalProperty(PauseDecreaseResistRate);
InstantiateMultiplierCalProperty(CripplingDecreaseResistRate);
InstantiateMultiplierCalProperty(DebuffDecreaseResistRate);
InstantiateMultiplierCalProperty(DOTDecreaseResistRate);
InstantiateMultiplierCalProperty(SpecialDecreaseResistRate);
InstantiateMultiplierCalProperty(ControlDecreaseTimeRate);
InstantiateMultiplierCalProperty(PauseDecreaseTimeRate);
InstantiateMultiplierCalProperty(CripplingDecreaseTimeRate);
InstantiateMultiplierCalProperty(SpecialDecreaseTimeRate);
InstantiateMultiplierCalProperty(ExtraPhysicDefenceRate);
InstantiateMultiplierCalProperty(ExtraPhysicDodgeRate);
InstantiateMultiplierCalProperty(ExtraParryRate);
InstantiateMultiplierCalProperty(ExtraStrikeRate);
InstantiateMultiplierCalProperty(ExtraMagicDodgeRate);
InstantiateMultiplierCalProperty(ExtraMagicResistanceRate);
InstantiateMultiplierCalProperty(ExtraNatureResistanceRate);
InstantiateMultiplierCalProperty(ExtraDestructionResistanceRate);
InstantiateMultiplierCalProperty(ExtraEvilResistanceRate);
InstantiateMultiplierCalProperty(ExtraCompleteResistanceRate);
InstantiateMultiplierCalProperty(ImmuneRate);
InstantiateMultiplierCalProperty(PunctureDamageImmuneRate);
InstantiateMultiplierCalProperty(ChopDamageImmuneRate);
InstantiateMultiplierCalProperty(BluntDamageImmuneRate);
InstantiateMultiplierCalProperty(NatureDamageImmuneRate);
InstantiateMultiplierCalProperty(DestructionDamageImmuneRate);
InstantiateMultiplierCalProperty(EvilDamageImmuneRate);
InstantiateMultiplierCalProperty(ControlDecreaseImmuneRate);
InstantiateMultiplierCalProperty(PauseDecreaseImmuneRate);
InstantiateMultiplierCalProperty(CripplingDecreaseImmuneRate);
InstantiateMultiplierCalProperty(DebuffDecreaseImmuneRate);
InstantiateMultiplierCalProperty(DOTDecreaseImmuneRate);
InstantiateMultiplierCalProperty(SpecialDecreaseImmuneRate);
InstantiateMultiplierCalProperty(CureImmuneRate);
InstantiateMultiplierCalProperty(MoveMagicImmuneRate);
InstantiateMultiplierCalProperty(NonTypePhysicsDamageImmuneRate);
InstantiateMultiplierCalProperty(NonTypeDamageImmuneRate);
InstantiateMultiplierCalProperty(NonTypeCureImmuneRate);
InstantiateMultiplierCalProperty(InterruptCastingProcessImmuneRate);

#undef InstantiateMultiplierCalProperty

CFighterCalInfo::CFighterCalInfo(void)
{
}

CFighterCalInfo::~CFighterCalInfo(void)
{
}

float CFighterCalInfo::CalStrikeRate(const CFighterBaseInfo* pFighter)const
{
	int32 uStrikeValue=m_StrikeValue.Get(pFighter);
	float fRate = uStrikeValue/(uStrikeValue+pFighter->CppGetLevel()*52+700.0f) + m_ExtraStrikeRate.Get();
	return max(0.0f, min(1.0f, fRate));
}

float CFighterCalInfo::CalNatureResistanceRate(const CFighterBaseInfo* pFighter)const
{
	float fRate = m_NatureResistanceValue.Get(pFighter)/(m_NatureResistanceValue.Get(pFighter) + pFighter->CppGetLevel()*52+700.0f) + m_ExtraNatureResistanceRate.Get();
	return max(0.0f, min(1.0f, fRate));
}

float CFighterCalInfo::CalDestructionResistanceRate(const CFighterBaseInfo* pFighter)const
{
	float fRate = m_DestructionResistanceValue.Get(pFighter)/(m_DestructionResistanceValue.Get(pFighter) + pFighter->CppGetLevel()*52+700.0f) + m_ExtraDestructionResistanceRate.Get();
	return max(0.0f, min(1.0f, fRate));
}

float CFighterCalInfo::CalEvilResistanceRate(const CFighterBaseInfo* pFighter)const
{
	float fRate = m_EvilResistanceValue.Get(pFighter)/(m_EvilResistanceValue.Get(pFighter) + pFighter->CppGetLevel()*52+700.0f) + m_ExtraEvilResistanceRate.Get();
	return max(0.0f, min(1.0f, fRate));
}

float CFighterCalInfo::CalPhysicalDefenceRate(const CFighterBaseInfo* pFighter)const
{
	int32 uDefence=m_Defence.Get(pFighter);
	float fRate = uDefence/float(uDefence+pFighter->CppGetLevel()*52+700) + m_ExtraPhysicDefenceRate.Get();
	return max(0.0f, min(1.0f, fRate));
}

float CFighterCalInfo::CalcSmashRate(const CFighterBaseInfo* pSelfBaseInfo, const CFighterCalInfo* pTargetCalInfo, const CFighterBaseInfo* pTargetBaseInfo, EAttackType attackType)const
{
	float fSmashRate = 1.0f;

	float fSmashLevel1 = 1.0f;
	float fSmashLevel2 = 1.25f;
	float fSmashLevel3 = 1.5f;
	float fSmashLevel4 = 1.75f;
	float fSmashLevel5 = 2.0f;

	switch(this->m_ValidityCoefficient.Get(pSelfBaseInfo))
	{
	case 0:
		return fSmashRate;
	case 2:
		fSmashLevel2 = 1.5f;
		fSmashLevel3 = 1.75f;
		fSmashLevel4 = 2.0f;
		fSmashLevel5 = 2.25f;
	default:
		break;
	}

	uint32 uSmashValue = uint32(-1);
	uint32 uRegistValue = uint32(-1);

	switch(attackType)
	{
	case eATT_Nature:
		uSmashValue = this->m_NatureSmashValue.Get(pSelfBaseInfo);
		uRegistValue = max(0, pTargetCalInfo->m_NatureResistanceValue.Get(pTargetBaseInfo));
		break;
	case eATT_Destroy:
		uSmashValue = this->m_DestructionSmashValue.Get(pSelfBaseInfo);
		uRegistValue = max(0, pTargetCalInfo->m_DestructionResistanceValue.Get(pTargetBaseInfo));
		break;
	case eATT_Evil:
		uSmashValue = this->m_EvilSmashValue.Get(pSelfBaseInfo);
		uRegistValue = max(0, pTargetCalInfo->m_EvilResistanceValue.Get(pTargetBaseInfo));
		break;
	case eATT_Puncture:
	case eATT_Chop:
	case eATT_BluntTrauma:
		uSmashValue = this->m_DefenceSmashValue.Get(pSelfBaseInfo);
		uRegistValue = max(0, pTargetCalInfo->m_Defence.Get(pTargetBaseInfo));
		break;
	default:
		return fSmashRate;
	}

	if(uRegistValue >= uSmashValue)
	{
		if(uRegistValue == 0 && uSmashValue == 0)
			fSmashRate = 0.5f;
		else
			fSmashRate = (fSmashLevel1 * float(2 * uSmashValue + uRegistValue) / float(uSmashValue + 2 * uRegistValue));
	}
	else if(uSmashValue > uRegistValue && uRegistValue >= uint32(0.75 * uSmashValue))
	{
		fSmashRate = (fSmashLevel2 * (2 * uSmashValue + uRegistValue) / float(uSmashValue + 2 * uRegistValue));
	}
	else if(uint32(0.75f * uSmashValue) > uRegistValue && uRegistValue >= uint32(0.5f * uSmashValue))
	{
		fSmashRate = (fSmashLevel3 * (2 * uSmashValue + uRegistValue) / float(uSmashValue + 2 * uRegistValue));
	}
	else if(uint32(0.5f * uSmashValue) > uRegistValue && uRegistValue >= uint32(0.25f * uSmashValue))
	{
		fSmashRate = (fSmashLevel4 * (2 * uSmashValue + uRegistValue) / float(uSmashValue + 2 * uRegistValue));
	}
	else if(uint32(0.25f * uSmashValue) > uRegistValue)
	{
		fSmashRate = (fSmashLevel5 * (2 * uSmashValue + uRegistValue) / float(uSmashValue + 2 * uRegistValue));
	}

	return fSmashRate;
}
