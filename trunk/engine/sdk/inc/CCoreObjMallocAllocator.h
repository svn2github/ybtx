#pragma once
#include "TNedAllocObject.h"
#include "TNedAllocator.h"

#define COREOBJ_MALLOC_POOL_CAPACITY	1024*1024*1
#define COREOBJ_MALLOC_POOL_THREAD		1

DeclareAllocObject(CCoreObjMallocObject, COREOBJ_MALLOC_POOL_CAPACITY, COREOBJ_MALLOC_POOL_THREAD);

DeclarePoolAllocator(CCoreObjMallocAllocator, CCoreObjMallocObject);
