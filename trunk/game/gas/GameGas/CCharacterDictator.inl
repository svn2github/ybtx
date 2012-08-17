#pragma once
#include "CCharacterDictator.h"
#include "CEntityServerManager.h"
#include "TCharacterAdapterServer.h"
#include "CFighterDictator.h"
#include "CCoreObjectDictator.h"

template<typename EntityTraits>
void CCharacterDictator::Init(CCoreObjectDictator *pCoreObj)
{
	uint32 uID;
	const_cast<CEntityServerManager*>(CEntityServerManager::GetInst())->AddGameEntity(this, uID);
	TCharacterAdapterServer<EntityTraits> *pAdapter = new TCharacterAdapterServer<EntityTraits>(class_cast<typename EntityTraits::EntityType*>(this));
	CEntityServer::Init<EntityTraits, TCharacterAdapterServer<EntityTraits> >(pCoreObj, uID, pAdapter);
	m_pFighter = new typename EntityTraits::FighterType;
	m_pFighter->Init();
	m_pFighter->SetHandler(pAdapter);
	if (pCoreObj->CastToObjMediator())
		InitPlayerVariant();
	else
		InitNpcVariant();
}
