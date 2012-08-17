//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "Timer.h"
#include "FPSCounter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace bc
{
namespace ut
{

CFPSCounter * CFPSCounter::one = NULL;

void CFPSCounter::RegisterFPS() 
{
	static DWORD m_dwStartTime = CTimer::GetSystemTime();
	m_dwFrameCnt++;
	if ( CTimer::GetSystemTime() - m_dwStartTime >= 1000  )
	{
		m_dwStartTime = CTimer::GetSystemTime();
		m_fps = m_dwFrameCnt;
		m_dwFrameCnt = 0;
	}
}

DWORD CFPSCounter::GetFPS()
{
	return (m_fps-1);
}

}
}