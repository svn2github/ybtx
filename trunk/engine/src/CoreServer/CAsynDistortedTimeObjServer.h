#pragma once
#include "CDistortedTimeObj.h"

namespace sqr
{
	class CAsynDistortedTimeSceneServer;
	class CDistortedTick;

	class CAsynDistortedTimeObjServer : public CDistortedTimeObj
	{
		friend class CAsynDistortedTimeSceneServer;
	public:
		virtual void RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc);
		virtual void UnregistDistortedTick(CDistortedTick* pTick);

		virtual void RefreshTickInterval(CDistortedTick* pTick);
		virtual uint64 GetDistortedFrameTime()const;
		virtual uint64 GetDistortedServerFrameTime()const;

	protected:
		CAsynDistortedTimeObjServer(CAsynDistortedTimeSceneServer* pScene);
		~CAsynDistortedTimeObjServer();

		virtual void OnTimeRatioChanged(float fOldTimeRatio);
	};
}

