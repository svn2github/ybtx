#pragma once
#include "CCoreServerJob.h"
#include "CCoreServerThreadMgr.h"
#include "CCoreServerResult.h"
#include "TTrThreadMsgBuffer.inl"

inline void* CCoreServerJob::operator new(size_t stSize)
{
	return CCoreServerThreadMgr::Inst()->AllocLeftMsgMem(stSize);
}

inline void* CCoreServerJob::AllocMem(size_t stSize)
{
	return CCoreServerThreadMgr::Inst()->AllocLeftMsgMem(stSize);
}

inline char* CCoreServerJob::CloneString(const char* szStr)
{
	const size_t uLen = strlen(szStr);
	char*const szString = (char*)AllocMem(uLen + 1);
	memcpy(szString, szStr, uLen);
	szString[uLen] = '\0';
	return szString;
}

inline char* CCoreServerJob::CloneString(const std::string& szStr)
{
	const size_t uLen = szStr.size();
	char*const szString = (char*)AllocMem(uLen + 1);
	memcpy(szString, szStr.c_str(), uLen);
	szString[uLen] = '\0';
	return szString;
}

inline void* CCoreServerJob::CloneData(const void *pData, size_t stSize)
{
	void* pCloneData = AllocMem(stSize);
	memcpy(pCloneData, pData, stSize);
	return pCloneData;
}

inline void CCoreServerJob::Add()
{
	CCoreServerThreadMgr::Inst()->AddLeftMsg(this);
}
