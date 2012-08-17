#include "stdafx.h"
#include "CErrLogCompress.h"
#include "bzip2/bzlib.h"

CErrLogCompress::CErrLogCompress()
: m_pCompressFun(NULL)
, m_pDeCompressFun(NULL)
{
	
}

CErrLogCompress::~CErrLogCompress()
{

}

CErrLogCompress& CErrLogCompress::Inst()
{
	static CErrLogCompress ls_Inst;
	return ls_Inst;
}

void CErrLogCompress::SetCompressFun(CompressFun pFunc)
{
	m_pCompressFun = pFunc;
}

void CErrLogCompress::SetDeCompressFun(DeCompressFun pFunc)
{
	m_pDeCompressFun = pFunc;
}

bool CErrLogCompress::Compress(char* szDest, uint32* uDestLen, const char* szSource, uint32 uSourceLen)
{
	if(!m_pCompressFun)
		return false;

	int ret = m_pCompressFun(szDest, uDestLen, 
		const_cast<char*>(szSource), uSourceLen, 9, 0, 30);

	if(ret != BZ_OK)
	{
		//printf("some error happen in compress log buffer\n");
		return false;
	}
	
	return true;
}

bool CErrLogCompress::DeCompress(char* szDest, uint32* uDestLen, const char* szSource, uint32 uSourceLen)
{
	if(!m_pDeCompressFun)
		return false;

	int ret = m_pDeCompressFun(szDest, uDestLen, 
		const_cast<char*>(szSource), uSourceLen, 0, 0);

	if(ret != BZ_OK)
	{
		//printf("some error happen in decompress log buffer");
		return false;
	}

	return true;
}

