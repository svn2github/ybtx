#pragma once
#include "TCOEventPosChanged.h"

template<typename Traits>
TCOEventPosChanged<Traits>::TCOEventPosChanged(CoreObj_t* pObj)
:Parent_t( pObj )
{
}

template<typename Traits>
TCOEventPosChanged<Traits>::~TCOEventPosChanged()
{
	typename Traits::CoreObject_t* pObj = Parent_t::GetCoreObj();
	if( !pObj )
		return;
	typename Traits::CoreObjectHandler_t* pHandler=pObj->GetHandler();
	if( !pHandler )
		return;
	pHandler->OnPosChanged();
}
