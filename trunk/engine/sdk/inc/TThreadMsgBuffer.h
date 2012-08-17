#pragma once
#include "CTrMsgMallocAllocator.h"

namespace sqr
{

	class CMemPool;

	template<typename MsgType>
	class TThreadMsgBuffer
	{
	public:
		TThreadMsgBuffer(size_t stMsgCount, size_t stMemBlock);
		~TThreadMsgBuffer();
		
		typedef vector<MsgType*, CTrMsgMallocAllocator<MsgType*> > MsgQueue_t;

		MsgQueue_t& GetMsgQueue();

		size_t GetMsgCount();

		void AddMsg(MsgType* pMsg);
	
		void* AllocMsgMem(size_t stSize);

		void ClearMsgBuffer();

		bool MsgQueueEmpty();
		
	private:
		MsgQueue_t					m_MsgQueue;
		
		size_t						m_stNextMsgIndex;

		CMemPool*					m_pMsgPool;

		size_t						m_stInitCount;
	};

}

