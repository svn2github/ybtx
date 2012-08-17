#pragma once

namespace sqr
{
	class CMemData;

	//return false表示压缩率太低，放弃压缩，sOut内容不会被修改
	void CompressBufferLZO( CMemData& bufOut,const char* szData,size_t stSize );

	//return false数据无效
	bool DecompressBufferLZO( char* szOutBuffer,size_t stOrgDataSize,const char* szData,size_t stSize );

}
