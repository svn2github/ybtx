#pragma once 
#include "TNetworkAllocator.h"

class CAsynPipeInBuffer
{
public:
	CAsynPipeInBuffer(size_t stBufferSize);

	//return false表示缓冲区已满
	void InBufferPushData(const char* pData, size_t size);
	void InBufferClear();
	char* InBufferGetData()const;
	size_t InBufferGetDataSize()const;
	void InBufferPopData(size_t stPopDataSize);
	bool IsFull()const;

private:
	
	typedef TNetworkAllocator<char> StringAlloc_t;
	typedef basic_string<char,std::char_traits<char>,StringAlloc_t> String_t;

	String_t m_sBuffer;

	const size_t	m_stMaxBufferSize;
};
