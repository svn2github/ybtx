#pragma once
#include "PropertyAdapter.h"
#include "TPropertyHolder_inl.inl"
#include "FighterProperty_inl.inl"

template<typename Class>
std::string GetPropertyClassName()
{
	string sRawName;

#ifdef _WIN32
	sRawName=typeid(Class).raw_name();
	sRawName=sRawName.substr(5,sRawName.size()-7);
#else
	sRawName=typeid(Class).name();

	TCHAR ch = sRawName[0];
	while(ch >= '0' && ch <= '9')
	{
		sRawName=sRawName.substr(1,sRawName.size()-1);
		ch = sRawName[0];
	}
	sRawName=sRawName.substr(1,sRawName.size()-1);
#endif

	//如果是服务端的类，则去掉Server这个后缀
	const TCHAR szServer[] = "Server";
	const uint32 uLength = sizeof(szServer)-1;

	if( sRawName.rfind( szServer ) == sRawName.size() - uLength )
		sRawName = sRawName.substr( 0, sRawName.size() - uLength );

	return sRawName;
}

template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
float TValueAdapter<FinalHolderType,HolderType,PropertyType,pProperty>::Get(const FinalHolderType* pHolder)const
{
	return float( pHolder->Get(pProperty) );
}

template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
void TValueAdapter<FinalHolderType,HolderType,PropertyType,pProperty>::Set(FinalHolderType*,float)
{
	GenErr("ValueAdapter can not change value.");
}


template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
string TValueAdapter<FinalHolderType,HolderType,PropertyType,pProperty>::GetName()
{
	return GetPropertyClassName<PropertyType>();
}


template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
float TAdderAdapter<FinalHolderType,HolderType,PropertyType,pProperty>::Get(const FinalHolderType* pHolder)const
{
	return float( (class_cast<const HolderType*>(pHolder)->*pProperty).GetAdder() );
}

template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
void TAdderAdapter<FinalHolderType,HolderType,PropertyType,pProperty>::Set(FinalHolderType* pHolder,float fValue)
{
	(class_cast<HolderType*>(pHolder)->*pProperty).SetAdder( pHolder, typename PropertyType::Value_t(fValue) );
}

template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
string TAdderAdapter<FinalHolderType,HolderType,PropertyType,pProperty>::GetName()
{
	return GetPropertyClassName<PropertyType>()+string("Adder");
}


template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
float TMultiplierAdapter<FinalHolderType,HolderType,PropertyType,pProperty>::Get(const FinalHolderType* pHolder)const
{
	return (class_cast<const HolderType*>(pHolder)->*pProperty).GetMultiplier();
}

template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
void TMultiplierAdapter<FinalHolderType,HolderType,PropertyType,pProperty>::Set(FinalHolderType* pHolder,float fValue)
{
	(class_cast<HolderType*>(pHolder)->*pProperty).SetMultiplier( pHolder, fValue );
}


template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
string TMultiplierAdapter<FinalHolderType,HolderType,PropertyType,pProperty>::GetName()
{
	return GetPropertyClassName<PropertyType>()+string("Multiplier");
}



template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
float TAgileValueAdapter<FinalHolderType,HolderType,PropertyType,pProperty>::Get(const FinalHolderType* pHolder)const
{
	return float( (class_cast<const HolderType*>(pHolder)->*pProperty).LimitGet() );
}

template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
void TAgileValueAdapter<FinalHolderType,HolderType,PropertyType,pProperty>::Set(FinalHolderType* pHolder,float fValue)
{
	(class_cast<HolderType*>(pHolder)->*pProperty).LimitSet( typename PropertyType::Value_t(fValue),pHolder );
}


template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
string TAgileValueAdapter<FinalHolderType,HolderType,PropertyType,pProperty>::GetName()
{
	return GetPropertyClassName<PropertyType>()+string("Agile");
}

