#pragma once
#include "TMallocAllocator.h"
#include "CMallocObject.h"
#include <fstream>

namespace sqr
{
#ifdef _WIN32
	#define COUNTOF(array) _countof(array)
	typedef int ( __stdcall *CompressFun)(char*, uint32*, char*, uint32, int32, int32, int32);
	typedef int ( __stdcall *DeCompressFun)(char*, uint32*, char*, uint32, int32, int32);

#else
	#define COUNTOF(array)	(sizeof(array)/sizeof(array[0]))

	typedef int (*CompressFun)(char*, uint32*, char*, uint32, int32, int32, int32);
	typedef int (*DeCompressFun)(char*, uint32*, char*, uint32, int32, int32);
#endif
}
