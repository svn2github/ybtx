#pragma once
#include "TCOEventMoveBegan.h"

template<typename Traits>
TCOEventMoveBegan<Traits>::TCOEventMoveBegan(CoreObj_t* pObj)
:Parent_t( pObj )
{
}

template<typename Traits>
TCOEventMoveBegan<Traits>::~TCOEventMoveBegan()
{
	typename Traits::CoreObject_t* pObj = Parent_t::GetCoreObj();
	if( !pObj )
		return;
	typename Traits::CoreObjectHandler_t* pHandler=pObj->GetHandler();
	if( !pHandler )
		return;
	pHandler->OnMoveBegan();
}