#pragma once
#include "Memory.h"
#include "TSqrAllocator.h"

namespace sqr
{
	template<typename T, typename AllocObject> 
	TSqrAllocator<T, AllocObject>::TSqrAllocator() throw()
	{
	} 

	template<typename T, typename AllocObject> 
	TSqrAllocator<T, AllocObject>::TSqrAllocator(const TSqrAllocator&) throw()
	{	
	} 

	template<typename T, typename AllocObject> 
	template<class U> 
	TSqrAllocator<T, AllocObject>::TSqrAllocator(const TSqrAllocator<U, AllocObject>&) throw() 
	{	
	}

	template<typename T, typename AllocObject> 
	TSqrAllocator<T, AllocObject>::~TSqrAllocator() throw() 
	{
	
	} 

	template<typename T, typename AllocObject> 
	bool TSqrAllocator<T, AllocObject>::operator==(const TSqrAllocator<T, AllocObject>& )const throw()
	{
		return true; 
	} 

	template<typename T, typename AllocObject> 
	typename TSqrAllocator<T, AllocObject>::pointer TSqrAllocator<T, AllocObject>::allocate(size_type size) 
	{	
		return reinterpret_cast<pointer>(SqrChunkNew(size*sizeof(T), AllocObject::GetPool()));
	} 

	template<typename T, typename AllocObject> 
	void TSqrAllocator<T, AllocObject>::deallocate(pointer p, size_type ) 
	{	
		SqrDelete(p, AllocObject::GetPool());	
	} 

	template<typename T, typename AllocObject> 
	typename TSqrAllocator<T, AllocObject>::size_type TSqrAllocator<T, AllocObject>::max_size() const
	{
		return size_type(size_type(-1) / sizeof(value_type));
	} 

	template<typename T, typename AllocObject> 
	void TSqrAllocator<T, AllocObject>::construct(pointer p, const value_type& value) 
	{
		new(p) value_type(value);
	} 

	template<typename T, typename AllocObject> 
	void TSqrAllocator<T, AllocObject>::destroy(pointer p) 
	{
		p->~T();
	} 

}

