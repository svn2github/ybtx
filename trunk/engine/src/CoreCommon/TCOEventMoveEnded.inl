#pragma once
#include "TCOEventMoveEnded.h"
//#include <iostream>

template<typename Traits>
TCOEventMoveEnded<Traits>::TCOEventMoveEnded(CoreObj_t* pObj,EMoveEndedType eMEType)
:Parent_t( pObj )
{
	m_eMEType = eMEType;
}


template<typename Traits>
TCOEventMoveEnded<Traits>::~TCOEventMoveEnded()
{
	typename Traits::CoreObject_t* pObj = Parent_t::GetCoreObj();
	if( !pObj )
		return;
	typename Traits::CoreObjectHandler_t* pHandler=pObj->GetHandler();
	if( !pHandler )
		return;
	pHandler->OnMoveEnded(m_eMEType);
}
