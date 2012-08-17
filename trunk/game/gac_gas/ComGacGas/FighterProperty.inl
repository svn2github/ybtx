#pragma once
#include "FighterProperty.h"
#include "CStaticAttribMgr.h"
#include "TypeLimit.h"
#include "CFighterBaseInfo.h"

template<typename ValueType>
inline void RoundValue(ValueType& value, double fInitialValue)
{
	if(fInitialValue > 0)
	{
		if((fInitialValue - value) > 0.5f)
		{
			value = ValueType(fInitialValue + 0.5f);
		}
	}
	else
	{
		if((fInitialValue - value) < -0.5f)
		{
			value = ValueType(fInitialValue - 0.5f);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
//TBaseProperty
//////////////////////////////////////////////////////////////////////////////////////
template<uint32 uId,typename ValueType>
inline TBaseProperty<uId,ValueType>::TBaseProperty()
:m_Property(0)
{
}

template<uint32 uId,typename ValueType>
inline void TBaseProperty<uId,ValueType>::SetProperty(Property_t Property)
{
	m_Property=Property;
}

template<uint32 uId,typename ValueType>
inline typename TBaseProperty<uId,ValueType>::Property_t TBaseProperty<uId,ValueType>::GetProperty()const
{
	return m_Property;
}

template<uint32 uId>
inline TBaseProperty<uId,float>::TBaseProperty()
:m_Property(0.0f)
{
}

template<uint32 uId>
inline void TBaseProperty<uId,float>::SetProperty(float Property)
{
	if(Property < 0.0f)
	{
		Property = float(int32(Property * 10000 - 0.5f)) / 10000.0f;
	}
	else
	{
		Property = float(int32(Property * 10000 + 0.5f)) / 10000.0f;
	}

	m_Property=Property;
}

template<uint32 uId>
inline float TBaseProperty<uId,float>::GetProperty()const
{
	return m_Property;
}

//////////////////////////////////////////////////////////////////////////////////////
//TBaseAProperty
//////////////////////////////////////////////////////////////////////////////////////
template<uint32 uId,typename AgileClass,typename ValueType>
inline ValueType TBaseAProperty<uId,AgileClass,ValueType>::GetAdder()const
{
	return this->GetProperty();
}

template<uint32 uId,typename ValueType>
inline ValueType TAProperty<uId,ValueType>::Get()const
{
	return this->GetAdder();
}

template<uint32 uId,typename ValueType>
inline ValueType TAProperty<uId,ValueType>::Get(const CFighterBaseInfo* pInfo)const
{
	return Get();
}
//////////////////////////////////////////////////////////////////////////////////////
//TBaseMProperty
//////////////////////////////////////////////////////////////////////////////////////
template<uint32 uId,typename AgileClass>
TBaseMProperty<uId,AgileClass>::TBaseMProperty()
{
	this->SetProperty(1.0f);
}

template<uint32 uId,typename AgileClass>
inline float TBaseMProperty<uId,AgileClass>::GetMultiplier()const
{
	return this->GetProperty();
}

template<uint32 uId,typename ValueType>
inline ValueType TMProperty<uId,ValueType>::Get()const

{
	return (ValueType)this->GetMultiplier();
}

template<uint32 uId,typename ValueType>
inline ValueType TMProperty<uId,ValueType>::Get(const CFighterBaseInfo* pInfo)const
{
	return Get();
}
//////////////////////////////////////////////////////////////////////////////////////
//TBaseAMProperty
//////////////////////////////////////////////////////////////////////////////////////
template<uint32 uId,typename AgileClass,typename ValueType>
inline ValueType TBaseAMProperty<uId,AgileClass,ValueType>::Get()const
{
	double fInitialValue = this->GetAdder() * this->GetMultiplier();
	ValueType value;
	ValueTypeToRetType(fInitialValue,value);
	RoundValue(value, fInitialValue);
	return value;
}

template<uint32 uId,typename AgileClass,typename ValueType>
inline ValueType TBaseAMProperty<uId,AgileClass,ValueType>::Get(const CFighterBaseInfo* pInfo)const
{
	return Get();
}
//////////////////////////////////////////////////////////////////////////////////////
//TBaseABProperty
//////////////////////////////////////////////////////////////////////////////////////
template<uint32 uId,typename AgileClass,typename ValueType,const ValueType CStaticAttribs::*OriginValue>
inline ValueType TBaseABProperty<uId,AgileClass,ValueType,OriginValue>::Get(const CFighterBaseInfo* pThis)const
{
	const CStaticAttribs& Attribs=
		CStaticAttribMgr::Inst().GetStaticAttribs(pThis->CppGetClass(),pThis->CppGetLevel());
	
	ValueType Value=Attribs.*OriginValue;
	ValueType Adder=TBaseAMProperty<uId,AgileClass,ValueType>::GetAdder();
	ValueType value;
	ValueTypeToRetType((Value +Adder),value);
	return value;
}

//////////////////////////////////////////////////////////////////////////////////////
//TBaseAMBProperty
//////////////////////////////////////////////////////////////////////////////////////
template<uint32 uId,typename AgileClass,typename ValueType,const ValueType CStaticAttribs::*OriginValue>
inline ValueType TBaseAMBProperty<uId,AgileClass,ValueType,OriginValue>::Get(const CFighterBaseInfo* pThis)const
{
	const CStaticAttribs& Attribs=
		CStaticAttribMgr::Inst().GetStaticAttribs(pThis->CppGetClass(),pThis->CppGetLevel());
	
	ValueType Value=Attribs.*OriginValue;
	ValueType Adder=TBaseAMProperty<uId,AgileClass,ValueType>::GetAdder();
	ValueType Temp;
	ValueTypeToRetType((Value +Adder),Temp);
	double fMultiplier=TBaseAMProperty<uId,AgileClass,ValueType>::GetMultiplier();
	double fInitialValue = Temp * fMultiplier;
	ValueType value;
	ValueTypeToRetType(fInitialValue,value);
	RoundValue(value,fInitialValue);
	return value;
}

//////////////////////////////////////////////////////////////////////////////////////
//TAgileProperty
//////////////////////////////////////////////////////////////////////////////////////
template<
typename ImpClass,
uint32 uId,
template<uint32 uId,typename AgileClass,const uint32 CStaticAttribs::*OriginValue>class TAnyIntegerBProperty,
const uint32 CStaticAttribs::*OriginValue
>
inline TAgileProperty<ImpClass,uId,TAnyIntegerBProperty,OriginValue>::TAgileProperty()
:m_Value(0)
{
}

template<
typename ImpClass,
uint32 uId,
template<uint32 uId,typename AgileClass,const uint32 CStaticAttribs::*OriginValue>class TAnyIntegerBProperty,
const uint32 CStaticAttribs::*OriginValue
>
inline uint32 TAgileProperty<ImpClass,uId,TAnyIntegerBProperty,OriginValue>::LimitGet()const
{
	return m_Value;
}
