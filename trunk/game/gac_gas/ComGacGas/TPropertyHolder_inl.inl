#pragma once
#include "TPropertyHolder.h"
#include "ExpHelper.h"

template<typename ImpClass>
template<typename PropertyHolder,typename PropertyType>
typename PropertyType::Value_t TPropertyHolder<ImpClass>::Get(PropertyType PropertyHolder::*pProperty)const
{
	const ImpClass* pRealThis=class_cast<const ImpClass*>(this);
	return (class_cast<const PropertyHolder*>(pRealThis)->*pProperty).Get(pRealThis);
}




template<typename ImpClass>
template<typename PropertyHolder,typename PropertyType>
typename PropertyType::Value_t TPropertyHolder<ImpClass>::GetAdder(PropertyType PropertyHolder::*pProperty)const
{
	return (class_cast<const PropertyHolder*>(this)->*pProperty).GetAdder();
}



template<typename ImpClass>
template<typename PropertyHolder,typename PropertyType>
void TPropertyHolder<ImpClass>::SetAdder(PropertyType PropertyHolder::*pProperty,typename PropertyType::Value_t Value)
{
	(class_cast<PropertyHolder*>(this)->*pProperty).SetAdder(Value);
}


template<typename ImpClass>
template<typename PropertyHolder,typename PropertyType>
typename PropertyType::Value_t TPropertyHolder<ImpClass>::GetMultiplier(PropertyType PropertyHolder::*pProperty)const
{
	return (class_cast<const PropertyHolder*>(this)->*pProperty).GetMultiplier();
}


template<typename ImpClass>
template<typename PropertyHolder,typename PropertyType>
void TPropertyHolder<ImpClass>::SetMultiplier(PropertyType PropertyHolder::*pProperty,typename PropertyType::Value_t Value)
{
	(class_cast<PropertyHolder*>(this)->*pProperty).SetMultiplier(this, Value);
}


template<typename ImpClass>
template<typename PropertyHolder,typename PropertyType>
void TPropertyHolder<ImpClass>::FullFill(PropertyType PropertyHolder::*pProperty)
{
	(class_cast<PropertyHolder*>(this)->*pProperty).FullFill(this);
}


template<typename ImpClass>
template<typename AdapterType>
void TPropertyHolder<ImpClass>::CreateAdapter(map<string,Adapter_t*>& mapAdapter, vector<Adapter_t*>& vecAdapter, vector<string>& vecAdapterName, uint32 uIndex)
{
	string sName=AdapterType::GetName();
	AdapterType* pAdapter=new AdapterType;
	Ver(  mapAdapter.insert(  make_pair( sName,pAdapter ) ).second  );
	
	Ver( GetMapName2Index().insert( make_pair(sName,uIndex) ).second );
	vecAdapter[uIndex] = pAdapter;
	vecAdapterName[uIndex] = sName;
}
