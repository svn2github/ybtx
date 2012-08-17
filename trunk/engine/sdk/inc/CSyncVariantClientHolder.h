#pragma once
#include "CCypherVariantHolder.h"
#include "CStaticObject.h"
#include "CVariantHolderDef.h"

namespace sqr
{
	class CSyncVariantClient;
	class CSyncVariantClientHolder
		: public virtual CStaticObject
		, private CCypherVariantHolderForDecode
	{
		friend class CCoreObjectFollower;
	public:
		CSyncVariantClientHolder(uint32 uVarDefID, CCypherVariantCodeTable *pCodeTable, bool bForSync, ECodeTableIndex eCodeTableIndex);
		~CSyncVariantClientHolder();
		CSyncVariantClient* GetRootVariant()const;
		
		const char* GetString( const char* szPathName )const;
		
		template<typename ValueType>
		ValueType GetNumber( const char* szPathName ) const;

		bool IsEmptyTable( const char* szPathName )const;

		template<typename CmdType>
		bool Decode(const CmdType* pCmd, bool bCheckDecodeTimes, const char* szVariantTypeName);
		void LogVariantValueChanged(const char* szPathName, uint32 uMsgLen);

		void Reset();
		void RecycleSubTree();

		uint32 GetDecodeTimes()const;
		uint32 GetRealDecodeTimes()const;
		void SetRealDecodeTimes(uint32 uChangeTimes);
		void EnableDecode(bool bEnableDecode);

		bool EnumVariant(const char* path, bool(*)(const void*, const char*, CCypherVariant&))const;
	private:
		virtual CCypherVariant* CreateVariant() const;
		virtual CCypherVariant* IntAddSubTreeForDecode(CCypherVariant* pRoot, uint32 uSubTreeType, uint32 uSubTreeIndex);

		uint8	m_uDecodeTimes;
		uint32	m_uRealDecodeTimes;
		bool	m_bForSync;
		ECodeTableIndex m_eCodeTableIndex;
		uint32	m_uVarDefID;
		bool	m_bDecodeBlock;
	};

}
