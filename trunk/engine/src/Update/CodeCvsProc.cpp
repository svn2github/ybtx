#include "stdafx.h"
#include "CodeCvsProc.h"

size_t Uint82Base16( const uint8* pUint8, size_t stSrcLen, char* pBase16, size_t sizeBuf )
{
	size_t nPos = 0;
	for( uint8* pBuf = (uint8*)pUint8; nPos < stSrcLen; ++nPos )
	{
		if(pBase16)
		{
			if( 2 * nPos + 1>= sizeBuf )
				break;
			pBase16[2 * nPos + 1] = pUint8[nPos] & 0x0f;
			pBase16[2 * nPos] = pUint8[nPos]>>4;
			pBase16[2 * nPos + 1] += pBase16[2 * nPos + 1] < 10 ? '0' : 'a' - 10;
			pBase16[2 * nPos] += pBase16[2 * nPos] < 10 ? '0' : 'a' - 10;
		}
	}
	if(pBase16 && 2 * nPos < sizeBuf)
		pBase16[2 * nPos] = '\0';

	return 2 * nPos + 1;
}

