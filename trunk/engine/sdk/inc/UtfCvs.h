#pragma once
#include "ModuleDefs.h"

namespace sqr
{
	//下面这几个函数在调用之后返回的字串必须调用FreeUtfCvsDest进行内存释放
	MODULE_API char* U16_to_U8(const wchar_t* wzSrc);
	MODULE_API wchar_t* U8_to_U16(const char* szSrc);
	
	MODULE_API char* U16_to_MB(const wchar_t* wzSrc);

	MODULE_API void FreeUtfCvsDest(void* dest);
}
