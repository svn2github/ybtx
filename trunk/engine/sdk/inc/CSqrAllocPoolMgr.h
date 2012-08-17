#pragma once
#include "ModuleDefs.h"
#include "CMallocObject.h"
#include "TMallocAllocator.h"
#include "ThreadTypes.h"
#include <set>

int LoadFindLeakageConfig();

namespace sqr
{
	class ISqrAllocPool;

	class MODULE_API CSqrAllocPoolMgr
		: public CMallocObject
	{
		friend int ::LoadFindLeakageConfig();
	public:
		static CSqrAllocPoolMgr& Inst();

		void AddPool(ISqrAllocPool* pPool);
		void DelPool(ISqrAllocPool* pPool);

		size_t GetAllMemUsage();

		size_t GetAllCapacity();
	
		void GetAllMemInfo(ostream& strm);

		void GetMemLeakage(ostream& strm);

		void Trim();

	private:
		CSqrAllocPoolMgr();
		~CSqrAllocPoolMgr();

		void EnableFindLeakageOnPool(const char* szPoolName);

	private:
		typedef basic_string<char,std::char_traits<char>,TMallocAllocator<char> > String_t;
		typedef pair< ISqrAllocPool* , String_t >	Pair_t;
		typedef TMallocAllocator< Pair_t >	Allocator_t;
		typedef map< ISqrAllocPool*, String_t, less<ISqrAllocPool*> ,Allocator_t >	PoolMap_t;
		typedef multiset< String_t, less< String_t> ,TMallocAllocator<char> >		NameSet_t;

		HLOCK	m_lkAllocPoolMap;

#ifdef _WIN32
#pragma warning (push)
#pragma warning (disable:4251)
#endif
		PoolMap_t m_mapAllocPool;
		NameSet_t m_setFindLeakagePoolName;
#ifdef _WIN32
#pragma warning (pop)
#endif
	};

}

