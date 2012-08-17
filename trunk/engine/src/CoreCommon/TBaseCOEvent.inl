#pragma once
#include "TBaseCOEvent.h"


template<typename Traits>
TBaseCOEvent<Traits>::TBaseCOEvent(CoreObj_t* pObj)
{
	m_RefCoreObj.Attach( *pObj->m_pRefByEvent );
}

template<typename Traits>
TBaseCOEvent<Traits>::~TBaseCOEvent(void)
{
}

template<typename Traits>
typename TBaseCOEvent<Traits>::CoreObj_t*
TBaseCOEvent<Traits>::GetCoreObj()const
{
	return m_RefCoreObj.Get();
}

