#pragma once

//////////////////////////////////////////////////////////////////////////
//summary:Md5 digest算法的实现类
//////////////////////////////////////////////////////////////////////////
#include "CommonDefs.h"

#ifndef _WIN32
#include "BaseTypes.h"
#endif

namespace sqr
{
	//<summary>md5 digester类。（md5签名算法）</summary>
	class COMMON_API CMd5Digester
	{
	private:
		uint	m_pCount[2];		/* message length in bits, lsw first */
		uint	m_pDigestBuffer[4];	/* digest buffer */
		uint8	m_pBuffer[64];		/* accumulate block */

		void Process(const uint8 *pData);
	public:

		//<summary>开始咀嚼（签名）。</summary>
		void Begin();


		//<summary>输入要咀嚼（签名）的内容。</summary>
		//<param name="pData">需要被咀嚼的数据。</param>
		//<param name="uLength">数据的长度。</param>


		void Append(const uint8 *pData,uint uLength);
		//<summary>结束咀嚼并获取内容的签名值。</summary>
		//<param name="pDigest">二进制的签名值。</param>
		void End(uint8 pDigest[16]);

		// 将得到的原始MD5码转换为BASE16格式的字符串
		void GetMD5Base16Str( char* pBase16 );

	};
}

