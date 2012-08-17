#pragma once
#include "TVariantAllocator.h"
#include "TSingleton.h"
#include "TObjectBank.h"
#include "CVariantHolderDef.h"

namespace sqr
{
	class CCypherVariant;

	template<typename AppType>
	class TRootVariantPoolMgr
		: public CVariantMallocObject
		, public Singleton<TRootVariantPoolMgr<AppType> >
	{
		typedef TObjectBank<CCypherVariant, TVariantAllocator> VariantPool;
		typedef map<uint32, VariantPool*, less<uint32>, TVariantAllocator<pair<uint32, VariantPool*> > > MapVariantPool;
		typedef vector<MapVariantPool*, TVariantAllocator<MapVariantPool*> > VecVariantPoolOfType;
	public:
		TRootVariantPoolMgr();
		~TRootVariantPoolMgr();

		VariantPool* GetPool(uint32 uVarDefID, bool bForSync, ECodeTableIndex eCodeTableIndex);
		VariantPool* GetSubTreePool(uint32 uSubTreeType);

	private:

		VecVariantPoolOfType	m_vecPoolForSync;
		VecVariantPoolOfType	m_vecPoolForView;
		MapVariantPool			m_mapSubTreePool;
	};
}
