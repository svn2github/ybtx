#include "stdafx.h"
#include "Memory.h"
#include "ExpHelper.h"
#include "CSqrAllocPool.h"

namespace sqr
{
	static CSqrAllocPool* GetGlobalPool()
	{
		//所有的CSqrAllocPool都是不应该被删除的
		static CSqrAllocPool* ls_pPool=new CSqrAllocPool("GlobalPool" );
		return ls_pPool;
	}

	static const size_t gs_stMaxNewSize = 1024 * 1024 * 30;

	MODULE_API void* SqrNew(size_t stSize,CSqrAllocPool* pPool)
	{
		if(stSize > gs_stMaxNewSize)
		{
			ostringstream str;
			str << "分配的内存大小: " << stSize << " 超过限定的最大分配大小: " << gs_stMaxNewSize << endl;
			GenErr(str.str().c_str());
		}

		return SqrChunkNew( stSize , pPool );
	}

	MODULE_API void* SqrChunkNew(size_t stSize, CSqrAllocPool* pPool)
	{
		if(!pPool)
			pPool = GetGlobalPool();

		return pPool->Alloc(stSize);
	}

	MODULE_API void* SqrRenew(void* pMem,size_t stSize, CSqrAllocPool* pPool)
	{
		if(stSize > gs_stMaxNewSize)
		{
			ostringstream str;
			str << "再分配的内存大小: " << stSize << " 超过限定的最大分配大小: " << gs_stMaxNewSize << endl;
			GenErr(str.str().c_str());
		}

		return SqrChunkRenew( pMem, stSize, pPool );
	}

	MODULE_API void* SqrChunkRenew(void* pMem,size_t stSize, CSqrAllocPool* pPool)
	{
		if(!pPool)
			pPool = GetGlobalPool();

		return pPool->Realloc(pMem,stSize);
	}

	MODULE_API void SqrDelete(void* pMem, CSqrAllocPool* pPool)
	{
		if(!pPool)
			pPool = GetGlobalPool();

		return pPool->Dealloc(pMem);
	}

	MODULE_API size_t SqrGetMemSize(void* pMem)
	{
		return CSqrAllocPool::GetMemSize( pMem );
	}

}

