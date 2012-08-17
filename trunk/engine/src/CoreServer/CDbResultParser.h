#pragma once
#include "TMtDbAllocator.h"

namespace sqr
{
	class IDbStatement;
	class IDbTextResult;
	class IDbResultBinder;
	class CValueTable;

	class CValueTable;

	class CDbResultParser
		:public CMtDbMallocObject
	{
	public:
		static CValueTable* ParseStatementResult(IDbStatement* pStatement);
		static CValueTable* ParseTextResult(IDbTextResult* pStatement);


		typedef std::basic_string<char,std::char_traits<char>,TMtDbAllocator<char> >	ColNameStr_t;
	};

}
