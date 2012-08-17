#pragma once 
#include "CBaseRefObject.h"
#include "ModuleDefs.h"

namespace sqr
{
	class MODULE_API CStaticObject : public CBaseRefObject 
	{
	public:
		enum {
			eBaseObjectType = eBOT_Static
		};

		CStaticObject();
		~CStaticObject();

		EBaseObjectType GetBaseObjectType() const { return eBOT_Static; }
	};
}
