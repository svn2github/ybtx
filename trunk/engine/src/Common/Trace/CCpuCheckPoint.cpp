#include "stdafx.h"
#include "CCpuCheckPoint.h"
#include "ExpHelper.h"

CCpuCheckPoint::CCpuCheckPoint(void)
{
	SetThreadTimeCheckPoint();
}

#ifdef _WIN32
inline static uint64 GetTimeFromFileTime(const FILETIME& ft)
{
	ULARGE_INTEGER li;

	li.LowPart=ft.dwLowDateTime;
	li.HighPart=ft.dwHighDateTime;
	return li.QuadPart;
}
#endif

static uint64 GetThreadTimeX100()
{
#ifdef _WIN32
	FILETIME	ftCreate,ftExit,ftKernel,ftUser;

	if( !::GetThreadTimes( GetCurrentThread() , &ftCreate,&ftExit,&ftKernel,&ftUser ) )
		GenErrnoErr("GetThreadTimeX100 failed.");

	return (GetTimeFromFileTime(ftUser) + GetTimeFromFileTime(ftKernel)) * 10;
#else
	timespec ts;
	if( -1 == clock_gettime(CLOCK_THREAD_CPUTIME_ID,&ts) )
		GenErrnoErr("clock_gettime failed.");
	return uint64(ts.tv_sec)*100000000 + ts.tv_nsec/10;
#endif
}

void CCpuCheckPoint::SetThreadTimeCheckPoint()
{
	m_uBaseTime = GetThreadTimeX100();
}

void CCpuCheckPoint::SetCheckPoint()
{
	SetThreadTimeCheckPoint();
	m_tcp.SetCheckPoint();
}


float CCpuCheckPoint::GetUsage()const
{
	const float fElapse=(float)m_tcp.GetHDElapse();

	const uint64 uTime=GetThreadTimeX100() - m_uBaseTime ;

	return uTime / fElapse;
}

uint64 CCpuCheckPoint::GetElapse()const
{
	return m_tcp.GetElapse();
}

