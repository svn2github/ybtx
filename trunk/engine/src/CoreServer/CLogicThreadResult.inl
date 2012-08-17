#pragma once
#include "CLogicThreadResult.h"
#include "CAoiThreadMgr.h"
#include "CAoi2LogicThreadMsgBuffer.h"
#include "CAoiJob.h"
#include "TTrThreadMsgBuffer.inl"

inline void* CLogicThreadResult::operator new(size_t stSize)
{
	return CAoi2LogicThreadMsgBuffer::Inst()->AllocRightMsgMem(stSize);
}

inline void* CLogicThreadResult::AllocMem(size_t stSize)
{
	return CAoi2LogicThreadMsgBuffer::Inst()->AllocRightMsgMem(stSize);
}

inline char* CLogicThreadResult::CloneString(const char *szStr)
{
	uint32 uLen = strlen(szStr);
	char* szString = (char*)AllocMem(uLen + 1);
	memcpy(szString, szStr, uLen);
	szString[uLen] = '\0';
	return szString;
}

inline void* CLogicThreadResult::CloneData(const void *pData, size_t stSize)
{
	void* pCloneData = AllocMem(stSize);
	memcpy(pCloneData, pData, stSize);
	return pCloneData;
}

inline void CLogicThreadResult::Add()
{
	CAoi2LogicThreadMsgBuffer::Inst()->AddRightMsg(this);
}
