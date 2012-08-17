#pragma once
#include "CPipeResult.h"
#include "CPipeThread.h"
#include "CPipeJob.h"
#include "TTrThreadMsgBuffer.inl"

inline void* CPipeResult::operator new(size_t stSize, CPipeThread* pPool)
{
	return pPool->AllocRightMsgMem(stSize);
}


inline void* CPipeResult::AllocMem(size_t stSize, CPipeThread *pThread)
{
	return pThread->AllocRightMsgMem(stSize);
}

inline char* CPipeResult::CloneString(const char *szStr, CPipeThread *pThread)
{
	size_t uLen = strlen(szStr);
	char* szString = (char*)AllocMem(uLen + 1, pThread);
	memcpy(szString, szStr, uLen);
	szString[uLen] = '\0';
	return szString;
}

inline void* CPipeResult::CloneData(const void *pData, size_t stSize, CPipeThread *pThread)
{
	void* pCloneData = AllocMem(stSize, pThread);
	memcpy(pCloneData, pData, stSize);
	return pCloneData;
}

inline void CPipeResult::Add(CPipeThread* pThread)
{
	pThread->AddRightMsg(this);
}
