#pragma once
#include "ModuleDefs.h"
#include "BaseTypes.h"
#include "CTimeCheckPoint.h"

namespace sqr
{

	class MODULE_API CCpuCheckPoint
	{
	public:
		CCpuCheckPoint();

		void SetCheckPoint();

		float GetUsage()const;
		uint64 GetElapse()const;
	private:
		uint64			m_uBaseTime;
		
		void SetThreadTimeCheckPoint();

		CTimeCheckPoint	m_tcp;
	};

}

