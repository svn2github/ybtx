#pragma once 
#include "CBaseRefObject.h"
#include "ModuleDefs.h"

namespace sqr 
{

	class MODULE_API CDynamicObject
		:public CBaseRefObject
	{
	public:
		enum {
			eBaseObjectType = eBOT_Dynamic
		};

		CDynamicObject();
		~CDynamicObject();

		EBaseObjectType GetBaseObjectType() const { return eBOT_Dynamic; }
	};

}
