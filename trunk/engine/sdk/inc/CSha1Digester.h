//////////////////////////////////////////////////////////////////////////
//summary:sha1 digest算法的实现类
//author:黄舒怀
//////////////////////////////////////////////////////////////////////////
#ifndef _CSHA1DIGESTER_H
#define _CSHA1DIGESTER_H

#include "CommonDefs.h"

namespace sqr
{
namespace Digester
{
//<summary>sha1 digester类。（sha1签名算法）</summary>
// typedef should not be placed inside a namespace.  flying commented.
#ifndef _WIN32
typedef unsigned int		uint;
#endif
class COMMON_API CSha1Digester
{
private:
	uint	m_pState[5];
	uint	m_pCount[2];
	uint8	m_pBuffer[64];

	void Process(uint8 pBuffer[64]);
public:

	//<summary>开始咀嚼（签名）。</summary>
	void Begin();


	//<summary>输入要咀嚼（签名）的内容。</summary>
	//<param name="pData">需要被咀嚼的数据。</param>
	//<param name="uLength">数据的长度。</param>
	void Append(uint8* pData,uint uLength);


	//<summary>结束咀嚼并获取内容的签名值。</summary>
	//<param name="pDigest">二进制的签名值。</param>
	void End(uint8 pDigest[20]);
};
}
}

::Digester;

#endif /* _CSHA1DIGESTER_H */
