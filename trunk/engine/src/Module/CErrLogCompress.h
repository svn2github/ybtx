#pragma once
#include "ModuleDefs.h"
#include "ErrLogDefs.h"

namespace sqr
{
	class CErrLogCompress
	{
	public:
		CErrLogCompress();
		~CErrLogCompress();

		static CErrLogCompress& Inst();

		bool Compress(char* szDest, uint32* uDestLen, const char* szSource, uint32 uSourceLen);
		bool DeCompress(char* szDest, uint32* uDestLen, const char* szSource, uint32 uSourceLen);
		
		void SetCompressFun(CompressFun pFun);	
		
		void SetDeCompressFun(DeCompressFun pFun);

	private:
		
		CompressFun		m_pCompressFun;
		DeCompressFun	m_pDeCompressFun;
	};

}
