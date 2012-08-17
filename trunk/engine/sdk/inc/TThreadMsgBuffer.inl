#pragma once
#include "TThreadMsgBuffer.h"
#include "CMemPool.h"
#include "ExpHelper.h"

template<typename MsgType>
TThreadMsgBuffer<MsgType>::TThreadMsgBuffer(size_t stMsgCount, size_t stMemBlock)
										 : m_stNextMsgIndex(0)
										 , m_stInitCount(stMsgCount)
{
	m_MsgQueue.resize(stMsgCount);

	m_pMsgPool = new CMemPool(stMemBlock,CTrMsgMallocObject::GetPool());
}


template<typename MsgType>
TThreadMsgBuffer<MsgType>::~TThreadMsgBuffer()
{
	delete m_pMsgPool;
}

template<typename MsgType>
inline void* TThreadMsgBuffer<MsgType>::AllocMsgMem(size_t stSize)
{
	return m_pMsgPool->Alloc(stSize);
}


template<typename MsgType>
inline void TThreadMsgBuffer<MsgType>::AddMsg(MsgType* pMsg)
{
	if(m_stNextMsgIndex == m_MsgQueue.size())
	{
		m_MsgQueue.push_back(pMsg);
	}
	else
	{
		m_MsgQueue[m_stNextMsgIndex] = pMsg;
	}

	++m_stNextMsgIndex;
}


template<typename MsgType>
inline typename TThreadMsgBuffer<MsgType>::MsgQueue_t& TThreadMsgBuffer<MsgType>::GetMsgQueue() 
{
	return m_MsgQueue;
}


template<typename MsgType>
inline void TThreadMsgBuffer<MsgType>::ClearMsgBuffer()
{
	m_stNextMsgIndex = 0;
	m_pMsgPool->Clear();
	m_MsgQueue.resize(m_stInitCount);
}



template<typename MsgType>
inline bool TThreadMsgBuffer<MsgType>::MsgQueueEmpty()
{
	return m_stNextMsgIndex == 0;
}


template<typename MsgType>
inline size_t TThreadMsgBuffer<MsgType>::GetMsgCount()
{
	return m_stNextMsgIndex;
}




