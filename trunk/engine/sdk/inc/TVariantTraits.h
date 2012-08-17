#pragma once
#include "VariantTypes.h"

namespace sqr
{

	template<typename ValueType>struct TVariantTraits {};


	template<>struct TVariantTraits<bool>
	{
		inline static const char* GetName()
		{
			return "bool";
		}
		inline static EVariantType GetId()
		{
			return eVT_Bool;
		}
	};

	template<>struct TVariantTraits<int8>
	{
		inline static const char* GetName()
		{
			return "int8";
		}
		inline static EVariantType GetId()
		{
			return eVT_Int8;
		}
	};

	template<>struct TVariantTraits<uint8>
	{
		inline static const char* GetName()
		{
			return "uint8";
		}
		inline static EVariantType GetId()
		{
			return eVT_Uint8;
		}
	};


	template<>struct TVariantTraits<int16>
	{
		inline static const char* GetName()
		{
			return "int16";
		}
		inline static EVariantType GetId()
		{
			return eVT_Int16;
		}
	};

	template<>struct TVariantTraits<uint16>
	{
		inline static const char* GetName()
		{
			return "uint16";
		}
		inline static EVariantType GetId()
		{
			return eVT_Uint16;
		}
	};


	template<>struct TVariantTraits<int32>
	{
		inline static const char* GetName()
		{
			return "int32";
		}
		inline static EVariantType GetId()
		{
			return eVT_Int32;
		}
	};

	template<>struct TVariantTraits<uint32>
	{
		inline static const char* GetName()
		{
			return "uint32";
		}
		inline static EVariantType GetId()
		{
			return eVT_Uint32;
		}
	};


	template<>struct TVariantTraits<int64>
	{
		inline static const char* GetName()
		{
			return "int64";
		}
		inline static EVariantType GetId()
		{
			return eVT_Int64;
		}
	};

	template<>struct TVariantTraits<uint64>
	{
		inline static const char* GetName()
		{
			return "uint64";
		}
		inline static EVariantType GetId()
		{
			return eVT_Uint64;
		}
	};

	template<>struct TVariantTraits<float>
	{
		inline static const char* GetName()
		{
			return "float";
		}
		inline static EVariantType GetId()
		{
			return eVT_Float;
		}
	};

	template<>struct TVariantTraits<double>
	{
		inline static const char* GetName()
		{
			return "double";
		}
		inline static EVariantType GetId()
		{
			return eVT_Double;
		}
	};


}

