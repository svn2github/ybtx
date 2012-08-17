#pragma once
#include "SyncPatternCOR.h"
#include "TCoreObjAllocator.h"

namespace sqr
{

	class CSyncCoreObjectServer;
	class CObjPosObservee;
	class CCoreObjDistObserver;
	class CCoreObjTracer;
	class ISyncObjPosObserverHandler;
	class CSyncWatchHandler;

	enum EPosObserverType
	{
		ePOT_DistObserver,
		ePOT_Tracer
	};

	//CoreObjectServer可以同时拥有多个observer,一个observer和一个observee建立一对关注关系
	class CObjPosObserver
		:public CPtCORHandler
		,public CCoreObjMallocObject
	{
		friend class CSyncCoreObjectServer;
	public:
		CObjPosObserver(CSyncCoreObjectServer* pCoreObj, ISyncObjPosObserverHandler* pHandler);
		virtual EPosObserverType GetType()const=0;

		void SetHandler(ISyncObjPosObserverHandler* pHandler);
		void IntObserve(CObjPosObservee* pObservee);
		void Destroy();
		virtual void StopObserve();

		virtual void OnObserverPosChanged();
		virtual void OnObserveePosChanged();

		CCoreObjDistObserver* CastToDistObserver();
		//CCoreObjTracer* CastToTracer();

	protected:
		virtual ~CObjPosObserver();

		CSyncCoreObjectServer* m_pCoreObj;
		ISyncObjPosObserverHandler* m_pHandler;
		CObjPosObservee* GetObservee();
	private:
		typedef map<ISyncObjPosObserverHandler*, CObjPosObserver*,less<ISyncObjPosObserverHandler*>,
			TCoreObjAllocator<pair<ISyncObjPosObserverHandler*,CObjPosObserver*> > > MapPosObserver;
		void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
		MapPosObserver::iterator m_itPosObserverMap;
		TPtRefer<CObjPosObserver, CObjPosObservee> m_Observee;
	};


	class CObjPosObservee
		:public CSyncPtCORSubject
		,public CCoreObjMallocObject
	{
		friend class CObjPosObserver;
		friend class CSyncCoreObjectDictator;
	public:
		CObjPosObservee(CSyncCoreObjectServer* pCoreObj);
		~CObjPosObservee();

		void OnPosChanged();
		CSyncCoreObjectServer* GetCoreObj()const;
	private:
		CSyncCoreObjectServer* m_pCoreObj;
		TPtRefee<CObjPosObservee, CObjPosObserver> m_RefsByObserver;
	};
}
