#pragma once
#include "CStaticObject.h"
#include "VariantTypes.h"
#include "CMtDbMallocObject.h"

namespace sqr
{
	class CValueTable;
	class CVariant;
	class CValueTableData;

	class CDbResultSet
		: public virtual CStaticObject
		, public CMtDbMallocObject
	{
	public:
		CDbResultSet();
		~CDbResultSet();

		void SetValueTable(CValueTable *pValueTable);
		CValueTable* GetValueTable();
		EVariantType GetType(uint32 column) ;
		const char* GetString(uint32 row, uint32 column) ;
		bool IsNull(uint32 row, uint32 column) ;

		EVariantType GetType(const char* szColumnName) ;
		const char* GetString(uint32 row, const char* szColumnName) ;
		bool IsNull(uint32 row, const char* szColumnName) ;

		template<typename ValueType>ValueType GetNumber(uint32 uRow, uint32 uCol);
		template<typename ValueType>ValueType GetNumber(uint32 uRow, const char* szName);

		CVariant* GetValue(CValueTableData* pData, uint32 uRow, uint32 uCol);
		CVariant* GetValue(CValueTableData* pData, uint32 uRow, const char* szName);

		uint32 GetRowNum() ;
		uint32 GetColNum() ;

		void Release();

	private:
		CValueTable*	m_pValueTable;
	};
}

