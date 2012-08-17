#pragma once
#include "CEntityClient.h"

template<class Traits, class AdapterType>
void CEntityClient::Init(CCoreObjectFollower *pCoreObj, uint32 uEntityID, AdapterType *pAdapter)
{
	m_uEntityID = uEntityID;
	m_pCoreObj = pCoreObj;
	m_uCoreObjID = pCoreObj->GetGlobalID();
	m_pCoreObj->SetHandler(pAdapter);
	m_pActor = new typename Traits::ActorType;
	m_pActor->SetHandler(pAdapter);
}
