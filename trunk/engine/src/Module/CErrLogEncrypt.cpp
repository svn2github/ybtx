#include "stdafx.h"
#include "CErrLogEncrypt.h"

CErrLogEncrypt::CErrLogEncrypt()
: m_uEncodeNum(0)
, m_uDecodeNum(0)
{

}

CErrLogEncrypt::~CErrLogEncrypt()
{

}

CErrLogEncrypt& CErrLogEncrypt::Inst()
{
	static CErrLogEncrypt ls_Inst;
	return ls_Inst;
}

void CErrLogEncrypt::Encode(char* buf, uint32 uLen)
{
	for(uint32 i = 0; i < uLen; i++)
	{
		buf[i] ^= 3;
	}
}

void CErrLogEncrypt::Decode(char* buf, uint32 uLen)
{
	return Encode(buf, uLen);
}

