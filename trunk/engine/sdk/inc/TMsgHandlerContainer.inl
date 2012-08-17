#pragma once
#include "TMsgHandlerContainer.h"
#include "TSqrAllocator.inl"

template<typename ImpClass>
TMsgHandlerContainer<ImpClass>::TMsgHandlerContainer()
:m_nIdOffset(0)
{
}

template<typename ImpClass>
TMsgHandlerContainer<ImpClass>::~TMsgHandlerContainer()
{
	for (size_t i=0;i<m_vecHandler.size();++i)
		delete m_vecHandler[i];
}

template<typename ImpClass>
uint32 TMsgHandlerContainer<ImpClass>::GetSize()const
{
	return (uint32)m_vecHandler.size();
}

template<typename ImpClass>
int32 TMsgHandlerContainer<ImpClass>::GetIdOffset()const
{
	return m_nIdOffset;
}

template<typename ImpClass>
bool TMsgHandlerContainer<ImpClass>::Resize(uint32 uSize,int32 nIdOffset)
{
	m_nIdOffset=nIdOffset;

	uSize += m_nIdOffset;

	if (m_vecHandler.size()>uSize)
		return false;

	m_vecHandler.resize(uSize);

	return true;
}

template<typename ImpClass>
typename TMsgHandlerContainer<ImpClass>::BaseHandler_t*
TMsgHandlerContainer<ImpClass>::GetHandler( uint32 uIndex )const
{
	return m_vecHandler[ uIndex + m_nIdOffset ];
}
