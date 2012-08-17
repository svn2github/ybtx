#pragma once
//$id$
//gac到gas的协议id分段文件

namespace sqr
{
	enum EGac2GasPrtlSegment
	{
		eGac2GasPS_Generic_Begin		=0,
		eGac2GasPS_Generic_End			=100,
		eGac2GasPS_Object_Begin			=eGac2GasPS_Generic_End+1,
		eGac2GasPS_Object_End			=255
	};
}


