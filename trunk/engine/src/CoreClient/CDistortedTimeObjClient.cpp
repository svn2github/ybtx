#include "stdafx.h"
#include "CDistortedTimeObjClient.h"
#include "CDistortedTimeSceneClient.h"
#include "CTimeSystemClient.h"
#include "CDistortedTimeObj.inl"
#include "CDistortedTick.h"
#include "CConnClient.h"
#include "ErrLogHelper.h"

void CDistortedTimeObjClient::RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc)
{
	CDistortedTimeObj::RegistDistortedTick<CTimeSystemClient>(pTick, uCyc);
}

void CDistortedTimeObjClient::UnregistDistortedTick(CDistortedTick* pTick)
{
	CDistortedTimeObj::UnregistDistortedTick<CTimeSystemClient>(pTick);
}

void CDistortedTimeObjClient::RefreshTickInterval(CDistortedTick* pTick)
{
	CDistortedTimeObj::RefreshTickInterval<CTimeSystemClient>(pTick);
}

uint64 CDistortedTimeObjClient::GetDistortedFrameTime()const
{
	return CDistortedTimeObj::GetDistortedFrameTime<CTimeSystemClient>(GetTimeRatio());
}

uint64 CDistortedTimeObjClient::GetDistortedServerFrameTime()const
{
	//cout << CTimeSystemClient::Inst()->GetFrameTime() << " " << CConnClient::Inst()->m_nClientServerTimeDiff << " " << GetTimeRatio() << endl; 
	//return uint64(CTimeSystemClient::Inst()->GetFrameTime() - CConnClient::Inst()->m_nClientServerTimeDiff * GetTimeRatio());
	return GetDistortedServerFrameTime(GetTimeRatio());
}

uint64 CDistortedTimeObjClient::GetDistortedServerFrameTime(float fTimeRatio)const
{
	uint64 uCurTime = CConnClient::Inst()->GetServerFrameTime();
	if(TimeNeedToBeDistorted())
	{
		uCurTime = uint64((int64(uCurTime) - int64(m_uLastRealServerFrameTimeWhenTimeRatioChanged)) * fTimeRatio + m_uLastDistortedServerFrameTimeWhenTimeRatioChanged);
	}

	//cout << "GetDistortedServerFrameTime " << m_uLastRealServerFrameTimeWhenTimeRatioChanged << " " << m_uLastDistortedServerFrameTimeWhenTimeRatioChanged << " " << uCurTime << endl;
	return uCurTime;
}

void CDistortedTimeObjClient::SetLastDistortedProcessTime(uint64 uLastDistortedTime, uint64 uLastProcessTime)
{
	m_uLastDistortedProcessTimeWhenTimeRatioChanged = CalDistortedTime(GetProcessTime(), uLastProcessTime, uLastDistortedTime, GetTimeRatio());
}

void CDistortedTimeObjClient::SetLastDistortedFrameTime(uint64 uLastDistortedTime, uint64 uLastFrameTime)
{
	m_uLastDistortedFrameTimeWhenTimeRatioChanged = CalDistortedTime(CTimeSystemClient::Inst()->GetFrameTime(), 
uLastFrameTime, uLastDistortedTime, GetTimeRatio());
	//cout << "setlastdistortedFrameTime " << m_uLastDistortedFrameTimeWhenTimeRatioChanged << endl;
}

CDistortedTimeObjClient::CDistortedTimeObjClient(CDistortedTimeSceneClient* pScene)
:CDistortedTimeObj(pScene)
,m_uLastDistortedServerFrameTimeWhenTimeRatioChanged(0)
,m_uLastRealServerFrameTimeWhenTimeRatioChanged(0)
{
	SQR_TRY
	{
		m_uLastDistortedServerFrameTimeWhenTimeRatioChanged = CConnClient::Inst()->GetServerFrameTime();
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
	m_uLastRealServerFrameTimeWhenTimeRatioChanged = m_uLastDistortedServerFrameTimeWhenTimeRatioChanged;
	m_uLastDistortedFrameTimeWhenTimeRatioChanged = CTimeSystemClient::Inst()->GetFrameTime();
	m_uLastRealFrameTimeWhenTimeRatioChanged = m_uLastDistortedFrameTimeWhenTimeRatioChanged;
	//cout << "CDistortedTimeObjClient::CDistortedTimeObjClient " << m_uLastRealServerFrameTimeWhenTimeRatioChanged << " " << m_uLastDistortedServerFrameTimeWhenTimeRatioChanged << endl;
}

CDistortedTimeObjClient::~CDistortedTimeObjClient()
{
}

void CDistortedTimeObjClient::OnTimeRatioChanged(float fOldTimeRatio)
{
	m_uLastDistortedServerFrameTimeWhenTimeRatioChanged = GetDistortedServerFrameTime(fOldTimeRatio);
	m_uLastRealServerFrameTimeWhenTimeRatioChanged = CConnClient::Inst()->GetServerFrameTime();
	//cout << "CDistortedTimeObjClient::OnTimeRatioChanged " << m_uLastRealServerFrameTimeWhenTimeRatioChanged << " " << m_uLastDistortedServerFrameTimeWhenTimeRatioChanged << " old " << fOldTimeRatio << " new " << GetTimeRatio() << endl;
	CDistortedTimeObj::OnTimeRatioChanged<CTimeSystemClient>(fOldTimeRatio);
}
