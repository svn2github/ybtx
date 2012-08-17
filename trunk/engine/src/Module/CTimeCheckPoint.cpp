#include "stdafx.h"
#include "CTimeCheckPoint.h"
#include "ExpHelper.h"
#ifndef _WIN32
#include <sys/time.h>
#endif


#ifdef _WIN32
class CFrequency
{
private:
	double	m_dMicroSecondRatio;

	CFrequency()
	{
		LARGE_INTEGER Frequency;
		if( !QueryPerformanceFrequency(&Frequency) )
			GenErrnoErr("QueryPerformanceFrequency failed.");
		
		m_dMicroSecondRatio=1000000.0/Frequency.QuadPart;
	}

public:
	inline double Get()const
	{
		return m_dMicroSecondRatio;
	}

	inline static CFrequency& Inst()
	{
		static CFrequency Inst;
		return Inst;
	}
};
#endif


CTimeCheckPoint::CTimeCheckPoint()
: m_uLastQueryTime(0)
{
	SetCheckPoint();
}

void CTimeCheckPoint::SetCheckPoint()
{
	m_uBaseTime = GetTime();
}

#ifdef _WIN32
uint64 CTimeCheckPoint::GetTime()const
#else
uint64 CTimeCheckPoint::GetTime()
#endif
{
	uint64 uTime;
#ifdef _WIN32
	LARGE_INTEGER uCounter;
	QueryPerformanceCounter( &uCounter );
	uTime=uint64( uCounter.QuadPart*CFrequency::Inst().Get() );
	if( uTime > m_uLastQueryTime )
		m_uLastQueryTime = uTime;
	else
		uTime = m_uLastQueryTime; 
#else
	timespec ts;
	if( -1 == clock_gettime(CLOCK_MONOTONIC,&ts) )
		GenErrnoErr("clock_gettime failed.");
	uTime = uint64(ts.tv_sec)*1000000 + ts.tv_nsec/1000;
#endif
	return uTime;
}

void CTimeCheckPoint::SetCheckPoint( uint32 uPassedTime )
{
	m_uBaseTime = GetTime();
	m_uBaseTime -= uPassedTime;
}

uint64 CTimeCheckPoint::GetBaseTime()const
{
	return m_uBaseTime;
}

void CTimeCheckPoint::SetBaseTime(uint64 uBaseTime)
{
	m_uBaseTime=uBaseTime;
}

uint64 CTimeCheckPoint::GetHDElapse()const
{
	return uint64( GetTime()- m_uBaseTime );
}

uint64 CTimeCheckPoint::GetElapse()const
{
	return GetHDElapse()/1000;//返回值单位为毫秒
}

