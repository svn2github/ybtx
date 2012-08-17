#pragma once
#include "ISyncObjPosObserverHandler.h"

namespace sqr
{
	class CSyncWatchHandler
		:public ISyncObjPosObserverHandler
	{
	public:
		CSyncWatchHandler(){}
		~CSyncWatchHandler(){}

		virtual CSyncWatchHandler* CastToWatchHandler(){ return this; }		

		void OnObserveeEntered(uint32 uObjID); 
		void OnObserveeLeft(uint32 uObjID);
	};
}
