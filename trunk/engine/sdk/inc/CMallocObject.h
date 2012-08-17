#pragma once
#include "ModuleDefs.h"

namespace sqr
{
	class CSqrAllocPool;

	class MODULE_API CMallocObject 
	{
	public:
		void *operator new( size_t stAllocateBlock);
		void operator delete(void* pMem);

		void *operator new[]( size_t stAllocateBlock);		
		void operator delete[](void* pMem);
		
		void* operator new( size_t stSize, void* ptr ) { return ptr; }
		void operator delete(void*, void*) {}
	};

}

