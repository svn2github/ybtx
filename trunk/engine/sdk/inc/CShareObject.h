#pragma once 
#include "CBaseObject.h"
#include "ModuleDefs.h"

namespace sqr
{
	class MODULE_API CShareObject
		: public virtual CBaseObject
	{
	public:
		enum {
			eBaseObjectType = eBOT_Share
		};

		CShareObject();
		~CShareObject();

		EBaseObjectType GetBaseObjectType() const { return eBOT_Share; }

		uint32 GetBaseObjectID() const{ return m_uID; }

	private:
		static uint32 GetNextObjectID();

	private:
		static volatile uint32	ms_uObjectID;

		uint32 m_uID;
	};
}
