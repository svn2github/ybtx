#include "stdafx.h"
#include "MagicOps_ChangeValue.h"
#include "CFighterDictator.h"
#include "CMagicOpArg.h"
#include "FighterProperty.inl"
#include "FighterProperty_inl.inl"
#include "CCfgCalc.inl"
#include "CTriggerEvent.h"
#include "CTempVarServer.h"
#include "FighterProperty.inl"
#include "ErrLogHelper.h"
#include "CSkillInstServer.inl"
#include "CCharacterDictator.h"
#include "CSkillMgrServer.h"

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
uint32 TChangeAgileInfoValueMOP<PropertyType,pProperty>::
Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{
	CValueData Value;
	uint32 uResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	MagicOpExec(pSkillInst,pFrom,pTo,Value,eHR_Hit);

	return uResult;
}

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
int32 TChangeAgileInfoValueMOP<PropertyType,pProperty>::
MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	int32 iValue=Arg.GetIntValue(pFrom, pTo, pSkillInst->GetLevelIndex());

	if (iValue < 0)
	{
		string sRawName;
#ifdef _WIN32
		sRawName=typeid(PropertyType).raw_name();
		sRawName = sRawName.substr(4,sRawName.size()-6);
#else
		sRawName=typeid(PropertyType).name();
		sRawName = sRawName.substr(2,sRawName.size()-2);
#endif

		if(sRawName.compare("CManaPointServer") == 0)
		{
			iValue = (int32)(iValue * pTo->Get(&CFighterSyncToDirectorCalInfo::m_MPConsumeRate));
			if (pSkillInst->GetAttackType() == eATT_Nature)
			{
				iValue = (int32)(iValue * pTo->Get(&CFighterSyncToDirectorCalInfo::m_NatureMPConsumeRate));
			}
			else if (pSkillInst->GetAttackType() == eATT_Destroy)
			{
				iValue = (int32)(iValue * pTo->Get(&CFighterSyncToDirectorCalInfo::m_DestructionMPConsumeRate));
			}
			else if (pSkillInst->GetAttackType() == eATT_Evil)
			{
				iValue = (int32)(iValue * pTo->Get(&CFighterSyncToDirectorCalInfo::m_EvilMPConsumeRate));
			}
		}
		else if (sRawName.compare("CRagePointServer") == 0)
		{	
			iValue = (int32)(iValue * pTo->Get(&CFighterSyncToDirectorCalInfo::m_RPConsumeRate));
		}
		else if (sRawName.compare("CEnergyPointServer") == 0)
		{
			iValue = (int32)(iValue * pTo->Get(&CFighterSyncToDirectorCalInfo::m_EPConsumeRate));
		}
	}

	*pValue = iValue;

	return eHR_Success;
}

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
void TChangeAgileInfoValueMOP<PropertyType,pProperty>::
MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	int32 iValue = Value.GetInt32();
	CTempVarMgrServer* pTemeVarMgrFrom= pFrom?pFrom->GetTempVarMgr():NULL;
	CTempVarMgrServer* pTemeVarMgrTo= pTo->GetTempVarMgr();
	Ast(pTemeVarMgrTo);
	string sRawName;
#ifdef _WIN32
	sRawName=typeid(PropertyType).raw_name();
	sRawName = sRawName.substr(4,sRawName.size()-6);
#else
	sRawName=typeid(PropertyType).name();
	sRawName = sRawName.substr(2,sRawName.size()-2);
#endif

	PropertyType& Property=(class_cast<CFighterAgileInfoServer*>(pTo)->*pProperty);
	int32 temp = Property.LimitGet() + iValue;
	int32 iOriginalValue = iValue;
	if (iValue > 0)
	{
		int32 iMaxProperty = Property.Get(pTo);
		iValue = temp > iMaxProperty ? iMaxProperty - Property.LimitGet() : iValue;
		if (pTemeVarMgrTo->m_bLockIncreaseAgile)
		{
			iValue=0;
		}
		//cout << "time = " << GetTickCount() << ", Before TChangeAgileInfoValueMOP CurMP = " << Property.LimitGet() << endl;
	}
	else
	{
		iValue = temp < 0 ? -1 * Property.LimitGet() : iValue;
		if(sRawName.compare("CManaPointServer") == 0)
		{
			if(pTemeVarMgrFrom) pTemeVarMgrFrom->SetSubMP(-iValue);
			pTemeVarMgrTo->SetBeSubMP(-iValue);
			CTriggerEvent::MessageEvent(eTST_All, pFrom, pTo, eSET_SubMP);
			iValue = -(int32)(pTemeVarMgrTo->GetBeSubMP());
			//cout << pTo->GetEntityID() << " mana iValue = " << iValue << endl;
		}
		else
		{
			if (pTemeVarMgrTo->m_bLockDecreaseAgile)
			{
				iValue=0;
			}
		}
	}

	CCharacterDictator* pCreatorChar = CCharacterDictator::GetCharacterByID(pSkillInst->GetCreatorID());
	CFighterDictator* pCreatorFighter = pCreatorChar?pCreatorChar->GetFighter():NULL;

	if(sRawName.compare("CManaPointServer") == 0)
	{
		if (iOriginalValue > 0)
		{
			pTo->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pFrom,iOriginalValue,eAT_MP,eHR_Hit);
			pTo->OnPrintFightInfo(EFII_RenewMP, pFrom?pFrom->GetEntityID():0, pTo->GetEntityID(), pSkillInst, iOriginalValue, eATT_None, uExtraValue, 0);
		}
		else if (iOriginalValue < 0)
		{
			pTo->OnPrintFightInfo(EFII_ConsumeMP, pFrom?pFrom->GetEntityID():0, pTo->GetEntityID(), pSkillInst, iOriginalValue, eATT_None, uExtraValue, 0);
		}
	
	}
	if(sRawName.compare("CRagePointServer") == 0)
	{
		if (iOriginalValue > 0)
		{
			pTo->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pFrom,iOriginalValue,eAT_MP,eHR_Hit);
			pTo->OnPrintFightInfo(EFII_RenewRP, pFrom?pFrom->GetEntityID():0, pTo->GetEntityID(), pSkillInst, iOriginalValue, eATT_None, uExtraValue, 0);
		}
		if (iOriginalValue < 0)
		{
			pTo->OnPrintFightInfo(EFII_ConsumeRP, pFrom?pFrom->GetEntityID():0, pTo->GetEntityID(), pSkillInst, iOriginalValue, eATT_None, uExtraValue, 0);
		}
	}

	Property.LimitSet( Property.LimitGet()+iValue,pTo );
	//cout << "time = " << GetTickCount() << ", After TChangeAgileInfoValueMOP CurMP = " << (Property.LimitGet()) << endl;
}

template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
void TChangeAdderMOP<PropertyType,HolderType,pProperty>::
MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	int32 iValue = Value.GetInt32();
	ChangeAdder(pSkillInst,iValue,pTo);
}

template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
int32 TChangeAdderMOP<PropertyType,HolderType,pProperty>::
MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	*pValue=Arg.GetIntValue(pFrom, pTo, pSkillInst->GetLevelIndex());
	return eHR_Success;
}

template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
uint32 TChangeAdderMOP<PropertyType,HolderType,pProperty>::
Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{
	CValueData Value;
	uint32 uResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,eHR_Hit);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return	uResult;
}
template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
void TChangeAdderMOP<PropertyType,HolderType,pProperty>::
CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value)
{
	int32 iValue = Value.GetInt32();
	ChangeAdder(pSkillInst,-iValue,pTo);
}

template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
void TChangeAdderMOP<PropertyType,HolderType,pProperty>::
Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CValueData Value;
	MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value);
	CancelExec(pSkillInst,pFrom,pTo,Value);
}

template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
void TChangeAdderMOP<PropertyType,HolderType,pProperty>::
ChangeAdder(CSkillInstServer* pSkillInst,int32 iValue,CFighterDictator* pFighter)
{
	PropertyType& Property=(class_cast<HolderType*>(pFighter)->*pProperty);
	Property.SetAdder( pFighter,Property.GetAdder()+iValue );
	pSkillInst->SetPropertyAdderChange(Property,pFighter->GetEntityID(),iValue);
}


template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
void TChangeMultiplierMOP<PropertyType,HolderType,pProperty>::
MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	float fValue = Value.GetFloat();
	ChangeMultiplier(pSkillInst,fValue,pTo);
}

template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
int32 TChangeMultiplierMOP<PropertyType,HolderType,pProperty>::
MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	*pValue=float(Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex()));
	return eHR_Success;
}

template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
uint32 TChangeMultiplierMOP<PropertyType,HolderType,pProperty>::
Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue)
{
	CValueData Value;
	uint32 uResult=MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value);
	MagicOpExec(pSkillInst,pFrom,pTo,Value,eHR_Hit);
	if (MajorMopValue)
	{
		*MajorMopValue=Value.GetInt32();
	}		
	return	uResult;
}
template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
void TChangeMultiplierMOP<PropertyType,HolderType,pProperty>::
CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value)
{
	float fValue= Value.GetFloat();
	ChangeMultiplier(pSkillInst,-fValue,pTo);
}

template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
void TChangeMultiplierMOP<PropertyType,HolderType,pProperty>::
Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CValueData Value;
	MagicOpCalc(pSkillInst,Arg,pFrom,pTo,&Value);
	CancelExec(pSkillInst,pFrom,pTo,Value);
}

template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
void TChangeMultiplierMOP<PropertyType,HolderType,pProperty>::
ChangeMultiplier(CSkillInstServer* pSkillInst,float fValue,CFighterDictator* pFighter)
{
	PropertyType& Property=(class_cast<HolderType*>(pFighter)->*pProperty);
	Property.SetMultiplier( pFighter,Property.GetMultiplier()+fValue );
	pSkillInst->SetPropertyMultiplierChange(Property,pFighter->GetEntityID(),fValue);
}

//对所有的AgileValueMOP的类模板进行实例化

#define InstantiateAgileValueMOP(Name) \
	template class TChangeAgileInfoValueMOP<C##Name##Server,&CFighterAgileInfoServer::m_##Name>;\
	template class TChangeAdderMOP<C##Name##Server,CFighterAgileInfoServer,&CFighterAgileInfoServer::m_##Name>;\
	template class TChangeMultiplierMOP<C##Name##Server,CFighterAgileInfoServer,&CFighterAgileInfoServer::m_##Name>

InstantiateAgileValueMOP(HealthPoint);
InstantiateAgileValueMOP(ManaPoint);
InstantiateAgileValueMOP(ComboPoint);
InstantiateAgileValueMOP(RagePoint);
InstantiateAgileValueMOP(EnergyPoint);

#undef InstantiateAgileValueMOP


#define InstantiateDirectiorCalValueMOP(Name) \
	template class TChangeMultiplierMOP<C##Name,CFighterSyncToDirectorCalInfo,&CFighterSyncToDirectorCalInfo::m_##Name>

InstantiateDirectiorCalValueMOP(MPConsumeRate);
InstantiateDirectiorCalValueMOP(RPConsumeRate);
InstantiateDirectiorCalValueMOP(EPConsumeRate);

InstantiateDirectiorCalValueMOP(NatureMPConsumeRate);
InstantiateDirectiorCalValueMOP(DestructionMPConsumeRate);
InstantiateDirectiorCalValueMOP(EvilMPConsumeRate);

#undef InstantiateDirectiorCalValueMOP

//对所有的CalValueMOP的类模板进行实例化

#define InstantiateCalValueMOP(Name) \
	template class TChangeAdderMOP<C##Name,CFighterCalInfo,&CFighterCalInfo::m_##Name>;\
	template class TChangeMultiplierMOP<C##Name,CFighterCalInfo,&CFighterCalInfo::m_##Name>

InstantiateCalValueMOP(Defence);

InstantiateCalValueMOP(HPUpdateRate);
InstantiateCalValueMOP(MPUpdateRate);
InstantiateCalValueMOP(RPUpdateValue);
InstantiateCalValueMOP(EPUpdateValue);
InstantiateCalValueMOP(RPProduceRate);
InstantiateCalValueMOP(RevertPer);

InstantiateCalValueMOP(StrikeMultiValue);
InstantiateCalValueMOP(StrikeValue);

InstantiateCalValueMOP(BlockDamage);
InstantiateCalValueMOP(PhysicalDodgeValue);
InstantiateCalValueMOP(MagicDodgeValue);
InstantiateCalValueMOP(MagicHitValue);


InstantiateCalValueMOP(RunSpeed);
InstantiateCalValueMOP(WalkSpeed);
InstantiateCalValueMOP(ParryValue);
InstantiateCalValueMOP(ResilienceValue);
InstantiateCalValueMOP(StrikeResistanceValue);
InstantiateCalValueMOP(AccuratenessValue);

InstantiateCalValueMOP(PhysicalDPS);
InstantiateCalValueMOP(PunctureDamage);
InstantiateCalValueMOP(ChopDamage);
InstantiateCalValueMOP(BluntDamage);

InstantiateCalValueMOP(MagicDamageValue);
InstantiateCalValueMOP(NatureDamageValue);
InstantiateCalValueMOP(DestructionDamageValue);
InstantiateCalValueMOP(EvilDamageValue);

InstantiateCalValueMOP(NatureResistanceValue);
InstantiateCalValueMOP(DestructionResistanceValue);
InstantiateCalValueMOP(EvilResistanceValue);


InstantiateCalValueMOP(PhysicalDamage);
InstantiateCalValueMOP(AssistPhysicalDamage);
InstantiateCalValueMOP(MagicDamage);
InstantiateCalValueMOP(DOTDamage);

InstantiateCalValueMOP(MainHandMinWeaponDamage);
InstantiateCalValueMOP(MainHandMaxWeaponDamage);
InstantiateCalValueMOP(AssistantMinWeaponDamage);
InstantiateCalValueMOP(AssistantMaxWeaponDamage);

InstantiateCalValueMOP(PenetrationValue);
InstantiateCalValueMOP(ProtectionValue);

InstantiateCalValueMOP(NatureSmashValue);
InstantiateCalValueMOP(DestructionSmashValue);
InstantiateCalValueMOP(EvilSmashValue);
InstantiateCalValueMOP(DefenceSmashValue);

InstantiateCalValueMOP(MainHandWeaponInterval);
InstantiateCalValueMOP(AssistantWeaponInterval);
InstantiateCalValueMOP(MHWeaponIntervalExtraDamageRate);
InstantiateCalValueMOP(AHWeaponIntervalExtraDamageRate);

#undef InstantiateCalValueMOP

#define InstantiateCalMultiplierValueMOP(Name) \
	template class TChangeMultiplierMOP<C##Name,CFighterCalInfo,&CFighterCalInfo::m_##Name>



InstantiateCalMultiplierValueMOP(BlockRate);
InstantiateCalMultiplierValueMOP(MissRate);

InstantiateCalMultiplierValueMOP(ImmuneRate);
InstantiateCalMultiplierValueMOP(PunctureDamageImmuneRate);
InstantiateCalMultiplierValueMOP(ChopDamageImmuneRate);
InstantiateCalMultiplierValueMOP(BluntDamageImmuneRate);
InstantiateCalMultiplierValueMOP(NatureDamageImmuneRate);
InstantiateCalMultiplierValueMOP(DestructionDamageImmuneRate);
InstantiateCalMultiplierValueMOP(EvilDamageImmuneRate);
InstantiateCalMultiplierValueMOP(ControlDecreaseImmuneRate);
InstantiateCalMultiplierValueMOP(PauseDecreaseImmuneRate);
InstantiateCalMultiplierValueMOP(CripplingDecreaseImmuneRate);
InstantiateCalMultiplierValueMOP(DebuffDecreaseImmuneRate);
InstantiateCalMultiplierValueMOP(DOTDecreaseImmuneRate);
InstantiateCalMultiplierValueMOP(SpecialDecreaseImmuneRate);
InstantiateCalMultiplierValueMOP(CureImmuneRate);
InstantiateCalMultiplierValueMOP(MoveMagicImmuneRate);
InstantiateCalMultiplierValueMOP(NonTypePhysicsDamageImmuneRate);
InstantiateCalMultiplierValueMOP(NonTypeDamageImmuneRate);
InstantiateCalMultiplierValueMOP(NonTypeCureImmuneRate);
InstantiateCalMultiplierValueMOP(InterruptCastingProcessImmuneRate);

InstantiateCalMultiplierValueMOP(ResistCastingInterruptionRate);
InstantiateCalMultiplierValueMOP(PunctureDamageResistanceRate);
InstantiateCalMultiplierValueMOP(ChopDamageResistanceRate);
InstantiateCalMultiplierValueMOP(BluntDamageResistanceRate);

InstantiateCalMultiplierValueMOP(ExtraDOTDamageRate);
InstantiateCalMultiplierValueMOP(ExtraPunctureDamageRate);
InstantiateCalMultiplierValueMOP(ExtraChopDamageRate);
InstantiateCalMultiplierValueMOP(ExtraBluntDamageRate);
InstantiateCalMultiplierValueMOP(ExtraNatureDamageRate);
InstantiateCalMultiplierValueMOP(ExtraEvilDamageRate);
InstantiateCalMultiplierValueMOP(ExtraDestructionDamageRate);
InstantiateCalMultiplierValueMOP(ExtraBowDamageRate);
InstantiateCalMultiplierValueMOP(ExtraCrossBowDamageRate);
InstantiateCalMultiplierValueMOP(ExtraTwohandWeaponDamageRate);
InstantiateCalMultiplierValueMOP(ExtraPolearmDamageRate);
InstantiateCalMultiplierValueMOP(ExtraPaladinWeaponDamageRate);
InstantiateCalMultiplierValueMOP(ExtraAssistWeaponDamageRate);
InstantiateCalMultiplierValueMOP(ExtraLongDistDamageRate);
InstantiateCalMultiplierValueMOP(ExtraShortDistDamageRate);
InstantiateCalMultiplierValueMOP(LongDistWeaponDamageRate);
InstantiateCalMultiplierValueMOP(DamageDecsRate);
InstantiateCalMultiplierValueMOP(CastingProcessTimeRatio);
InstantiateCalMultiplierValueMOP(ExtraCureValueRate);
InstantiateCalMultiplierValueMOP(ExtraBeCuredValueRate);
InstantiateCalMultiplierValueMOP(ControlDecreaseResistRate);
InstantiateCalMultiplierValueMOP(PauseDecreaseResistRate);
InstantiateCalMultiplierValueMOP(CripplingDecreaseResistRate);
InstantiateCalMultiplierValueMOP(DebuffDecreaseResistRate);
InstantiateCalMultiplierValueMOP(DOTDecreaseResistRate);
InstantiateCalMultiplierValueMOP(SpecialDecreaseResistRate);
InstantiateCalMultiplierValueMOP(ControlDecreaseTimeRate);
InstantiateCalMultiplierValueMOP(PauseDecreaseTimeRate);
InstantiateCalMultiplierValueMOP(CripplingDecreaseTimeRate);
InstantiateCalMultiplierValueMOP(SpecialDecreaseTimeRate);
InstantiateCalMultiplierValueMOP(ExtraPhysicDefenceRate);
InstantiateCalMultiplierValueMOP(ExtraPhysicDodgeRate);
InstantiateCalMultiplierValueMOP(ExtraParryRate);
InstantiateCalMultiplierValueMOP(ExtraStrikeRate);
InstantiateCalMultiplierValueMOP(ExtraMagicDodgeRate);
InstantiateCalMultiplierValueMOP(ExtraMagicResistanceRate);
InstantiateCalMultiplierValueMOP(ExtraNatureResistanceRate);
InstantiateCalMultiplierValueMOP(ExtraDestructionResistanceRate);
InstantiateCalMultiplierValueMOP(ExtraEvilResistanceRate);
InstantiateCalMultiplierValueMOP(ExtraCompleteResistanceRate);

#undef InstantiateCalMultiplierValueMOP

void SetMainHandIntervalPercent(CFighterDictator* pFighter, float fSpeed)
{
	pFighter->m_MainHandWeaponInterval.SetMultiplier(pFighter,fSpeed);
}

void SetOffHandIntervalPercent(CFighterDictator* pFighter, float fSpeed)
{
	pFighter->m_AssistantWeaponInterval.SetMultiplier(pFighter,fSpeed);
}


void SetMainHandSpeed(CFighterDictator* pFighter, float fSpeed)
{
	pFighter->m_MainHandWeaponInterval.SetAdder(pFighter,fSpeed);
	pFighter->m_MHWeaponIntervalExtraDamageRate.SetAdder(pFighter,fSpeed);
}
void SetOffHandSpeed(CFighterDictator* pFighter, float fSpeed)
{
	pFighter->m_AssistantWeaponInterval.SetAdder(pFighter,fSpeed);
	pFighter->m_AHWeaponIntervalExtraDamageRate.SetAdder(pFighter,fSpeed);
}

void SetRunSpeed(CFighterDictator* pFighter, float fSpeed)
{
	pFighter->m_RunSpeed.SetAdder(pFighter,fSpeed);
}

void SetWalkSpeed(CFighterDictator* pFighter, float fSpeed)
{
	pFighter->m_WalkSpeed.SetAdder(pFighter,fSpeed);
}

void SetRunSpeedPercent(CFighterDictator* pFighter, float fPercent)
{
	pFighter->m_RunSpeed.SetMultiplier(pFighter, fPercent);
}

void SetWalkSpeedPercent(CFighterDictator* pFighter, float fPercent)
{
	pFighter->m_WalkSpeed.SetMultiplier(pFighter, fPercent);
}

void SetArmorValueRateMultiplier(CFighterDictator* pFighter,float fBluntRate,float fPunctureRate,float fChopRate)
{
	pFighter->m_BluntDamageResistanceRate.SetMultiplier(pFighter,fBluntRate);
	pFighter->m_PunctureDamageResistanceRate.SetMultiplier(pFighter,fPunctureRate);
	pFighter->m_ChopDamageResistanceRate.SetMultiplier(pFighter,fChopRate);
}

void SetRPConsumeRate(CFighterDictator* pFighter, float fMulti)
{
	pFighter->m_RPConsumeRate.SetMultiplier(pFighter, fMulti);
}

void CChangeSelfMagicDamageAdderMOP::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	int32 iValue = Value.GetInt32();
	ChangeAdder(pSkillInst,iValue,pFrom);
}


void CChangeSelfMagicDamageAdderMOP::CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value)
{
	int32 iValue = Value.GetInt32();
	ChangeAdder(pSkillInst,-iValue,pFrom);
}

