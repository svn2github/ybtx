#pragma once

namespace sqr
{
	// 将Uint8转为Base16, pBuf为空返回装载char的字符个数
	size_t Uint82Base16( const uint8* pUint8, size_t stSrcLen, char* pBase16, size_t sizeBuf );

	/**
	\brief
	将字符串转换成内存镜像的16进制字符串序列，如(竞技场介绍)->(bebabcbcb3a1bde9c9dc),
	这里起名为mbcs_to_MemH16,标识该函数主要用来将mbcs字符串转换为对应的内存16进制索引
	串，以索引对应的utf8格式的字符串
	*/
	std::string mbcs_to_MemH16(const std::string& src);


	/**
	\brief
	将字符串转换成内存镜像的64进制字符串序列，如(竞技场介绍)->(vrq8vLOhvenJ3A),
	这里起名为mbcs_to_MemH64,标识该函数主要用来将mbcs字符串转换为对应的内存64进制索引
	串，以索引对应的utf8格式的字符串
	*/
	std::string mbcs_to_MemH64(const std::string& src);
	std::string mbcs_to_MemH64(const char* src, uint32 length);

	/**
	\brief
	将字符串转的内存镜像64进制标识序列还原为mbcs字符串，如(vrq8vLOhvenJ3A)->(竞技场介绍),
	这里起名为MemH64_to_mbcs,标识该函数主要用来将内存64进制索引串还原为对应的mbcs字符串
	*/
	std::string MemH64_to_mbcs(const std::string& src);

	
	//下面这些函数我们不能放到module模块里面，因为返回的是string对象
	//会造成在dll里面分配但是在dll外面释放，创建和释放的new，delete不匹配
	// 将utf8转为unicode, pBuf为空返回装载Unicode字符的个数
	std::wstring& Utf8ToUCS2( std::wstring& dest, const std::string& src );
	std::wstring Utf8ToUCS2(const std::string& src);
	
	// 将Unicode转为utf8, pBuf为空返回装载char的字符个数
	std::string& UCS2ToUtf8( std::string& dest, const std::wstring& src );
	std::string UCS2ToUtf8(const std::wstring& src);

	/**
	\brief
		将utf8编码的unicode转换成utf16编码
	*/
	std::wstring& utf8_to_utf16(std::wstring& dest, const std::string& src);
	std::wstring  utf8_to_utf16(const std::string& src);

	/**
	\brief
		将utf16编码的unicode转换成utf8编码
	*/
	std::string& utf16_to_utf8(std::string& dest, const std::wstring& src);
	std::string  utf16_to_utf8(const std::wstring& src);

	/**
	\brief
		将mbcs编码的字符串转换成utf16编码
	*/
	bool			gbk_to_utf16(std::wstring& dest, const std::string& src);
	std::wstring	gbk_to_utf16(const std::string& src);

	/**
	\brief
		将utf16编码的字符串转换成mbcs编码
	*/
	bool		 utf16_to_gbk(std::string& dest, const std::wstring& src);
	std::string  utf16_to_gbk(const std::wstring& src);

	/**
	\brief
		将mbcs编码的字符串转换成utf8编码
	*/
	std::string gbk_to_utf8(const std::string& src);

	bool gbk_to_utf8(std::string& dest, const std::string& src);
	/**
	\brief
		将utf8编码的字符串转换成mbcs编码
	*/
	std::string utf8_to_gbk(const std::string& src);

	bool utf8_to_gbk(std::string& dest, const std::string& src);

	/**
	\brief
		代理mbcs_to_u8
	*/
	const char* U8(const std::string& src);
	

}
