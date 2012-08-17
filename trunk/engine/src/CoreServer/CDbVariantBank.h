#pragma once
#include "TObjectBank.h"


namespace sqr
{

	class CDbVariantBank
		:public TObjectBank<CVariant,TMtDbAllocator>
	{
	public:
		typedef TObjectBank<CVariant,TMtDbAllocator>	Parent_t;
		CDbVariantBank(uint32 uSize,bool bPrepare)
			:Parent_t( uSize,bPrepare )
		{
		}
	};

}
