#pragma once
#include "CDistortedTimeObj.h"

namespace sqr
{
	class CDistortedTimeSceneClient;
	class CDistortedTick;
	class CCoreObjectFollower;
	class CCoreObjectDirector;

	class CDistortedTimeObjClient 
		: public CDistortedTimeObj
	{
		friend class CDistortedTimeSceneClient;
		friend class CCoreObjectFollower;
		friend class CCoreObjectDirector;
	public:
		virtual void RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc);
		virtual void UnregistDistortedTick(CDistortedTick* pTick);

		virtual void RefreshTickInterval(CDistortedTick* pTick);

		virtual uint64 GetDistortedFrameTime()const;
		virtual uint64 GetDistortedServerFrameTime()const;

	protected:
		CDistortedTimeObjClient(CDistortedTimeSceneClient* pScene);
		~CDistortedTimeObjClient();

		virtual void OnTimeRatioChanged(float fOldTimeRatio);

	private:
		uint64 GetDistortedServerFrameTime(float fTimeRatio)const;
		void SetLastDistortedProcessTime(uint64 uLastDistortedTime, uint64 uLastProcessTime);
		void SetLastDistortedFrameTime(uint64 uLastDistortedTime, uint64 uLastFrameTime);

		uint64 m_uLastDistortedServerFrameTimeWhenTimeRatioChanged;		//记录上次时间系数改变时的扭曲服务端时间
		uint64 m_uLastRealServerFrameTimeWhenTimeRatioChanged;			//记录上次时间系数改变时的服务端时间

	};
}