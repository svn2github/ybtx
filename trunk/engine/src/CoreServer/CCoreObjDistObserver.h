#pragma once
#include "CoreObjectServerPosRelation.h"
#include "CoreObjectDefs.h"

namespace sqr
{
	class CSyncCoreObjectDictator;
	class CSyncCoreObjectServer;
	class CObjPosObservee;

	class CCoreObjDistObserver
		:public CObjPosObserver
	{
		friend class CSyncCoreObjectServer;
	public:
		//Dist需要考虑CSyncCoreObjectDictator中的BarrierSize,因此只能用Dictator对象来观察
		explicit CCoreObjDistObserver(CSyncCoreObjectDictator* pCoreObj, CSyncWatchHandler* pHandler);
		virtual EPosObserverType GetType()const;

		EWatchResult Observe(CObjPosObservee* pObservee, float fPixelDistThreshold);
		virtual void StopObserve();
		virtual void OnObserverPosChanged();
		virtual void OnObserveePosChanged();
		
		void SetPixelDistThreshold(float fPixelDist);
	private:
		CSyncWatchHandler* GetHandler();
		void DistChanged();
		~CCoreObjDistObserver();
		float m_fPixelDistThreshold;
		bool m_bObserveeInScope;
	};
}

