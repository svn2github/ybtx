#pragma once
#include "TTrThreadMsgBuffer.h"
#include "TThreadMsgBuffer.inl"
#include "ErrLogHelper.h"

template<typename Traits>
TTrThreadMsgBuffer<Traits>::TTrThreadMsgBuffer(size_t stMsgCount, size_t stMemBlock,
											   CTrMsgBufferSwapper* pMgr)
											   : CTrThreadMsgBuffer(pMgr)
{
	m_pWaitedLeftBuffer = new LeftBuffer_t(stMsgCount, stMemBlock);
	m_pSwapLeftBuffer = new LeftBuffer_t(stMsgCount, stMemBlock);
	m_pDoingLeftBuffer = new LeftBuffer_t(stMsgCount, stMemBlock);

	m_pWaitedRightBuffer = new RightBuffer_t(stMsgCount, stMemBlock);
	m_pSwapRightBuffer = new RightBuffer_t(stMsgCount, stMemBlock);
	m_pDoingRightBuffer = new RightBuffer_t(stMsgCount, stMemBlock);
}

template<typename Traits>
TTrThreadMsgBuffer<Traits>::~TTrThreadMsgBuffer()
{
	ClearLeftMsg(m_pWaitedLeftBuffer);
	ClearLeftMsg(m_pSwapLeftBuffer);
	ClearLeftMsg(m_pDoingLeftBuffer);

	delete m_pWaitedLeftBuffer;
	delete m_pSwapLeftBuffer;
	delete m_pDoingLeftBuffer;

	ClearRightMsg(m_pWaitedRightBuffer);
	ClearRightMsg(m_pSwapRightBuffer);
	ClearRightMsg(m_pDoingRightBuffer);

	delete m_pWaitedRightBuffer;
	delete m_pSwapRightBuffer;
	delete m_pDoingRightBuffer;
}

template<typename Traits>
inline void* TTrThreadMsgBuffer<Traits>::AllocLeftMsgMem(size_t stSize)
{
	return m_pWaitedLeftBuffer->AllocMsgMem(stSize);
}

template<typename Traits>
inline void* TTrThreadMsgBuffer<Traits>::AllocRightMsgMem(size_t stSize)
{
	return m_pWaitedRightBuffer->AllocMsgMem(stSize);
}

template<typename Traits>
inline void TTrThreadMsgBuffer<Traits>::AddLeftMsg(ImpLeftMsgType_t* pLeftMsg)
{
	m_pWaitedLeftBuffer->AddMsg(pLeftMsg);
}

template<typename Traits>
inline void TTrThreadMsgBuffer<Traits>::AddRightMsg(ImpRightMsgType_t* pRightMsg)
{
	m_pWaitedRightBuffer->AddMsg(pRightMsg);
}


template<typename Traits>
void TTrThreadMsgBuffer<Traits>::HandleLeftMsg(LeftBuffer_t* pBuffer)
{
	size_t stLeftMsgCount = pBuffer->GetMsgCount();

	LeftMsgQueue_t& vecLeftMsg = pBuffer->GetMsgQueue();

	size_t i = 0;

	for( ;; )
	{
		SQR_TRY
		{
			while(i < stLeftMsgCount)
			{
				ImpLeftMsgType_t* pLeftMsg = vecLeftMsg[i++];
				pLeftMsg->~ImpLeftMsgType_t();
			}
			break;
		}
		SQR_CATCH(exp)
		{
			//如果有错误，我们跳过这一个job
			OnLeftException(exp);
			LogExp(exp);
		}
		SQR_TRY_END;
	}

}


template<typename Traits>
inline void TTrThreadMsgBuffer<Traits>::ClearLeftMsg(LeftBuffer_t* pBuffer)
{
//#ifdef _DEBUG
//	size_t stLeftMsgCount = pBuffer->GetMsgCount();
//
//	vector<ImpLeftMsgType_t*>& vecLeftMsg = pBuffer->GetMsgQueue();
//
//	for(size_t i = 0; i < stLeftMsgCount; i++)
//	{
//		ImpLeftMsgType_t* pLeftMsg = vecLeftMsg[i];
//		pLeftMsg->~ImpLeftMsgType_t();	
//		vecLeftMsg[i] = NULL;
//	}
//#endif

	pBuffer->ClearMsgBuffer();
}

template<typename Traits>
void TTrThreadMsgBuffer<Traits>::HandleRightMsg(RightBuffer_t* pBuffer)
{
	size_t stRightMsgCount = pBuffer->GetMsgCount();

	RightMsgQueue_t& vecRightMsg = pBuffer->GetMsgQueue();

	size_t i = 0;

	do 
	{
		SQR_TRY
		{
			while(i < stRightMsgCount)
			{
				ImpRightMsgType_t* pRightMsg = vecRightMsg[i];
				pRightMsg->~ImpRightMsgType_t();
				i++;
			}
			break;
		}
		SQR_CATCH(exp)
		{
			//如果有错误，我们跳过这一个result
			i++;
			OnRightException(exp);
			LogExp(exp);
		}
		SQR_TRY_END;

	} while(true);

}

template<typename Traits>
inline void TTrThreadMsgBuffer<Traits>::ClearRightMsg(RightBuffer_t* pBuffer)
{
//#ifdef _DEBUG	
//	size_t stRightMsgCount = pBuffer->GetMsgCount();
//
//	vector<ImpRightMsgType_t*>& vecRightMsg = pBuffer->GetMsgQueue();
//
//	for(size_t i = 0; i < stRightMsgCount; i++)
//	{
//		ImpRightMsgType_t* pRightMsg = vecRightMsg[i];
//		pRightMsg->~ImpRightMsgType_t();
//		vecRightMsg[i] = NULL;
//	}
//#endif

	pBuffer->ClearMsgBuffer();
}


template<typename Traits>
void TTrThreadMsgBuffer<Traits>::IntSwapLeftBuffer()
{
	std::swap(m_pWaitedLeftBuffer, m_pSwapLeftBuffer);
}

template<typename Traits>
void TTrThreadMsgBuffer<Traits>::IntSwapRightBuffer()
{
	std::swap(m_pWaitedRightBuffer, m_pSwapRightBuffer);
}

template<typename Traits>
bool TTrThreadMsgBuffer<Traits>::IntLeftBufferEmpty()
{
	return m_pWaitedLeftBuffer->MsgQueueEmpty();
}

template<typename Traits>
bool TTrThreadMsgBuffer<Traits>::IntRightBufferEmpty()
{
	return m_pWaitedRightBuffer->MsgQueueEmpty();
}

template<typename Traits>
void TTrThreadMsgBuffer<Traits>::IntOnLeftBufferSwaped()
{
	Ast(m_LeftMsgBallState == eTBS_NotOwned);

	std::swap(m_pSwapLeftBuffer, m_pDoingLeftBuffer);
	
	NotifyLeft(eTBM_BallReturned);
	
	HandleLeftMsg(m_pDoingLeftBuffer);

	//清除已经处理的job
	ClearLeftMsg(m_pDoingLeftBuffer);
}

template<typename Traits>
void TTrThreadMsgBuffer<Traits>::IntOnRightBufferSwaped()
{
	Ast(m_RightMsgBallState == eTBS_NotOwned);

	std::swap(m_pSwapRightBuffer, m_pDoingRightBuffer);

	NotifyRight(eTBM_BallReturned);
	
	HandleRightMsg(m_pDoingRightBuffer);

	//清除已经处理的result
	ClearRightMsg(m_pDoingRightBuffer);
}

//template<typename Traits>
//void TTrThreadMsgBuffer<Traits>::OnLeftException(CError& error)
//{
//
//}
//
//template<typename Traits>
//void TTrThreadMsgBuffer<Traits>::OnRightException(CError& error)
//{
//
//}

