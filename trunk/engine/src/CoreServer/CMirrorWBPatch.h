#pragma once
#include "CMirrorPatch.h"

namespace sqr
{
	class CMirrorBuffer;
	
	class CMirrorWBPatchMgr;

	template<typename T>
	class TMemPoolAllocator;

	class CMirrorWBPatch : public CMirrorPatch
	{
	public:
		void AddCmd(CMirrorPatchCmd* pCmd);
		void* AllocCmdMem(size_t stSize);
		uint64 GetVersion() const { return m_uVersion; }
	
	private:
		friend class CMirrorWBPatchMgr;

		typedef TMemPoolAllocator<CMirrorPatchCmd*>	MemPool_t;

		CMirrorWBPatch(uint64 uVersion, MemPool_t* pMemPool);
		~CMirrorWBPatch();

		MemPool_t* GetMemPool() { return m_pMemPool; }
		void Patch(CMirrorBuffer* pBuffer);

	private:
		MemPool_t*				m_pMemPool;
		uint64					m_uVersion;

		typedef deque<CMirrorPatchCmd*, MemPool_t>	DequeCmd_t;
		DequeCmd_t*				m_pQueCmd;

		
	};

}
