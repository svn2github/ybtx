#pragma once 
#include "CDataFileMallocObject.h"
#include "CVariant.h"
#include "CStaticObject.h"

namespace sqr 
{
	class CVariant;

	class CTableFileValue : public CStaticObject
						  , public CDataFileMallocObject
	{
	public:
		CTableFileValue();
		~CTableFileValue();

		CVariant* GetValue() { return &m_Value; }

		void SetNumber(double value);
		void SetCStr(const char* str);
		void SetPtr(CBaseObject* pObject);

	private:
		CVariant	m_Value;
	};

}
