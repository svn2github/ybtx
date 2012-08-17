#pragma once
#include "TCOEventMovePathChanged.h"



template<typename Traits>
TCOEventMovePathChanged<Traits>::TCOEventMovePathChanged(CoreObj_t* pObj)
:Parent_t( pObj )
{
}


template<typename Traits>
TCOEventMovePathChanged<Traits>::~TCOEventMovePathChanged()
{
	typename Traits::CoreObject_t* pObj = Parent_t::GetCoreObj();
	if( !pObj )
		return;
	typename Traits::CoreObjectHandler_t* pHandler=pObj->GetHandler();
	if( !pHandler )
		return;
	pHandler->OnMovePathChanged();
}
