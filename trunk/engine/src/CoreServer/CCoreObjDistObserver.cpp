#include "stdafx.h"
#include "CCoreObjDistObserver.h"
#include "CSyncWatchHandler.h"
#include "CSyncCoreObjectDictator.h"

CCoreObjDistObserver::CCoreObjDistObserver(CSyncCoreObjectDictator* pCoreObj, CSyncWatchHandler* pHandler)
:CObjPosObserver(pCoreObj, pHandler)
,m_fPixelDistThreshold(0.0f)
,m_bObserveeInScope(false)
{
}

CCoreObjDistObserver::~CCoreObjDistObserver()
{
	StopObserve();
}

EPosObserverType CCoreObjDistObserver::GetType()const
{
	return ePOT_DistObserver;
}

EWatchResult CCoreObjDistObserver::Observe(CObjPosObservee* pObservee, float fPixelDistThreshold)
{
	IntObserve(pObservee);
	m_pCoreObj->AddGPCNFlagRef();
	SetPixelDistThreshold(fPixelDistThreshold);
	return m_bObserveeInScope ? eWR_InScope : eWR_NotInScope;
}

void CCoreObjDistObserver::StopObserve()
{
	if(!GetObservee())
		return;
	m_pCoreObj->DelGPCNFlagRef();
	CObjPosObserver::StopObserve();
}

void CCoreObjDistObserver::OnObserverPosChanged()
{
	DistChanged();
}

void CCoreObjDistObserver::OnObserveePosChanged()
{
	DistChanged();
}

CSyncWatchHandler* CCoreObjDistObserver::GetHandler()
{
	return static_cast<CSyncWatchHandler*>(m_pHandler);
}

void CCoreObjDistObserver::DistChanged()
{
	if(!m_pHandler)
		return;

	if(!GetObservee())
		return;

	float fCurDist = m_pCoreObj->CastToObjDictator()->Distance(GetObservee()->GetCoreObj());
	bool bObserveeInScope = fCurDist <= m_fPixelDistThreshold;

	//cout << m_pCoreObj->GetGlobalID() << " ObserveePosChanged " << fCurDist << " " << m_fPixelDistThreshold << " " << m_bObserveeInScope << "->" << bObserveeInScope << endl;
	if(m_bObserveeInScope != bObserveeInScope)
	{
		if(bObserveeInScope)
			class_cast<CSyncWatchHandler*>(m_pHandler)->OnObserveeEntered(m_pCoreObj->GetGlobalID());
		else
			class_cast<CSyncWatchHandler*>(m_pHandler)->OnObserveeLeft(m_pCoreObj->GetGlobalID());
		m_bObserveeInScope = bObserveeInScope;
	}
}
		
void CCoreObjDistObserver::SetPixelDistThreshold(float fPixelDist)
{
	Ast(m_pHandler);
	Ast(GetObservee());
	m_fPixelDistThreshold = fPixelDist;

	float fCurDist = m_pCoreObj->CastToObjDictator()->Distance(GetObservee()->GetCoreObj());
	bool bObserveeInScope;
	if(fCurDist <= m_fPixelDistThreshold)
		bObserveeInScope = true;
	else
		bObserveeInScope = false;

	if(m_bObserveeInScope != bObserveeInScope)
	{
		if(bObserveeInScope)
			class_cast<CSyncWatchHandler*>(m_pHandler)->OnObserveeEntered(m_pCoreObj->GetGlobalID());
		else
			class_cast<CSyncWatchHandler*>(m_pHandler)->OnObserveeLeft(m_pCoreObj->GetGlobalID());
		m_bObserveeInScope = bObserveeInScope;
	}
}
