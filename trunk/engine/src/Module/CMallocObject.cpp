#include "stdafx.h"
#include "CMallocObject.h"
#include "SqrAlloc.h"

void* CMallocObject::operator new( size_t stAllocateBlock)
{
	return SqrMalloc( stAllocateBlock );
}

void* CMallocObject::operator new[]( size_t stAllocateBlock)
{
	return operator new( stAllocateBlock );
}

void CMallocObject::operator delete(void* pMem)
{
	SqrFree( pMem );
}

void CMallocObject::operator delete[](void* pMem)
{
	operator delete( pMem );
}
