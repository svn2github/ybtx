#pragma once
#include "CPos.h"

namespace sqr
{
	class CMirrorBuffer;

	class CMirrorBufferData
	{
	public:
		static void* operator new(size_t stSize, CMirrorBuffer* pBuffer, uint32 uSlot);
		static void operator delete(void* pMem, CMirrorBuffer* pBuffer, uint32 uSlot);
		static void operator delete(void* pMem);
	};

}
