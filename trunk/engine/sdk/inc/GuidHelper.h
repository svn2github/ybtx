#pragma once
#include "ModuleDefs.h"
#include "GuidTypes.h"

namespace sqr
{	
	//对于一般的guid的字符串表示，linux下和win下面都是使用format为“%08x-%04x-%04x-%04x-%012x”

	//guid 是一个128bit的字段，所以我们可以用unsigned char 16存储
	MODULE_API void CreateGuid(guid_t guid);
	
	MODULE_API void CreateStringGuid(str_guid_t sz_guid);

	//将guid转换成对应的字符串
	MODULE_API void GuidToString(const guid_t guid, str_guid_t sz_guid);

	//将guid字串转成相应的guid
	MODULE_API void StringToGuid(const str_guid_t sz_guid, guid_t guid);

}

