#pragma once


namespace sqr
{
	void* SqrMallocForNew(size_t stSize, CSqrAllocPool* pPool);
	void SqrFreeForDelete(void* pMem, size_t stSize, CSqrAllocPool* pPool);
}
