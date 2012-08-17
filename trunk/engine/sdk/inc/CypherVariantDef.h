#pragma once
#include "VariantTypes.h"

namespace sqr
{
	enum ECypherVariantPathType
	{
		eCVPT_Code8,
		eCVPT_Code16,
		eCVPT_Code32,
		eCVPT_Path
	};

	enum ECypherVariantAction
	{
		eCVA_Set_Bool =eVT_Bool,
		eCVA_Set_Int8 =eVT_Int8,
		eCVA_Set_Uint8 =eVT_Uint8,
		eCVA_Set_Int16 =eVT_Int16,
		eCVA_Set_Uint16 =eVT_Uint16,
		eCVA_Set_Int32 =eVT_Int32,
		eCVA_Set_Uint32 =eVT_Uint32,
		eCVA_Set_Int64 =eVT_Int64,
		eCVA_Set_Uint64 =eVT_Uint64,
		eCVA_Set_Float =eVT_Float,
		eCVA_Set_Double =eVT_Double,
		eCVA_Set_String =eVT_String,
		eCVA_Set_Table =eVT_Table,
		eCVA_Clr_Table,
		eCVA_Del_Member,
	};


	struct CCVCFirstByte
	{
		uint8 m_uPathType:2;
		uint8 m_uAction:6;
	};
}
