#pragma once

namespace sqr
{
	/*
		T为实际的子类
		stCapacity为该pool的大小
		uThread为该pool被几个线程访问，uThread可以自动扩展
	*/

	class CSqrAllocPool;

	template<typename AllocObject>
	class TSqrAllocObject
	{
	public:
		static void *operator new( size_t stAllocateBlock);
		static void* operator new( size_t , void* ptr) { return ptr; }
		static void *operator new[]( size_t stAllocateBlock);
		static void operator delete(void* pMem);
		static void operator delete(void* , void* ) { }
		static void operator delete[](void* pMem);
	};


#define DeclareAllocObject(ClassName) \
	class ClassName : public TSqrAllocObject<ClassName> \
	{\
	public:\
		static CSqrAllocPool* GetPool();\
	};
}

