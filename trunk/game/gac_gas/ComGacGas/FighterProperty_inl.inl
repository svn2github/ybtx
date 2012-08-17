#pragma once
#include "FighterProperty.inl"
#include "BaseMath.h"
#include <iostream>

using namespace std;

template<uint32 uId,typename AgileClass,typename ValueType>
template<typename FighterInfoClass>
void TBaseAProperty<uId,AgileClass,ValueType>::SetAdder(const FighterInfoClass* pInfo,ValueType Value)
{
	//cout << "SetAdder\t" << uId << "\t" << Value << endl;

	if(this->GetProperty() == Value)
		return;
	this->SetProperty(Value);
	static_cast<AgileClass*>(this)->OnAdderSet(pInfo);
}


template<uint32 uId,typename AgileClass,typename ValueType>
template<typename FighterInfoClass>
void TBaseAProperty<uId,AgileClass,ValueType>::OnAdderSet(const FighterInfoClass* pInfo)const
{
	pInfo->OnCalValueChanged( uId * ePFT_Count + ePFT_Adder, float(this->GetAdder()) );
}



template<uint32 uId,typename AgileClass>
template<typename FighterInfoClass>
void TBaseMProperty<uId,AgileClass>::SetMultiplier(const FighterInfoClass* pInfo,float fMultiplier)
{
	//cout << "SetMultiplier\t" << uId << "\t" << fMultiplier<< endl;
	if(abs(fMultiplier - this->GetProperty()) < 0.000001f) 
		return;
	this->SetProperty(fMultiplier);
	static_cast<AgileClass*>(this)->OnMultiplierSet(pInfo);
}



template<uint32 uId,typename AgileClass>
template<typename FighterInfoClass>
void TBaseMProperty<uId,AgileClass>::OnMultiplierSet(const FighterInfoClass* pInfo)const
{
	pInfo->OnCalValueChanged( uId * ePFT_Count + ePFT_Multiplier, this->GetMultiplier() );
}



template<
typename ImpClass,
uint32 uId,
template<uint32 uId,typename AgileClass,const uint32 CStaticAttribs::*OriginValue>class TAnyIntegerBProperty,
const uint32 CStaticAttribs::*OriginValue
>
template<typename FighterInfoClass>
void TAgileProperty<ImpClass,uId,TAnyIntegerBProperty,OriginValue>::LimitSet(int32 Value,const FighterInfoClass* pInfo)
{
	Value = limit2( Value , 0 , int32(Get(pInfo)) );

	//cout << "LimitSet\t" << uId << "\t" << Value << endl;
	if(Value == int32(m_Value))
		return;

	m_Value=Value;

	static_cast<ImpClass*>(this)->OnLimitSet(Value, pInfo);

	pInfo->OnAgileValueChanged( uId * ePFT_Count + ePFT_Agile, float(m_Value) );
}

template<
typename ImpClass,
uint32 uId,
template<uint32 uId,typename AgileClass,const uint32 CStaticAttribs::*OriginValue>class TAnyIntegerBProperty,
const uint32 CStaticAttribs::*OriginValue
>
template<typename FighterInfoClass>
void TAgileProperty<ImpClass,uId,TAnyIntegerBProperty,OriginValue>::FullFill(const FighterInfoClass* pInfo)
{
	LimitSet(Get(pInfo),pInfo);
}


template<
typename ImpClass,
uint32 uId,
template<uint32 uId,typename AgileClass,const uint32 CStaticAttribs::*OriginValue>class TAnyIntegerBProperty,
const uint32 CStaticAttribs::*OriginValue
>
template<typename FighterInfoClass>
void TAgileProperty<ImpClass,uId,TAnyIntegerBProperty,OriginValue>::OnAdderSet(const FighterInfoClass* pInfo)
{
	pInfo->OnAgileValueChanged( uId * ePFT_Count + ePFT_Adder, float(this->GetAdder()) );
	LimitSet(LimitGet(),pInfo);
}

template<
typename ImpClass,
uint32 uId,
template<uint32 uId,typename AgileClass,const uint32 CStaticAttribs::*OriginValue>class TAnyIntegerBProperty,
const uint32 CStaticAttribs::*OriginValue
>
template<typename FighterInfoClass>
void TAgileProperty<ImpClass,uId,TAnyIntegerBProperty,OriginValue>::OnMultiplierSet(const FighterInfoClass* pInfo)
{
	pInfo->OnAgileValueChanged( uId * ePFT_Count + ePFT_Multiplier, this->GetMultiplier() );
	LimitSet(LimitGet(),pInfo);
}

template<
typename ImpClass,
uint32 uId,
template<uint32 uId,typename AgileClass,const uint32 CStaticAttribs::*OriginValue>class TAnyIntegerBProperty,
const uint32 CStaticAttribs::*OriginValue
>
template<typename FighterInfoClass>
void TAgileProperty<ImpClass,uId,TAnyIntegerBProperty,OriginValue>::OnLimitSet(int32 iValue,const FighterInfoClass* pInfo)
{
}
