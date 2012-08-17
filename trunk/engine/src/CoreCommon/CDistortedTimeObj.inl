#pragma once
#include "CDistortedTimeObj.h"
#include "CDistortedTick.h"
#include "CTick.h"
#include "TimeHelper.h"
#include "TSqrAllocator.inl"
#include "TIDPtrMap.inl"

template<typename TTimeSystem>
bool CDistortedTimeObj::IntRegistDistortedTick(CDistortedTick* pTick, uint32 uCyc)
{
	if(!pTick)
		return false;
	
	if(pTick->m_pDistortedTime && static_cast<IDistortedTime*>(this) != pTick->m_pDistortedTime)
		return false;

	//IntUnregistDistortedTick<TTimeSystem>(pTick);
	
	if(!pTick->m_pDistortedTime)
	{
		pTick->m_uKeyOfDistortedTickMap =m_TickContainer.Add(pTick);
		pTick->SetDistortedTime(this);
	}

	uint32 uDistortedCyc = uint32(uCyc / GetTimeRatio());
	TTimeSystem::Inst()->Register(pTick->m_pTick, uDistortedCyc);

	return true;
}

template<typename TTimeSystem>
bool CDistortedTimeObj::IntUnregistDistortedTick(CDistortedTick* pTick)
{
	if(!pTick)
		return false;

	if(!pTick->m_pDistortedTime)
		return false;

	if(pTick->m_pDistortedTime && static_cast<IDistortedTime*>(this) != pTick->m_pDistortedTime)
		return false;

	pTick->SetDistortedTime(NULL);
	m_TickContainer.Del(pTick->m_uKeyOfDistortedTickMap);
	pTick->m_uKeyOfDistortedTickMap = uint32(-1);
	TTimeSystem::Inst()->UnRegister(pTick->m_pTick);
	
	return true;
}

template<typename TTimeSystem>
void CDistortedTimeObj::RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc)
{
	if(IntRegistDistortedTick<TTimeSystem>(pTick, uCyc))
		pTick->m_uTickCycInRealTime = uCyc;
}

template<typename TTimeSystem>
void CDistortedTimeObj::UnregistDistortedTick(CDistortedTick* pTick)
{
	if(IntUnregistDistortedTick<TTimeSystem>(pTick))
		pTick->m_uTickCycInRealTime = uint32(-1);
}

template<typename TTimeSystem>
void CDistortedTimeObj::OnTimeRatioChanged(float fOldTimeRatio)
{
	//CDistortedTimeObj::OnTimeRatioChanged(fOldTimeRatio);
	m_uLastDistortedProcessTimeWhenTimeRatioChanged = GetDistortedProcessTime(fOldTimeRatio);
	m_uLastRealProcessTimeWhenTimeRatioChanged = GetProcessTime();
	m_uLastDistortedFrameTimeWhenTimeRatioChanged = GetDistortedFrameTime<TTimeSystem>(fOldTimeRatio);
	m_uLastRealFrameTimeWhenTimeRatioChanged = TTimeSystem::Inst()->GetFrameTime();

	TickContainer::Iterator it = m_TickContainer.Begin();
	for(; it != m_TickContainer.End(); ++it)
	{
		CDistortedTick* pTick = &*it;
		uint32 uLeftDistortedTime = pTick->m_pTick->GetLeftTime();
		uint32 uLeftRealTime = uint32(uLeftDistortedTime * fOldTimeRatio);
		if(IntRegistDistortedTick<TTimeSystem>(pTick, uLeftRealTime))
		{
			pTick->m_bIntervalNeedToBeRefreshed = true;
		}
	}
}

template<typename TTimeSystem>
uint64 CDistortedTimeObj::GetDistortedFrameTime(float fTimeRatio)const
{
	//cout << "GetDistortedFrameTime " << TTimeSystem::Inst()->GetFrameTime() << " " << m_uLastRealFrameTimeWhenTimeRatioChanged << " " << m_uLastDistortedFrameTimeWhenTimeRatioChanged << endl;

	if(TimeNeedToBeDistorted())
	{
		return CalDistortedTime(TTimeSystem::Inst()->GetFrameTime(), m_uLastRealFrameTimeWhenTimeRatioChanged, m_uLastDistortedFrameTimeWhenTimeRatioChanged, fTimeRatio);
	}

	return TTimeSystem::Inst()->GetFrameTime();
}

template<typename TTimeSystem>
void CDistortedTimeObj::RefreshTickInterval(CDistortedTick* pTick)
{
	if(!pTick)
		return;

	if(pTick->m_bIntervalNeedToBeRefreshed && pTick->m_uTickCycInRealTime != uint32(-1) && 
		IntRegistDistortedTick<TTimeSystem>(pTick, pTick->m_uTickCycInRealTime))
	{
		pTick->m_bIntervalNeedToBeRefreshed = false;
	}
}
