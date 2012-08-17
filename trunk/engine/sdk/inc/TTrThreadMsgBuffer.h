#pragma once
#include "CTrThreadMsgBuffer.h"
#include "CTrMsgMallocAllocator.h"

namespace sqr
{
	class CTrMsgBufferSwapper;
	class CError;

	template<typename MsgType>
	class TThreadMsgBuffer;

	template<typename Traits>
	class TTrThreadMsgBuffer
		:public CTrThreadMsgBuffer
	{
		typedef typename Traits::LeftMsgType	ImpLeftMsgType_t;
		typedef typename Traits::RightMsgType	ImpRightMsgType_t;
	
		typedef vector<ImpLeftMsgType_t*, CTrMsgMallocAllocator<ImpLeftMsgType_t*> > LeftMsgQueue_t;
		typedef vector<ImpRightMsgType_t*, CTrMsgMallocAllocator<ImpRightMsgType_t*> > RightMsgQueue_t;

	public:
		TTrThreadMsgBuffer(size_t stMsgCount, size_t stMemBlock, CTrMsgBufferSwapper* pMgr); 
		~TTrThreadMsgBuffer();

		inline void AddLeftMsg(ImpLeftMsgType_t* pJob);
		inline void AddRightMsg(ImpRightMsgType_t* pResult);

		inline void* AllocLeftMsgMem(size_t stSize);
		inline void* AllocRightMsgMem(size_t stSize);

	protected:
		
		void IntSwapLeftBuffer();
		void IntSwapRightBuffer();
		
		bool IntLeftBufferEmpty();
		bool IntRightBufferEmpty();

		void IntOnLeftBufferSwaped();
		void IntOnRightBufferSwaped();

		virtual void OnLeftException(CError& error) = 0;
		virtual void OnRightException(CError& error) = 0;

	private:
		typedef TThreadMsgBuffer<ImpLeftMsgType_t>	LeftBuffer_t;
		typedef TThreadMsgBuffer<ImpRightMsgType_t>	RightBuffer_t;
	
		void HandleLeftMsg(LeftBuffer_t *pBuffer);
		void HandleRightMsg(RightBuffer_t* pBuffer);
		void ClearLeftMsg(LeftBuffer_t* pBuffer);
		void ClearRightMsg(RightBuffer_t* pBuffer);

	protected:

		LeftBuffer_t*	m_pWaitedLeftBuffer;
		LeftBuffer_t*	m_pSwapLeftBuffer;
		LeftBuffer_t*	m_pDoingLeftBuffer;

		RightBuffer_t*	m_pWaitedRightBuffer;
		RightBuffer_t*	m_pSwapRightBuffer;
		RightBuffer_t*	m_pDoingRightBuffer;
	};

}
