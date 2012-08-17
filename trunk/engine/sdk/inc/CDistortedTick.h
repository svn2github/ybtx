#pragma once
#include "CDynamicObject.h"
#include "CDistortedTimeAllocator.h"

namespace sqr
{
	class CTick;
	class CDistortedTimeObj;
	class IDistortedTime;
	class CDistortedTickImp;

	class CTraitsServer;
	class CSyncTraitsServer;
	class CTraitsClient;
	class CTraitsStress;
	template<typename Traits>
	class TApp;

	class CDistortedTick
		:public virtual CDynamicObject
	{
		friend class CDistortedTimeObj;
		friend class CDistortedTickImp;
		friend class TApp<CTraitsServer>;
		friend class TApp<CSyncTraitsServer>;
		friend class TApp<CTraitsClient>;
		friend class TApp<CTraitsStress>;
	public:
		CDistortedTick();
		virtual	~CDistortedTick(void);

		void SetTickName(const char* szName);
		virtual void OnTick();

		bool Registered() const;
		uint32 GetInterval()const;
		uint32 GetLeftTime()const;

	private:
		void SetDistortedTime(IDistortedTime* pDistortedTime);

		CTick*	m_pTick;
		IDistortedTime*	m_pDistortedTime;				//保存IDistortedTime接口指针仅仅是为了在注册tick时做检测,不应该在CDistortedTick的成员函数中调用IDistortedTime上的接口.

		uint32	m_uKeyOfDistortedTickMap;

		uint32			m_uTickCycInRealTime;			//注册tick时的原始时间间隔,如果没有tick注册则为-1
		bool			m_bIntervalNeedToBeRefreshed;	//已注册的tick在时间系数发生变化时会计算剩余时间并注册一个新的tick,并将该标志位置为true.当新tick时间到后得恢复原来的tick间隔.
	};
}

