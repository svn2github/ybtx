#pragma once
#include "MagicConds_TestValue.h"
#include "CFighterDictator.h"
#include "CMagicOpArg.h"
#include "CSkillInstServer.h"
#include "TPropertyHolder_inl.inl"
#include "CoreCommon.h"


template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
uint32 TAgileValueLesserCond<PropertyType,pProperty>::
Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value=MagicCondCalc(pSkillInst,Arg,pFrom,pTo);
	bool bResult= MagicCondExec(pSkillInst,pFrom,pTo,Value);
	if (bResult)
		return eDSR_Success;
	else
		return eDSR_Fail;
}

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
CValueData TAgileValueLesserCond<PropertyType,pProperty>::
MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	typedef typename PropertyType::Value_t					ValueType;
	ValueType ArgValue=CMagicOpArgGetter<ValueType>::Acquire(&Arg,pTo,pSkillInst,(pSkillInst->GetSkillLevel()-1));
	CValueData Value;
	Value.SetBy<ValueType>(ArgValue);
	return Value;
}

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
bool TAgileValueLesserCond<PropertyType,pProperty>::
MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const
{
	typedef typename PropertyType::Value_t					ValueType;
	ValueType ArgValue=Value.GetBy<ValueType>();
	return (pTo->*pProperty).LimitGet()<ArgValue && pTo->CppIsAlive();
}

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
uint32 TAgileValueGreaterOrEqualCond<PropertyType,pProperty>::
Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value=MagicCondCalc(pSkillInst,Arg,pFrom,pTo);
	bool bResult=MagicCondExec(pSkillInst,pFrom,pTo,Value);
	if (bResult)
		return eDSR_Success;
	else
	{		
		string sRawName;
#ifdef _WIN32
		sRawName=typeid(PropertyType).raw_name();
		sRawName = sRawName.substr(4,sRawName.size()-6);
#else
		sRawName=typeid(PropertyType).name();
		sRawName = sRawName.substr(2,sRawName.size()-2);
#endif

		if(sRawName.compare("CHealthPointServer") == 0)
			return eDSR_HealthPointIsLesser;
		else if(sRawName.compare("CManaPointServer") == 0)
			return eDSR_ManaPointIsLesser;
		else if(sRawName.compare("CEnergyPointServer") == 0)
			return eDSR_EnergyPointIsLesser;
		else if(sRawName.compare("CRagePointServer") == 0)
			return eDSR_RagePointIsLesser;
		else if(sRawName.compare("CComboPointServer") == 0)
			return eDSR_ComboPointIsLesser;
		else
			return eDSR_Fail;
	}
}

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
CValueData TAgileValueGreaterOrEqualCond<PropertyType,pProperty>::
MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	typedef typename PropertyType::Value_t					ValueType;
	ValueType ArgValue=CMagicOpArgGetter<ValueType>::Acquire(&Arg,pTo,pSkillInst,(pSkillInst->GetSkillLevel()-1));
	CValueData Value;// = ArgValue;
	Value.SetBy<ValueType>(ArgValue);
	return Value;
}

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
bool TAgileValueGreaterOrEqualCond<PropertyType,pProperty>::
MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const

{
	typedef typename PropertyType::Value_t					ValueType;
	ValueType ArgValue=Value.GetBy<ValueType>();

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
		ArgValue = (int32)(ArgValue * pTo->Get(&CFighterSyncToDirectorCalInfo::m_MPConsumeRate));
		if (pSkillInst->GetAttackType() == eATT_Nature)
		{
			ArgValue = (int32)(ArgValue * pTo->Get(&CFighterSyncToDirectorCalInfo::m_NatureMPConsumeRate));
		}
		else if (pSkillInst->GetAttackType() == eATT_Destroy)
		{
			ArgValue = (int32)(ArgValue * pTo->Get(&CFighterSyncToDirectorCalInfo::m_DestructionMPConsumeRate));
		}
		else if (pSkillInst->GetAttackType() == eATT_Evil)
		{
			ArgValue = (int32)(ArgValue * pTo->Get(&CFighterSyncToDirectorCalInfo::m_EvilMPConsumeRate));
		}
	}
	else if (sRawName.compare("CRagePointServer") == 0)
	{	
		ArgValue = (int32)(ArgValue * pTo->Get(&CFighterSyncToDirectorCalInfo::m_RPConsumeRate));
	}
	else if (sRawName.compare("CEnergyPointServer") == 0)
	{
		ArgValue = (int32)(ArgValue * pTo->Get(&CFighterSyncToDirectorCalInfo::m_EPConsumeRate));
	}
	return (pTo->*pProperty).LimitGet()>=ArgValue;
}

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
uint32 TAgileMaxValueLesserCond<PropertyType,pProperty>::
Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value=MagicCondCalc(pSkillInst,Arg,pFrom,pTo);
	bool bResult= MagicCondExec(pSkillInst,pFrom,pTo,Value);
	if (bResult)
		return eDSR_Success;
	else
		return eDSR_Fail;
}

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
CValueData TAgileMaxValueLesserCond<PropertyType,pProperty>::
MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	typedef typename PropertyType::Value_t					ValueType;
	ValueType ArgValue=CMagicOpArgGetter<ValueType>::Acquire(&Arg,pTo,pSkillInst,(pSkillInst->GetSkillLevel()-1));
	CValueData Value;
	Value.SetBy<ValueType>(ArgValue);
	return Value;
}

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
bool TAgileMaxValueLesserCond<PropertyType,pProperty>::
MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const
{
	typedef typename PropertyType::Value_t					ValueType;
	ValueType ArgValue=Value.GetBy<ValueType>();
	return (pTo->*pProperty).Get(pTo)<ArgValue && pTo->CppIsAlive();
}

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
uint32 TAgileMaxValueGreaterOrEqualCond<PropertyType,pProperty>::
Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value=MagicCondCalc(pSkillInst,Arg,pFrom,pTo);
	bool bResult=MagicCondExec(pSkillInst,pFrom,pTo,Value);
	if (bResult)
		return eDSR_Success;
	else
	{		
//		string sRawName;
//#ifdef _WIN32
//		sRawName=typeid(PropertyType).raw_name();
//		sRawName = sRawName.substr(4,sRawName.size()-6);
//#else
//		sRawName=typeid(PropertyType).name();
//		sRawName = sRawName.substr(2,sRawName.size()-2);
//#endif
//
//		if(sRawName.compare("CHealthPointServer") == 0)
//			return eDSR_HealthPointIsLesser;
//		else if(sRawName.compare("CManaPointServer") == 0)
//			return eDSR_ManaPointIsLesser;
//		else if(sRawName.compare("CEnergyPointServer") == 0)
//			return eDSR_EnergyPointIsLesser;
//		else if(sRawName.compare("CRagePointServer") == 0)
//			return eDSR_RagePointIsLesser;
//		else if(sRawName.compare("CComboPointServer") == 0)
//			return eDSR_ComboPointIsLesser;
//		else
			return eDSR_Fail;
	}
}

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
CValueData TAgileMaxValueGreaterOrEqualCond<PropertyType,pProperty>::
MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	typedef typename PropertyType::Value_t					ValueType;
	ValueType ArgValue=CMagicOpArgGetter<ValueType>::Acquire(&Arg,pTo,pSkillInst,(pSkillInst->GetSkillLevel()-1));
	CValueData Value;// = ArgValue;
	Value.SetBy<ValueType>(ArgValue);
	return Value;
}

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
bool TAgileMaxValueGreaterOrEqualCond<PropertyType,pProperty>::
MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const

{
	typedef typename PropertyType::Value_t					ValueType;
	ValueType ArgValue=Value.GetBy<ValueType>();
	//cout << "AgileMaxValue " << (pTo->*pProperty).Get(pTo) << " >= " << ArgValue << endl;
	return (pTo->*pProperty).Get(pTo)>=ArgValue;
}



template<typename PropertyType,PropertyType CFighterCalInfo::*pProperty>
uint32 TCalValueLesserCond<PropertyType,pProperty>::
Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value=MagicCondCalc(pSkillInst,Arg,pFrom,pTo);
	bool bResult= MagicCondExec(pSkillInst,pFrom,pTo,Value);
	if (bResult)
		return eDSR_Success;
	else
		return eDSR_Fail;
}

template<typename PropertyType,PropertyType CFighterCalInfo::*pProperty>
CValueData TCalValueLesserCond<PropertyType,pProperty>::
MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	typedef typename PropertyType::Value_t					ValueType;
	ValueType ArgValue=CMagicOpArgGetter<ValueType>::Acquire(&Arg,pTo,pSkillInst,(pSkillInst->GetSkillLevel()-1));
	CValueData Value;
	Value.SetBy<ValueType>(ArgValue);
	return Value;
}

template<typename PropertyType,PropertyType CFighterCalInfo::*pProperty>
bool TCalValueLesserCond<PropertyType,pProperty>::
MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const
{
	typedef typename PropertyType::Value_t					ValueType;
	ValueType ArgValue=Value.GetBy<ValueType>();
	return (pTo->*pProperty).Get(pTo)<ArgValue && pTo->CppIsAlive();
}

template<typename PropertyType,PropertyType CFighterCalInfo::*pProperty>
uint32 TCalValueGreaterOrEqualCond<PropertyType,pProperty>::
Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value=MagicCondCalc(pSkillInst,Arg,pFrom,pTo);
	bool bResult=MagicCondExec(pSkillInst,pFrom,pTo,Value);
	if (bResult)
	{
		return eDSR_Success;
	}
	else
	{		
		return eDSR_Fail;
	}
}

template<typename PropertyType,PropertyType CFighterCalInfo::*pProperty>
CValueData TCalValueGreaterOrEqualCond<PropertyType,pProperty>::
MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	typedef typename PropertyType::Value_t					ValueType;
	ValueType ArgValue=CMagicOpArgGetter<ValueType>::Acquire(&Arg,pTo,pSkillInst,(pSkillInst->GetSkillLevel()-1));
	CValueData Value;
	Value.SetBy<ValueType>(ArgValue);
	return Value;
}

template<typename PropertyType,PropertyType CFighterCalInfo::*pProperty>
bool TCalValueGreaterOrEqualCond<PropertyType,pProperty>::
MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const

{
	typedef typename PropertyType::Value_t					ValueType;
	ValueType ArgValue=Value.GetBy<ValueType>();
	return (pTo->*pProperty).Get(pTo)>=ArgValue;
}






//对所有的AgileValueMCOND的类模板进行实例化

#define InstantiateAgileValueMCOND(Name) \
	template class TAgileValueLesserCond<C##Name##Server,&CFighterAgileInfoServer::m_##Name>;\
	template class TAgileValueGreaterOrEqualCond<C##Name##Server,&CFighterAgileInfoServer::m_##Name>;

InstantiateAgileValueMCOND(HealthPoint)
InstantiateAgileValueMCOND(ManaPoint)
InstantiateAgileValueMCOND(ComboPoint)
InstantiateAgileValueMCOND(RagePoint)
InstantiateAgileValueMCOND(EnergyPoint)

#undef InstantiateAgileValueMCOND

#define InstantiateCalValueMCOND(Name) \
	template class TCalValueLesserCond<C##Name,&CFighterCalInfo::m_##Name>;\
	template class TCalValueGreaterOrEqualCond<C##Name,&CFighterCalInfo::m_##Name>;

InstantiateCalValueMCOND(Defence)
InstantiateCalValueMCOND(NatureResistanceValue)
InstantiateCalValueMCOND(DestructionResistanceValue)
InstantiateCalValueMCOND(EvilResistanceValue)
#undef InstantiateCalValueMCOND
