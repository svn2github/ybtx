#pragma once
#include "TIntObjAdapterClient.h"
#include "TEntityAdapterClient.inl"

template < class Traits >
TIntObjClientAdapter< Traits >::TIntObjClientAdapter(typename Traits::EntityType *pEntity)
:TEntityAdapterClient< Traits >(pEntity)
{
}

template < class Traits >
TIntObjClientAdapter< Traits >::~TIntObjClientAdapter()
{
}
