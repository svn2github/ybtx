#pragma once
#include "CPipeJob.h"
#include "CPipeThread.h"
#include "CPipeResult.h"
#include "TTrThreadMsgBuffer.inl"

inline void* CPipeJob::operator new(size_t stSize,CPipeThread* pThread)
{
	return pThread->AllocLeftMsgMem( stSize );
}

inline void* CPipeJob::AllocMem(size_t stSize, CPipeThread *pThread)
{
	return pThread->AllocLeftMsgMem(stSize);
}

inline char* CPipeJob::CloneString(const char *szStr, CPipeThread *pThread)
{
	size_t uLen = strlen(szStr);
	char* szString = (char*)AllocMem(uLen + 1, pThread);
	memcpy(szString, szStr, uLen);
	szString[uLen] = '\0';
	return szString;
}

inline void* CPipeJob::CloneData(const void *pData, size_t stSize, CPipeThread *pThread)
{
	void* pCloneData = AllocMem(stSize, pThread);
	memcpy(pCloneData, pData, stSize);
	return pCloneData;
}

inline void CPipeJob::Add(CPipeThread* pThread)
{
	pThread->AddLeftMsg(this);
}
