#pragma once
#include "CCharacterFollower.h"
#include "CCharacterVariantHandler.h"
#include "CRenderLinkMgr.h"
#include "CDirectionMgr.h"
#include "CEntityVariantCallback.h"

template<typename Traits>
void CCharacterFollower::Init(typename Traits::CoreObjectType* pCoreObj, uint32 uID)
{
	TCharacterAdapterClient<Traits> *pAdapter = new TCharacterAdapterClient<Traits>(class_cast<typename Traits::EntityType*>(this));
	CEntityClient::Init<Traits, TCharacterAdapterClient<Traits> >(pCoreObj, uID, pAdapter);
	m_pFighter = new typename Traits::FighterType;
	m_pFighter->SetHandler(pAdapter);
	m_pCallbackHandler = NULL;
	m_bSyncDataReceived = false;
	for (uint32 i=0; i<(uint32)eCTI_End; i++)
	{
		m_vecSyncVariantHandler.push_back(new CCharacterVariantHandler);
		pCoreObj->SetSyncVariantHandler(m_vecSyncVariantHandler[i], (ECodeTableIndex)i);
	}
	for (uint32 i=0; i<(uint32)eCTI_End; i++)
	{
		m_vecViewVariantHandler.push_back(new CCharacterVariantHandler);
		pCoreObj->SetViewVariantHandler(m_vecViewVariantHandler[i], (ECodeTableIndex)i);
	}
	m_pFighter->InitVariantCallback();
	m_pRenderLinkMgr = new CRenderLinkMgr(this);
	m_pFacingMgr = new CFacingMgr;
	m_pDirectionMgr	= new CDirectionMgr(this);
	GetActor()->RegVariantCallbackFunc();
	RegSyncVarChangedCallback("BottomSize", new CEntityBottomSizeVariantCallback(this), eCTI_Agile);
}
