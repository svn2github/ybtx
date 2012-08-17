#pragma once
#include "CAoiJob.h"
#include "CAoiThreadMgr.h"
#include "CAoiResult.h"
#include "TTrThreadMsgBuffer.inl"

inline void* CAoiJob::operator new(size_t stSize)
{
	return CAoiThreadMgr::Inst()->AllocLeftMsgMem(stSize); 
}

inline void* CAoiJob::AllocMem(size_t stSize)
{
	return CAoiThreadMgr::Inst()->AllocLeftMsgMem(stSize);
}

inline char* CAoiJob::CloneString(const char *szStr)
{
	uint32 uLen = strlen(szStr);
	char* szString = (char*)AllocMem(uLen + 1);
	memcpy(szString, szStr, uLen);
	szString[uLen] = '\0';
	return szString;
}

inline void* CAoiJob::CloneData(const void *pData, size_t stSize)
{
	void* pCloneData = AllocMem(stSize);
	memcpy(pCloneData, pData, stSize);
	return pCloneData;
}

inline void CAoiJob::Add()
{
	CAoiThreadMgr::Inst()->AddLeftMsg(this);	
}

