#include "stdafx.h"
#include "CConstStringPool.h"
#include "TSqrAllocator.inl"
#include "CMemPool.h"

CConstStringPool::CConstStringPool()
{
	m_pMemPool = new CMemPool(1024, CValueMallocObject::GetPool());

	void* buffer = m_pMemPool->Alloc(sizeof(StringSet_t));
#ifdef _WIN32
	m_pStringSet = new (buffer)StringSet_t(hash_compare<const char*, string_less>(), m_pMemPool);
#else
	m_pStringSet = new (buffer)StringSet_t(100, string_hash(), string_equal(), m_pMemPool);
#endif

}

CConstStringPool::~CConstStringPool()
{
	delete m_pMemPool;
}

char* CConstStringPool::GetStr(const char* str, bool bForceInsert )
{
	if (!str)
		return NULL;
	
	StringSet_t::iterator iter = m_pStringSet->find(str);

	if (iter == m_pStringSet->end())
	{
		if (bForceInsert)
		{
			char* buffer = (char*)m_pMemPool->Alloc(strlen(str) + 1);
			strcpy(buffer, str);
			iter = m_pStringSet->insert(buffer).first;
		}
		else
		{
			return NULL;
		}
	}
	return const_cast<char*>(*iter);
}


