#pragma once
#include "CDynamicObject.h"
#include "TMtDbAllocator.h"

namespace sqr
{
	class CValueTable;

	class CDbContext
		:public virtual CDynamicObject
		,public CMtDbMallocObject
	{
		typedef basic_string<char,std::char_traits<char>,TMtDbAllocator<char> >	ReportString_t;
	public:		
		//下面两个函数应该是protected语义，但由于lua导出机制不支持protected,所以不得不写成public
		CDbContext();
		~CDbContext();
		void Clear();

		void SetValueTable(CValueTable* pValueTable);
		CValueTable* GetValueTable();

		void SetReport(const ReportString_t& strReport);
		const char* GetReport();

	private:
		CValueTable*		m_pValueTable;
		ReportString_t	m_strReport;
	};

}
