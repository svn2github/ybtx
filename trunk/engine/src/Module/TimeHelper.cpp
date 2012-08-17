#include "stdafx.h"
#include "TimeHelper.h"
#include "CTimeCheckPoint.h"

namespace sqr
{
	static CTimeCheckPoint gs_tcpProcess;

	uint64 GetHDProcessTime()
	{
		return gs_tcpProcess.GetHDElapse();
	}
	uint64 GetProcessTime()
	{
		return gs_tcpProcess.GetElapse();
	}

	uint64 GetProcessBeginTime()
	{
		return gs_tcpProcess.GetBaseTime();
	}

	void SetProcessBeginTime(uint64 uTime)
	{
		gs_tcpProcess.SetBaseTime(uTime);
	}	
}

