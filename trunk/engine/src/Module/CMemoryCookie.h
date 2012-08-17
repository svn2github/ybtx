#pragma once


namespace sqr
{
	class CThreadAllocPool;
	class CMemCallStackInfo;

#pragma pack(push,1)
	struct CMemoryCookie
	{
		//这个union必须放在最前面，因为对m_pNext的原子操作要求必须8字节（32bit）对齐。
		//dlmalloc分配出来的内存的首地址符合这个要求，所以这些变量要放在首地址的位置
		union
		{
			CMemoryCookie*volatile		m_pNext;//这个指针仅在跨线程释放内存的时候使用
			CMemCallStackInfo*			m_pStackInfo;
			size_t 						m_uCheckSum;			
		};

		size_t				m_stSize;
		CThreadAllocPool*	m_pPool;
	};
#pragma pack(pop)

}
