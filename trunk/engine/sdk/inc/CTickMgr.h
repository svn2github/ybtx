#pragma once

#include "CommonDefs.h"
#include "BaseTypes.h"
#include "CTick.h"
#include "TTickHandler.h"
#include "CTickMallocAllocator.h"
#include "TIDPtrMap.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#endif


namespace sqr
{
	template<typename,template<typename>class>
	class TObjectBank;
	class CExpCounter;

	class CTickQueue;
	class CTickSlot;
	class CTick;
	
	class COMMON_API CTickMgr
		:public CTick
	{
		friend class CTickImp;
		friend class CTestSuiteTickMgr;
		friend class CTickSlot;
		typedef std::vector< CTickSlot*, CTickMallocAllocator<CTickSlot*> > VecTickSlot_t;
	public:
		//uTickCyc Tick的周期=多少毫秒Tick一次
		explicit CTickMgr(uint32 uTickCyc,uint16 uSlotNum);
		~CTickMgr();
		uint32 GetInterval()const;
		void OnTick();

		//uInterval Tick的周期=多少毫秒Tick一次
		void Register(CTick*,uint32 uInterval);
		void UnRegister(CTick*);

		//得到当前注册tick的详细信息，tick名字，tick数量
		void GetRegisterTickInfo(string& strInfo);
		//得到当前注册tick的总数量
		size_t GetRegisterTickNum();

	private:
		CTickImp * m_pLastTickImp; //在调用OnTick前，把要调用的Tick指针保存下来。看自己是否在OnTick中被删除
		uint32 m_uRing; //多少轮
		uint32 m_uCurPos; //当前第几个巢
		uint32 m_uInterval; //多少时间，算一个脉搏。用毫秒做单位。

		VecTickSlot_t m_vecSlot;

		typedef TObjectBank<CTickQueue, CTickMallocAllocator>	QueueBank_t;
		QueueBank_t*					m_pTickQueueBank;

		typedef TObjectBank<CTickImp, CTickMallocAllocator>		ImpBank_t;
		ImpBank_t*						m_pTickImpBank;
		
		typedef TIDPtrMap<CTickImp>	MapTickImp_t;
		MapTickImp_t	m_mapRegisteredTick;

		//在调用OnTick里面，保存当前的queue，防止注销tick之后该queue为空了被删除
		CTickQueue* m_pLastTickQueue; 
									   
		void PrintAndUnregisterTick();
		void OnCaughtTickException(CTick* pTick);

		inline bool CallTick(CTickImp* pTick);
	};
}

#ifdef _WIN32
#pragma warning(pop)
#endif

