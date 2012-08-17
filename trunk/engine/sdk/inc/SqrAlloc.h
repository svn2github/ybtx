#pragma once 

#include "ModuleDefs.h"

namespace sqr
{
	/*
		下列函数是底层内存分配函数，这些函数所分配内存不计入内存泄漏统计，一般代码应尽量避免直接使用下列函数
	*/
	
	MODULE_API void* SqrMalloc(size_t stSize);
	MODULE_API void SqrFree(void* pMem);
	MODULE_API void* SqrRealloc(void* pMem, size_t stNewSize);
	MODULE_API void SqrTrim(size_t pad);
}

