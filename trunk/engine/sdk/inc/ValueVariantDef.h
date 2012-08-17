#pragma once 

namespace sqr 
{

	enum EValueVariantType
	{
		eVVT_Bool = eVT_Bool,
		eVVT_Int8 = eVT_Int8,
		eVVT_Uint8 = eVT_Uint8,
		eVVT_Int16 = eVT_Int16,
		eVVT_Uint16 = eVT_Uint16,
		eVVT_Int32 = eVT_Int32,
		eVVT_Uint32 = eVT_Uint32,
		eVVT_Int64 = eVT_Int64,
		eVVT_Uint64 = eVT_Uint64,
		eVVT_Float = eVT_Float,
		eVVT_Double = eVT_Double,
		eVVT_String = eVT_String,
		eVVT_Table = eVT_Table,
		eVVT_CStr,
		eVVT_Ptr,
	};

}
