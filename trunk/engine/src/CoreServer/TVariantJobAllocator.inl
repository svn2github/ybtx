#pragma once
#include "TVariantJobAllocator.h"


template<class T>
inline TVariantJobAllocator<T>::TVariantJobAllocator()
{
	m_bWorkInCreateJobThread = GetCurTID() == CCoreObjVariantSyncStreamJob::ms_CreateJobThreadID;
}

template<class T>
inline typename TVariantJobAllocator<T>::pointer TVariantJobAllocator<T>::allocate(size_type sz)
{
	if( m_bWorkInCreateJobThread )
	{
		Ast( GetCurTID() == CCoreObjVariantSyncStreamJob::ms_CreateJobThreadID );
		return static_cast<pointer>( CCoreServerJob::AllocMem( sz ) );
	}
	else
	{
		return reinterpret_cast<pointer>(SqrNew(sz, CCypherVariantMallocObject::GetPool()));
	}
}

template<class T>
inline void TVariantJobAllocator<T>::deallocate(pointer p, size_type /*n*/)
{
	if( !m_bWorkInCreateJobThread )
	{
		SqrDelete(p, CCypherVariantMallocObject::GetPool());
	}	
}

template<class T>
inline void TVariantJobAllocator<T>::construct(pointer p, const value_type& value)
{
	new(p) value_type(value);
}

template<class T>
inline void TVariantJobAllocator<T>::destroy(pointer p)
{
	p->~T();
}

template<class T>
inline typename TVariantJobAllocator<T>::size_type TVariantJobAllocator<T>::max_size() const
{
	return size_type(size_type(-1) / sizeof(value_type));
}
