#pragma once
#include "stdafx.h"
#include "TEntityAdapterServer.h"


template < class Traits >
TEntityAdapterServer< Traits >::TEntityAdapterServer(typename Traits::EntityType *pEntity):
	m_pEntity(pEntity)
{
}

template < class Traits >
TEntityAdapterServer< Traits >::~TEntityAdapterServer()
{
}

