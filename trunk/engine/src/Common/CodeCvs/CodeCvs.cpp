#include "stdafx.h"
#include "CodeCvs.h"
#include "UtfCvs.h"
#include "ExpHelper.h"

#ifdef _WIN32
#include <Windows.h>
#endif


namespace sqr
{
#ifndef _WIN32
	bool code_convert(const char* inCharSet,char* inChar,size_t inLen,const char* outCharSet,char* outChar,size_t outLen)
	{
		iconv_t cd = iconv_open(outCharSet,inCharSet);
		if(cd == (iconv_t)(-1))
		{
			return false;
		}
		memset(outChar,0,outLen);
		if((int)iconv(cd,&inChar,&inLen,&outChar,&outLen)==-1)
		{
			return false;
		}
		iconv_close(cd);
		return true;
	}
#endif

	char RadixTable[65] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
											'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
											'0','1','2','3','4','5','6','7','8','9','+','-','='
										  };

	size_t Uint82Base16( const uint8* pUint8, size_t stSrcLen, char* pBase16, size_t sizeBuf )
	{
		size_t nPos = 0;
		for( ; nPos < stSrcLen; ++nPos )
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

		return 2 * nPos;
	}


	std::string mbcs_to_MemH16(const std::string& src)
	{
		string strResult;
		int length = static_cast<int>(src.length());
		char oneChar;
		char destChar[2];
		destChar[1] = '\0';
		for (int i=0;i<length;++i)
		{
			oneChar = src[i];
			sprintf(destChar,"%x",static_cast<int>(oneChar>>4&0x0f));
			strResult = strResult + destChar;
			sprintf(destChar,"%x",static_cast<int>(oneChar&0x0f));
			strResult = strResult + destChar;
		}
		return strResult;
	}


	std::string mbcs_to_MemH64( const char* src, uint32 length )
	{
		int moreCount = length%3;
		int resultSize = length;
		if (moreCount)
			resultSize = (resultSize+3-moreCount)*4/3 + moreCount - 3;
		else
			resultSize = resultSize*4/3;
		string strResult;
		strResult.resize(resultSize);
		for (uint32 i=0;i<length-moreCount;)
		{
			uint8 one =  static_cast<uint8>((src[i]&0xfc)>>2);
			uint8 two =  static_cast<uint8>((src[i]&0x03)<<4) +  static_cast<uint8>((src[i+1]&0xf0)>>4);
			uint8 three =  static_cast<uint8>((src[i+1]&0x0f)<<2) +  static_cast<uint8>((src[i+2]&0xc0)>>6);
			uint8 four =  static_cast<uint8>(src[i+2]&0x3f);
			strResult[(i/3)*4] = RadixTable[one];
			strResult[(i/3)*4+1] = RadixTable[two];
			strResult[(i/3)*4+2] = RadixTable[three];
			strResult[(i/3)*4+3] = RadixTable[four];
			i += 3;
		}
		if(moreCount==2)
		{
			uint8 one =  static_cast<uint8>((src[length-2]&0xfc)>>2);
			uint8 two =  static_cast<uint8>((src[length-2]&0x03)<<4) +  static_cast<uint8>((src[length-1]&0xf0)>>4);
			uint8 three =  static_cast<uint8>((src[length-1]&0x0f)<<2) +  static_cast<uint8>((0&0xc0)>>6);
			strResult[resultSize-3] = RadixTable[one];
			strResult[resultSize-2] = RadixTable[two];
			strResult[resultSize-1] = RadixTable[three];
		}
		else if(moreCount ==1)
		{
			uint8 one =  static_cast<uint8>((src[length-1]&0xfc)>>2);
			uint8 two =  static_cast<uint8>((src[length-1]&0x03)<<4) +  static_cast<uint8>((0&0xf0)>>4);
			strResult[resultSize-2] = RadixTable[one];
			strResult[resultSize-1] = RadixTable[two];
		}
		return strResult;
	}

	std::string mbcs_to_MemH64(const std::string& src)
	{
		if (src == "")
			return src;

		return mbcs_to_MemH64(src.c_str(), (uint32)(src.length()));
	}

	int GetRadix64CharPos(char testChar)
	{
		if (testChar>='A' && testChar <= 'Z')
		{
			return testChar-'A';
		}
		else if (testChar>='a' && testChar <= 'z')
		{
			return 26+testChar-'a';
		} 
		else if (testChar>='0' && testChar <= '9')
		{
			return 52+testChar-'0';
		}
		else if (testChar == '+')
		{
			return 62;
		}else if (testChar == '-')
		{
			return 63;
		}
		else
		{
			return 64;				//返回64标识错误，所查找的字符串并非由mbcs_to_MemH64函数所生成
		}
	}

	std::string MemH64_to_mbcs(const std::string& src)
	{
		if (src == "")
			return src;
		int length = static_cast<int>(src.length());
		int moreCount = length%4;
		int resultSize = length;
		if (moreCount)
		{
			resultSize = (resultSize+4-moreCount)*3/4 + moreCount - 4;
		}
		else
		{
			resultSize = resultSize*3/4;
		}
		string strResult;
		strResult.resize(resultSize);
		for (int i=0;i<length-moreCount;)
		{
			uint8 one   = GetRadix64CharPos(src[i]);
			uint8 two   = GetRadix64CharPos(src[i+1]);
			uint8 three = GetRadix64CharPos(src[i+2]);
			uint8 four   = GetRadix64CharPos(src[i+3]);
			strResult[i*3/4]		= static_cast<uint8>((one&0x3f)<<2) + static_cast<uint8>((two&0x30)>>4);
			strResult[i*3/4+1] = static_cast<uint8>((two&0x0f)<<4) + static_cast<uint8>((three&0x3c)>>2);
			strResult[i*3/4+2] = static_cast<uint8>((three&0x03)<<6) + static_cast<uint8>(four&0x3f);
			i += 4;
		}
		if(moreCount==2)
		{
			uint8 one = GetRadix64CharPos(src[length-2]);
			uint8 two = GetRadix64CharPos(src[length-1]);
			strResult[resultSize-1] = static_cast<uint8>((one&0x3f)<<2) + static_cast<uint8>((two&0x30)>>4);
		}
		else if(moreCount ==3)
		{
			uint8 one   = GetRadix64CharPos(src[length-3]);
			uint8 two   = GetRadix64CharPos(src[length-2]);
			uint8 three = GetRadix64CharPos(src[length-1]);
			strResult[resultSize-2] = static_cast<uint8>((one&0x3f)<<2) + static_cast<uint8>((two&0x30)>>4);
			strResult[resultSize-1] = static_cast<uint8>((two&0x0f)<<4) + static_cast<uint8>((three&0x3c)>>2);
		}
		return strResult;
	}


	std::wstring& Utf8ToUCS2( std::wstring& dest, const string& src )
	{
		dest = L"";

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
				cUCS2 = ( ( ( pBuf[0] )&0x1F )<<6 )|( pBuf[1]&0x3F );
				pBuf += 2;
			}
			else
			{
				cUCS2 = ( ( ( pBuf[0] )&0x0F )<<12 )|( ( pBuf[1]&0x3F ) << 6 )|( pBuf[2]&0x3F );
				pBuf += 3;
			}

			dest.append(1, cUCS2);
		}

		return dest;
	}

	std::wstring Utf8ToUCS2(const std::string& src)
	{
		wstring dest;
		return Utf8ToUCS2(dest, src);
	}
	

	std::string& UCS2ToUtf8( std::string& dest, const wstring& src )
	{
		dest = "";

		for ( size_t i=0; i<src.length(); ++i)
		{
			wchar_t cUCS2 = src[i];
			char	cUTF8 = 0;
			if( cUCS2 < 0x80 )
			{
				cUTF8 = (char)cUCS2;
				dest.append(1, cUTF8);
			}
			else if( cUCS2 < 0x07FF )
			{
				cUTF8			= ( cUCS2 >> 6 )|0xC0;
				dest.append(1, cUTF8);
				cUTF8			= ( cUCS2&0x3f )|0x80;
				dest.append(1, cUTF8);
			}
			else
			{
				cUTF8			= ( cUCS2 >> 12 )|0xE0;
				dest.append(1, cUTF8);
				cUTF8			= ( ( cUCS2 >> 6 )&0x3f )|0x80;
				dest.append(1, cUTF8);
				cUTF8			= ( cUCS2&0x3f )|0x80;
				dest.append(1, cUTF8);
			}
		}

		return dest;
	}

	std::string UCS2ToUtf8(const std::wstring& src)
	{
		string dest;
		return UCS2ToUtf8(dest, src);
	}


	std::wstring& utf8_to_utf16(std::wstring& dest, const std::string& src)
	{
		Utf8ToUCS2(dest, src);
		return dest;
	}

	std::wstring utf8_to_utf16(const std::string& src)
	{
		return Utf8ToUCS2(src);
	}
	
	std::string& utf16_to_utf8( std::string& dest, const std::wstring& src )
	{
		UCS2ToUtf8(dest, src);
		return dest;
	}

	std::string utf16_to_utf8(const std::wstring& src)
	{
		return UCS2ToUtf8(src);
	}

	bool gbk_to_utf16( std::wstring& dest, const std::string& src )
	{
#ifdef _WIN32
		dest.resize(src.size());
		const string old_locale = setlocale(LC_CTYPE, NULL);
		setlocale(LC_CTYPE, ".936");
		size_t unicodeLength = mbstowcs(const_cast<wchar_t*>(dest.data()), src.c_str(), src.size());
		setlocale(LC_CTYPE, old_locale.c_str());
		if (unicodeLength == -1)
		{
			dest = L"";
			return false;
		}
		dest.resize(unicodeLength);
#else
		string strTmp;
		if (!gbk_to_utf8(strTmp, src))
			return false;
		utf8_to_utf16(dest, strTmp);
		if (dest==L"")
		{
			return false;
		}
#endif
		return true;
	}
	
	std::wstring gbk_to_utf16(const std::string& src)
	{
		wstring strTemp;
		gbk_to_utf16(strTemp,src);
		return strTemp;
	}

	
	bool utf16_to_gbk( std::string& dest, const std::wstring& src )
	{
#ifdef _WIN32
		dest.resize(src.size()*2);
		const string old_locale = setlocale(LC_CTYPE, NULL);
		setlocale(LC_CTYPE, ".936");
		size_t mbcsLength = wcstombs(const_cast<char*>(dest.data()), src.c_str(), dest.size());
		setlocale(LC_CTYPE, old_locale.c_str());
		if (mbcsLength == -1)
		{
			dest = "";
			return false;
		}
		dest.resize(mbcsLength);
		return true;
#else
		string strTmp;
		utf16_to_utf8(strTmp, src);
		if (dest=="")
		{
			return false;
		}
		return utf8_to_gbk(dest, strTmp);
#endif
	}

	std::string utf16_to_gbk(const std::wstring& src)
	{
		string strTemp;
		utf16_to_gbk(strTemp,src);
		return strTemp;
	}

	bool	 gbk_to_utf8(std::string& dest, const std::string& src)
	{
#ifdef _WIN32
		wstring wcs_tmp;
		if (!gbk_to_utf16(wcs_tmp,src))
		{
			return false;
		}
		utf16_to_utf8(dest, wcs_tmp);
		if (dest=="")
		{
			return false;
		}
#else
		size_t inleft =  src.length() + 1;
		size_t outleft = inleft*4;
		char* pout = new char[outleft];

		if (!code_convert("CP936",const_cast<char*>(src.data()),inleft,"UTF-8",pout,outleft))
		{
			delete[] pout;
			return false;
		}
		else
		{
			dest.resize(strlen(pout));
			dest = pout;
			delete[] pout;
		}
#endif
		return true;
	}

	std::string gbk_to_utf8( const std::string& src )
	{
		string strTemp;
		gbk_to_utf8(strTemp,src);
		return strTemp;
	}

	bool utf8_to_gbk(std::string& dest, const std::string& src)
	{
#ifdef _WIN32
		wstring wcs_tmp;
		utf8_to_utf16(wcs_tmp, src);
		if (wcs_tmp==L"")
		{
			return false;
		}
		return utf16_to_gbk(dest, wcs_tmp);
#else
		size_t inleft =  src.length() + 1;
		size_t outleft = inleft;
		char* pout = new char[outleft];

		if (!code_convert("UTF-8",const_cast<char*>(src.data()),inleft,"CP936",pout,outleft))
		{
			delete[] pout;
			return false;
		}
		else
		{
			dest.resize(strlen(pout));
			dest = pout;
			delete[] pout;
		}
#endif
		return true;
	}

	std::string utf8_to_gbk( const std::string& src )
	{
		string strTemp;
		utf8_to_gbk(strTemp,src);
		return strTemp;
	}

	const char* U8( const std::string& src )
	{
		static string szTemp;
		szTemp.empty();
		gbk_to_utf8(szTemp, src);
		return szTemp.c_str();
	}
}
