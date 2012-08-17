#pragma once
#include "TPart.h"



template<typename ImpPartClass,typename ImpWholeClass>
TPart<ImpPartClass,ImpWholeClass>::TPart(Whole_t* pWhole)
:m_pWhole(pWhole)
{
	m_pWhole->m_ImpParts.push_front(static_cast<ImpPartClass*>(this));
	m_itWhole=m_pWhole->m_ImpParts.begin();
}

template<typename ImpPartClass,typename ImpWholeClass>
TPart<ImpPartClass,ImpWholeClass>::~TPart(void)
{
	m_pWhole->m_ImpParts.erase(m_itWhole);
}

template<typename ImpPartClass,typename ImpWholeClass>
typename TPart<ImpPartClass,ImpWholeClass>::Whole_t* 
TPart<ImpPartClass,ImpWholeClass>::GetWhole()const
{
	return m_pWhole;
}

template<typename ImpPartClass,typename ImpWholeClass>
void TPart<ImpPartClass,ImpWholeClass>::Delete()
{
	delete static_cast<ImpPartClass*>(this);
}
