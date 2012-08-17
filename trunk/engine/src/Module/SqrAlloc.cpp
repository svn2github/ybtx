#include "stdafx.h"
#include "SqrAlloc.h"

namespace sqr
{
	void* SqrMalloc(size_t stSize)
	{
		return malloc( stSize );
	}

	void SqrFree(void* pMem)
	{
		free( pMem );
	}
	
	void* SqrRealloc(void* pMem, size_t stSize)
	{
		return realloc( pMem, stSize );
	}

	void SqrTrim(size_t pad)
	{
		//malloc_trim( pad );
	}
}
