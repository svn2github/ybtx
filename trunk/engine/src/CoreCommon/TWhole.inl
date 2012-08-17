#pragma once
#include "TWhole.h"


template<typename ImpWholeClass,typename ImpPartClass>
TWhole<ImpWholeClass,ImpPartClass>::TWhole(void)
{
}

template<typename ImpWholeClass,typename ImpPartClass>
TWhole<ImpWholeClass,ImpPartClass>::~TWhole(void)
{
	ClearParts();
}


template<typename ImpWholeClass,typename ImpPartClass>
void TWhole<ImpWholeClass,ImpPartClass>::ClearParts(void)
{
	for(typename ImpParts_t::iterator it=m_ImpParts.begin();it!=m_ImpParts.end();)
		static_cast<Part_t*>(*it++)->Delete();
}

template<typename ImpWholeClass,typename ImpPartClass>
const typename TWhole<ImpWholeClass,ImpPartClass>::ImpParts_t&
TWhole<ImpWholeClass,ImpPartClass>::GetParts(void)const
{
	return m_ImpParts;
}

template<typename ImpWholeClass,typename ImpPartClass>
bool TWhole<ImpWholeClass,ImpPartClass>::HasPart(void)const
{
	return ! m_ImpParts.empty();
}


template<typename ImpWholeClass,typename ImpPartClass>
typename TWhole<ImpWholeClass,ImpPartClass>::Part_t*
TWhole<ImpWholeClass,ImpPartClass>::GetFirstPart(void)const
{
	if( !HasPart() )
		return NULL;
	return m_ImpParts.front();
}
