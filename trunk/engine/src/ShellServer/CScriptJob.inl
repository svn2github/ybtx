#pragma once
#include "CScriptJob.h"
#include "CScriptThread.h"
#include "CScriptResult.h"
#include "TTrThreadMsgBuffer.inl"

inline void* CScriptJob::operator new(size_t stSize, CScriptThread *pThread)
{
	return pThread->AllocLeftMsgMem(stSize);
}

inline void* CScriptJob::AllocMem(size_t stSize, CScriptThread *pThread)
{
	return pThread->AllocLeftMsgMem(stSize);
}


inline void* CScriptJob::CloneData(const void *pData, size_t stSize, CScriptThread *pThread)
{
	void* pCloneData = AllocMem(stSize, pThread);
	memcpy(pCloneData, pData, stSize);
	return pCloneData;
}

inline void CScriptJob::Add(CScriptThread* pThread)
{
	pThread->AddLeftMsg(this);
}

