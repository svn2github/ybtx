#pragma once
#include "CommonDefs.h"
#include "CypherVariantDef.h"
#include "CVariantMallocObject.h"
#include "CCypherVariantMallocObject.h"
#include "TCypherVariantAllocator.h"
#include "TVariantAllocator.h"

namespace sqr
{
	class ICypherVariantHolderHandler;
	class CCypherVariantCodeTable;
	class CBaseCypherVariantCode;
	class CCypherVariant;
	class CCypherVariantDynCode;
	template<typename, template<typename>class>
	class TObjectBank;
	struct CCypherVariantMemberAdder;
	class CBaseCypherVariantOutputStream;
	class CBaseCypherVariantInputStream;

	class COMMON_API CCypherVariantHolder
		: public CCypherVariantMallocObject
	{
		friend struct CCypherVariantMemberAdder;
		typedef TObjectBank<CCypherVariant, TVariantAllocator> VariantPool;
	protected:

		CCypherVariantHolder(CCypherVariantCodeTable* pCodeTable);
		virtual ~CCypherVariantHolder();

		const char* GetString( const char* szPathName )const;
		CCypherVariant* SetString( const char* szPathName, const char* szValue, CBaseCypherVariantInputStream* strm );

		template<typename ValueType>
		ValueType GetNumber( const char* szPathName ) const;

		template<typename ValueType>
		CCypherVariant* SetNumber( const char* szPathName, ValueType Value,CBaseCypherVariantInputStream* strm );

		void SetEmptyTable( const char* szPathName, CBaseCypherVariantInputStream* strm );
		void SetDynRoot( const char* szPathName, CBaseCypherVariantInputStream* strm );

		bool IsEmptyTable( const char* szPathName )const;

		bool DelMember( const char* szPathName,CBaseCypherVariantInputStream* strm );

		CCypherVariant* AddSubTree(const char* path, const char* subTreeName, const char* newName);


		template<typename CypherVariantImp>
		CypherVariantImp* AcquireRootVariantFromPool(VariantPool* pPool);
		CCypherVariant* IntGetRootVariant()const;
		CCypherVariant* GetRootVariant()const;

		bool EnumMember(const char* path, void*, bool(*CallBack)(const void* pContext,const char*,CCypherVariant&))const;
		bool EnumMember(CCypherVariant* pVar, void*, bool(*CallBack)(const void* pContext,const char*,CCypherVariant&))const;

		virtual void Reset();
		CCypherVariant* BuildSubTree(CCypherVariant* pRoot, uint32 uSubTreeType, uint32 uSubTreeIndex, const char* szNodeName);
		void GetNameFromIndex(string& strSubTreeRootName, uint32 uSubTreeRootCodeIndex)const;
		virtual void OnMemberAdded(CCypherVariant* pVariant);

	protected:
		CCypherVariant* GetMember( const char* szPathName )const;
		CCypherVariant* GetMemberNoExp( const char* szPathName )const;
		CCypherVariant* SetMember( const char* szPathName, CBaseCypherVariantInputStream* pStream );

		template<typename CypherVariantImp>
		CCypherVariant* IntAddSubTree(CCypherVariant* pRoot, const char* newName, uint32 uSubTreeType);
		virtual CCypherVariant* IntAddSubTree(CCypherVariant* pRoot, const char* newName, uint32 uSubTreeType);
		void InitSubTree(CCypherVariant* pRoot, CCypherVariant* pSubTree, const char* szSubTreeRootName, uint32 uSubTreeRootCodeIndex, uint32 uSubTreeType);

		CCypherVariantCodeTable *m_pCodeTable;
		CBaseCypherVariantCode  *m_pDynCodeRoot;
		mutable CCypherVariant *m_pRootVariant;

	private:
		virtual CCypherVariant* CreateVariant() const;
		virtual CCypherVariant* IntSetMember( CCypherVariant* pVariant, const char* szNodeName, CBaseCypherVariantInputStream* pStream );

	};

	class CTestSuiteVariant;

	class COMMON_API CCypherVariantHolderForDecode
		: protected CCypherVariantHolder
	{
		friend class CTestSuiteVariant;
	protected:
		CCypherVariantHolderForDecode(CCypherVariantCodeTable* pCodeTable);
		virtual ~CCypherVariantHolderForDecode();

		inline void SetHandler(ICypherVariantHolderHandler*pHander)
		{
			m_pHandler = pHander;
		}
		inline ICypherVariantHolderHandler* GetHandler() const
		{
			return m_pHandler;
		}

		void Decode(CBaseCypherVariantOutputStream& strm);
		virtual void LogVariantValueChanged(const char* szPathName, uint32 uMsgLen);
	protected:
		void SetDynRootForDecode( const char* szPathName, CBaseCypherVariantOutputStream* strm );

		template<typename CypherVariantImp>
		CCypherVariant* IntAddSubTreeForDecode(CCypherVariant* pRoot, uint32 uSubTreeType, uint32 uSubTreeIndex);
		virtual CCypherVariant* IntAddSubTreeForDecode(CCypherVariant* pRoot, uint32 uSubTreeType, uint32 uSubTreeIndex);
		void InitVariantContainer();
		virtual void OnMemberAdded(CCypherVariant* pVariant);

		typedef vector<CCypherVariant*, TCypherVariantAllocator<CCypherVariant*> > VariantContainer;
		VariantContainer			m_VariantContainer;
	private:
		typedef deque<string, TCypherVariantAllocator<string> >	PathContainer;

		virtual CCypherVariant* IntSetMember( CCypherVariant* pVariant, const char* szNodeName, CBaseCypherVariantInputStream* pStream );

		void CallValueChangedCallback( CCypherVariant* pVariant );
		template<typename ValueType>
		ValueType ReadNumber(CBaseCypherVariantOutputStream& strm)const;
		CCypherVariant* GetMemberParent( PathContainer& PathElems );
		void GetPathElemsById( uint32 uId , PathContainer& PathElems ) const;
		void GetDecodeTarget(CBaseCypherVariantOutputStream& strm, ECypherVariantPathType eType, CCypherVariant*& pVariant)const;
		uint32 GetDecodeTargetPathFromStrm(CBaseCypherVariantOutputStream& strm, ECypherVariantPathType eType)const;

		ICypherVariantHolderHandler	*m_pHandler;
	};

}
