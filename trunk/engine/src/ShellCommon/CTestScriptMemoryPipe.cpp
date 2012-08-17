#include "stdafx.h"
#include "CTestScriptMemoryPipe.h"

size_t CTestScriptMemoryPipe::Send(const void* pData,size_t stSize)
{
	int32 stSendLen;
	stSendLen = ((70000 - (int32)m_nCurPos) - (int32)stSize) >= 0 ? stSize : m_nCurPos;
	memcpy(reinterpret_cast<void *>(m_Buff + m_nCurPos),pData,stSendLen);
	m_nCurPos += stSendLen;
	return stSendLen;
}

void* CTestScriptMemoryPipe::GetRecvData() const
{
	return (void *)m_Buff;
}

size_t CTestScriptMemoryPipe::GetRecvDataSize() const
{
	return m_nCurPos;
}

void CTestScriptMemoryPipe::PopRecvData(size_t stSize)
{
	if(stSize == 0)
		return;
	memcpy(m_Buff,m_Buff+stSize,m_nCurPos - stSize );
	m_nCurPos = m_nCurPos - stSize ;
}

