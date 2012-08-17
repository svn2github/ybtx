#pragma once
#include "TMemPoolAllocator.h"
#include "CMirrorMallocObject.h"

namespace sqr
{
	/*
		当在rb创建一个对象之后，我们会创建一个cmd用来上patch，每当rb和ib交换的时候
		这个cmd就会执行
		当在wb创建对应的这个对象或者删除的时候，我们会把这个对象所在的slot加入一个discarded列表
		然后交换wb,ib，在交换ib,rb，这时候rb上面的cmd就会执行，但是在执行前，首先会检查
		该cmd里面对应的slot是不是在discarded列表里面，如果在，那么就丢弃这个cmd，不执行
	*/

	class CTrDiscardedCmdMgr : public CMirrorMallocObject
	{
	public:
		CTrDiscardedCmdMgr();
		~CTrDiscardedCmdMgr();

		void AddDiscardedCmd(uint32 uSlot);
		
		void SwapWIDiscardedCmd();
		void SwapRIDiscardedCmd();

		bool IsDiscardedCmd(uint32 uSlot);
		void ClearDiscardedCmd();

	private:
		void CreateWBMem();
		void CreateIBMem();

	private:
		typedef TMemPoolAllocator<uint32>	SlotAlloc_t;
		typedef set<uint32, less<uint32>, SlotAlloc_t>	SlotSet_t;

		SlotAlloc_t*	m_pIBAlloc;
		SlotAlloc_t*	m_pRBAlloc;

		typedef deque<uint32, CMirrorMallocAllocator<uint32> >	QueCmd_t;
		QueCmd_t	m_queWBDiscardedCmd;
		SlotSet_t*				m_pIBDiscardedCmd;
		SlotSet_t*				m_pRBDiscardedCmd;

	};

}
