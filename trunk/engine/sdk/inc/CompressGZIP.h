#pragma once

#ifdef _WIN32
#ifndef _DEBUG
#pragma comment( lib, "zlib.lib" )
#else
#pragma comment( lib, "zlibd.lib" )
#endif
#endif

//压缩或者解压后的内容是被append到sOut中，因此请注意是否要在调用下列函数之前清空sOut

#define GZIP_MIN_DATA_SIZE	60

namespace sqr
{
	class CMemData;

	//return false表示压缩率太低，放弃压缩，sOut内容不会被修改
	void CompressBufferGZIP( CMemData& bufOut,const char* szData,size_t stSize );

	//return false数据无效
	bool DecompressBufferGZIP( char* szOutBuffer,size_t stOrgDataSize,const char* szData,size_t stSize );

}
