#pragma once
#include "CLock.h"
#include "ThreadHelper.h"

inline HLOCK* CLock::GetLock()
{
	return &m_Lock;
}

inline void CLock::Lock()
{
	sqr::Lock(&m_Lock);
}

inline void CLock::Unlock()
{
	sqr::Unlock(&m_Lock);
}

