#include "stdafx.h"
#include "CAsynDistortedTimeObjServer.h"
#include "CAsynDistortedTimeSceneServer.h"
#include "CAsynTimeSystemServer.h"
#include "CDistortedTimeObj.inl"
#include "CDistortedTick.h"

void CAsynDistortedTimeObjServer::RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc)
{
	CDistortedTimeObj::RegistDistortedTick<CAsynTimeSystemServer>(pTick, uCyc);
}

void CAsynDistortedTimeObjServer::UnregistDistortedTick(CDistortedTick* pTick)
{
	CDistortedTimeObj::UnregistDistortedTick<CAsynTimeSystemServer>(pTick);
}

void CAsynDistortedTimeObjServer::RefreshTickInterval(CDistortedTick* pTick)
{
	CDistortedTimeObj::RefreshTickInterval<CAsynTimeSystemServer>(pTick);
}

uint64 CAsynDistortedTimeObjServer::GetDistortedFrameTime()const
{
	return CDistortedTimeObj::GetDistortedFrameTime<CAsynTimeSystemServer>(GetTimeRatio());
}

uint64 CAsynDistortedTimeObjServer::GetDistortedServerFrameTime()const
{
	return CDistortedTimeObj::GetDistortedFrameTime<CAsynTimeSystemServer>(GetTimeRatio());
}

CAsynDistortedTimeObjServer::CAsynDistortedTimeObjServer(CAsynDistortedTimeSceneServer* pScene)
:CDistortedTimeObj(pScene)
{
	m_uLastDistortedFrameTimeWhenTimeRatioChanged = CAsynTimeSystemServer::Inst()->GetFrameTime();
	m_uLastRealFrameTimeWhenTimeRatioChanged = m_uLastDistortedFrameTimeWhenTimeRatioChanged;
}

CAsynDistortedTimeObjServer::~CAsynDistortedTimeObjServer()
{
}

void CAsynDistortedTimeObjServer::OnTimeRatioChanged(float fOldTimeRatio)
{
	CDistortedTimeObj::OnTimeRatioChanged<CAsynTimeSystemServer>(fOldTimeRatio);
}
