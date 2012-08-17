#pragma once
#include "VariantTypes.h"
#include "TVariantTraits.h"
#include "ExpHelper.h"
#include "CommonDefs.h"
#include <iosfwd>
#include "CypherVariantDef.h"
#include "CCypherVariantMallocObject.h"
#include "TCypherVariantAllocator.h"


namespace sqr
{
	class CCypherVariantCodeTable;
	class CCypherVariantHolder;
	class CBaseCypherVariantCode;
	class CCypherVariant;
	class CBaseCypherVariantInputStream;

	
	class COMMON_API CCypherVariant
		:public CCypherVariantMallocObject
	{
		friend class CTestSuiteVariant;
		friend class CCypherVariantHolder;
		friend class CCypherVariantHolderForDecode;
		friend class CDynamicCypherVaraint;
		friend class CBaseCypherVariantInputStream;

		typedef vector<CCypherVariant*, TCypherVariantAllocator<CCypherVariant*> > MemberMap_t;

	private:
		CCypherVariant(const CCypherVariant& other);
	public:

		EVariantType GetType() const 
		{
			return m_eType;
		}

		bool IsTable()const;

		bool IsEmptyTable() const;
		//用整個Variant的內容產生字節流，用于網絡同步。
		void Encode(CBaseCypherVariantInputStream& strm);

		template<typename ValueType>
		ValueType GetNumber() const;

		template<typename ValueType>
		void SetNumber(ValueType Value,CBaseCypherVariantInputStream* strm=NULL);

		void SetString(const char* szValue,CBaseCypherVariantInputStream* strm=NULL);
		const char* GetString()const;

		void SetEmptyTable(CBaseCypherVariantInputStream* pStrm=NULL);

		CCypherVariant* SetMember(const char* szName,CBaseCypherVariantInputStream* strm=NULL);
		CCypherVariant* GetMember(const char* szName)const;
		bool DelMember(const char* szName,CBaseCypherVariantInputStream* strm=NULL);
		
		CCypherVariant* GetRoot()const;
		CCypherVariant* GetParent()const;

		const char* GetName() const;
		inline const CBaseCypherVariantCode* GetCode() const
		{
			return m_pCode;
		}

		bool EnumMember(const void* pContext,bool(*)(const void* pContext,const char* szKey,CCypherVariant& Member))const;

		template < typename FuncType >
		void ForEachMember( FuncType Func )
		{
			MemberMap_t* pMemberMap = GetMemberMap();
			if ( GetType() != eVT_Table || pMemberMap == NULL)
			{
				return;
			}
			for ( MemberMap_t::iterator it = pMemberMap->begin(); it != pMemberMap->end(); ++it )
			{
				Func(  *it  );
			}
		}
		virtual ~CCypherVariant();

	protected:
		CBaseCypherVariantCode* m_pCode;

		CCypherVariant();
		void WritePath(CBaseCypherVariantInputStream& strm);

		CCypherVariant* SetMemberForDecode(const char* szName, CBaseCypherVariantInputStream* strm=NULL);
		CCypherVariant* SetMemberForDecode(CBaseCypherVariantCode* pCode, CBaseCypherVariantInputStream* strm=NULL);
		void SetEmptyTableForDecode(CBaseCypherVariantInputStream* pStrm);

		//void InitAllMember(bool bEncode);			//根据自身code创建子Variant
		virtual void ResetDataMember();

		virtual void RecycleSubTree(){}
		template<typename CypherVariantImp>
		void IntRecycleSubTree();
		CCypherVariant* GetMemberByIndex(uint32 uIndex)const;

		bool IsValid()const;

	private:
		CCypherVariant* SetMemberWithCode(CBaseCypherVariantCode* pCode);
		void SetMemberWithVariant(CCypherVariant* pVariant);

		CCypherVariant* InnerSetMember(const char* szName);
		CCypherVariant* InnerSetMember(CBaseCypherVariantCode* pCode);

		CCypherVariant* InnerSetMember(const char* szName,bool bEncode,CBaseCypherVariantInputStream* strm=NULL);
		CCypherVariant* InnerSetMember(CBaseCypherVariantCode* pCode,bool bEncode,CBaseCypherVariantInputStream* strm=NULL);

		CCypherVariant* IntSetMember(const char* szName,CBaseCypherVariantInputStream* strm=NULL);

		CCypherVariant* IntGetMember(const char* szName)const;
		CCypherVariant* IntGetMemberByIndex(uint32 uIndex)const;

		bool IntDelMember(const char* szName,CBaseCypherVariantInputStream* strm);

		bool IntEnumMember(const void* pContext,bool(*)(const void* pContext,const char* szKey,CCypherVariant& Member))const;

		void IntSetEmptyTable(CBaseCypherVariantInputStream* pStrm);

		void IntSetType(EVariantType eType) { m_eType = eType; }
		bool IntIsType(EVariantType eType) const { return m_eType == eType; }

		MemberMap_t* GetMemberMap();
		const MemberMap_t* GetMemberMap() const;

		void SetMemberMap(MemberMap_t* pMemberMap);

		void DoEncode(CBaseCypherVariantInputStream& strm);

		void CheckVariableName(const char* szName) const;

		template<typename CodeType>void WriteCypherCode(CBaseCypherVariantInputStream& strm);

		void WriteCypherPath(CBaseCypherVariantInputStream& strm);

		void WriteHeaderAndPath(CBaseCypherVariantInputStream& strm,ECypherVariantAction eAction);

		virtual CCypherVariant* CreateVariant()const;

		template<typename ValueType>void WriteNumber(CBaseCypherVariantInputStream& strm);

		void FreeDataMem();
		void DelVariant(CCypherVariant* pVariant);

		char* IntGetString();
		void IntFreeString();

		CCypherVariant*		m_pParent;
		void*				m_pData;
		char				m_szNumBuffer[sizeof(uint64)];
		bool				m_bDataNotInit;

		EVariantType		m_eType;
	};
}
