#pragma once
#include "CEntityServer.h"
#include "CActorServer.h"
#include "CActorIntObj.h"
#include "CCoreObjectDictator.h"
#include "CAoiDimMgr.h"
#include "CMoveServerManager.h"
#include "CSyncVariantServerHolder.h"

template<class Traits, class AdapterType>
void CEntityServer::Init(CCoreObjectDictator *pCoreObj, uint32 uID, AdapterType *pAdapter)
{
	m_uEntityID = uID;
	m_pCoreObj = pCoreObj;
	m_pActor = new typename Traits::ActorType;
	m_pAoiDimMgr = new CAoiDimMgr;

	GetCoreObj()->SetHandler(pAdapter);
	GetActor()->SetHandler(pAdapter);
}
