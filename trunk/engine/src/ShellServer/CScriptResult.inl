#pragma once
#include "CScriptResult.h"
#include "CScriptThread.h"
#include "CScriptJob.h"
#include "TTrThreadMsgBuffer.inl"

inline void* CScriptResult::operator new(size_t stSize, CScriptThread *pThread)
{
	return pThread->AllocRightMsgMem(stSize);
}

inline void* CScriptResult::AllocMem(size_t stSize, CScriptThread *pThread)
{
	return pThread->AllocRightMsgMem(stSize);
}

inline char* CScriptResult::CloneString(const char* str, CScriptThread *pThread)
{
	uint32 uLen = strlen(str);
	char* szString = (char*)AllocMem(uLen + 1, pThread);
	memcpy(szString, str, uLen);
	szString[uLen] = '\0';
	return szString;
}

inline void* CScriptResult::CloneData(const void *pData, size_t stSize, CScriptThread *pThread)
{
	void* pCloneData = AllocMem(stSize, pThread);
	memcpy(pCloneData, pData, stSize);
	return pCloneData;
}

inline void CScriptResult::Add(CScriptThread* pThread)
{
	pThread->AddRightMsg(this);
}

