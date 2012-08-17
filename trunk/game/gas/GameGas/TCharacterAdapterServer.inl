#pragma once
#include "stdafx.h"
#include "TCharacterAdapterServer.h"

template < class Traits >
TCharacterAdapterServer< Traits >::TCharacterAdapterServer(typename Traits::EntityType *pEntity):
TEntityAdapterServer< Traits >(pEntity)
{
}

template < class Traits >
TCharacterAdapterServer< Traits >::~TCharacterAdapterServer()
{
}
