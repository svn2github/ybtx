//$Id: CTickImp.h 54290 2007-04-02 08:19:54Z shhuang $
#pragma once
#include "CommonDefs.h"
#include <deque>
#include <list>
#include "CTickMallocObject.h"
#include "CTickMallocAllocator.h"

class CTestSuiteTickMgr;

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#endif

namespace sqr
{
	class CTickMgr;
	class CTickSlot;
	class CTick;
	
	class  COMMON_API CTickImp : public CTickMallocObject
	{
		friend class CTickMgr;
		friend class CTickSlot;
		friend class CTestSuiteTickMgr;

	public:
		uint32 GetInterval()const;

		CTickImp();
		~CTickImp();

		void SetTickName(const char* szName);
		const char* GetTickName()const;	

		uint32 GetLeftTime()const;

		void UnRegister();

	private:
		uint32						m_uQueuePos;
		uint32						m_uKeyOfTickMap;		//保存在CTickMgr的Infolist的位置，析构时删除

		uint16						m_uSlotPos;
		uint32						m_uInterval;		//多少时间处理一次,毫秒做单位
		uint32						m_uRing;			//哪轮开始运行

		//这个变量有双重含义
		//当TickInterval大于等于TickMgrInterval时，记录的是Tick调用滞后的毫秒数
		//当TickInterval小于TickMgrInterval时，记录的是Tick下一次调用的多出毫秒数
		uint32						m_uLoan;
		CTickMgr*					m_pTickMgr;			//挂在哪个mgr上，构造的时候为空

		typedef basic_string<char,std::char_traits<char>,CTickMallocAllocator<char> > TickName_t;
		TickName_t					m_sTickName;
		const char*					m_szClassName;

		void AddToSlot(uint16 uSlotPos,uint32 uRing);
		void DelFrSlot();
		CTickSlot*	GetSlot()const;

		void OnCaughtTickException(CTick* pTick);

		CTick*	m_pTick;
	};
}

#ifdef _WIN32
#pragma warning(pop)
#endif
