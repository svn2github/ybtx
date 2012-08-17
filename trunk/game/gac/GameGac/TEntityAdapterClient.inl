#pragma once
#include "TEntityAdapterClient.h"

template < class Traits >
TEntityAdapterClient< Traits >::TEntityAdapterClient(typename Traits::EntityType *pEntity):
m_pEntity(pEntity)
{
}

template < class Traits >
TEntityAdapterClient< Traits >::~TEntityAdapterClient()
{
}
