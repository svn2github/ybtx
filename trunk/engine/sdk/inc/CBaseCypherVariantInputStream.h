#pragma once

namespace sqr
{
	class CBaseCypherVariantInputStream
	{
	public:
		virtual ~CBaseCypherVariantInputStream(){}
		virtual void Write(const char* sData, size_t uSize) = 0;
		virtual void Finalize() = 0;
		void operator << ( const char* sData )
		{
			Write(sData, strlen(sData));
		};
		virtual void Clear() = 0;
		void Str(const char* sData)
		{
			Write(sData, strlen(sData));
		};
	};
}

