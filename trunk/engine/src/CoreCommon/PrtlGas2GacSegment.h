#pragma once
//$id$
//gas到gac的协议id分段文件

namespace sqr
{
	enum EGas2GacPrtlSegment
	{
		eGas2GacPS_Generic_Begin		=0,
		eGas2GacPS_Generic_End			=100,
		eGas2GacPS_Object_Begin			=eGas2GacPS_Generic_End+1,
		eGas2GacPS_Object_End			=255
	};
}


