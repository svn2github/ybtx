#pragma once
#include "CommonDefs.h"
#include "CMemData.h"
#include "CTrMsgMallocObject.h"
#include "CCypherVariantMallocObject.h"
#include "CBaseCypherVariantInputStream.h"
#include "CBaseCypherVariantOutputStream.h"


namespace sqr
{
	class CMemData;
	class CSqrAllocPool;

	template<typename AllocObject>
	class COMMON_API TVariantStream
		: public AllocObject
		, public CMemData
		, private CBaseCypherVariantInputStream
		, private CBaseCypherVariantOutputStream
	{
		friend class CSyncVariantClientHolder;
		friend class CCacheVariantServerHolder;
		friend class CTestSuiteVariant;

	public:
		TVariantStream();
		~TVariantStream();
		void Clear();
		CBaseCypherVariantInputStream& GetInputStream();
		CBaseCypherVariantOutputStream& GetOutputStream();

	private:
		void ReadData(void*& Value, size_t uLen);
		size_t RemainCount();
		void GetLine(string& Value);
		void Write(const char* sData, size_t uSize);
		void Finalize();

	private:	
		bool	m_bFinalize;
		size_t	m_uCurPos;
	};
}

