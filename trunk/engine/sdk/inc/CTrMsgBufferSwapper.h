#pragma once
#include "ThreadTypes.h"
#include "CTrMsgMallocAllocator.h"

namespace sqr
{
	/*
		多线程消息buffer三倍缓冲

		我们需要处理的模型是如下的线程模型

		有一个主线程，主要用来分发left msg和处理right msg
		有多个从线程，主要用来处理left msg和向主线程投递right msg

		消息的投送是明确指定目标，因此三倍缓冲并不适合消息可投递给任意工作线程的模型（例如DbThread）。

		主线程和相应的从线程使用一个TrMsgBuffer
		同时主线程使用TrMsgBufferSwapper来进行管理这些三倍缓冲

		我们使用TrMsgBufferSwapper来进行主线程上面的相应处理
		使用TrMsgBuffer在每个线程上面进行相应的处理

		TrMsgBufferSwapper主要负责：
		在OnMainFrameUpdate即每次主线程循环的时候把要处理的left msg放到相应的从线程的swap msg buffer
		在OnCanceled里面响应从线程投递过来的消息，进行相应的处理（包括交换right msg队列，获取从线程归还的球）

		TrMsgBuffer主要负责
		在OnFrameUpdate即从线程每次循环的时候把要处理的right msg放到相应的swap msg buffer
		在OnCanceled里面相应主线程投递过来的消息，进行相应的处理

		主线程和从线程交互，以main和thread为例

		在主线程方面
		1，在main里面添加Waited Left Msg Buffer
		2，到main的update的时候，检查left msg的ball是否拥有
		3，如果有ball，则交换Waited Left Msg Buffer和Swap Left Msg Buffer，
		   同时把球设置成NotOwned，给thread发出LeftMsgBufferSwaped消息
		4，thread被Canceled唤醒，处理LeftMsgBufferSwaped消息，
		   交换SwapLeftMsgBuffer和DoingLeftMsgBuffer，处理left msg，然后给main发出ReturnFromMinor消息
	    5，main被Canceled唤醒，处理ReturnFromMinor消息，把球设置成Owned
		6，在第2步中，如果此时没有ball，那么就不处理，直到ball为Owned状态

		从线程也是一样的处理方式

		三倍缓冲的使用
		在外部使用的时候，我们首先要写一个swapper类继承CTrMsgBufferSwapper
		然后在该类里面重新实现主线程唤醒函数
		然后写一个类继承TTrThreadMsgBuffer，用来实现实际的三倍缓冲
		并在该类里面重新实现从线程函数
		TTrThreadMsgBuffer里面的AllocLeftMsgMem，AddCommLeftMsg，AllocRightLeftMem，AddCommRightMsg
		则用来实现向三倍缓冲中添加left和right msg以及分配相应的内存
	*/

	class CTrThreadMsgBuffer;

	class CTrMsgBufferSwapper
	{
		friend class CTrThreadMsgBuffer;

	public:
		//主线程每次循环调用
		void FlushAllLeftMsg();

		//主线程被唤醒调用
		void HandleAllRightMsg();
		
		//供从线程调用唤醒主线程使用
		virtual void OnWakeLeft() = 0;

		void NotifyRightToQuit();

	protected:
		CTrMsgBufferSwapper() ;
		virtual ~CTrMsgBufferSwapper();

		void AddTrBuffer(CTrThreadMsgBuffer* pTrBuffer);
		void RemoveTrBuffer(CTrThreadMsgBuffer* pTrBuffer);

		bool IsInSameThread();

		void AddControlMsg(uint32 uIndex, uint32 uMsg);

	protected:

		vector<CTrThreadMsgBuffer*>	m_vecTrBuffer;
		
		HTHREADID					m_hThread;

		HSPINLOCK					m_Lock;

		/*
			因为mgr里面负责的是left msg队列，所以其他的线程消息都会投递到这里
			所以我们统一使用一个消息队列进行管理，每一个消息包含两个部分
			一个是投递线程的id，另一个则是投递的消息类型
		*/
		typedef pair<uint32, uint32>	IndexMsgPair_t;
		typedef vector<IndexMsgPair_t, CTrMsgMallocAllocator<IndexMsgPair_t> >	VecMsg_t;

		VecMsg_t*	m_pWaitingRightControlMsg;
		VecMsg_t*	m_pDoingRightControlMsg;
	};

}
