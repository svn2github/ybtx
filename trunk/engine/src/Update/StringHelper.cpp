#include "stdafx.h"

std::wstring Utf8ToUCS2( const string& src )
{
	wstring result;

	for( uint8* pBuf = (uint8*)(&src[0]); *pBuf; )
	{		
		wchar_t cUCS2;
		if( ( ( pBuf[0] )&0x80 ) == 0 )
		{
			cUCS2 = pBuf[0];
			pBuf++;
		}
		else if( ( ( pBuf[0] )&0xE0 ) == 0xC0 )
		{
			cUCS2 = ( ( ( pBuf[0] )&0x1F )<<5 )|( pBuf[1]&0x3F );
			pBuf += 2;
		}
		else
		{
			cUCS2 = ( ( ( pBuf[0] )&0x0F )<<12 )|( ( pBuf[1]&0x3F ) << 6 )|( pBuf[2]&0x3F );
			pBuf += 3;
		}

		result.append(1, cUCS2);
	}

	return result;
}