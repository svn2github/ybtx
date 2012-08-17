#pragma once
#include "CTick.h"
#include <vector>

namespace sqr
{
	class CTestSuiteRenderSlot;
	class CAppClient;

	class CRenderSlot;
	class CRenderSlotOwner
	{
		friend class CTestSuiteRenderSlot;
	public:
		typedef vector<CRenderSlot*> VecRenderSlot_t;

		static CRenderSlotOwner& Inst();

		void Update(uint64 uUsedTime);

		void Insert(CRenderSlot* pRenderSlot);
		bool Delete(CRenderSlot* pRenderSlot);

		uint32 Size()const;
		void PrintSlot()const;

	private:
		CRenderSlotOwner();
		void UpdateFromSlot(uint64 uUsedTime, uint32 uSlotIndex);
		void Defrag();

		VecRenderSlot_t	m_vecRenderSlot;
		uint32			m_uCurUpdateIndex;
	};

	//由渲染驱动的tick
	class CRenderSlot
	{
		friend class CTestSuiteRenderSlot;
		friend class CRenderSlotOwner;
	public:
		typedef CRenderSlotOwner::VecRenderSlot_t VecRenderSlot_t;

		virtual void OnUpdate(uint64 uLastFrameUsedTime){}	//上帧消耗时间,单位微秒

	protected:
		CRenderSlot();
		virtual ~CRenderSlot();

	private:
		uint32						m_uSlotIndex;
		CRenderSlotOwner*			m_pRenderSlotOwner;
	};
}

