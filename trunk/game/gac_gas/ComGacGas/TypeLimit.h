#pragma once
#include <float.h>
#include <limits.h>
#include "ExpHelper.h"

template <typename Value_t>
struct ValueTypeLimit
{
	//自定义类型需要定义ValueTypeLimit否则使用Convert会编译不过
};

template<>
struct ValueTypeLimit<double>
{
	//默认使用FPU_DOUBLE 如果使用其它浮点精度需要更改此处
	typedef double Value_t;
	typedef double Check_t;
	typedef double CheckLimit_t;
	static inline Value_t GetMaxLimit()
	{
		return DBL_MAX;
	}
	static inline Value_t GetMinLimit()
	{
		return (-1)*DBL_MAX-1;
	}
};

template<>
struct ValueTypeLimit<float>
{
	typedef float Value_t;
	typedef float Check_t;
	typedef double CheckLimit_t;
	static inline Value_t GetMaxLimit()
	{
		return FLT_MAX;
	}
	static inline Value_t GetMinLimit()
	{
		return (-1)*FLT_MAX-1;
	}
};

template<>
struct ValueTypeLimit<uint64>
{
	typedef uint64 Value_t;
	typedef int64 Check_t;
	typedef int64 CheckLimit_t;
	static inline Value_t GetMaxLimit()
	{
		return ULLONG_MAX;
	}
	static inline Value_t GetMinLimit()
	{
		return 0;
	}
};

template<>
struct ValueTypeLimit<uint32>
{
	typedef uint32 Value_t;
	typedef int32 Check_t;
	typedef double CheckLimit_t;
	static inline Value_t GetMaxLimit()
	{
		return UINT_MAX;
	}
	static inline Value_t GetMinLimit()
	{
		return 0;
	}
};

template<>
struct ValueTypeLimit<uint16>
{
	typedef uint16 Value_t;
	typedef int16 Check_t;
	typedef float CheckLimit_t;
	static inline Value_t GetMaxLimit()
	{
		return USHRT_MAX;
	}
	static inline Value_t GetMinLimit()
	{
		return 0;
	}
};

template<>
struct ValueTypeLimit<int64>
{
	typedef int64 Value_t;
	typedef int64 Check_t;
	typedef int64 CheckLimit_t;
	static inline Value_t GetMaxLimit()
	{
		return LLONG_MAX;
	}
	static inline Value_t GetMinLimit()
	{
		return LLONG_MIN;
	}
};

template<>
struct ValueTypeLimit<int32>
{
	typedef int32 Value_t;
	typedef int32 Check_t;
	typedef double CheckLimit_t;
	static inline Value_t GetMaxLimit()
	{
		return INT_MAX;
	}
	static inline Value_t GetMinLimit()
	{
		return INT_MIN;
	}
};

template<>
struct ValueTypeLimit<int16>
{
	typedef int16 Value_t;
	typedef int16 Check_t;
	typedef float CheckLimit_t;
	static inline Value_t GetMaxLimit()
	{
		return SHRT_MAX;
	}
	static inline Value_t GetMinLimit()
	{
		return SHRT_MIN;
	}
};

template <typename ValueType, typename RetType>
bool  CheckValueTypeToType(const ValueType& value)
{
	typedef ValueTypeLimit<RetType> traits_type;
	typedef typename traits_type::Value_t	value_t;
	typedef ValueTypeLimit<ValueType> traits_temp_type;
	typedef typename traits_temp_type::CheckLimit_t type_t;
	value_t MaxLimit = traits_type::GetMaxLimit();
	value_t MinLimit = traits_type::GetMinLimit();
	type_t  MaxTemp = type_t(MaxLimit) - type_t(value);
	type_t  MinTemp = type_t(MinLimit) - type_t(value);
	if (value!=value_t(0)&&(MaxTemp<(type_t)(0)||MinTemp>(type_t)(0)))
	{
		return false;
	}
	return true;
}

template <typename ValueType, typename RetType>
void  ValueTypeToRetType(const ValueType& value, RetType& retvalue)
{
	typedef ValueTypeLimit<RetType> traits_type;
	typedef typename traits_type::Value_t	value_t;
	typedef ValueTypeLimit<ValueType> traits_temp_type;
	typedef typename traits_temp_type::CheckLimit_t type_t;
	value_t MaxLimit = traits_type::GetMaxLimit();
	value_t MinLimit = traits_type::GetMinLimit();
	type_t  MaxTemp = type_t(MaxLimit) - type_t(value);
	type_t  MinTemp = type_t(MinLimit) - type_t(value);
	if (value!=value_t(0)&&(MaxTemp<(type_t)(0)||MinTemp>(type_t)(0)))
	{
		retvalue = RetType(0);
		ostringstream oss;
		oss << "ValueTypeToRetType OverFlow : " << value;
		GenErr(oss.str().c_str());
		return;
	}
	retvalue = RetType(value);
}

template <typename ValueType>
void  CheckUnsignedUnderFlow(const ValueType& value)
{
	typedef ValueTypeLimit<ValueType> traits_type;
	typedef typename traits_type::CheckLimit_t	value_t;
	typedef typename traits_type::Check_t type_t;
	value_t MinLimit = traits_type::GetMinLimit();
	type_t  Temp = type_t(value);
	if (MinLimit==value_t(0)&&Temp<(type_t)(0))
	{
		ostringstream oss;
		oss << "UnsignedUnderFlow : " << value;
		GenErr(oss.str().c_str());
		return;
	}
}
