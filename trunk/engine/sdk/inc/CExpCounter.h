#pragma once 
#include "ModuleDefs.h"

namespace sqr
{
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4275)	//dll linkage
#pragma warning(disable:4251)	//dll linkage
#endif

	/*
	异常计数器
	在单位时间内异常计数到到一定范围就开始进行相关处理
	*/

	class MODULE_API CExpCounter
	{
	public:
		CExpCounter(uint64 uInterval, uint32 uNum = 5);
		~CExpCounter();

		//如果返回true，则证明需要开始进行异常处理了		
		bool AddExpCounter(uint64 uTime);

	private:
		std::list<uint64>	m_lstOccurTime;  //保存异常出现的时间
		uint64				m_uInterval;	 //单位时间
		uint32				m_uMaxExpNum;    //在单位时间内最多允许发生的异常次数  
	};

#ifdef _WIN32
#pragma warning(pop)
#endif
}

