#pragma once
#include "CStaticObject.h"
#include "CCypherVariantHolder.h"
#include <sstream>
#include "CVariantHolderDef.h"

namespace sqr
{
	class CCoreObjectDictator;
	//class CSyncCoreObjectDictator;
	class CCacheVariantServerHolder;
	class CCypherVariantCodeTable;
	class CSyncVariantServer;
	class CObjVarDefCfg;

	class CSyncVariantServerHolder
		:public virtual CStaticObject
		,private CCypherVariantHolder
	{
		friend class CCoreObjectDictator;
		friend class CSyncVariantServer;
		//friend class CSyncCoreObjectDictator;
	public:

		CSyncVariantServer* GetRootVariant() const;

		const char* GetString( const char* szPathName )const;
		CSyncVariantServer* SetString( const char* szPathName, const char* szValue);

		template<typename ValueType>
		ValueType GetNumber( const char* szPathName ) const;
		template<typename ValueType>
		CSyncVariantServer* SetNumber( const char* szPathName, ValueType Value);
		void SetDynRoot( const char* szPathName );
		void SetEmptyTable( const char* szPathName );

		bool IsEmptyTable( const char* szPathName )const;

		CSyncVariantServer* AddSubTree(const char* path, const char* subTreeName, const char* newName);
		bool DelMember(const char* szPathName);

	private:
		CSyncVariantServerHolder(uint32 uVarDefID,CCypherVariantCodeTable *pCodeTable,CCoreObjectDictator* pCoreObj,bool bForSync,ECodeTableIndex eCodeTableIndex);
		~CSyncVariantServerHolder(void);

		virtual CCypherVariant* CreateVariant() const;
		virtual CCypherVariant* IntAddSubTree(CCypherVariant* pRoot, const char* newName, uint32 uSubTreeType);

		CCoreObjectDictator*			m_pCoreObj;
		bool							m_bForSync:1;
		ECodeTableIndex					m_eCodeTableIndex;
		uint32							m_uVarDefID;

	};
}
