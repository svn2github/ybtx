#pragma once 
#include "CValueMallocObject.h"
#include "TValueAllocator.h"
#include "TMemPoolAllocator.h"

#ifdef _WIN32
#include <hash_map>
#include <hash_set>
using namespace stdext;
#else
#include <ext/hash_map>
#include <ext/hash_set>
using namespace __gnu_cxx;

#endif

namespace sqr 
{

	class CMemPool;

	class CConstStringPool : public CValueMallocObject
	{
		friend class CConstString;
		
	private:
		CConstStringPool();
		~CConstStringPool();

		char* GetStr(const char* str, bool bForceInsert);
	
	private:

		CMemPool*		m_pMemPool;


#ifdef _WIN32
		struct string_less : public binary_function<const char*, const char*, bool>
		{
			bool operator()(const char* szLeft, const char* szRight) const 
			{
				return (strcmp(szLeft, szRight) < 0) ? true : false;
			}
		};

		typedef hash_set<const char*, hash_compare<const char*, string_less >, 
			TMemPoolAllocator<const char*> >	StringSet_t;
#else
		struct string_hash     
		{                                                                                                                            
			size_t operator()(const char* str) const                                                                               
			{                                                                                                                        
				return __stl_hash_string(str);                                                                               
			}                                                                                                                        
		};    

		struct string_equal
		{
			bool operator()(const char* szLeft, const char* szRight) const
			{
				return strcmp(szLeft, szRight) == 0;
			}
		};

		typedef hash_set<const char*, string_hash, string_equal, 
			TMemPoolAllocator<const char*> > StringSet_t;
#endif

		StringSet_t*		m_pStringSet;

	};

}
