#include "stdafx.h"
#include "CExpCounter.h"

CExpCounter::CExpCounter(uint64 uInterval, uint32 uNum)
: m_uInterval(uInterval)
, m_uMaxExpNum(uNum)
{

}

CExpCounter::~CExpCounter()
{

}


bool CExpCounter::AddExpCounter(uint64 uTime)
{
	if (m_lstOccurTime.empty())
	{
		m_lstOccurTime.push_back(uTime);
		return false;
	}

	uint64 uFirstTime = m_lstOccurTime.front();

	//如果这次发生异常的时间与链表里面第一次时间相比大于间隔时间，我们把第一次的删除
	if (uTime - uFirstTime > m_uInterval)
	{
		m_lstOccurTime.pop_front(); 
		m_lstOccurTime.push_back(uTime);
		return false;
	}

	m_lstOccurTime.push_back(uTime);

	//如果链表里面的数目大于最大允许的数目，说明在单位时间内产生的错误大于最大值了，要进行
	//相关错误处理了
	if (m_lstOccurTime.size() > m_uMaxExpNum)
	{
		m_lstOccurTime.pop_front();
		return true;
	}

	return false;
}

