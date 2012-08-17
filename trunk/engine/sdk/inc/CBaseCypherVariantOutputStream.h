#pragma once

namespace sqr
{
	class CBaseCypherVariantOutputStream
	{
	public:
		virtual ~CBaseCypherVariantOutputStream(){}
		virtual void ReadData(void*& pData, size_t uLen) = 0;
		template<typename ValueType>
		void Read(ValueType*& Value, size_t uLen)
		{
			void* pData;
			ReadData(pData, uLen);
			Value = (ValueType*)pData;
		};
		virtual size_t RemainCount() = 0;
		virtual void GetLine(string& Value) = 0;
	};
}

