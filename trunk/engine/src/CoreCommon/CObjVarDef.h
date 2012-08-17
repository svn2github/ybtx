#pragma once
#include "CVariantHolderDef.h"
namespace sqr
{
	class CSyncCoreObjectDictator;
	class CCoreObjectDictator;
	class CCoreObjectFollower;
	class CCypherVariantCodeTable;
	
	class CObjVarDef
	{
	public:
		CObjVarDef(const char* szFileName);
		~CObjVarDef();
		const char* GetFileName()const;
	
	private:
		vector<CCypherVariantCodeTable*>	m_vecSyncCodeTable;
		vector<CCypherVariantCodeTable*>	m_vecViewCodeTable;

		CCypherVariantCodeTable* GetSyncCodeTable(ECodeTableIndex eIndex)const;
		CCypherVariantCodeTable* GetViewCodeTable(ECodeTableIndex eIndex)const;
		size_t GetSyncCodeTableCount();
		size_t GetViewCodeTableCount();

		friend class CSyncCoreObjectDictator;
		friend class CCoreObjectDictator;
		friend class CCoreObjectFollower;
		string	m_sFileName;
	};	
}

