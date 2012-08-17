#pragma once
#include "CCoreServerResult.h"
#include "CCoreServerThreadMgr.h"
#include "CCoreServerJob.h"
#include "TTrThreadMsgBuffer.inl"

inline void* CCoreServerResult::operator new(size_t stSize)
{
	return CCoreServerThreadMgr::Inst()->AllocRightMsgMem(stSize);
}

inline void* CCoreServerResult::AllocMem(size_t stSize)
{
	return CCoreServerThreadMgr::Inst()->AllocRightMsgMem(stSize);
}

inline char* CCoreServerResult::CloneString(const char *szStr)
{
	size_t uLen = strlen(szStr);
	char* szString = (char*)AllocMem(uLen + 1);
	memcpy(szString, szStr, uLen);
	szString[uLen] = '\0';
	return szString;
}

inline void* CCoreServerResult::CloneData(const void *pData, size_t stSize)
{
	void* pCloneData = AllocMem(stSize);
	memcpy(pCloneData, pData, stSize);
	return pCloneData;
}

inline void CCoreServerResult::Add()
{
	CCoreServerThreadMgr::Inst()->AddRightMsg(this);
}
