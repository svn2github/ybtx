#include "stdafx.h"
#include "CTrThreadMsgBuffer.h"
#include "CTrMsgBufferSwapper.h"
#include "ThreadHelper.h"
#include "ExpHelper.h"
#include "ThreadHelper.inl"
#include "TSqrAllocator.inl"

CTrThreadMsgBuffer::CTrThreadMsgBuffer(CTrMsgBufferSwapper* pMgr)
: m_pMgr(pMgr)
, m_LeftMsgBallState(eTBS_Owned)
, m_RightMsgBallState(eTBS_Owned)
, m_bQuit(false)
{
	m_pMgr->AddTrBuffer(this);

	CreateSpinLock(&m_Lock);

	m_pWaitingLeftControlMsg = new VecMsg_t;
	m_pDoingLeftControlMsg = new VecMsg_t;

	m_pWaitingLeftControlMsg->reserve(2);
	m_pDoingLeftControlMsg->reserve(2);
}

CTrThreadMsgBuffer::~CTrThreadMsgBuffer()
{
	delete m_pWaitingLeftControlMsg;
	delete m_pDoingLeftControlMsg;

	DestroySpinLock(&m_Lock);

	m_pMgr->RemoveTrBuffer(this);
}

void CTrThreadMsgBuffer::SetIndex(size_t stIndex)
{
	m_stIndex = stIndex;
}

size_t CTrThreadMsgBuffer::GetIndex() const
{
	return m_stIndex;
}

bool CTrThreadMsgBuffer::LeftBallOwnedByLeft()
{
	return m_LeftMsgBallState == eTBS_Owned;
}

bool CTrThreadMsgBuffer::RightBallOwnedByRight()
{
	return m_RightMsgBallState == eTBS_Owned;
}

void CTrThreadMsgBuffer::NotifyLeft(ETrBallMsg eMsg)
{
	m_pMgr->AddControlMsg((uint32)m_stIndex, eMsg);
	m_pMgr->OnWakeLeft();
}

void CTrThreadMsgBuffer::NotifyRight(ETrBallMsg eMsg)
{
	AddControlMsg(eMsg);
	OnWakeRight();
}

void CTrThreadMsgBuffer::AddControlMsg(ETrBallMsg eMsg)
{
	SpinLock(&m_Lock);
	m_pWaitingLeftControlMsg->push_back(eMsg);
	SpinUnlock(&m_Lock);
}

void CTrThreadMsgBuffer::HandleLeftControlMsg(ETrBallMsg eMsg)
{
	switch(eMsg)
	{
	case eTBM_BallReturned:
		{
			Ast(m_RightMsgBallState == eTBS_NotOwned);
			m_RightMsgBallState = eTBS_Owned;
		}
		break;
	case eTBM_BufferSwaped:
		{
			IntOnLeftBufferSwaped();
		}
		break;
	case eTBM_Quit:
		m_bQuit = true;
		break;
	default:
		GenErr("Invalid Msg");
		break;
	}
}

void CTrThreadMsgBuffer::HandleRightControlMsg(ETrBallMsg eMsg)
{
	switch(eMsg)
	{
	case eTBM_BallReturned:
		{
			Ast(m_LeftMsgBallState == eTBS_NotOwned);
			m_LeftMsgBallState = eTBS_Owned;
		}
		break;
	case eTBM_BufferSwaped:
		{
			IntOnRightBufferSwaped();
		}
		break;	
	//不允许从从线程通知主线程退出，所以这里不处理此消息
	//case eTBM_Quit:
	//	m_pMgr->m_bQuit = true;
	//	break;
	default:
		GenErr("Invalid Msg");
		break;
	}
}



bool CTrThreadMsgBuffer::HandleAllLeftMsg()
{
	SpinLock(&m_Lock);
	std::swap(m_pWaitingLeftControlMsg, m_pDoingLeftControlMsg);
	SpinUnlock(&m_Lock);

	const size_t stSize = m_pDoingLeftControlMsg->size();
	for(size_t i = 0; i < stSize; i++)
	{
		ETrBallMsg eMsg = m_pDoingLeftControlMsg->at(i);
		HandleLeftControlMsg(eMsg);
	}

	m_pDoingLeftControlMsg->clear();

	return m_bQuit;
}

void CTrThreadMsgBuffer::FlushLeftMsg()
{
	if(!LeftBallOwnedByLeft())
		return;

	if(IntLeftBufferEmpty())
		return;

	SwapLeftBuffer();
}

void CTrThreadMsgBuffer::SwapLeftBuffer()
{
	IntSwapLeftBuffer();

	m_LeftMsgBallState = eTBS_NotOwned;
		
	NotifyRight(eTBM_BufferSwaped);	
}

bool CTrThreadMsgBuffer::FlushRightMsg()
{
	if(!RightBallOwnedByRight())
		return false;		

	if(IntRightBufferEmpty())
		return false;

	SwapRightBuffer();

	return true;
}

void CTrThreadMsgBuffer::SwapRightBuffer()
{
	IntSwapRightBuffer();

	m_RightMsgBallState = eTBS_NotOwned;

	NotifyLeft(eTBM_BufferSwaped); 		
}




