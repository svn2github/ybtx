#pragma once
#include "CStaticAttribs.h"

class CFighterBaseInfo;
class CFighterDictator;

template<typename ValueType>struct TAdderType{};
template<>struct TAdderType<uint64>{typedef int64	Type;};
template<>struct TAdderType<uint32>{typedef int32	Type;};
template<>struct TAdderType<uint16>{typedef int16	Type;};
template<>struct TAdderType<int64>{typedef int64	Type;};
template<>struct TAdderType<int32>{typedef int32	Type;};
template<>struct TAdderType<int16>{typedef int16	Type;};
template<>struct TAdderType<float>{typedef float	Type;};

template<typename ValueType>
void RoundValue(ValueType& value, double fInitialValue);


template<uint32 uId,typename ValueType>
class TBaseProperty
{
protected:
	typedef typename TAdderType<ValueType>::Type	Property_t;

	TBaseProperty();

	void SetProperty(Property_t Property);
	Property_t GetProperty()const;
private:
	Property_t	m_Property;
};

template<uint32 uId>
class TBaseProperty<uId,float>
{
protected:
	explicit TBaseProperty();
	void SetProperty(float Property);
	float GetProperty()const;
private:
	float	m_Property;
};

//没有基础值，完全由魔法效果和装备添加出来的属性
//Adder
template<uint32 uId,typename AgileClass,typename ValueType>
class TBaseAProperty
	:public TBaseProperty<uId,ValueType>
{
	template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
	friend class TAdderAdapter;
	template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
	friend class TChangeAdderMOP;
	template<typename Property>
	friend class CPropertyAdderHolder;
	friend class CServantData;
	friend void SetMainHandSpeed(CFighterDictator* pFighter, float fSpeed);
	friend void SetOffHandSpeed(CFighterDictator* pFighter, float fSpeed);
	friend void SetRunSpeed(CFighterDictator* pFighter, float fSpeed);
	friend void SetWalkSpeed(CFighterDictator* pFighter, float fSpeed);
public:
	typedef ValueType	Value_t;
	ValueType GetAdder()const;
protected:
	template<typename FighterInfoClass>
	void SetAdder(const FighterInfoClass* pInfo,ValueType Adder);
private:
	template<typename FighterInfoClass>
	void OnAdderSet(const FighterInfoClass*)const;
};

template<uint32 uId,typename ValueType>
class TAProperty
	:public TBaseAProperty<uId,TAProperty<uId,ValueType>,ValueType>
{
public:
	ValueType Get()const;
	ValueType Get(const CFighterBaseInfo* pInfo)const;
};

//没有基础值，完全由魔法效果和装备添加出来的属性
//Multiplier
template<uint32 uId,typename AgileClass>
class TBaseMProperty
	:public TBaseProperty<uId,float>
{
	template<typename PropertyType,typename HolderType,PropertyType HolderType::*pProperty>
	friend class TChangeMultiplierMOP;
	template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
	friend class TMultiplierAdapter;
	template<typename Property>
	friend class CPropertyMultiplierHolder;
	friend void SetMainHandIntervalPercent(CFighterDictator* pFighter, float fSpeed);
	friend void SetOffHandIntervalPercent(CFighterDictator* pFighter, float fSpeed);
	friend void SetRunSpeedPercent(CFighterDictator* pFighter, float fPercent);
	friend void SetWalkSpeedPercent(CFighterDictator* pFighter, float fPercent);
	friend void SetArmorValueRateMultiplier(CFighterDictator* pFighter,float fBluntRate,float fPunctureRate,float fChopRate);
	friend void SetRPConsumeRate(CFighterDictator* pFighter, float fMulti);
public:
	typedef float	Value_t;
	TBaseMProperty();
	float GetMultiplier()const;
protected:
	template<typename FighterInfoClass>
	void SetMultiplier(const FighterInfoClass* pInfo,float fMultiplier);
private:
	template<typename FighterInfoClass>
	void OnMultiplierSet(const FighterInfoClass*)const;
};

template<uint32 uId,typename ValueType>
class TMProperty
	:public TBaseMProperty<uId,TMProperty<uId,ValueType> >
{
public:
	ValueType Get()const;
	ValueType Get(const CFighterBaseInfo* pInfo)const;
};

//没有基础值，完全由魔法效果和装备添加出来的属性，并且增加一个倍数因子，可用于“提高某属性1%”这类需求
//Adder
//Multiplier
template<uint32 uId,typename AgileClass,typename ValueType>
class TBaseAMProperty
	:public TBaseAProperty<uId,AgileClass,ValueType>
	,public TBaseMProperty<uId,AgileClass>
{
public:
	typedef ValueType	Value_t;
	ValueType Get()const;
	ValueType Get(const CFighterBaseInfo* pInfo)const;
};

template<uint32 uId,typename ValueType>
class TAMProperty
	:public TBaseAMProperty<uId,TAMProperty<uId,ValueType>,ValueType>
{
};


//有基础值，属性值是基础值加上由魔法效果和装备添加值的和
//Adder
//Base
template<
uint32 uId,typename AgileClass,typename ValueType,
const ValueType CStaticAttribs::*OriginValue
>
class TBaseABProperty
	:public TBaseAProperty<uId,AgileClass,ValueType>
{
public:
	ValueType Get(const CFighterBaseInfo* pThis)const;
};

template<
uint32 uId,typename ValueType,
const ValueType CStaticAttribs::*OriginValue
>
class TABProperty
	:public TBaseABProperty<uId,TABProperty<uId,ValueType,OriginValue>,ValueType,OriginValue>
{
};

//有基础值，属性值是基础值乘上乘法因子
//Multiplier
//Base
template<
uint32 uId,typename AgileClass,typename ValueType,
const ValueType CStaticAttribs::*OriginValue
>
class TBaseMBProperty
	:public TBaseMProperty<uId,AgileClass>
{
public:
	ValueType Get(const CFighterBaseInfo* pThis)const;
};

template<
uint32 uId,typename ValueType,
const ValueType CStaticAttribs::*OriginValue
>
class TMBProperty
	:public TBaseMBProperty<uId,TMBProperty<uId,ValueType,OriginValue>,ValueType,OriginValue>
{
};

//有基础值，属性值是基础值加上由魔法效果和装备添加值的和，再乘上乘法因子
//Adder
//Multiplier
//Base
template<
uint32 uId,typename AgileClass,typename ValueType,
const ValueType CStaticAttribs::*OriginValue
>
class TBaseAMBProperty
	:public TBaseAMProperty<uId,AgileClass,ValueType>
{
public:
	ValueType Get(const CFighterBaseInfo* pThis)const;
};

template<
uint32 uId,typename AgileClass,
const uint32 CStaticAttribs::*OriginValue
>
class TBaseIntegerAMBProperty
	:public TBaseAMBProperty<uId,AgileClass,uint32,OriginValue>
{
};

template<
uint32 uId,typename ValueType,
const ValueType CStaticAttribs::*OriginValue
>
class TAMBProperty
	:public TBaseAMBProperty<uId,TAMBProperty<uId,ValueType,OriginValue>,ValueType,OriginValue>
{
};

//具有上限的易变值（例如血、蓝），以另外一个具有基础值的属性作为上限，这些值必须是整形
template<
typename ImpClass,
uint32 uId,
template<uint32 uId,typename AgileClass,const uint32 CStaticAttribs::*OriginValue>class TAnyIntegerBProperty,
const uint32 CStaticAttribs::*OriginValue
>
class TAgileProperty
	:public TAnyIntegerBProperty<uId,TAgileProperty<ImpClass,uId,TAnyIntegerBProperty,OriginValue>,OriginValue>
{
public:
	TAgileProperty();
	
	uint32 LimitGet()const;
	template<typename FighterInfoClass>
	void LimitSet(int32 iValue,const FighterInfoClass* pInfo);
	template<typename FighterInfoClass>
	void FullFill(const FighterInfoClass* pInfo);
	template<typename FighterInfoClass>
	void OnLimitSet(int32 iValue,const FighterInfoClass* pInfo);

private:
	friend class TBaseAProperty<uId,TAgileProperty<ImpClass,uId,TAnyIntegerBProperty,OriginValue>,uint32>;
	friend class TBaseMProperty<uId,TAgileProperty<ImpClass,uId,TAnyIntegerBProperty,OriginValue> >;

	template<typename FighterInfoClass>
	void OnAdderSet(const FighterInfoClass* pInfo);
	template<typename FighterInfoClass>
	void OnMultiplierSet(const FighterInfoClass* pInfo);

	uint32	m_Value;
};
