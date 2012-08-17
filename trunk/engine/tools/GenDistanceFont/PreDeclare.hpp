#ifndef _PREDECLARE_HPP
#define _PREDECLARE_HPP

#include <boost/smart_ptr.hpp>

namespace sqr
{
	template <typename T, int N>
	class Vector_T;
	typedef Vector_T<float, 1>	float1;
	typedef Vector_T<float, 2>	float2;
	typedef Vector_T<float, 3>	float3;
	typedef Vector_T<float, 4>	float4;

	typedef Vector_T<int, 1>	int1;
	typedef Vector_T<int, 2>	int2;
	typedef Vector_T<int, 3>	int3;
	typedef Vector_T<int, 4>	int4;

}

#endif
