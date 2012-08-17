#pragma once 

#include "CommonDefs.h"
#include "CValueMallocObject.h"

namespace sqr 
{
	class CConstStringPool;

	class COMMON_API CConstString : public CValueMallocObject
	{
	public:
		static CConstStringPool* CreatePool();
		static void DestroyPool(CConstStringPool* pPool);

		CConstString(const char* str, CConstStringPool* pPool, bool bForceInsert = true);
		~CConstString();

		const char* GetStr() { return m_str; }

		operator const char* () { return m_str; }

	private:
		char*		m_str;
	};

}
