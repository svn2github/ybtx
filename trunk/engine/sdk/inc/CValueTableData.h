#pragma once 
#include "CValueMallocObject.h"
#include "CVariant.h"
#include "CStaticObject.h"

namespace sqr 
{
	class CValueTable;

	class CValueTableData
		: public CStaticObject
		, public CValueMallocObject
	{
	public:
		CValueTableData();
		~CValueTableData();

	private:
		CVariant* GetData() { return &m_Data; }

		friend class CValueTable;

		void SetData(CValueTable* pValueTable, uint32 uRow, uint32 uCol);

	private:
		CVariant	m_Data;

	};
}
