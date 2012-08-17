#include "stdafx.h"
#include "CoreObjectServerPosRelation.h"
#include "ISyncObjPosObserverHandler.h"
#include "CSyncCoreObjectServer.h"
#include "CCoreObjTracer.h"
#include "CCoreObjDistObserver.h"
#include "TSqrAllocator.inl"

enum EServerObjectPosEvent
{
	eSOPE_PosChanged = 0,
	eSOPE_Count,
};


CObjPosObserver::CObjPosObserver(CSyncCoreObjectServer* pCoreObj, ISyncObjPosObserverHandler* pHandler)
:m_pCoreObj(pCoreObj)
,m_pHandler(pHandler)
{
	Ast(m_pCoreObj);
	pair<MapPosObserver::iterator, bool> ret = m_pCoreObj->m_mapPosObserver.insert(make_pair(m_pHandler, this));
	Ast(ret.second);
	m_itPosObserverMap = ret.first;
	m_Observee.SetHolder(this);
}

CObjPosObserver::~CObjPosObserver()
{
}

CCoreObjDistObserver* CObjPosObserver::CastToDistObserver()
{
	return GetType() == ePOT_DistObserver ? static_cast<CCoreObjDistObserver*>(this) : NULL;
}

//CCoreObjTracer* CObjPosObserver::CastToTracer()
//{
//	return GetType() == ePOT_Tracer ? static_cast<CCoreObjTracer*>(this) : NULL;
//}

void CObjPosObserver::Destroy()
{
	m_pCoreObj->m_mapPosObserver.erase(m_itPosObserverMap);
	StopObserve();
	delete this;
}

void CObjPosObserver::SetHandler(ISyncObjPosObserverHandler* pHandler)
{
	m_pHandler = pHandler;
}

void CObjPosObserver::IntObserve(CObjPosObservee* pObservee)
{
	Ast(pObservee);
	if(GetObservee())
		StopObserve();

	m_Observee.Attach(pObservee->m_RefsByObserver);
	GetObservee()->Attach(this, eSOPE_PosChanged);
	CSyncCoreObjectServer* pCoreObj = GetObservee()->GetCoreObj();
	pCoreObj->AddGPCNFlagRef();
}

void CObjPosObserver::StopObserve()
{
	CObjPosObservee* pObservee = GetObservee();
	if(!pObservee)
		return;

	pObservee->GetCoreObj()->DelGPCNFlagRef();
	pObservee->Detach(this, eSOPE_PosChanged);
	m_Observee.Detach();
}

void CObjPosObserver::OnObserverPosChanged()
{
}

void CObjPosObserver::OnObserveePosChanged()
{
}

void CObjPosObserver::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	switch(uEvent)
	{
	case eSOPE_PosChanged:
		OnObserveePosChanged();
		break;
	default:
		break;
	}
}

CObjPosObservee* CObjPosObserver::GetObservee()
{
	return m_Observee.Get();
}

CObjPosObservee::CObjPosObservee(CSyncCoreObjectServer* pCoreObj)
:CSyncPtCORSubject(eSOPE_Count)
,m_pCoreObj(pCoreObj)
{
	m_RefsByObserver.SetHolder(this);
}

CObjPosObservee::~CObjPosObservee()
{
}

void CObjPosObservee::OnPosChanged()
{
	Notify(eSOPE_PosChanged);
}

CSyncCoreObjectServer* CObjPosObservee::GetCoreObj()const
{
	return m_pCoreObj;
}
