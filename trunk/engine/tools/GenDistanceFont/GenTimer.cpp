#include "stdafx.h"
#include "GenTimer.h"

namespace sqr
{
	void Timer::restart()
	{
		m_starttime = timeGetTime();
	}

	uint Timer::elapsed()
	{
		return (timeGetTime() - m_starttime) / 1000;
	}
}