//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "Timer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace bc
{
namespace ut
{

CTimer * CTimer::one = NULL;

void CTimer::Start()
{
	m_dwThisTime = m_dwLastTime = GetSystemTime();
}

bool CTimer::IsOverEF()
{
	m_dwThisTime = GetSystemTime();
	DWORD delta = m_dwThisTime - m_dwLastTime;
	m_dwFrameTime += delta;
	m_iDeltaTime += delta;
	m_dwLastTime = m_dwThisTime;
	return m_iDeltaTime >= m_dwTimePerEF;
}

DWORD CTimer::GetFrameTime()
{
	return m_dwFrameTime;
}

void CTimer::Clear()
{
	m_dwFrameTime = 0;
}

void CTimer::PauseUpdate()
{
	m_bStop = true;
}

void CTimer::ResumeUpdate()
{
	m_dwLastTime = GetSystemTime();
	m_bStop = false;
}

bool CTimer::IsStop()
{
	return m_bStop;
}

void CTimer::Down()
{
	m_iDeltaTime -= m_dwTimePerEF;
}

}
}