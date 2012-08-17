#pragma once
#include "CDistortedTimeObj.h"

namespace sqr
{
	class CSyncDistortedTimeSceneServer;
	class CDistortedTick;

	class CSyncDistortedTimeObjServer : public CDistortedTimeObj
	{
		friend class CSyncDistortedTimeSceneServer;
	public:
		virtual void RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc);
		virtual void UnregistDistortedTick(CDistortedTick* pTick);

		virtual void RefreshTickInterval(CDistortedTick* pTick);

		virtual uint64 GetDistortedFrameTime()const;
		virtual uint64 GetDistortedServerFrameTime()const;
	protected:
		CSyncDistortedTimeObjServer(CSyncDistortedTimeSceneServer* pScene);
		~CSyncDistortedTimeObjServer();

		virtual void OnTimeRatioChanged(float fOldTimeRatio);
	};
}

