#include "stdafx.h"
#include "MagicOps_Damage.h"
#include "CSkillInstServer.h"
#include "CFighterDictator.h"
#include "CMagicOpArg.h"
#include "FighterProperty.inl"
#include "TPropertyHolder_inl.inl"
#include "CTriggerEvent.h"
#include "NormalAttack.h"
#include "CMagicStateInstServer.h"
#include "CoreCommon.h"
#include "CSpeedChangeMgr.h"
#include "CCharacterDictator.h"
#include "CFighterMediator.h"
#include "ICharacterMediatorCallbackHandler.h"
#include "TypeLimit.h"
#include "CSyncVariantServer.h"
#include "MagicOps_ChangeValue.h"
#include "CSkillMgrServer.h"
#include "CTempVarServer.h"

inline static float GetRandFloat()
{
	return rand() / float(RAND_MAX);
}

inline int32 roundi(double dValue)
{
	return int32(floor(dValue + 0.5));
}

inline double roundf(double dValue)
{
	return floor(dValue + 0.5);
}

inline bool DblOverflow(double dValue)
{
	return !CheckValueTypeToType<double, int32>(dValue);
}

double ReviseExtraValue(EHurtResult eHurtResult, double uExtraValue, float fRate)
{
	if(eHurtResult == eHR_Strike || eHurtResult == eHR_Block)
		return uExtraValue * fRate;
	return uExtraValue;
}

float CalPenetrationFinalRate(CFighterDictator* pFrom, CFighterDictator* pTo)
{
	float fPenetrationFinalRate = 1.0f;//穿透最终比例
	const CStaticAttribs& Attribs=
		CStaticAttribMgr::Inst().GetStaticAttribs(pTo->CppGetClass(), pTo->CppGetLevel());
	switch(pTo->GetNormalAttackMgr()->GetMHNormalAttack()->GetAttackType())
	{
	case eATT_Puncture:
	case eATT_Chop:
	case eATT_BluntTrauma:
		{
			uint32 DefencePen_pF	= pFrom->Get(&CFighterCalInfo::m_DefencePenetrationValue);
			uint32 ValidityCoe_pT	= pTo->Get(&CFighterCalInfo::m_ValidityCoefficient);
			uint32 Defence_pT		= Attribs.m_Defence;//这个要基础值(读静态表数据),这个值理论上不会为0
			if (Defence_pT == 0)
				return fPenetrationFinalRate;
			fPenetrationFinalRate = 1+((float)DefencePen_pF/(float)Defence_pT)*ValidityCoe_pT;
		}
		break;
	case eATT_Nature:
		{
			uint32 NatureDec_pF		= pFrom->Get(&CFighterCalInfo::m_NatureDecreaseResistanceValue);
			uint32 ValidityCoe_pT	= pTo->Get(&CFighterCalInfo::m_ValidityCoefficient);
			uint32 NatureRes_pT		= Attribs.m_NatureResistanceValue;
			if (NatureRes_pT == 0)
				return fPenetrationFinalRate;
			fPenetrationFinalRate	= 1+((float)NatureDec_pF/(float)NatureRes_pT)*ValidityCoe_pT;
		}
		break;
	case eATT_Destroy:
		{
			uint32 pFrom_NatureDec	= pFrom->Get(&CFighterCalInfo::m_DestructionDecreaseResistanceValue);
			uint32 ValidityCoe_pT	= pTo->Get(&CFighterCalInfo::m_ValidityCoefficient);
			uint32 DestructionRes_pT = Attribs.m_DestructionResistanceValue;
			if (DestructionRes_pT == 0)
				return fPenetrationFinalRate;
			fPenetrationFinalRate	= 1+((float)pFrom_NatureDec/(float)DestructionRes_pT)*ValidityCoe_pT;
		}
		break;
	case eATT_Evil:
		{
			uint32 pFrom_NatureDec	= pFrom->Get(&CFighterCalInfo::m_EvilDecreaseResistanceValue);
			uint32 ValidityCoe_pT	= pTo->Get(&CFighterCalInfo::m_ValidityCoefficient);
			uint32 EvilRes_pT		= Attribs.m_EvilResistanceValue;
			if (EvilRes_pT == 0)
				return fPenetrationFinalRate;
			fPenetrationFinalRate	= 1+((float)pFrom_NatureDec/(float)EvilRes_pT)*ValidityCoe_pT;
		}
		break;
	default:
		break;
	}
	return fPenetrationFinalRate;
}

//float CalcSmashRateHelper(float fExtraSmashRate, uint32 uSmashValue, uint32 uRegistValue)
//{
//	if( uSmashValue == 0 && uRegistValue == 0)
//		return 1.0f;
//
//	
//	float fSmashRate = 1.0f;
//	if(uRegistValue >= uSmashValue)
//	{
//		fSmashRate = (float(2 * uSmashValue + uRegistValue) / float(uSmashValue + 2 * uRegistValue)) + fExtraSmashRate;
//	}
//	else if(uSmashValue > uRegistValue && uRegistValue >= uint32(0.75 * uSmashValue))
//	{
//		fSmashRate = (1.25f * (2 * uSmashValue + uRegistValue) / float(uSmashValue + 2 * uRegistValue)) + fExtraSmashRate;
//	}
//	else if(uint32(0.75f * uSmashValue) > uRegistValue && uRegistValue >= uint32(0.5f * uSmashValue))
//	{
//		fSmashRate = (1.5f * (2 * uSmashValue + uRegistValue) / float(uSmashValue + 2 * uRegistValue)) + fExtraSmashRate;
//	}
//	else if(uint32(0.5f * uSmashValue) > uRegistValue && uRegistValue >= uint32(0.25f * uSmashValue))
//	{
//		fSmashRate = (1.75f * (2 * uSmashValue + uRegistValue) / float(uSmashValue + 2 * uRegistValue)) + fExtraSmashRate;
//	}
//	else if(uint32(0.25f * uSmashValue) > uRegistValue)
//	{
//		fSmashRate = (2.0f * (2 * uSmashValue + uRegistValue) / float(uSmashValue + 2 * uRegistValue)) + fExtraSmashRate;
//	}
//	return fSmashRate;
//}

//float CalcSmashRate(const CFighterDictator* pFrom, const CFighterDictator* pTo, EAttackType attackType)
//{
//	if(pFrom->m_ValidityCoefficient.Get(pFrom) == 0)
//		return 1.0f;
//
//	float fSmashRate = 1.0f;
//	uint32 uSmashValue = 0;
//	uint32 uRegistValue = 0;
//
//	if(attackType == eATT_Nature)
//	{
//		uSmashValue = pFrom->m_NatureSmashValue.Get(pFrom);
//		uRegistValue = pTo->m_NatureResistanceValue.Get(pTo);
//	}
//	else if(attackType == eATT_Destroy)
//	{
//		uSmashValue = pFrom->m_DestructionSmashValue.Get(pFrom);
//		uRegistValue = pTo->m_DestructionResistanceValue.Get(pTo);
//	}
//	else if(attackType == eATT_Evil)
//	{
//		uSmashValue = pFrom->m_EvilSmashValue.Get(pFrom);
//		uRegistValue = pTo->m_EvilResistanceValue.Get(pTo);
//	}
//	else if(attackType == eATT_Puncture || attackType == eATT_Chop || attackType == eATT_BluntTrauma)
//	{
//		uSmashValue = pFrom->m_DefenceSmashValue.Get(pFrom);
//		uRegistValue = pTo->m_Defence.Get(pTo);
//	}
//
//	float fExtraSmashRate = pFrom->m_ExtraSmashRate.Get(pFrom);
//	if( uSmashValue == 0 && uRegistValue == 0)
//		return 1.0f;
//	
//	if(uRegistValue >= uSmashValue)
//	{
//		fSmashRate = (float(2 * uSmashValue + uRegistValue) / float(uSmashValue + 2 * uRegistValue)) + fExtraSmashRate;
//	}
//	else if(uSmashValue > uRegistValue && uRegistValue >= uint32(0.75 * uSmashValue))
//	{
//		fSmashRate = (1.25f * (2 * uSmashValue + uRegistValue) / float(uSmashValue + 2 * uRegistValue)) + fExtraSmashRate;
//	}
//	else if(uint32(0.75f * uSmashValue) > uRegistValue && uRegistValue >= uint32(0.5f * uSmashValue))
//	{
//		fSmashRate = (1.5f * (2 * uSmashValue + uRegistValue) / float(uSmashValue + 2 * uRegistValue)) + fExtraSmashRate;
//	}
//	else if(uint32(0.5f * uSmashValue) > uRegistValue && uRegistValue >= uint32(0.25f * uSmashValue))
//	{
//		fSmashRate = (1.75f * (2 * uSmashValue + uRegistValue) / float(uSmashValue + 2 * uRegistValue)) + fExtraSmashRate;
//	}
//	else if(uint32(0.25f * uSmashValue) > uRegistValue)
//	{
//		fSmashRate = (2.0f * (2 * uSmashValue + uRegistValue) / float(uSmashValue + 2 * uRegistValue)) + fExtraSmashRate;
//	}
//
//	return fSmashRate;
//}

uint32 CTargetChangeHPMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult = MagicOpCalc(pSkillInst,Arg,pTo,pTo,&Value,&uExtraValue); //将pTo指针传进去进行计算伤害
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		

	return eResult;
}

uint32 CSuckBloodMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{
	if(!pTo->CppIsAlive())
		return eHR_Fail;

	CValueData Value;
	uint32 uExtraValue;
	MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eHR_SuckBlood), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		

	return eHR_SuckBlood;
}

void CIgnoreImmuneChangeHPMOP::GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue)
{
}

uint32 CChangeHPMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{
	if(!pTo->CppIsAlive())
	{
		return eHR_Fail;
	}

	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult = MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		

	return eResult;
}

int32 CChangeHPMOP::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	*pValue=roundi(Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex()) * GetLevelPressRate(pFrom, pTo));
	return eHR_Hit;
}

void CChangeHPMOP::GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue)
{
	if(iValue < 0)
	{
		float fImmunePercent	= pTo->Get(&CFighterCalInfo::m_ImmuneRate);
		if(fImmunePercent >= 1.0f)
		{
			eHurtResult = eHR_Immune;
			return;
		}

		if(pTo->Get(&CFighterCalInfo::m_NonTypeDamageImmuneRate) >= 1.0f)
		{
			eHurtResult = eHR_Immune;
			return;
		}
	}
	else if(iValue > 0)
	{
		if(pTo->Get(&CFighterCalInfo::m_NonTypeCureImmuneRate) >= 1.0f)
		{
			eHurtResult = eHR_Immune;
			return;
		}
	}
}


uint32 CCalculatePetHurtMop::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult = MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return eResult;
}

int32 CCalculatePetHurtMop::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	EHurtResult eResult = eHR_Hit;
	float fAddtionalHurtRate = (float)Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex());
	uint32 uAdditionalHurt = GetAdditionalHurt(pFrom);
	CCharacterDictator* pCharacter = pFrom->GetCharacter();
	CFighterDictator* pMaster = pCharacter?pCharacter->GetMaster()->GetFighter():NULL;
	if(pMaster == NULL)
	{
		return 0;
	}

	*pValue=-roundi(uAdditionalHurt + (pMaster->m_PhysicalDPS.Get(pMaster) + pMaster->m_MagicDamageValue.Get(pMaster)) * fAddtionalHurtRate * GetLevelPressRate(pFrom, pTo));	
	return eResult;
}

void CCalculatePetHurtMop::GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue)
{
	float fImmunePercent	= pTo->Get(&CFighterCalInfo::m_ImmuneRate);
	if(fImmunePercent >= 1.0f)
	{
		eHurtResult = eHR_Immune;
		return;
	}
}


uint32 CCalPetPhysicHurtMop::GetAdditionalHurt(CFighterDictator* pFrom)
{
	return pFrom->m_PhysicalDamage.Get();
}

uint32 CCalPetMagicHurtMop::GetAdditionalHurt(CFighterDictator* pFrom)
{
	return pFrom->m_MagicDamage.Get();
}

EHurtResult CCalculatePhysicsHurtMop::CalProbability(PropertyOfPhysicsCal Pro, bool bIgnoreMiss, bool bIgnorePhyDodge, bool bIgnoreParry, bool bIgnoreBlock, bool bIgnoreStrike)
{
	if (ePAT_AssistHand == Pro.m_ePhysicalAttackType)
		Pro.m_fMissRate += 0.2f;
	else if (ePAT_TwoHand == Pro.m_ePhysicalAttackType)
		Pro.m_fMissRate += 0.05f;
	uint32 uDiff = max(0, (int32)(Pro.m_uAccuratenessValue - Pro.m_uPhysicalDodgeValue - Pro.m_uParryValue));
	float fMissPercent = max(0.0f, (float)(0.05 + Pro.m_fMissRate - (float)(uDiff) / (float)(uDiff + Pro.m_uTargetLevel * 52 + 700.0f)));

	uDiff = max(0, (int32)(Pro.m_uPhysicalDodgeValue - Pro.m_uAccuratenessValue));
	float fDodgePercent	= max(0.0f, (float)(uDiff) / (float)(uDiff + Pro.m_uTargetLevel * 52 + 700.0f) + Pro.m_fExtraPhysicDodgeRate);

	uDiff = max(0, (int32)(Pro.m_uParryValue - max(0,(int32)(Pro.m_uAccuratenessValue - Pro.m_uPhysicalDodgeValue))));
	float fParryPercent	= max(0.0f, (float)(uDiff) / (float)(uDiff + Pro.m_uTargetLevel * 52 + 700.0f) + Pro.m_fExtraParryRate);

	float fBlockPercent	= Pro.m_fBlockRate;

	uDiff = max(0, (int32)(Pro.m_uStrikeValue - Pro.m_uStrikeResistanceValue));
	float fStrikePercent = max(0.0f, min(1.0f, (float)(uDiff) / (float)(uDiff  + Pro.m_uAttackerLevel * 52 + 700.0f) + Pro.m_fExtraStrikeRate));

	float fDice				= GetRandFloat();
	float fCurPercentPool	= 0;
	if(!bIgnoreMiss && fDice <= (fCurPercentPool += fMissPercent) )				return eHR_Miss;
	else if(!bIgnorePhyDodge && fDice <= ( fCurPercentPool += fDodgePercent ) )	return eHR_PhyDodge;
	else if(!bIgnoreParry && fDice <= ( fCurPercentPool += fParryPercent ) )	return eHR_Parry;
	else if(!bIgnoreBlock && fDice <= ( fCurPercentPool += fBlockPercent ) )	return eHR_Block;
	else if(!bIgnoreStrike && fDice <= ( fCurPercentPool += fStrikePercent ) )	return eHR_Strike;
	else																		return eHR_Hit;
}

uint32 CCalculatePhysicsHurtMop::GetStrikeValue(const CFighterDictator* pFrom)const
{
	return pFrom->Get(&CFighterCalInfo::m_StrikeValue);
}

float CCalculatePhysicsHurtMop::GetExtraStrikeRate(const CFighterDictator* pFrom)const
{
	return pFrom->Get(&CFighterCalInfo::m_ExtraStrikeRate);
}

int32 CCalculatePhysicsHurtMop::GetSkillPhyicsDamage(const CFighterDictator* pFrom)
{
	return pFrom->Get(&CFighterCalInfo::m_PhysicalDamage);
}

// 物理伤害运算
EHurtResult CCalculatePhysicsHurtMop::DicePhysicalHurt(CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	PropertyOfPhysicsCal pro;

	pro.m_ePhysicalAttackType = ePAT_Normal;
	if(pSkillInst->GetType() == eIT_NormalAttackInst)
	{
		if(!pSkillInst->GetNAInstServer()->IsMainHandNA())
		{
			pro.m_ePhysicalAttackType = ePAT_AssistHand; //副手未命中率增加20%
		}
		CNormalAttackMgr* pAttackMgr = pFrom->GetNormalAttackMgr();
		Ast(pAttackMgr);
		CSingleHandNATick* pMainAttack= pAttackMgr->GetMHNormalAttack();
		Ast(pMainAttack);
		EWeaponType eWeaponType = pMainAttack->GetWeaponType();
		if(IsShortDistTwoHandsWeapon(eWeaponType))
		{
			pro.m_ePhysicalAttackType = ePAT_TwoHand; //双手武器未命中率增加5%
		}
	}


	pro.m_uAttackerLevel			= pFrom->CppGetLevel();
	pro.m_fMissRate					= pFrom->Get(&CFighterCalInfo::m_MissRate);
	pro.m_uAccuratenessValue		= pFrom->Get(&CFighterCalInfo::m_AccuratenessValue);
	pro.m_uStrikeValue				= GetStrikeValue(pFrom);
	pro.m_fExtraStrikeRate			= GetExtraStrikeRate(pFrom);
	pro.m_uStrikeResistanceValue	=  pTo->Get(&CFighterCalInfo::m_StrikeResistanceValue);
	pro.m_uTargetLevel				= pTo->CppGetLevel();
	pro.m_uPhysicalDodgeValue		= pTo->Get(&CFighterCalInfo::m_PhysicalDodgeValue);
	pro.m_fExtraPhysicDodgeRate		= pTo->Get(&CFighterCalInfo::m_ExtraPhysicDodgeRate);
	pro.m_uParryValue				= pTo->Get(&CFighterCalInfo::m_ParryValue);
	pro.m_fExtraParryRate			= pTo->Get(&CFighterCalInfo::m_ExtraParryRate);
	pro.m_fBlockRate				= pTo->Get(&CFighterCalInfo::m_BlockRate);

	return CalProbability(pro);
}

uint32 CCalculatePhysicsHurtMop::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult = MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}

	return eResult;
}

int32 CCalculatePhysicsHurtMop::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	EHurtResult eHurtResult = eHR_Miss;

	double hurt = 0.0;			// 最终伤害
	double dExtraValue = 0.0;
	*pExtraValue = 0;			// 额外伤害
	double physicHurtBase = 0;	// 普攻伤害
	int32 skillPhysicDamage = 0;// 技能附加物理伤害
	double param = 1.0f;		// 攻击类型参数
	double temp = 0.0;			// 中间计算出的f伤害1
	float phyHurtRecent = (float)Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex());

	if(pSkillInst->GetType() == eIT_NormalAttackInst)
	{
		eHurtResult = pSkillInst->GetNAInstServer()->GetHurtResult();
		if (eHR_End==eHurtResult)
		{
			eHurtResult = DicePhysicalHurt(pSkillInst,pFrom,pTo);
		}
		else
		{
			pSkillInst->GetNAInstServer()->SetHurtResult(eHR_End);
		}
	}
	else
	{
		eHurtResult = DicePhysicalHurt(pSkillInst, pFrom,pTo);
	}

	// 根据攻击类型和护甲类型得出攻击类型参数
	EAttackType attackType;
	if(pSkillInst->GetType() == eIT_NormalAttackInst)
	{
		 attackType = GetAttackType(pFrom); 
	}
	else
	{
		attackType = pSkillInst->GetAttackType();
	}


	// 根据攻击类型的不同 算出三种普攻伤害
	skillPhysicDamage = GetSkillPhyicsDamage(pFrom);	// 有时序依赖必须在计算物理基础伤害之前，后面的计算可能触发附加物理伤害的撤销
	physicHurtBase = CalInitialDamage(pFrom, pTo);

	if(pTo->Get(&CFighterCalInfo::m_ImmuneRate) >= 1.0f)
		eHurtResult = eHR_Immune;

	switch(attackType)
	{
	case eATT_Puncture:
		param = 1.0f - pTo->m_PunctureDamageResistanceRate.Get(pTo);
		if(pTo->m_PunctureDamageImmuneRate.Get(pTo) >= 1.0f)
			eHurtResult = eHR_Immune;
		break;
	case eATT_Chop:
		param = 1.0f - pTo->m_ChopDamageResistanceRate.Get(pTo);
		if(pTo->m_ChopDamageImmuneRate.Get(pTo) >= 1.0f)
			eHurtResult = eHR_Immune;
		break;
	case eATT_BluntTrauma:
		param = 1.0f - pTo->m_BluntDamageResistanceRate.Get(pTo);
		if(pTo->m_BluntDamageImmuneRate.Get(pTo) >= 1.0f)
			eHurtResult = eHR_Immune;
		break;
	case eATT_None:
		if(pTo->m_NonTypePhysicsDamageImmuneRate.Get(pTo) >= 1.0f)
			eHurtResult = eHR_Immune;
		break;
	default:
		break;
	}
	//暂时先屏蔽param,即攻击类型参数对伤害的影响
	param = 1.0f;
	
	if(param<0.0f)
	{
		GenErr("CCalculatePhysicsHurtMop魔法操作计算uint32下限溢出");
	}

	const double dDefenceRate = 1-pTo->CFighterCalInfo::CalPhysicalDefenceRate(pTo)*param;

	switch(eHurtResult)
	{
	case	eHR_Miss:
		hurt = 0.0;
		break;				
	case	eHR_PhyDodge:
		hurt = 0.0;
		break;
	case	eHR_Immune:
		hurt = 0.0;
		break;
	case	eHR_Parry:		
		hurt = 0.0;
		break;
	case	eHR_Block:
		temp = phyHurtRecent*physicHurtBase + skillPhysicDamage;
		hurt =max(0.0, (temp-pTo->Get(&CFighterCalInfo::m_BlockDamage))*dDefenceRate);
		dExtraValue = temp * dDefenceRate - hurt;
		if(DblOverflow(dExtraValue))
		{
			*pExtraValue = roundi(dExtraValue);
			GenErr("CCalculatePhysicsHurtMop魔法操作计算uint32下限溢出");
		}
		break;
	case	eHR_Strike:
		{
			int32 uStrikeMultiValue=max(0,int32(pFrom->Get(&CFighterCalInfo::m_StrikeMultiValue)));
			int32 uResilienceValue=max(0,pTo->Get(&CFighterCalInfo::m_ResilienceValue));
			temp = phyHurtRecent*physicHurtBase + skillPhysicDamage;
			float diff =  float(uStrikeMultiValue-uResilienceValue);
			hurt = temp*(max(1.0f,diff/1000.0f + 2.0f))*dDefenceRate;
			dExtraValue = hurt - temp * dDefenceRate;
			if(DblOverflow(dExtraValue))
			{
				*pExtraValue = roundi(dExtraValue);
				GenErr("CCalculatePhysicsHurtMop魔法操作计算uint32下限溢出");
			}
			break;
		}
	case	eHR_Hit:
		temp = phyHurtRecent*physicHurtBase + skillPhysicDamage;
		hurt = temp*dDefenceRate;
		break;
	default:
		GenErr("伤害类型错误!");
		break;
	}
	if(DblOverflow(hurt))
	{
		*pExtraValue = roundi(dExtraValue);
		GenErr("CCalculatePhysicsHurtMop魔法操作计算uint32下限溢出");
	}

	switch(attackType)
	{
	case eATT_Puncture:
		hurt *= pFrom->Get(&CFighterCalInfo::m_ExtraPunctureDamageRate);
		dExtraValue = ReviseExtraValue(eHurtResult, dExtraValue, pFrom->Get(&CFighterCalInfo::m_ExtraPunctureDamageRate));
		break;
	case eATT_Chop:
		hurt *= pFrom->Get(&CFighterCalInfo::m_ExtraChopDamageRate);
		dExtraValue = ReviseExtraValue(eHurtResult, dExtraValue, pFrom->Get(&CFighterCalInfo::m_ExtraChopDamageRate));
		break;
	case eATT_BluntTrauma:
		hurt *= pFrom->Get(&CFighterCalInfo::m_ExtraBluntDamageRate);
		dExtraValue = ReviseExtraValue(eHurtResult, dExtraValue, pFrom->Get(&CFighterCalInfo::m_ExtraBluntDamageRate));
		break;
	default:
		break;
	}

	if(DblOverflow(hurt))
	{
		*pExtraValue = roundi(dExtraValue);
		GenErr("CCalculatePhysicsHurtMop魔法操作计算uint32下限溢出");
	}
	EWeaponType eWeaponType = pFrom->GetNormalAttackMgr()->GetMHNormalAttack()->GetWeaponType();
	if(IsLongDistWeapon(eWeaponType))	//远程武器
	{
		switch(eWeaponType)
		{
		case eWT_Bow:
			hurt *= pFrom->Get(&CFighterCalInfo::m_ExtraBowDamageRate);
			dExtraValue = ReviseExtraValue(eHurtResult, dExtraValue, pFrom->Get(&CFighterCalInfo::m_ExtraBowDamageRate));
			break;
		case eWT_CrossBow:
			hurt *= pFrom->Get(&CFighterCalInfo::m_ExtraCrossBowDamageRate);
			dExtraValue = ReviseExtraValue(eHurtResult, dExtraValue, pFrom->Get(&CFighterCalInfo::m_ExtraCrossBowDamageRate));
			break;
		default:
			break;
		}

		//CFPos posAttacker, posTarget;
		//posAttacker=pFrom->GetPixelPos();
		//posTarget=pTo->GetPixelPos();
		//float x = posAttacker.x - posTarget.x;
		//float y = posAttacker.y - posTarget.y;
		//float fDistance = sqrt(x*x + y*y)/eGridSpanForObj;

		hurt *= 1.0f + pFrom->Get(&CFighterCalInfo::m_ExtraLongDistDamageRate);	
		dExtraValue = ReviseExtraValue(eHurtResult, dExtraValue, 1.0f + pFrom->Get(&CFighterCalInfo::m_ExtraLongDistDamageRate));

		float fLongDistWeaponDamageRate = min(1.0f, max(0.0f, pFrom->Get(&CFighterCalInfo::m_LongDistWeaponDamageRate)));	
		hurt *= fLongDistWeaponDamageRate;
		dExtraValue = ReviseExtraValue(eHurtResult, dExtraValue, fLongDistWeaponDamageRate);
	}
	else	//近程武器
	{
		if(IsPolearmWeapon(eWeaponType))
		{
			hurt *= pFrom->Get(&CFighterCalInfo::m_ExtraPolearmDamageRate);
			dExtraValue = ReviseExtraValue(eHurtResult, dExtraValue, pFrom->Get(&CFighterCalInfo::m_ExtraPolearmDamageRate));
		}
		if(IsShortDistTwoHandsWeapon(eWeaponType))
		{
			hurt *= pFrom->Get(&CFighterCalInfo::m_ExtraTwohandWeaponDamageRate);
			dExtraValue = ReviseExtraValue(eHurtResult, dExtraValue, pFrom->Get(&CFighterCalInfo::m_ExtraTwohandWeaponDamageRate));
		}
		if(eWeaponType == eWT_PaladinGun)
		{
			hurt *= pFrom->Get(&CFighterCalInfo::m_ExtraPaladinWeaponDamageRate);
			dExtraValue = ReviseExtraValue(eHurtResult, dExtraValue,pFrom->Get(&CFighterCalInfo::m_ExtraPaladinWeaponDamageRate));
		}
		hurt *= 1.0f + pFrom->Get(&CFighterCalInfo::m_ExtraShortDistDamageRate);
		dExtraValue = ReviseExtraValue(eHurtResult, dExtraValue, 1.0f + pFrom->Get(&CFighterCalInfo::m_ExtraShortDistDamageRate));
	}
	if(DblOverflow(hurt))
	{
		*pExtraValue = roundi(dExtraValue);
		GenErr("CCalculatePhysicsHurtMop魔法操作计算uint32下限溢出");
	}

	CalMainAssistHandDamage(pSkillInst, pFrom, hurt, dExtraValue, eHurtResult);

	float PenetrationFinalRate = CalPenetrationFinalRate(pFrom,pTo);
	//float fSmashRate = CalcSmashRate(pFrom, pTo, attackType);
	float fSmashRate = pFrom->CalcSmashRate(pFrom, pTo, pTo, attackType);
	//cout << "mat    $$$$$  SmashRate = " << fSmashRate << endl;

	hurt *= pFrom->Get(&CFighterCalInfo::m_DamageDecsRate) * PenetrationFinalRate * fSmashRate;
	*pExtraValue = roundi(ReviseExtraValue(eHurtResult, dExtraValue, pFrom->Get(&CFighterCalInfo::m_DamageDecsRate) * PenetrationFinalRate * fSmashRate));

	*pValue = -roundi(hurt * GetLevelPressRate(pFrom, pTo));
	return eHurtResult;
}



EAttackType CCalculateMainHandPhysicsHurtMop::GetAttackType(const CFighterDictator* pFrom)const
{
	CNormalAttackMgr* pAttackMgr = pFrom->GetNormalAttackMgr();
	Ast(pAttackMgr);
	CSingleHandNATick* pMainAttack= pAttackMgr->GetMHNormalAttack();
	Ast(pMainAttack);
	return pMainAttack->GetAttackType();
}

double CCalculateMainHandPhysicsHurtMop::GenerateWeaponDamage(const CFighterDictator* pFrom)
{
	return GetRandFloat()*(pFrom->Get(&CFighterCalInfo::m_MainHandMaxWeaponDamage)-
		pFrom->Get(&CFighterCalInfo::m_MainHandMinWeaponDamage))+pFrom->Get(&CFighterCalInfo::m_MainHandMinWeaponDamage);
}

double CCalculateMainHandPhysicsHurtMop::CalInitialDamage(const CFighterDictator* pFrom, const CFighterDictator* pTo)
{
	double physicHurtBase = (int32)((pFrom->Get(&CFighterCalInfo::m_PhysicalDPS)) * (pFrom->Get(&CFighterCalInfo::m_MHWeaponIntervalExtraDamageRate)));
	double weaponHurt = GenerateWeaponDamage(pFrom);
	EAttackType eAttackType = (const_cast<CFighterDictator*>(pFrom))->GetNormalAttackMgr()->GetMHNormalAttack()->GetAttackType(); //攻击类型为主手武器的攻击类型

	// 根据攻击类型的不同 算出三种普攻伤害
	switch(eAttackType)
	{
	case eATT_Puncture:		// 穿刺
		physicHurtBase = weaponHurt 
			+ physicHurtBase
			+ pFrom->Get(&CFighterCalInfo::m_PunctureDamage);
		break;
	case eATT_Chop:			// 砍斫
		physicHurtBase = weaponHurt 
			+ physicHurtBase
			+ pFrom->Get(&CFighterCalInfo::m_ChopDamage);
		break;
	case eATT_BluntTrauma:	// 钝击
		physicHurtBase = weaponHurt 
			+ physicHurtBase
			+ pFrom->Get(&CFighterCalInfo::m_BluntDamage);
		break;
	default:
		physicHurtBase = weaponHurt 
			+ physicHurtBase;
		break;
	}

	if(DblOverflow(physicHurtBase)||DblOverflow(weaponHurt))
	{
		GenErr("CCalculateMainHandPhysicsHurtMop魔法操作计算uint32下限溢出");
	}

	CTriggerEvent::MessageEvent(eTST_Trigger, const_cast<CFighterDictator*>(pFrom), const_cast<CFighterDictator*>(pTo), eSET_MainHandNADamage);
	return physicHurtBase;
}

void CCalculateMainHandPhysicsHurtMop::CalMainAssistHandDamage(CSkillInstServer* pSkillInst, const CFighterDictator* pFrom, double& hurt, double& uExtraHurt, EHurtResult eHurtResult)
{
}

EAttackType CCalculateAssistantPhyicsHurtMop::GetAttackType(const CFighterDictator* pFrom)const
{
	CNormalAttackMgr* pAttackMgr = pFrom->GetNormalAttackMgr();
	Ast(pAttackMgr);
	CSingleHandNATick* pMainAttack= pAttackMgr->GetAHNormalAttack();
	Ast(pMainAttack);
	return pMainAttack->GetAttackType();
}

double CCalculateAssistantPhyicsHurtMop::GenerateWeaponDamage(const CFighterDictator* pFrom)
{
	return GetRandFloat()*(pFrom->Get(&CFighterCalInfo::m_AssistantMaxWeaponDamage)-
		pFrom->Get(&CFighterCalInfo::m_AssistantMinWeaponDamage))+pFrom->Get(&CFighterCalInfo::m_AssistantMinWeaponDamage);
}

double CCalculateAssistantPhyicsHurtMop::CalInitialDamage(const CFighterDictator* pFrom, const CFighterDictator* pTo)
{
	double physicHurtBase = (int32)((pFrom->Get(&CFighterCalInfo::m_PhysicalDPS)) * (pFrom->Get(&CFighterCalInfo::m_AHWeaponIntervalExtraDamageRate)));
	double weaponHurt = GenerateWeaponDamage(pFrom);
	EAttackType eAttackType = (const_cast<CFighterDictator*>(pFrom))->GetNormalAttackMgr()->GetAHNormalAttack()->GetAttackType(); //攻击类型为副手武器的攻击类型

	// 根据攻击类型的不同 算出三种普攻伤害
	switch(eAttackType)
	{
	case eATT_Puncture:		// 穿刺
		physicHurtBase = weaponHurt 
			+ physicHurtBase
			+ pFrom->Get(&CFighterCalInfo::m_PunctureDamage);
		break;
	case eATT_Chop:			// 砍斫
		physicHurtBase = weaponHurt 
			+ physicHurtBase
			+ pFrom->Get(&CFighterCalInfo::m_ChopDamage);
		break;
	case eATT_BluntTrauma:	// 钝击
		physicHurtBase = weaponHurt 
			+ physicHurtBase
			+ pFrom->Get(&CFighterCalInfo::m_BluntDamage);
		break;
	default:
		break;
	}
	if(DblOverflow(physicHurtBase)||DblOverflow(weaponHurt))
	{
		GenErr("CCalculateAssistantPhyicsHurtMop魔法操作计算uint32下限溢出");
	}
	return physicHurtBase;
}

int32 CCalculateAssistantPhyicsHurtMop::GetSkillPhyicsDamage(const CFighterDictator* pFrom)
{
	return pFrom->Get(&CFighterCalInfo::m_AssistPhysicalDamage);
}

void CCalculateAssistantPhyicsHurtMop::CalMainAssistHandDamage(CSkillInstServer* pSkillInst, const CFighterDictator* pFrom, double& hurt, double& uExtraHurt, EHurtResult eHurtResult)
{
	int32 temp=int32(hurt);
	if(temp<0)
	{
		GenErr("CCalculateAssistantPhyicsHurtMop魔法操作计算uint32下限溢出");
	}

	float fRate = 0.5f + pFrom->Get(&CFighterCalInfo::m_ExtraAssistWeaponDamageRate);
	hurt *= fRate;
	uExtraHurt = ReviseExtraValue(eHurtResult, uExtraHurt, fRate);
}

EHurtResult CCalculateMagicHurtMop::CalProbability(PropertyOfMagicCal Pro)
{
	float fDiff = (float)(max(0, (int32)(Pro.m_uMagicDodgeValue - Pro.m_uMagicHitValue)));
	float fDodgePercent	= max(0.0f, fDiff / (fDiff + Pro.m_uTargetLevel * 52 + 700.0f) + Pro.m_fExtraMagicDodgeRate);
	
	const float fFactor	= 3.0f;
	fDiff		= (float)max(0, (int32)(Pro.m_uMagicHitValue - Pro.m_uMagicDodgeValue));
	const float fNatureFactor =(float)max(0, (int32)(Pro.m_uNatureResistanceValue - fDiff));
	float fNature		= max(0.0f, min(1.0f, (fNatureFactor / (fNatureFactor + Pro.m_uTargetLevel * 52 + 700.0f) + Pro.m_fExtraNatureResistanceRate)));
	const float fDestructionFactor = (float)max(0, (int32)(Pro.m_uDestructionResistanceValue - fDiff));
	float fDestruction	= max(0.0f, min(1.0f, (fDestructionFactor / (fDestructionFactor + Pro.m_uTargetLevel * 52 + 700.0f) + Pro.m_fExtraDestructionResistanceRate)));
	const float fEvilFactor = (float)max(0, (int32)(Pro.m_uEvilResistanceValue - fDiff));
	float fEvil			= max(0.0f, min(1.0f, (fEvilFactor / (fEvilFactor + Pro.m_uTargetLevel * 52 + 700.0f) + Pro.m_fExtraEvilResistanceRate)));

	float fCompleteResistPercent	= max(0.0f, min(1.0f, (fNature + fDestruction + fEvil) / fFactor + Pro.m_fExtraCompleteResistanceRate));
	
	float sum = float(max(0, (int32)(Pro.m_uNatureResistanceValue + Pro.m_uDestructionResistanceValue + Pro.m_uEvilResistanceValue)));
	float fMagicResistPercent		= max(0.0f, min(1.0f, sum / (sum + Pro.m_uTargetLevel * 52 + 700) + Pro.m_fExtraMagicResistanceRate));

	fDiff = float(max(0, (int32)(Pro.m_uStrikeValue - Pro.m_uStrikeResistanceValue)));
	float fStrikePercent = max(0.0f, min(1.0f, fDiff / (fDiff  + Pro.m_uAttackerLevel * 52 + 700.0f) + Pro.m_fExtraStrikeRate));

	float fDice				= GetRandFloat();
	float fCurPercentPool	= fDodgePercent;
	if( fDice <= fCurPercentPool )										return eHR_MagDodge;
	else if( fDice <= ( fCurPercentPool += fCompleteResistPercent ) )	return eHR_ComResist;
	else if( fDice <= ( fCurPercentPool += fMagicResistPercent ) )		return eHR_Resist;
	else if( fDice <= ( fCurPercentPool += fStrikePercent ) )			return eHR_Strike;
	else																return eHR_Hit;
}

uint32 CCalculateMagicHurtMop::GetStrikeValue(const CFighterDictator* pFrom)const
{
	return pFrom->Get(&CFighterCalInfo::m_StrikeValue);
}

float CCalculateMagicHurtMop::GetExtraStrikeRate(const CFighterDictator* pFrom)const
{
	return pFrom->Get(&CFighterCalInfo::m_ExtraStrikeRate);
}

uint32 CCalculateMagicHurtMop::GetNatureResistanceValue(const CFighterDictator* pTo)const
{
	return pTo->Get(&CFighterCalInfo::m_NatureResistanceValue);
}

float CCalculateMagicHurtMop::GetExtraNatureResistanceRate(const CFighterDictator* pTo)const
{
	return pTo->Get(&CFighterCalInfo::m_ExtraNatureResistanceRate);
}

uint32 CCalculateMagicHurtMop::GetDestructionResistanceValue(const CFighterDictator* pTo)const
{
	return pTo->Get(&CFighterCalInfo::m_DestructionResistanceValue);
}

float CCalculateMagicHurtMop::GetExtraDestructionResistanceRate(const CFighterDictator* pTo)const
{
	return pTo->Get(&CFighterCalInfo::m_ExtraDestructionResistanceRate);
}

uint32 CCalculateMagicHurtMop::GetEvilResistanceValue(const CFighterDictator* pTo)const
{
	return pTo->Get(&CFighterCalInfo::m_EvilResistanceValue);
}

float CCalculateMagicHurtMop::GetExtraEvilResistanceRate(const CFighterDictator* pTo)const
{
	return pTo->Get(&CFighterCalInfo::m_ExtraEvilResistanceRate);
}

float CCalculateMagicHurtMop::GetExtraMagicResistanceRate(const CFighterDictator* pTo)const
{
	return pTo->Get(&CFighterCalInfo::m_ExtraMagicResistanceRate);
}

float CCalculateMagicHurtMop::GetExtraCompleteResistanceRate(const CFighterDictator* pTo)const
{
	return pTo->Get(&CFighterCalInfo::m_ExtraCompleteResistanceRate);
}

// 计算法术伤害的基类
EHurtResult CCalculateMagicHurtMop::DiceMagicHurt(CFighterDictator* pFrom,CFighterDictator* pTo)
{
	PropertyOfMagicCal pro;

	pro.m_uAttackerLevel			= pFrom->CppGetLevel();
	pro.m_uMagicHitValue			= pFrom->Get(&CFighterCalInfo::m_MagicHitValue);
	pro.m_uStrikeValue				= GetStrikeValue(pFrom);
	pro.m_fExtraStrikeRate			= GetExtraStrikeRate(pFrom);
	pro.m_uStrikeResistanceValue	=pTo->Get(&CFighterCalInfo::m_StrikeResistanceValue);
	pro.m_uTargetLevel				= pTo->CppGetLevel();
	pro.m_uMagicDodgeValue			= pTo->Get(&CFighterCalInfo::m_MagicDodgeValue);
	pro.m_fExtraMagicDodgeRate		= pTo->Get(&CFighterCalInfo::m_ExtraMagicDodgeRate);
	pro.m_uNatureResistanceValue			= GetNatureResistanceValue(pTo);
	pro.m_fExtraNatureResistanceRate		= GetExtraNatureResistanceRate(pTo);
	pro.m_uDestructionResistanceValue		= GetDestructionResistanceValue(pTo);
	pro.m_fExtraDestructionResistanceRate	= GetExtraDestructionResistanceRate(pTo);
	pro.m_uEvilResistanceValue				= GetEvilResistanceValue(pTo);
	pro.m_fExtraEvilResistanceRate			= GetExtraEvilResistanceRate(pTo);
	pro.m_fExtraMagicResistanceRate			= GetExtraMagicResistanceRate(pTo);
	pro.m_fExtraCompleteResistanceRate		= GetExtraCompleteResistanceRate(pTo);

	return CalProbability(pro);
}

uint32 CCalculateMagicHurtMop::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return eResult;
}

int32 CCalculateMagicHurtMop::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	EHurtResult eHurtResult = eHR_Miss;

	double hurt = 0;		// 最终造成的伤害
	*pExtraValue = 0;		// 被目标抵抗/格挡掉的伤害, 或者是由于爆击所增加的伤害
	double dExtraValue = 0;
	float temp;				// 计算伤害的中间零时变量,没什么意义

	if(pSkillInst->GetType() == eIT_NormalAttackInst)
	{
		eHurtResult = pSkillInst->GetNAInstServer()->GetHurtResult();
		if (eHR_End==eHurtResult)
		{
			eHurtResult = DiceMagicHurt(pFrom,pTo);
		}
		else
		{
			pSkillInst->GetNAInstServer()->SetHurtResult(eHR_End);
		}
	}
	else
	{
		eHurtResult = DiceMagicHurt(pFrom,pTo);
	}

	int32 uMagicDamageValue = pFrom->Get(&CFighterCalInfo::m_MagicDamageValue);
	int32 uCertainMagicDamageValue = GetCertainMagicDamageValue(pFrom);
	int32 uMagicDamage = pFrom->Get(&CFighterCalInfo::m_MagicDamage);	//技能附加法伤

	float fMagicDamageRate = (float)Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex());	// 该法术受法伤加成的影响率

	temp = uMagicDamage + (uMagicDamageValue + uCertainMagicDamageValue) * fMagicDamageRate;

	if((int32)temp<0||uMagicDamageValue<0||uCertainMagicDamageValue<0||uMagicDamage<0)
	{
		GenErr("CCalculateMagicHurtMop魔法操作计算uint32下限溢出");
	}

	if(pTo->Get(&CFighterCalInfo::m_ImmuneRate) >= 1.0f)
		eHurtResult = eHR_Immune;
	else	
		GetImmuneResultByType(pTo, eHurtResult);

	switch(eHurtResult)
	{
	case eHR_MagDodge:
		hurt = 0.0;
		break;
	case eHR_Immune:
		hurt = 0.0;
		break;
	case eHR_ComResist:
		hurt = 0.0;
		break;
	case eHR_Resist:
		hurt = max(0.0f, temp * (1 - GetResistRate(pTo)));
		dExtraValue = temp - hurt;
		if((temp - (int32)hurt)<0)
		{
			*pExtraValue = roundi(dExtraValue);
			GenErr("CCalculateMagicHurtMop魔法操作计算uint32下限溢出");
		}
		break;
	case eHR_Strike:
		{
			int32 uStrikeMultiValue=max(0,int32(pFrom->Get(&CFighterCalInfo::m_StrikeMultiValue)));
			int32 uResilienceValue=max(0,pTo->Get(&CFighterCalInfo::m_ResilienceValue));
			float diff =  float(uStrikeMultiValue-uResilienceValue);
			hurt = temp*(max(1.0f, diff/1000.0f + 2.0f));
			dExtraValue = hurt - temp;
			if((roundi(hurt) - (int32)temp)<0)
			{
				*pExtraValue = roundi(dExtraValue);
				GenErr("CCalculateMagicHurtMop魔法操作计算uint32下限溢出");
			}
		}
		break;
	case eHR_Hit:
		hurt =temp;
		break;
	default:
		GenErr("伤害类型错误!");
		break;
	}

	//EAttackType eAttackType = pSkillInst->m_eAttackType;
	//switch(eAttackType)
	//{
	//case eATT_Nature:
	//	hurt = (int32)(hurt * pFrom->Get(&CFighterCalInfo::m_ExtraNatureDamageRate));
	//	*pExtraValue = ReviseExtraValue(eHurtResult, *pExtraValue, pFrom->Get(&CFighterCalInfo::m_ExtraNatureDamageRate));
	//	break;
	//case eATT_Destroy:
	//	hurt = (int32)(hurt * pFrom->Get(&CFighterCalInfo::m_ExtraDestructionDamageRate));
	//	*pExtraValue = ReviseExtraValue(eHurtResult, *pExtraValue, pFrom->Get(&CFighterCalInfo::m_ExtraDestructionDamageRate));
	//	break;
	//case eATT_Evil:
	//	hurt = (int32)(hurt * pFrom->Get(&CFighterCalInfo::m_ExtraEvilDamageRate));
	//	*pExtraValue = ReviseExtraValue(eHurtResult, *pExtraValue, pFrom->Get(&CFighterCalInfo::m_ExtraEvilDamageRate));
	//	break;
	//default:
	//	break;
	//}
	dExtraValue = this->GetExtraCertainMagicDamageValue(pFrom, dExtraValue, eHurtResult, hurt);

	float PenetrationFinalRate = this->GetPenetrationFinalRate(pFrom,pTo);
	float fSmashRate = this->GetSmashRate(pFrom, pTo);
	//cout << "mat    $$$$$  SmashRate = " << fSmashRate << endl;

	hurt *= pFrom->Get(&CFighterCalInfo::m_DamageDecsRate) * PenetrationFinalRate * fSmashRate;
	*pExtraValue = roundi(ReviseExtraValue(eHurtResult, dExtraValue, pFrom->Get(&CFighterCalInfo::m_DamageDecsRate) * PenetrationFinalRate * fSmashRate));
	if(DblOverflow(hurt))
	{
		GenErr("CCalculateAssistantPhyicsHurtMop魔法操作计算uint32下限溢出");
	}
	*pValue= -roundi(hurt * GetLevelPressRate(pFrom, pTo));
 	return eHurtResult;
}

float CCalNatureMagHurtMop::GetResistRate(const CFighterDictator* pTo)
{
	return pTo->CFighterCalInfo::CalNatureResistanceRate(pTo);
}

void CCalNatureMagHurtMop::GetImmuneResultByType(const CFighterDictator* pTo, EHurtResult& eHurtResult)
{
	if(pTo->m_NatureDamageImmuneRate.Get(pTo) >= 1.0f)
		eHurtResult = eHR_Immune;
}


float CCalNatureMagHurtMop::GetSmashRate(const CFighterDictator* pFrom, const CFighterDictator* pTo)const
{
	//return CalcSmashRate(pFrom, pTo, eATT_Nature);
	return pFrom->CalcSmashRate(pFrom, pTo, pTo, eATT_Nature);
}

uint32 CCalNatureMagHurtMop::GetCertainMagicDamageValue(const CFighterDictator* pFrom)
{
	return pFrom->Get(&CFighterCalInfo::m_NatureDamageValue);
}

double CCalNatureMagHurtMop::GetExtraCertainMagicDamageValue(const CFighterDictator* pFrom, double uExtraValue, EHurtResult eHurtResult, double& hurt)
{
	hurt *= pFrom->Get(&CFighterCalInfo::m_ExtraNatureDamageRate);
	return ReviseExtraValue(eHurtResult, uExtraValue, pFrom->Get(&CFighterCalInfo::m_ExtraNatureDamageRate));
}

float CCalNatureMagHurtMop::GetPenetrationFinalRate(CFighterDictator* pFrom,CFighterDictator* pTo)
{
	return CalPenetrationFinalRate(pFrom,pTo);
}

void CCalDestructionMagHurtMop::GetImmuneResultByType(const CFighterDictator* pTo, EHurtResult& eHurtResult)
{
	if(pTo->m_DestructionDamageImmuneRate.Get(pTo) >= 1.0f)
		eHurtResult = eHR_Immune;
}

float CCalDestructionMagHurtMop::GetResistRate(const CFighterDictator* pTo)
{
	return pTo->CFighterCalInfo::CalDestructionResistanceRate(pTo);
}

float CCalDestructionMagHurtMop::GetSmashRate(const CFighterDictator* pFrom, const CFighterDictator* pTo)const
{
	//return CalcSmashRate(pFrom, pTo, eATT_Destroy);
	return pFrom->CalcSmashRate(pFrom, pTo, pTo, eATT_Destroy);
}

uint32 CCalDestructionMagHurtMop::GetCertainMagicDamageValue(const CFighterDictator* pFrom)
{
	return pFrom->Get(&CFighterCalInfo::m_DestructionDamageValue);
}

double CCalDestructionMagHurtMop::GetExtraCertainMagicDamageValue(const CFighterDictator* pFrom, double uExtraValue, EHurtResult eHurtResult, double& hurt)
{
	hurt *= pFrom->Get(&CFighterCalInfo::m_ExtraDestructionDamageRate);
	return ReviseExtraValue(eHurtResult, uExtraValue, pFrom->Get(&CFighterCalInfo::m_ExtraDestructionDamageRate));
}

float CCalDestructionMagHurtMop::GetPenetrationFinalRate(CFighterDictator* pFrom,CFighterDictator* pTo)
{
	return CalPenetrationFinalRate(pFrom,pTo);
}

float CCalEvilMagHurtMop::GetResistRate(const CFighterDictator* pTo)
{
	return pTo->CFighterCalInfo::CalEvilResistanceRate(pTo);
}
void CCalEvilMagHurtMop::GetImmuneResultByType(const CFighterDictator* pTo, EHurtResult& eHurtResult)
{
	if(pTo->m_EvilDamageImmuneRate.Get(pTo) >= 1.0f)
		eHurtResult = eHR_Immune;
}

float CCalEvilMagHurtMop::GetSmashRate(const CFighterDictator* pFrom, const CFighterDictator* pTo)const
{
	//return CalcSmashRate(pFrom, pTo, eATT_Evil);
	return pFrom->CalcSmashRate(pFrom, pTo, pTo, eATT_Evil);
}

uint32 CCalEvilMagHurtMop::GetCertainMagicDamageValue(const CFighterDictator* pFrom)
{
	return pFrom->Get(&CFighterCalInfo::m_EvilDamageValue);
}

double CCalEvilMagHurtMop::GetExtraCertainMagicDamageValue(const CFighterDictator* pFrom, double uExtraValue, EHurtResult eHurtResult, double& hurt)
{
	hurt *= pFrom->Get(&CFighterCalInfo::m_ExtraEvilDamageRate);
	return ReviseExtraValue(eHurtResult, uExtraValue, pFrom->Get(&CFighterCalInfo::m_ExtraEvilDamageRate));
}

float CCalEvilMagHurtMop::GetPenetrationFinalRate(CFighterDictator* pFrom,CFighterDictator* pTo)
{
	return CalPenetrationFinalRate(pFrom,pTo);
}

uint32 CCalDOTHurtMop::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{
	//这个魔法操作的Do()函数其实完全没有作用，能进来就是怪事了，囧
	//if(pSkillInst->GetType() != eIT_MagicStateInst)
	//{
	//	GenErr("DOT伤害计算魔法操作不能在魔法状态以外的效果中调用\n");
	//}
	//pSkillInst->SetInterval(true);
	CValueData Value;
	uint32 eHurtResult=MagicOpCalc(pSkillInst, Arg, pFrom, pTo, &Value, 0);
	MagicOpExec(pSkillInst, pFrom, pTo, Value, eHR_Hit, 0);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	//pSkillInst->SetInterval(false);
	return eHurtResult;
}

int32 CCalDOTHurtMop::MagicOpCalc(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo, CValueData *pValue, uint32* pExtraValue)
{
	pSkillInst->SetInterval(true);

	//DOT的单次伤害=（DOT伤害+（该系法伤+法伤）*法伤加成比例）*（1+攻击类型伤害比例）
	double iHurt = 0;

	uint32 uCertainMagicDamageValue = 0;
	float fExtraDamageRate = 1.0f;


	EAttackType attackType = pSkillInst->GetAttackType();
	switch(attackType)
	{
	case eATT_Nature:
		uCertainMagicDamageValue = pFrom->Get(&CFighterCalInfo::m_NatureDamageValue);
		fExtraDamageRate = pFrom->Get(&CFighterCalInfo::m_ExtraNatureDamageRate);
		break;
	case eATT_Destroy:
		uCertainMagicDamageValue = pFrom->Get(&CFighterCalInfo::m_DestructionDamageValue);
		fExtraDamageRate = pFrom->Get(&CFighterCalInfo::m_ExtraDestructionDamageRate);
		break;
	case eATT_Evil:
		uCertainMagicDamageValue = pFrom->Get(&CFighterCalInfo::m_EvilDamageValue);
		fExtraDamageRate = pFrom->Get(&CFighterCalInfo::m_ExtraEvilDamageRate);
		break;
	case eATT_Puncture:
		uCertainMagicDamageValue = pFrom->Get(&CFighterCalInfo::m_PunctureDamage);
		fExtraDamageRate = pFrom->Get(&CFighterCalInfo::m_ExtraPunctureDamageRate);
		break;
	case eATT_Chop:
		uCertainMagicDamageValue = pFrom->Get(&CFighterCalInfo::m_ChopDamage);
		fExtraDamageRate = pFrom->Get(&CFighterCalInfo::m_ExtraChopDamageRate);
		break;
	case eATT_BluntTrauma:
		uCertainMagicDamageValue = pFrom->Get(&CFighterCalInfo::m_BluntDamage);
		fExtraDamageRate = pFrom->Get(&CFighterCalInfo::m_ExtraBluntDamageRate);
		break;
	default:
		break;
	}

	uint32 uMagicDamageValue = pFrom->Get(&CFighterCalInfo::m_MagicDamageValue);
	uint32 uDOTDamage = pFrom->Get(&CFighterCalInfo::m_DOTDamage);	//技能附加DOT伤害
	float fMagicDamageRate = (float)Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex());	// 该法术受法伤加成的影响率

	float fDOTDamageRate = pTo->Get(&CFighterCalInfo::m_ExtraDOTDamageRate);
	iHurt = uDOTDamage + (uMagicDamageValue + uCertainMagicDamageValue) * fMagicDamageRate;

	iHurt *= fExtraDamageRate * fDOTDamageRate;

	float PenetrationFinalRate = CalPenetrationFinalRate(pFrom,pTo);
	float fSmashRate = pFrom->CalcSmashRate(pFrom, pTo, pTo, attackType);
	//cout << "mat    $$$$$  SmashRate = " << fSmashRate << endl;

	iHurt *= pFrom->Get(&CFighterCalInfo::m_DamageDecsRate) * PenetrationFinalRate * fSmashRate;
	
	if(DblOverflow(iHurt))
	{
		GenErr("CCalDOTHurtMop魔法操作计算uint32下限溢出");
	}

	*pValue = -roundi(iHurt * GetLevelPressRate(pFrom, pTo));

	return eHR_Hit;
}

void CCalDOTHurtMop::GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue)
{
	float fImmunePercent			= pTo->Get(&CFighterCalInfo::m_ImmuneRate);
	if(fImmunePercent >= 1.0f)
	{
		eHurtResult = eHR_Immune;
		return;
	}

	EAttackType attackType = pSkillInst->GetAttackType();
	switch(attackType)
	{
	case eATT_Nature:
		if(pTo->Get(&CFighterCalInfo::m_NatureDamageImmuneRate) >= 1.0f)
		{
			eHurtResult = eHR_Immune;
			return;
		}
		break;
	case eATT_Destroy:
		if(pTo->Get(&CFighterCalInfo::m_DestructionDamageImmuneRate) >= 1.0f)
		{
			eHurtResult = eHR_Immune;
			return;
		}
		break;
	case eATT_Evil:
		if(pTo->Get(&CFighterCalInfo::m_EvilDamageImmuneRate) >= 1.0f)
		{
			eHurtResult = eHR_Immune;
			return;
		}
		break;
	case eATT_Puncture:
		if(pTo->Get(&CFighterCalInfo::m_PunctureDamageImmuneRate) >= 1.0f)
		{
			eHurtResult = eHR_Immune;
			return;
		}
		break;
	case eATT_Chop:
		if(pTo->Get(&CFighterCalInfo::m_ChopDamageImmuneRate) >= 1.0f)
		{
			eHurtResult = eHR_Immune;
			return;
		}
		break;
	case eATT_BluntTrauma:
		if(pTo->Get(&CFighterCalInfo::m_BluntDamageImmuneRate) >= 1.0f)
		{
			eHurtResult = eHR_Immune;
			return;
		}
		break;
	case eATT_None:
		if(pTo->Get(&CFighterCalInfo::m_NonTypeDamageImmuneRate) >= 1.0f)
		{
			eHurtResult = eHR_Immune;
			return;
		}
		break;
	default:
		break;
	}
}





int32 CCalInstantHurtMop::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	*pValue=-roundi(Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex()) * GetLevelPressRate(pFrom, pTo));
	if(pValue->GetInt32() > 0)
	{
		stringstream str;
		str << Arg.GetTestString();
		GenErr("CCalNonTypeHurtMop操作参数值小于0, 参数值：", str.str());
	}
	return eHR_Hit;
}



// 治疗魔法操作
uint32 CCureMop::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}
	return eResult;
}

int32 CCureMop::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	double cureValue = 0;
	*pExtraValue = 0;
	double dExtraValue = 0;
	float cureRate = float(Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex()));
	float fStrikePercent			= pFrom->CFighterCalInfo::CalStrikeRate(pFrom);
	cureValue = pFrom->Get(&CFighterCalInfo::m_MagicDamage) + pFrom->Get(&CFighterCalInfo::m_MagicDamageValue)*cureRate;

	EHurtResult cureResult = eHR_Hit;

	//if(pTo->m_CureImmuneRate.Get(pTo) >= 1.0f)
	//{
	//	cureResult = eHR_Immune;
	//	cureValue = 0;
	//	*pExtraValue = 0;
	//}
	//else
	//{
		float rate = GetRandFloat();
		if(rate <= fStrikePercent)
		{
			cureResult = eHR_Strike;
			int32 tmp = (int32)cureValue;
			int32 uStrikeMultiValue=max(0,int32(pFrom->Get(&CFighterCalInfo::m_StrikeMultiValue)));
			//int32 uResilienceValue=max(0,pTo->Get(&CFighterCalInfo::m_ResilienceValue));
			cureValue = cureValue*((float)(uStrikeMultiValue)/1000.0f + 2.0f);
			if(DblOverflow(cureValue)||(int32)cureValue - tmp<0)
			{
				GenErr("CCureMop魔法操作计算uint32下限溢出");
			}
			dExtraValue = cureValue - tmp;
		}

		cureValue *= pFrom->Get(&CFighterCalInfo::m_ExtraCureValueRate) * pTo->Get(&CFighterCalInfo::m_ExtraBeCuredValueRate);
		*pExtraValue = roundi(ReviseExtraValue(cureResult, dExtraValue, pFrom->Get(&CFighterCalInfo::m_ExtraCureValueRate) * pTo->Get(&CFighterCalInfo::m_ExtraBeCuredValueRate)));
	//}

	*pValue = roundi(cureValue);

	return cureResult;
}

void CCureMop::GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue)
{
	if(pTo->m_CureImmuneRate.Get(pTo) >= 1.0f)
	{
		eHurtResult = eHR_Immune;
		return;
	}
}


//改变移动速度百分比
uint32 CChangePercentMoveSpeedMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{	
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return eResult;
	
}
int32 CChangePercentMoveSpeedMOP::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	*pValue = float(Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex()));
	return eHR_Success;
}
void CChangePercentMoveSpeedMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	float fMoveSpeed = Value.GetFloat();
	CSpeedChangeMgr* pSpeedMgr = pTo->GetMoveSpeedChangeMgr();
	double dSpeedChangeRate = fMoveSpeed + 1.0;
	pSpeedMgr->AddChange(dSpeedChangeRate);
	if(dSpeedChangeRate <= 0.0)
	{
		GenErr("移动速度不能小于等于0\n");
		return;
	}
	float fValue = float(pTo->m_RunSpeed.GetMultiplier() * dSpeedChangeRate);
	SetRunSpeedPercent(pTo,fValue);
	//cout << "现在的跑步速度是：" << pTo->m_RunSpeed.Get(pTo) << endl;

	fValue = float(pTo->m_WalkSpeed.GetMultiplier() * dSpeedChangeRate);
	SetWalkSpeedPercent(pTo,fValue);
	//cout << "现在的步行速度是：" << pTo->m_WalkSpeed.Get(pTo) << endl;

	if(fMoveSpeed != 0.0f) 
	{
		if (pTo->GetCharacter()->CppGetCtrlState(eFCS_InWalkState))
			pTo->OnMoveSpeedChange(pTo->m_WalkSpeed.Get(pTo), "走执行", pSkillInst->GetSkillName().c_str());
		else
			pTo->OnMoveSpeedChange(pTo->m_RunSpeed.Get(pTo), "跑执行", pSkillInst->GetSkillName().c_str());
		pTo->GetCharacter()->OnChangeSpeedBySkill();
	}
} 
void CChangePercentMoveSpeedMOP::CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value)
{
	float fMoveSpeed=Value.GetFloat();
	CSpeedChangeMgr* pSpeedMgr = pTo->GetMoveSpeedChangeMgr();
	double dSpeedChangeRate = fMoveSpeed + 1.0;
	pSpeedMgr->DecChange(dSpeedChangeRate/*, pFrom == pTo*//*pSkillInst->m_uCreatorID == pTo->GetEntityID()*/);
	if(dSpeedChangeRate <= 0.0)
	{
		GenErr("移动速度不能小于等于0\n");
		return;
	}
	float fValue =float(pTo->m_RunSpeed.GetMultiplier() / dSpeedChangeRate);
	SetRunSpeedPercent(pTo,fValue);
	//cout << "现在的跑步速度是：" << pTo->m_RunSpeed.Get(pTo) << endl;

	fValue =float(pTo->m_WalkSpeed.GetMultiplier() / dSpeedChangeRate);
	SetWalkSpeedPercent(pTo,fValue);
	//cout << "现在的步行速度是：" << pTo->m_WalkSpeed.Get(pTo) << endl;

	if(fMoveSpeed != 0.0f) 
	{
		if (pTo->GetCharacter()->CppGetCtrlState(eFCS_InWalkState))
			pTo->OnMoveSpeedChange(pTo->m_WalkSpeed.Get(pTo), "走撤销", pSkillInst->GetSkillName().c_str());
		else
			pTo->OnMoveSpeedChange(pTo->m_RunSpeed.Get(pTo), "跑撤销", pSkillInst->GetSkillName().c_str());
		pTo->GetCharacter()->OnChangeSpeedBySkill();
	}
}

void CChangePercentMoveSpeedMOP::Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CValueData Value;
	MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value);
	CancelExec(pSkillInst,pFrom,pTo,Value);
}

//改变步行速度百分比
uint32 CChangePercentWalkSpeedMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{	
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return eResult;

}
int32 CChangePercentWalkSpeedMOP::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	*pValue = float(Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex()));
	return eHR_Success;
}
void CChangePercentWalkSpeedMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	float fWalkSpeed = Value.GetFloat();
	CSpeedChangeMgr* pSpeedMgr = pTo->GetWalkSpeedChangeMgr();
	double dSpeedChangeRate = fWalkSpeed + 1.0;
	pSpeedMgr->AddChange(dSpeedChangeRate/*, pFrom == pTo*/);
	if(dSpeedChangeRate <= 0.0)
	{
		GenErr("步行速度不能小于等于0\n");
		return;
	}
	float fValue =float(pTo->m_WalkSpeed.GetMultiplier() * dSpeedChangeRate);
	SetWalkSpeedPercent(pTo,fValue);
	pTo->GetCharacter()->OnChangeSpeedBySkill();
	//cout << "现在的步行速度是：" << pTo->m_WalkSpeed.Get(pTo) << endl;
} 
void CChangePercentWalkSpeedMOP::CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value)
{
	float fWalkSpeed=Value.GetFloat();
	CSpeedChangeMgr* pSpeedMgr = pTo->GetMoveSpeedChangeMgr();
	double dSpeedChangeRate = fWalkSpeed + 1.0;
	pSpeedMgr->DecChange(dSpeedChangeRate/*, pFrom == pTo*/);
	if(dSpeedChangeRate <= 0.0)
	{
		GenErr("步行速度不能小于等于0\n");
		return;
	}
	float fValue =float(pTo->m_WalkSpeed.GetMultiplier() / dSpeedChangeRate);
	SetWalkSpeedPercent(pTo,fValue);
	pTo->GetCharacter()->OnChangeSpeedBySkill();
	//cout << "现在的步行速度是：" << pTo->m_WalkSpeed.Get(pTo) << endl;
}

void CChangePercentWalkSpeedMOP::Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CValueData Value;
	MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value);
	CancelExec(pSkillInst,pFrom,pTo,Value);
}

//改变普通攻击速度百分比
uint32 CChangePercentNormalAttackSpeedMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo, int32* MajorMopValue)
{	
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult = MagicOpCalc(pSkillInst, Arg, pFrom, pTo, &Value, &uExtraValue);
	MagicOpExec(pSkillInst, pFrom, pTo, Value, EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return eResult;

}

int32 CChangePercentNormalAttackSpeedMOP::MagicOpCalc(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo, CValueData *pValue, uint32* pExtraValue)
{
	float fPercentNASpeed = float(Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex()));
	*pValue=fPercentNASpeed;

	if (fPercentNASpeed > 0)
		fPercentNASpeed = 1/(1 + fPercentNASpeed) - 1;
	else
		fPercentNASpeed = 1*(1 - fPercentNASpeed) - 1;
	
	*pValue=*(int32*)&fPercentNASpeed;
	return eHR_Success;
}
void CChangePercentNormalAttackSpeedMOP::MagicOpExec(CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CFighterDictator* pTo, const CValueData& Value, EHurtResult eResult, uint32 uExtraValue)
{
	Ast(pTo->m_MainHandWeaponInterval.GetMultiplier() == pTo->m_AssistantWeaponInterval.GetMultiplier());

	float fPercentNASpeed = Value.GetFloat();
	CSpeedChangeMgr* pSpeedMgr = pTo->GetNASpeedChangeMgr();
	double dSpeedChangeRate = fPercentNASpeed + 1.0;
	pSpeedMgr->AddChange(dSpeedChangeRate/*, pFrom == pTo*/);

	if(dSpeedChangeRate <= 0.0)
	{
		GenErr("攻击速度不能小于等于0\n");
		return;
	}

	float fValue = float(pTo->m_MainHandWeaponInterval.GetMultiplier() * dSpeedChangeRate);
	SetMainHandIntervalPercent(pTo,fValue);
	SetOffHandIntervalPercent(pTo,fValue);

	//cout << pTo->GetEntityID() << " 现在的普通攻击主手速度是：" << pTo->m_MainHandWeaponInterval.Get(pTo) << endl;
	//cout << pTo->GetEntityID() << " 现在的普通攻击副手速度是：" << pTo->m_AssistantWeaponInterval.Get(pTo) << endl;
} 
void CChangePercentNormalAttackSpeedMOP::CancelExec(CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CFighterDictator* pTo, const CValueData& Value)
{
	Ast(pTo->m_MainHandWeaponInterval.GetMultiplier() == pTo->m_AssistantWeaponInterval.GetMultiplier());

	float fPercentNASpeed = Value.GetFloat();
	double dSpeedChangeRate = fPercentNASpeed + 1.0;
	pTo->GetNASpeedChangeMgr()->DecChange(dSpeedChangeRate/*, pFrom == pTo*/);

	if(dSpeedChangeRate <= 0.0)
	{
		GenErr("攻击速度不能小于等于0\n");
		return;
	}

	float fValue = float(pTo->m_MainHandWeaponInterval.GetMultiplier() / dSpeedChangeRate);
	SetMainHandIntervalPercent(pTo,fValue);
	SetOffHandIntervalPercent(pTo,fValue);

	//cout << "现在的普通攻击主手速度是：" << pTo->m_MainHandWeaponInterval.Get(pTo) << endl;
	//cout << "现在的普通攻击副手速度是：" << pTo->m_AssistantWeaponInterval.Get(pTo) << endl;
}

void CChangePercentNormalAttackSpeedMOP::Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CValueData Value;
	MagicOpCalc(pSkillInst, Arg, pFrom, pTo, &Value);
	CancelExec(pSkillInst, pFrom, pTo, Value);
}


//uint32 CTaskTypeHurtMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
//{
//	int32 iValue;
//	uint32 uExtraValue;
//	uint32 eResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&iValue,&uExtraValue);
//	MagicOpExec(pSkillInst,pFrom,pTo,iValue,EHurtResult(eResult), uExtraValue);
//	if (MajorMopValue)
//	{
//		*MajorMopValue=iValue;
//	}		
//	return eResult;
//}

//int32 CTaskTypeHurtMOP::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32 *pValue,uint32* pExtraValue)
//{
//	stringstream sExp;
//	size_t comma = Arg.GetTestString().find(',');
//	if(comma == string::npos)
//	{
//		sExp << "魔法操作[造成任务类型伤害]的魔法操作参数[" << Arg.GetTestString() << "]不能只有一项\n";
//		GenErr(sExp.str());
//		*pValue = 0;
//		return eHR_Miss;
//	}
//
//	float fImmunePercent	= pTo->Get(&CFighterCalInfo::m_ImmuneRate);
//	if(fImmunePercent >= 1.0f)
//	{
//		*pValue = 0;
//		return eHR_Immune;
//	}
//
//	string sArg1 = Arg.GetTestString().substr(0, comma);
//	if(pTo->IsInTaskAttackImmuneType(sArg1))
//	{
//		*pValue = 0;
//		return eHR_Immune;
//	}
//	else
//	{
//		CCfgCalc calcArg2(Arg.GetTestString().substr(comma + 1));
//		*pValue = -calcArg2.GetIntValue(pFrom, pTo, pSkillInst->GetLevelIndex());
//		return eHR_Hit;
//	}
//}

void CTaskNonBattleHurtMOP::GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue)
{
	float fImmunePercent	= pTo->Get(&CFighterCalInfo::m_ImmuneRate);
	if(fImmunePercent >= 1.0f)
	{
		eHurtResult = eHR_Immune;
		return;
	}

	if(pTo->IsInTaskAttackImmuneType("任务非战斗"))
	{
		eHurtResult = eHR_Immune;
		return;
	}
}

void CTaskBombHurtMOP::GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue)
{
	float fImmunePercent	= pTo->Get(&CFighterCalInfo::m_ImmuneRate);
	if(fImmunePercent >= 1.0f)
	{
		eHurtResult = eHR_Immune;
		return;
	}

	if(pTo->IsInTaskAttackImmuneType("任务炸弹"))
	{
		eHurtResult = eHR_Immune;
		return;
	}
}

void CTaskSpecialHurtMOP::GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue)
{
	float fImmunePercent	= pTo->Get(&CFighterCalInfo::m_ImmuneRate);
	if(fImmunePercent >= 1.0f)
	{
		eHurtResult = eHR_Immune;
		return;
	}

	if(pTo->IsInTaskAttackImmuneType("任务特殊"))
	{
		eHurtResult = eHR_Immune;
		return;
	}
}

void CTaskNonNationBattleBuildHurtMOP::GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue)
{
	float fImmunePercent	= pTo->Get(&CFighterCalInfo::m_ImmuneRate);
	if(fImmunePercent >= 1.0f)
	{
		eHurtResult = eHR_Immune;
		return;
	}

	if(pTo->IsInTaskAttackImmuneType("非国战建筑"))
	{
		eHurtResult = eHR_Immune;
		return;
	}
}

void CCalculateMagicHurtMop::GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue)
{
	float fImmunePercent	= pTo->Get(&CFighterCalInfo::m_ImmuneRate);
	if(fImmunePercent >= 1.0f)
	{
		eHurtResult = eHR_Immune;
		return;
	}

	GetImmuneResultByType(pTo,eHurtResult);
}

//void CTaskTypeHurtMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,int32 iValue,EHurtResult eResult,uint32 uExtraValue)
//{
//	int32 temp = pTo->m_HealthPoint.LimitGet() + iValue;
//	iValue = temp < 0 ? -1 * pTo->m_HealthPoint.LimitGet() : iValue;
//	if(pTo->GetTempVarMgr()->m_bLockDecreaseAgile)
//	{
//		iValue=0;
//	}
//	pTo->m_HealthPoint.LimitSet( pTo->m_HealthPoint.LimitGet() + iValue,pTo );
//}

//

uint32 CChangeNpcAttackScopeToMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{	
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return eResult;

}
int32 CChangeNpcAttackScopeToMOP::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	*pValue = (float)Arg.GetIntValue(pFrom, pTo, pSkillInst->GetLevelIndex());
	return eHR_Success;
}
void CChangeNpcAttackScopeToMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	float fAttackScope = Value.GetFloat();

	CCharacterDictator* pChar = pTo->GetCharacter();
	if(!pChar)
		return;

	pChar->ReSetNpcGridAttackScope(fAttackScope);
} 

void CChangeNpcAttackScopeToMOP::CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value)
{
	CCharacterDictator* pChar = pTo->GetCharacter();
	if(!pChar)
		return;

	pChar->RevertNpcAttackScope();
}

void CChangeNpcAttackScopeToMOP::Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CValueData Value;
	MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value);
	CancelExec(pSkillInst,pFrom,pTo,Value);
}

void CNoEventChangeHPMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue/* =0 */)
{
	int32 iFinalValue = Value.GetInt32();
	int32 CurHP = pTo->m_HealthPoint.LimitGet();
	int32 nMaxHP = int32(pTo->m_HealthPoint.Get(pTo));
	uint32 uRealChangeHP = iFinalValue>0?iFinalValue:(0-iFinalValue);
	if (iFinalValue < 0 && (CurHP+iFinalValue)<0 )
	{
		uRealChangeHP = CurHP;
	}
	else if (iFinalValue > 0 && (CurHP+iFinalValue) > nMaxHP)
	{
		uRealChangeHP = nMaxHP - CurHP;
	}
	if(!pTo->PrintInfoIsOff())
	{
		CCharacterDictator* pCreatorChar = CCharacterDictator::GetCharacterByID(pSkillInst->GetCreatorID());
		CFighterDictator* pCreatorFighter = pCreatorChar?pCreatorChar->GetFighter():NULL;
		bool bIsSkill = pSkillInst->GetType()==eIT_NormalAttackInst ? false: true;
		bool bIsMagic = pSkillInst->GetIsMagic();
		bool bIsArea = pSkillInst->GetIsArea();
		if (bIsMagic)
		{
			pTo->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pFrom,iFinalValue,eResult,bIsSkill);
		}
		else
		{
			pTo->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pFrom,iFinalValue,eResult,pSkillInst->GetInterval(),bIsSkill,bIsArea);
		}
		OnPrintFightInfo(EFII_ConsumeHP, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeHP);
	}

	pTo->m_HealthPoint.LimitSet(pTo->m_HealthPoint.LimitGet() + iFinalValue, pTo);

	if (pTo->m_HealthPoint.LimitGet() <= 0 && CurHP != 0)
	{
		if (pTo->CastToFighterMediator())
			pTo->CastToFighterMediator()->CppDie(pFrom);
		else
			pTo->CppDie(pFrom,pSkillInst->GetSkillName().c_str());
	}

	HurtResultToActionState( pSkillInst, pFrom, pTo, iFinalValue, eResult, false);

	if(pFrom->CastToFighterMediator())
		pFrom->CastToFighterMediator()->ChangeTargetHP(iFinalValue);
}

void CDrugChangeHPMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	int32 iFinalValue = Value.GetInt32();
	int32 CurHP = pTo->m_HealthPoint.LimitGet();

	if(!pTo->PrintInfoIsOff())
	{
		CCharacterDictator* pCreatorChar = CCharacterDictator::GetCharacterByID(pSkillInst->GetCreatorID());
		CFighterDictator* pCreatorFighter = pCreatorChar?pCreatorChar->GetFighter():NULL;
		bool bIsSkill = pSkillInst->GetType()==eIT_NormalAttackInst ? false: true;
		bool bIsMagic = pSkillInst->GetIsMagic();
		bool bIsArea = pSkillInst->GetIsArea();
		if (bIsMagic)
		{
			pTo->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pFrom,iFinalValue,eResult,bIsSkill);
		}
		else
		{
			pTo->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pFrom,iFinalValue,eResult,pSkillInst->GetInterval(),bIsSkill,bIsArea);
		}
		OnPrintFightInfo(EFII_Heal, pSkillInst, pFrom, pTo, iFinalValue>0 ? iFinalValue:0-iFinalValue, uExtraValue, 0);
	}

	pTo->m_HealthPoint.LimitSet(pTo->m_HealthPoint.LimitGet() + iFinalValue, pTo);

	if (pTo->m_HealthPoint.LimitGet() <= 0 && CurHP != 0)
	{
		if (pTo->CastToFighterMediator())
			pTo->CastToFighterMediator()->CppDie(pFrom);
		else
			pTo->CppDie(pFrom,pSkillInst->GetSkillName().c_str());
	}

	HurtResultToActionState( pSkillInst, pFrom, pTo, iFinalValue, eResult, false);

	if(pFrom && pFrom->CastToFighterMediator())
		pFrom->CastToFighterMediator()->ChangeTargetHP(iFinalValue);
}

void CDrugChangeHPMOP::StatisticFightHPChange(EFightInfoIndex eFightInfoIndex, CSkillInstServer* pSkillInst, CFighterDictator* pFrom, uint32 uRealChangeValue)
{
	return; //药品不计入伤害统计
}

uint32 CChangeExpRateMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{	
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return eResult;
	
}
int32 CChangeExpRateMOP::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	*pValue = float(Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex()));
	return eHR_Success;
}
void CChangeExpRateMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	float fExpModulus = Value.GetFloat();
	if (pTo->CastToFighterMediator())
	{
		ICharacterMediatorCallbackHandler* pHandler = pTo->CastToFighterMediator()->GetCallBackHandler();
		if (pHandler)
		{
			pHandler->OnChangeModulus(pTo->GetEntityID(),nCB_ExpModulusChange,fExpModulus);
		}
	}
} 
void CChangeExpRateMOP::CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value)
{
	float fExpModulus = 0-Value.GetFloat();
	if (pTo->CastToFighterMediator())
	{
		ICharacterMediatorCallbackHandler* pHandler = pTo->CastToFighterMediator()->GetCallBackHandler();
		if (pHandler)
		{
			pHandler->OnChangeModulus(pTo->GetEntityID(),nCB_ExpModulusChange,fExpModulus);
		}
	}
}

void CChangeExpRateMOP::Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CValueData Value;
	MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value);
	CancelExec(pSkillInst,pFrom,pTo,Value);
}

uint32 CChangeExpRateInFBMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{	
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return eResult;

}
int32 CChangeExpRateInFBMOP::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	*pValue = float(Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex()));
	return eHR_Success;
}
void CChangeExpRateInFBMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	float fExpModulus = Value.GetFloat();
	if (pTo->CastToFighterMediator())
	{
		ICharacterMediatorCallbackHandler* pHandler = pTo->CastToFighterMediator()->GetCallBackHandler();
		if (pHandler)
		{
			pHandler->OnChangeModulus(pTo->GetEntityID(),nCB_ExpModulusInFBChange,fExpModulus);
		}
	}
} 
void CChangeExpRateInFBMOP::CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value)
{
	float fExpModulus = 0-Value.GetFloat();
	if (pTo->CastToFighterMediator())
	{
		ICharacterMediatorCallbackHandler* pHandler = pTo->CastToFighterMediator()->GetCallBackHandler();
		if (pHandler)
		{
			pHandler->OnChangeModulus(pTo->GetEntityID(),nCB_ExpModulusInFBChange,fExpModulus);
		}
	}
}

void CChangeExpRateInFBMOP::Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CValueData Value;
	MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value);
	CancelExec(pSkillInst,pFrom,pTo,Value);
}

uint32 CChangeFetchRateMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{	
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return eResult;

}
int32 CChangeFetchRateMOP::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	*pValue = float(Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex()));
	return eHR_Success;
}
void CChangeFetchRateMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	float fExpModulus = Value.GetFloat();
	if (pTo->CastToFighterMediator())
	{
		ICharacterMediatorCallbackHandler* pHandler = pTo->CastToFighterMediator()->GetCallBackHandler();
		if (pHandler)
		{
			pHandler->OnChangeModulus(pTo->GetEntityID(),nCB_FetchModulusChange,fExpModulus);
		}
	}
} 
void CChangeFetchRateMOP::CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value)
{
	float fExpModulus = 0-Value.GetFloat();
	if (pTo->CastToFighterMediator())
	{
		ICharacterMediatorCallbackHandler* pHandler = pTo->CastToFighterMediator()->GetCallBackHandler();
		if (pHandler)
		{
			pHandler->OnChangeModulus(pTo->GetEntityID(),nCB_FetchModulusChange,fExpModulus);
		}
	}
}

void CChangeFetchRateMOP::Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CValueData Value;
	MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value);
	CancelExec(pSkillInst,pFrom,pTo,Value);
}

uint32 CCalculateMainHandPhysicsNoStrikeHurtMop::GetStrikeValue(const CFighterDictator* pFrom)const
{
	return 0;
}

float CCalculateMainHandPhysicsNoStrikeHurtMop::GetExtraStrikeRate(const CFighterDictator* pFrom)const
{
	return 0;
}

EHurtResult CCalculateMainHandPhysicsOnlyHitAndStrikeHurtMop::DicePhysicalHurt(CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	PropertyOfPhysicsCal pro;

	pro.m_ePhysicalAttackType = ePAT_Normal;
	if(pSkillInst->GetType() == eIT_NormalAttackInst)
	{
		if(!pSkillInst->GetNAInstServer()->IsMainHandNA())
		{
			pro.m_ePhysicalAttackType = ePAT_AssistHand; //副手未命中率增加20%
		}
		CNormalAttackMgr* pAttackMgr = pFrom->GetNormalAttackMgr();
		Ast(pAttackMgr);
		CSingleHandNATick* pMainAttack= pAttackMgr->GetMHNormalAttack();
		Ast(pMainAttack);
		EWeaponType eWeaponType = pMainAttack->GetWeaponType();
		if(IsShortDistTwoHandsWeapon(eWeaponType))
		{
			pro.m_ePhysicalAttackType = ePAT_TwoHand; //双手武器未命中率增加5%
		}
	}


	pro.m_uAttackerLevel			= pFrom->CppGetLevel();
	pro.m_fMissRate					= pFrom->Get(&CFighterCalInfo::m_MissRate);
	pro.m_uAccuratenessValue		= pFrom->Get(&CFighterCalInfo::m_AccuratenessValue);
	pro.m_uStrikeValue				= GetStrikeValue(pFrom);
	pro.m_fExtraStrikeRate			= GetExtraStrikeRate(pFrom);
	pro.m_uStrikeResistanceValue	=  pTo->Get(&CFighterCalInfo::m_StrikeResistanceValue);
	pro.m_uTargetLevel				= pTo->CppGetLevel();
	pro.m_uPhysicalDodgeValue		= pTo->Get(&CFighterCalInfo::m_PhysicalDodgeValue);
	pro.m_fExtraPhysicDodgeRate		= pTo->Get(&CFighterCalInfo::m_ExtraPhysicDodgeRate);
	pro.m_uParryValue				= pTo->Get(&CFighterCalInfo::m_ParryValue);
	pro.m_fExtraParryRate			= pTo->Get(&CFighterCalInfo::m_ExtraParryRate);
	pro.m_fBlockRate				= pTo->Get(&CFighterCalInfo::m_BlockRate);

	return CalProbability(pro, true, true, true, true);
}

uint32 CCalNatureMagNoStrikeHurtMop::GetStrikeValue(const CFighterDictator* pFrom)const
{
	return 0;
}

float CCalNatureMagNoStrikeHurtMop::GetExtraStrikeRate(const CFighterDictator* pFrom)const
{
	return 0;
}

uint32 CCalDestructionMagNoStrikeHurtMop::GetStrikeValue(const CFighterDictator* pFrom)const
{
	return 0;
}

float CCalDestructionMagNoStrikeHurtMop::GetExtraStrikeRate(const CFighterDictator* pFrom)const
{
	return 0;
}

uint32 CCalEvilMagNoStrikeHurtMop::GetStrikeValue(const CFighterDictator* pFrom)const
{
	return 0;
}

float CCalEvilMagNoStrikeHurtMop::GetExtraStrikeRate(const CFighterDictator* pFrom)const
{
	return 0;
}

uint32 CCalDOTNoStrikeHurtMop::GetStrikeValue(const CFighterDictator* pFrom)const
{
	return 0;
}

float CCalDOTNoStrikeHurtMop::GetExtraStrikeRate(const CFighterDictator* pFrom)const
{
	return 0;
}


uint32 CChangeStealthMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{	
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return eResult;
}

int32 CChangeStealthMOP::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	*pValue = float(Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex()));
	return eHR_Success;
}

void CChangeStealthMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	float fStealthChange = Value.GetFloat();
	if (pTo && pTo->GetCharacter())
	{
		float fCurStalth = pTo->GetCharacter()->GetStealth();
		float fRes = fCurStalth + fStealthChange;
		bool bZero = abs(fRes) < 0.1;
		fRes = bZero ? 0 : fRes;
		pTo->GetCharacter()->SetStealth(fRes);
		//cout<<"Stealth ： "<<fStealthChange<<"当前值： " <<fCurStalth + fStealthChange<<endl;
	}
} 
void CChangeStealthMOP::CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value)
{
	if (pTo && pTo->GetCharacter())
	{
		float fCurStealth = pTo->GetCharacter()->GetStealth();
		float fStealthChange = 0 - Value.GetFloat();
		float fRes = fCurStealth + fStealthChange;
		bool bZero = abs(fRes) < 0.1;
		fRes = bZero ? 0 : fRes;
		pTo->GetCharacter()->SetStealth(fRes);
		//cout<<"Stealth ： "<<fStealthChange<<"当前值： "<< fRes<<endl;
	}
}

void CChangeStealthMOP::Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CValueData Value;
	MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value);
	CancelExec(pSkillInst,pFrom,pTo,Value);
}

uint32 CChangeKeennessMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{	
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return eResult;
}

int32 CChangeKeennessMOP::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	*pValue = float(Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex()));
	return eHR_Success;
}
void CChangeKeennessMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	float fStealthChange = Value.GetFloat();
	if (pTo && pTo->GetCharacter())
	{
		float fCurStalth = pTo->GetCharacter()->GetKeenness();
		pTo->GetCharacter()->SetKeenness(fCurStalth + fStealthChange);
	}
} 
void CChangeKeennessMOP::CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value)
{
	if (pTo && pTo->GetCharacter())
	{
		float fCurStealth = pTo->GetCharacter()->GetKeenness();
		float fStealthChange = 0 - Value.GetFloat();
		float fRes = fCurStealth + fStealthChange;
		fRes = (abs(fRes) > 0.1 ) ? fRes : 0;
		pTo->GetCharacter()->SetKeenness(fRes);
	}
}

void CChangeKeennessMOP::Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CValueData Value;
	MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value);
	CancelExec(pSkillInst,pFrom,pTo,Value);
}

uint32 CDisableStealthMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{	
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return eResult;
}

int32 CDisableStealthMOP::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	CCharacterDictator* pTarget = pTo->GetCharacter();
	Ast (pTarget);
	*pValue = float(pTarget->GetStealth());
	return eHR_Success;
}

void CDisableStealthMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	if (pTo && pTo->GetCharacter())
	{
		pTo->GetCharacter()->SetStealth(0.0f);
	}
} 
void CDisableStealthMOP::CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value)
{
	if (pTo && pTo->GetCharacter())
	{
		float fCurStealth = pTo->GetCharacter()->GetStealth();
		float fStealthChange = Value.GetFloat();
		float fRes = fCurStealth + fStealthChange;
		fRes = (abs(fRes) > 0.1 ) ? fRes : 0;
		pTo->GetCharacter()->SetStealth(fRes);
	}
}

void CDisableStealthMOP::Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CValueData Value;
	MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value);
	CancelExec(pSkillInst,pFrom,pTo,Value);
}


uint32 CDisableKeennessMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{	
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return eResult;
}

int32 CDisableKeennessMOP::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	CCharacterDictator* pTarget = pTo->GetCharacter();
	Ast (pTarget);
	*pValue = float(pTarget->GetKeenness());
	return eHR_Success;
}

void CDisableKeennessMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	if (pTo && pTo->GetCharacter())
	{
		pTo->GetCharacter()->SetKeenness(0.0f);
	}
} 

void CDisableKeennessMOP::CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value)
{
	if (pTo && pTo->GetCharacter())
	{
		float fCurStealth = pTo->GetCharacter()->GetKeenness();
		float fStealthChange = Value.GetFloat();
		float fRes = fCurStealth + fStealthChange;
		fRes = (abs(fRes) > 0.1 ) ? fRes : 0;
		pTo->GetCharacter()->SetKeenness(fRes);
	}
}

void CDisableKeennessMOP::Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CValueData Value;
	MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value);
	CancelExec(pSkillInst,pFrom,pTo,Value);
}

uint32 CAbsorbManaPointMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{	
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return eResult;
}

int32 CAbsorbManaPointMOP::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	*pValue = Arg.GetIntValue(pFrom, pTo, pSkillInst->GetLevelIndex());
	return eHR_Success;
}

void CAbsorbManaPointMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	int32 iValue = Value.GetInt32();
	int32 iOriginalValue = iValue;
	CTempVarMgrServer* pTemeVarMgrFrom= pFrom?pFrom->GetTempVarMgr():NULL;
	CTempVarMgrServer* pTemeVarMgrTo= pTo->GetTempVarMgr();
	Ast(pTemeVarMgrTo);

	int32 temp = pTo->m_ManaPoint.LimitGet() + iValue;
	if (iValue > 0)
	{
		iValue = temp > (int32)pTo->m_ManaPoint.Get(pTo) ? (int32)pTo->m_ManaPoint.Get(pTo) - (int32)pTo->m_ManaPoint.LimitGet() : iValue;
	}
	else
	{
		iValue = temp < 0 ? -1 * pTo->m_ManaPoint.LimitGet() : iValue;
		if(pTemeVarMgrFrom) pTemeVarMgrFrom->SetSubMP(-iValue);
		pTemeVarMgrTo->SetBeSubMP(-iValue);
		CTriggerEvent::MessageEvent(eTST_All, pFrom, pTo, eSET_SubMP);
		iValue = -(int32)(pTemeVarMgrTo->GetBeSubMP());
	}
	CCharacterDictator* pCreatorChar = CCharacterDictator::GetCharacterByID(pSkillInst->GetCreatorID());
	CFighterDictator* pCreatorFighter = pCreatorChar?pCreatorChar->GetFighter():NULL;
	pTo->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pFrom,iValue,eAT_MP,eHR_Hit);

	if (iOriginalValue > 0)
	{
		pTo->OnPrintFightInfo(EFII_RenewMP, pFrom?pFrom->GetEntityID():0, pTo->GetEntityID(), pSkillInst, iOriginalValue, eATT_None, 0, 0);
	}
	if (iOriginalValue < 0)
	{
		pTo->OnPrintFightInfo(EFII_LostMP, pFrom?pFrom->GetEntityID():0, pTo->GetEntityID(), pSkillInst, iOriginalValue, eATT_None, 0, 0);
	}
	pTo->m_ManaPoint.LimitSet( pTo->m_ManaPoint.LimitGet()+iValue,pTo );
}

uint32 CAbsorbRagePointMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo,int32* MajorMopValue)
{	
	CValueData Value;
	uint32 uExtraValue;
	uint32 eResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value,&uExtraValue);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,EHurtResult(eResult), uExtraValue);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return eResult;
}

int32 CAbsorbRagePointMOP::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	*pValue = Arg.GetIntValue(pFrom, pTo, pSkillInst->GetLevelIndex());
	return eHR_Success;
}

void CAbsorbRagePointMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	int32 iValue = Value.GetInt32();
	int32 iOriginalValue = iValue;
	int32 temp = pTo->m_RagePoint.LimitGet() + iValue;
	if (iValue > 0)
	{
		iValue = temp > (int32)pTo->m_RagePoint.Get(pTo) ? (int32)pTo->m_RagePoint.Get(pTo) - (int32)pTo->m_RagePoint.LimitGet() : iValue;
	}
	else
	{
		iValue = temp < 0 ? -1 * pTo->m_RagePoint.LimitGet() : iValue;
	}
	CCharacterDictator* pCreatorChar = CCharacterDictator::GetCharacterByID(pSkillInst->GetCreatorID());
	CFighterDictator* pCreatorFighter = pCreatorChar?pCreatorChar->GetFighter():NULL;
	pTo->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pFrom,iValue,eAT_RP,eHR_Hit);

	if (iOriginalValue < 0)
	{
		pTo->OnPrintFightInfo(EFII_LostRP, pFrom?pFrom->GetEntityID():0, pTo->GetEntityID(), pSkillInst, iOriginalValue, eATT_None, 0, 0);
	}

	pTo->m_RagePoint.LimitSet( pTo->m_RagePoint.LimitGet()+iValue,pTo );
}

uint32 CCalDestructionMagNoResistHurtMop::GetNatureResistanceValue(const CFighterDictator* pTo)const{return 0;}
float CCalDestructionMagNoResistHurtMop::GetExtraNatureResistanceRate(const CFighterDictator* pTo)const{return 0;}
uint32 CCalDestructionMagNoResistHurtMop::GetDestructionResistanceValue(const CFighterDictator* pTo)const{return 0;}
float CCalDestructionMagNoResistHurtMop::GetExtraDestructionResistanceRate(const CFighterDictator* pTo)const{return 0;}
uint32 CCalDestructionMagNoResistHurtMop::GetEvilResistanceValue(const CFighterDictator* pTo)const{return 0;}
float CCalDestructionMagNoResistHurtMop::GetExtraEvilResistanceRate(const CFighterDictator* pTo)const{return 0;}
float CCalDestructionMagNoResistHurtMop::GetExtraMagicResistanceRate(const CFighterDictator* pTo)const{return 0;}
float CCalDestructionMagNoResistHurtMop::GetExtraCompleteResistanceRate(const CFighterDictator* pTo)const{return 0;}
