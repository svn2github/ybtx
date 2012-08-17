#include "stdafx.h"
#include "CTrMsgBufferSwapper.h"
#include "ThreadHelper.h"
#include "ExpHelper.h"
#include "CTrThreadMsgBuffer.h"
#include "TSqrAllocator.inl"


CTrMsgBufferSwapper::CTrMsgBufferSwapper()
{
	m_hThread = GetCurTID();

	CreateSpinLock(&m_Lock);

	m_pWaitingRightControlMsg = new VecMsg_t;
	m_pDoingRightControlMsg = new VecMsg_t;
}

CTrMsgBufferSwapper::~CTrMsgBufferSwapper()
{
	delete m_pWaitingRightControlMsg;
	delete m_pDoingRightControlMsg;

	DestroySpinLock(&m_Lock);
}


void CTrMsgBufferSwapper::AddTrBuffer(CTrThreadMsgBuffer* pTrBuffer)
{
	Ast(IsInSameThread());

	pTrBuffer->SetIndex( m_vecTrBuffer.size() );
	m_vecTrBuffer.push_back( pTrBuffer );	
}

void CTrMsgBufferSwapper::RemoveTrBuffer(CTrThreadMsgBuffer* pTrBuffer)
{
	Ast(IsInSameThread());

	size_t stIndex = pTrBuffer->GetIndex();
	Ast(stIndex >= 0 && stIndex < m_vecTrBuffer.size());

	m_vecTrBuffer.back()->SetIndex( stIndex );
	m_vecTrBuffer[stIndex] = m_vecTrBuffer.back();
	pTrBuffer->SetIndex(-1);
	m_vecTrBuffer.pop_back();
}

bool CTrMsgBufferSwapper::IsInSameThread()
{
	return m_hThread == GetCurTID();	
}

void CTrMsgBufferSwapper::FlushAllLeftMsg()
{
	for(size_t i = 0; i < m_vecTrBuffer.size(); i++)
	{
		CTrThreadMsgBuffer* pBuffer = m_vecTrBuffer[i];
		if(!pBuffer)
			continue;

		pBuffer->FlushLeftMsg();
	}
}

void CTrMsgBufferSwapper::HandleAllRightMsg()
{
	SpinLock(&m_Lock);
	std::swap(m_pWaitingRightControlMsg, m_pDoingRightControlMsg);
	SpinUnlock(&m_Lock);

	size_t stSize = m_pDoingRightControlMsg->size();

	for(size_t i = 0; i < stSize; i++)
	{
		IndexMsgPair_t& Value = m_pDoingRightControlMsg->at(i);
		CTrThreadMsgBuffer* pBuffer = m_vecTrBuffer[Value.first];

		pBuffer->HandleRightControlMsg((ETrBallMsg)Value.second);
	}
	
	m_pDoingRightControlMsg->clear();
}


void CTrMsgBufferSwapper::AddControlMsg(uint32 uIndex, uint32 uMsg)
{
	SpinLock(&m_Lock);
	m_pWaitingRightControlMsg->push_back(IndexMsgPair_t(uIndex, uMsg));
	SpinUnlock(&m_Lock);
}

void CTrMsgBufferSwapper::NotifyRightToQuit()
{
	for(size_t i = 0; i < m_vecTrBuffer.size(); i++)
	{
		CTrThreadMsgBuffer* pBuffer = m_vecTrBuffer[i];
		if(!pBuffer)
			continue;	
		pBuffer->NotifyRight(eTBM_Quit);
	}
}







