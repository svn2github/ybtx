#pragma once 
#include "ModuleDefs.h"

namespace sqr
{

	#define _LOG_USE_ENCRYPT   //是否使用加密，仅用于开发期，实际运营的时候log绝对会加密的

	//errlog的加密解密类
	class CErrLogEncrypt
	{
	public:
		CErrLogEncrypt();
		~CErrLogEncrypt();

		static CErrLogEncrypt& Inst();

		//对输入的字串进行加密与解密
		void Encode(char* buf, uint32 uLen);
		void Decode(char* buf, uint32 uLen);

		
	private:
		uint32 m_uEncodeNum;
		uint32 m_uDecodeNum;
	};

}

