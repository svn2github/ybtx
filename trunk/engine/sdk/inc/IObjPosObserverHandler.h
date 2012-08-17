#pragma once
#include "CCoreObjectServer.h"

namespace sqr
{
	class CCoreObjectServer;

	class IObjPosObserverHandler
	{
		friend class CCoreObjectServer;
	public:
		IObjPosObserverHandler():m_uID(0){}
		virtual ~IObjPosObserverHandler(){}

		virtual IWatchHandler* CastToWatchHandler(){ return NULL; }		
	private:
		uint32 m_uID;	//ID只在CCoreObjectServer对象范围内有效
	};

	class IWatchHandler
		:public IObjPosObserverHandler
	{
	public:
		IWatchHandler(){}
		~IWatchHandler(){}

		virtual IWatchHandler* CastToWatchHandler(){ return this; }		

		virtual void OnObserveeEntered(){}
		virtual void OnObserveeLeft(){}
		virtual void OnWatched(EWatchResult eResult) {}
	};
}
