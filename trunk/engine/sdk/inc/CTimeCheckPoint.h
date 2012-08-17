#pragma once
#include "CDynamicObject.h"
#include "CMallocObject.h"

namespace sqr
{

	class MODULE_API CTimeCheckPoint
		:public virtual CDynamicObject
		,public CMallocObject
	{
	public:
		CTimeCheckPoint();

		void SetCheckPoint();
		void SetCheckPoint(uint32 uPassedTime);

		void SetBaseTime(uint64 uBaseTime);
		uint64 GetBaseTime()const;

		uint64 GetElapse()const;	//millisecond
		uint64 GetHDElapse()const;	//microsecond

	private:
		uint64			m_uBaseTime;
		//线程会在不同的cpu上运行，一些BIOS或者HAL的bug会导致每个cpu上的counter的进度不一样，获取的counter可能比之前的counter小。
		//这个变量用于确保后获取的时间一定大于前面获取的时间
		mutable uint64	m_uLastQueryTime;

#ifdef _WIN32
		uint64 GetTime()const;
#else
		static uint64 GetTime();
#endif
	};
	
}
