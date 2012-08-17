#pragma once
#include "CLogicThreadJob.h"
#include "CAoiThreadMgr.h"
#include "CAoi2LogicThreadMsgBuffer.h"
#include "TTrThreadMsgBuffer.inl"

inline void* CLogicThreadJob::operator new(size_t stSize)
{
	return CAoi2LogicThreadMsgBuffer::Inst()->AllocLeftMsgMem(stSize);
}

inline void* CLogicThreadJob::AllocMem(size_t stSize)
{
	return CAoi2LogicThreadMsgBuffer::Inst()->AllocLeftMsgMem(stSize);
}

inline char* CLogicThreadJob::CloneString(const char *szStr)
{
	uint32 uLen = strlen(szStr);
	char* szString = (char*)AllocMem(uLen + 1);
	memcpy(szString, szStr, uLen);
	szString[uLen] = '\0';
	return szString;
}

inline void* CLogicThreadJob::CloneData(const void *pData, size_t stSize)
{
	void* pCloneData = AllocMem(stSize);
	memcpy(pCloneData, pData, stSize);
	return pCloneData;
}

inline void CLogicThreadJob::Add()
{
	CAoi2LogicThreadMsgBuffer::Inst()->AddLeftMsg(this);
}
