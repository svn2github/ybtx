#pragma once
#include "CSyncVariantClientHolder.h"
#include "TSingleton.h"
#include "CTick.h"

namespace sqr
{

	class CSyncVariantClientHolderPackage
		:public CCypherVariantMallocObject
		,public CTick
	{
		typedef vector<CSyncVariantClientHolder*,TCypherVariantAllocator<CSyncVariantClientHolder*> > VecSyncVariantClientHolder_t;
	public:
		CSyncVariantClientHolderPackage(uint32 uGlobalID);
		~CSyncVariantClientHolderPackage();

		void Release();
		void OnTick();
		VecSyncVariantClientHolder_t	m_vecSyncVariantHolder;
		VecSyncVariantClientHolder_t	m_vecViewVariantHolder;
		uint32 m_uGlobalID;
	};

	class CSyncVariantClientHolderPool
		: public CCypherVariantMallocObject
		, public Singleton<CSyncVariantClientHolderPool>
	{
		typedef map<uint32, CSyncVariantClientHolderPackage*, less<uint32>, TVariantAllocator<pair<uint32, CSyncVariantClientHolderPackage*> > > MapSyncVariantClientHolder;
	public:
		CSyncVariantClientHolderPool();
		~CSyncVariantClientHolderPool();

		CSyncVariantClientHolderPackage* GetSyncVariantClientHolderById(uint32 uId);
		void PushSyncVariantClientHolder(uint32 uId, CSyncVariantClientHolderPackage* pData);
		void Clear();
	private:
		MapSyncVariantClientHolder m_mapSyncVariantClientHolder;
	};

}
