#include "stdafx.h"
#include "CSyncDistortedTimeObjServer.h"
#include "CSyncDistortedTimeSceneServer.h"
#include "CSyncTimeSystemServer.h"
#include "CDistortedTimeObj.inl"
#include "CDistortedTick.h"

void CSyncDistortedTimeObjServer::RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc)
{
	CDistortedTimeObj::RegistDistortedTick<CSyncTimeSystemServer>(pTick, uCyc);
}

void CSyncDistortedTimeObjServer::UnregistDistortedTick(CDistortedTick* pTick)
{
	CDistortedTimeObj::UnregistDistortedTick<CSyncTimeSystemServer>(pTick);
}

void CSyncDistortedTimeObjServer::RefreshTickInterval(CDistortedTick* pTick)
{
	CDistortedTimeObj::RefreshTickInterval<CSyncTimeSystemServer>(pTick);
}

uint64 CSyncDistortedTimeObjServer::GetDistortedFrameTime()const
{
	return CDistortedTimeObj::GetDistortedFrameTime<CSyncTimeSystemServer>(GetTimeRatio());
}

uint64 CSyncDistortedTimeObjServer::GetDistortedServerFrameTime()const
{
	return CDistortedTimeObj::GetDistortedFrameTime<CSyncTimeSystemServer>(GetTimeRatio());
}

CSyncDistortedTimeObjServer::CSyncDistortedTimeObjServer(CSyncDistortedTimeSceneServer* pScene)
:CDistortedTimeObj(pScene)
{
	m_uLastDistortedFrameTimeWhenTimeRatioChanged = CSyncTimeSystemServer::Inst()->GetFrameTime();
	m_uLastRealFrameTimeWhenTimeRatioChanged = m_uLastDistortedFrameTimeWhenTimeRatioChanged;
}

CSyncDistortedTimeObjServer::~CSyncDistortedTimeObjServer()
{
}

void CSyncDistortedTimeObjServer::OnTimeRatioChanged(float fOldTimeRatio)
{
	CDistortedTimeObj::OnTimeRatioChanged<CSyncTimeSystemServer>(fOldTimeRatio);
}
