#pragma once
#ifdef _WIN32
#include "CRefObject.h"
#endif

namespace sqr
{
	class CDistortedTick;

	//暴露给外部代码的接口, 区域时间系统内部代码不应该调用上面的接口
	class IDistortedTime
#ifdef _WIN32
		: public CRefObject
#endif
	{
	public:
		IDistortedTime(void){}
		virtual ~IDistortedTime(void){}
		virtual void RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc)=0;
		virtual void UnregistDistortedTick(CDistortedTick* pTick)=0;
		virtual uint64 GetDistortedProcessTime()const=0;
		virtual uint64 GetDistortedFrameTime()const=0;
		virtual uint64 GetDistortedServerFrameTime()const=0;
	};

}

