#pragma once
#include "CAoiResult.h"
#include "CAoiThreadMgr.h"
#include "CAoiJob.h"
#include "TTrThreadMsgBuffer.inl"

inline void* CAoiResult::operator new(size_t stSize)
{
	return CAoiThreadMgr::Inst()->AllocRightMsgMem(stSize);
}

inline void* CAoiResult::AllocMem(size_t stSize)
{
	return CAoiThreadMgr::Inst()->AllocRightMsgMem(stSize);
}

inline char* CAoiResult::CloneString(const char *szStr)
{
	uint32 uLen = strlen(szStr);
	char* szString = (char*)AllocMem(uLen + 1);
	memcpy(szString, szStr, uLen);
	szString[uLen] = '\0';
	return szString;
}

inline void* CAoiResult::CloneData(const void *pData, size_t stSize)
{
	void* pCloneData = AllocMem(stSize);
	memcpy(pCloneData, pData, stSize);
	return pCloneData;
}

inline void CAoiResult::Add()
{
	CAoiThreadMgr::Inst()->AddRightMsg(this);
}
