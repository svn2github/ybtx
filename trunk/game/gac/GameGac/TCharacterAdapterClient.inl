#pragma once
#include "TCharacterAdapterClient.h"

template<typename Traits>
TCharacterAdapterClient<Traits>::TCharacterAdapterClient(typename Traits::EntityType *pEntity)
:TEntityAdapterClient<Traits>(pEntity)
 {
 }
 
template<typename Traits>
TCharacterAdapterClient<Traits>::~TCharacterAdapterClient()
{
}
