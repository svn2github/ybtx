#include "stdafx.h"
#include "UtfCvs.h"
#include "ExpHelper.h"

#ifdef _WIN32
#include <Windows.h>
#endif

//	UNICODE UTF-8 
//	00000000 - 0000007F 0xxxxxxx 
//	00000080 - 000007FF 110xxxxx 10xxxxxx 
//	00000800 - 0000FFFF 1110xxxx 10xxxxxx 10xxxxxx 
//	00010000 - 001FFFFF 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx 
//	00200000 - 03FFFFFF 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 
//	04000000 - 7FFFFFFF 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 

namespace sqr
{

	size_t Utf8ToUCS2( const char* pUtf8, wchar_t* pUCS2, size_t sizeBuf )
	{	
		size_t nPos = 0;
		for( uint8* pBuf = (uint8*)pUtf8; *pBuf; nPos++ )
		{		
			wchar_t cUCS2;
			if( ( ( pBuf[0] )&0x80 ) == 0 )
			{
				cUCS2 = pBuf[0];
				pBuf++;
			}
			else if( ( ( pBuf[0] )&0xE0 ) == 0xC0 )
			{
				cUCS2 = ( ( ( pBuf[0] )&0x1F )<<6 )|( pBuf[1]&0x3F );
				pBuf += 2;
			}
			else
			{
				cUCS2 = ( ( ( pBuf[0] )&0x0F )<<12 )|( ( pBuf[1]&0x3F ) << 6 )|( pBuf[2]&0x3F );
				pBuf += 3;
			}

			if( pUCS2 )
			{
				if( nPos >= sizeBuf )
					break;
				pUCS2[nPos] = cUCS2;
			}
		}

		if( nPos < sizeBuf && pUCS2 )
			pUCS2[nPos] = 0;

		return nPos;
	}


	size_t UCS2ToUtf8( const wchar_t* pUCS2, char* pUtf8, size_t sizeBuf )
	{
		size_t nPos = 0;
		while( *pUCS2 )
		{
			wchar_t cUCS2 = *pUCS2++;
			if( cUCS2 < 0x80 )
			{
				if( pUtf8 )
				{
					if( nPos >= sizeBuf )
						break;
					pUtf8[nPos] = (char)cUCS2;
				}
				nPos++;
			}
			else if( cUCS2 < 0x07FF )
			{
				if( pUtf8 )
				{
					if( nPos + 1 >= sizeBuf )
						break;
					pUtf8[nPos]		= ( cUCS2 >> 6 )|0xC0;
					pUtf8[nPos+1]	= ( cUCS2&0x3f )|0x80;
				}
				nPos += 2;
			}
			else
			{
				if( pUtf8 )
				{
					if( nPos + 2 >= sizeBuf )
						break;
					pUtf8[nPos]		= ( cUCS2 >> 12 )|0xE0;
					pUtf8[nPos+1]	= ( ( cUCS2 >> 6 )&0x3f )|0x80;
					pUtf8[nPos+2]	= ( cUCS2&0x3f )|0x80;
				}
				nPos += 3;
			}
		}

		if( nPos < sizeBuf && pUtf8 )
			pUtf8[nPos] = 0;

		return nPos;
	}


	char* U16_to_U8(const wchar_t* wzSrc)
	{
		size_t stLen=wcslen(wzSrc);
		char* dest = NULL;
		size_t dest_len = stLen * 2 + 1 ;
		dest = (char*)malloc(dest_len);

		UCS2ToUtf8(wzSrc, dest, dest_len);
		return dest;		
	}

	wchar_t* U8_to_U16(const char* szSrc)
	{
		size_t stLen=strlen(szSrc);
		wchar_t* dest = NULL;
		size_t dest_len = stLen + 1;
		dest = (wchar_t*)malloc(sizeof(wchar_t) * dest_len);

		Utf8ToUCS2(szSrc, dest, dest_len);
		return dest;
	}

#ifdef _WIN32
	char* U16_to_MB(const wchar_t* wzSrc)
	{
		size_t stLen=wcslen(wzSrc);
		size_t dest_len = stLen * 2 + 1;
		char* dest = (char*)malloc(dest_len);

		const string old_loc = setlocale(LC_CTYPE, NULL);
		setlocale(LC_CTYPE, ".936");
		size_t mbcsLength = wcstombs(dest, wzSrc, dest_len);
		setlocale(LC_CTYPE, old_loc.c_str());

		if (mbcsLength == -1)
		{
			DbgGenErr("CharsetCvs Error", "U16_to_MB() failed.");
			free(dest);
			return NULL;
		}

		return dest;
	}
#endif

	void FreeUtfCvsDest(void* dest)
	{
		free(dest);
	}
}


