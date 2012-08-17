#pragma once
#include "CCoreObjMallocObject.h"

namespace sqr
{
	class CSyncCoreObjectServer;
	class CSyncWatchHandler;
	class ISyncObjPosObserverHandler
		:public CCoreObjMallocObject
	{
		friend class CSyncCoreObjectServer;
	public:
		ISyncObjPosObserverHandler():m_uID(0){}
		virtual ~ISyncObjPosObserverHandler(){}

		virtual CSyncWatchHandler* CastToWatchHandler(){ return NULL; }		
	protected:
		uint32 m_uID;
	};

}

