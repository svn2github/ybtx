#pragma once
#include "TIntObjAdapterServer.h"

template < class Traits >
TIntObjAdapterServer< Traits >::TIntObjAdapterServer(typename Traits::EntityType *pEntity):
TEntityAdapterServer< Traits >(pEntity)
{
}

template < class Traits >
TIntObjAdapterServer< Traits >::~TIntObjAdapterServer()
{
}
