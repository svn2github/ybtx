#pragma once
#include "TSqrAllocObject.h"
#include "TSqrAllocator.h"

namespace sqr
{
	/*
		我们把tick内部的内存请求放入一个池中进行管理
	*/

	DeclareAllocObject(CMirrorMallocObject);

	DeclarePoolAllocator(CMirrorMallocAllocator, CMirrorMallocObject);
}
