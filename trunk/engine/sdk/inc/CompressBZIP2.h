#pragma once

#ifdef _WIN32
#pragma comment( lib, "libsbz2.lib" )
#endif

//压缩或者解压后的内容是被append到sOut中，因此请注意是否要在调用下列函数之前清空sOut

namespace sqr
{

#define BZIP_MIN_DATA_SIZE	150

	class CMemData;

	void CompressBufferBZIP2( CMemData& buffOut,const char* szData,size_t stSize );

	bool DecompressBufferBZIP2( char* szOutBuffer,size_t stOrgDataSize,const char* szData,size_t stSize );

}
