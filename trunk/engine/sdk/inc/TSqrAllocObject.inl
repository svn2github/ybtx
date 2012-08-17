#pragma once
#include "TSqrAllocObject.h"
#include "Memory.h"
#include "CSqrAllocPool.h"
#include "ExpHelper.h"

namespace sqr
{

	template<typename AllocObject>
	void* TSqrAllocObject<AllocObject>::operator new(size_t stSize)
	{
		return SqrNew(stSize, AllocObject::GetPool());	
	}

	template<typename AllocObject>
	void* TSqrAllocObject<AllocObject>::operator new[](size_t stSize)
	{
		return SqrNew(stSize, AllocObject::GetPool());			
	}

	template<typename AllocObject>
	void TSqrAllocObject<AllocObject>::operator delete(void* pMem)
	{
		SqrDelete(pMem, AllocObject::GetPool());		
	}

	template<typename AllocObject>
	void TSqrAllocObject<AllocObject>::operator delete[](void* pMem)
	{
		SqrDelete(pMem, AllocObject::GetPool());
	}

	/*
	我们的pool通过GetPool函数取得，为了保证线程安全
	所以我们在下面立刻声明并定义一个initpool，这样在其初始化的时候
	就能够保证pool被初始化
	*/

#define DefineAllocObject(ClassName)\
	template class TSqrAllocObject<ClassName>; \
	CSqrAllocPool* ClassName::GetPool()\
	{\
	static CSqrAllocPool* ClassName##_Pool = new CSqrAllocPool(#ClassName); \
	return ClassName##_Pool;\
	}\
	class ClassName##InitPool \
	{ \
	public:\
	ClassName##InitPool() \
	{ \
	ClassName::GetPool(); \
	} \
	};\
	static ClassName##InitPool	ClassName##_InitPool;


}

