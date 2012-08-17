#include "stdafx.h"
#include "CDistortedTimeObj.h"
#include "CDistortedTimeScene.h"
#include "TimeHelper.h"
#include "CDistortedTick.h"
#include "TSqrAllocator.inl"
#include "TIDPtrMap.inl"
#include "ErrLogHelper.h"

CDistortedTimeObj::CDistortedTimeObj(CDistortedTimeScene* pScene)
:m_uLastDistortedProcessTimeWhenTimeRatioChanged(GetProcessTime())
,m_uLastRealProcessTimeWhenTimeRatioChanged(m_uLastDistortedProcessTimeWhenTimeRatioChanged)
,m_uTimeGroup(1)
,m_pDistortedTimeScene(pScene)
{
#ifdef _WIN32
	SQR_TRY
	{
		AddRef();
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
#endif
	m_pDistortedTimeScene->AddDistortedTimeObj(this);
}

CDistortedTimeObj::~CDistortedTimeObj()
{
	m_pDistortedTimeScene->DelDistortedTimeObj(this);
	m_pDistortedTimeScene->OnDistortedTimeObjDestroyed();

	TickContainer::Iterator it = m_TickContainer.Begin();
	for(; it != m_TickContainer.End(); ++it)
	{
		it->SetDistortedTime(NULL);
	}
}

uint64 CDistortedTimeObj::GetDistortedProcessTime(float fTimeRatio)const
{
	if(TimeNeedToBeDistorted())
	{
		return CalDistortedTime(GetProcessTime(), m_uLastRealProcessTimeWhenTimeRatioChanged, m_uLastDistortedProcessTimeWhenTimeRatioChanged, fTimeRatio);
	}

	return GetProcessTime();
}

uint64 CDistortedTimeObj::CalDistortedTime(uint64 uCurTime, uint64 uLastRealTimeWhenTimeRatioChanged, uint64 uLastDistortedTimeWhenTimeRatioChanged, float fTimeRatio)const
{
	return uint64((uCurTime - uLastRealTimeWhenTimeRatioChanged) * fTimeRatio + uLastDistortedTimeWhenTimeRatioChanged);
}

uint64 CDistortedTimeObj::GetDistortedProcessTime()const
{
	return GetDistortedProcessTime(GetTimeRatio());
}

void CDistortedTimeObj::SetTimeGroup(uint32 uTimeGroup)
{
	float fOldTimeRatio = GetTimeRatio();
	m_uTimeGroup = uTimeGroup;

	if(fOldTimeRatio != GetTimeRatio())
		OnTimeRatioChanged(fOldTimeRatio);
}

uint32 CDistortedTimeObj::GetTimeGroup()const
{
	return m_uTimeGroup;
}

float CDistortedTimeObj::GetTimeRatio()const
{
	if(TimeNeedToBeDistorted())
		return m_pDistortedTimeScene->GetTimeDistortedRatio();

	return 1;
}

void CDistortedTimeObj::TransferTo(CDistortedTimeScene* pNewScene)
{
	if(pNewScene == m_pDistortedTimeScene)
		return;
	CDistortedTimeScene* pOldScene = m_pDistortedTimeScene;

	pOldScene->DelDistortedTimeObj(this);
	pNewScene->AddDistortedTimeObj(this);
	m_pDistortedTimeScene = pNewScene;

	if(pOldScene->GetTimeDistortedRatio() != m_pDistortedTimeScene->GetTimeDistortedRatio())
	{
		OnTimeRatioChanged(pOldScene->GetTimeDistortedRatio());
	}
}

bool CDistortedTimeObj::TimeNeedToBeDistorted()const
{
	return m_uTimeGroup != m_pDistortedTimeScene->GetTimeGroup();
}

void CDistortedTimeObj::OnTimeRatioChanged(float fOldTimeRatio)
{
}
