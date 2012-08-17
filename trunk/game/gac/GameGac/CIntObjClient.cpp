#pragma once
#include "stdafx.h"
#include "CIntObjClient.h"
#include "CCoreObjectFollower.h"
#include "IIntObjCallbackHandler.h"
#include "TEntityTraitsClient.h"
#include "CEntityClient.inl"
#include "IActorIntObjHandler.h"
#include "IFighterFollowerHandler.h"
#include "IIntObjClientHandler.h"
#include "TIntObjAdapterClient.h"
#include "CEntityClientManager.h"
#include "CActorIntObjClient.h"
#include "CEntityClient.h"
#include "TIntObjAdapterClient.inl"
#include "CFighterFollower.h"
#include "CSyncVariantClient.h"
#include "CRenderSystemClient.h"
#include "CRenderTime.h"
#include "CoreCommon.h"
#include "IRenderObjClientHandler.h"
#include "TIntObjAdapterClient.h"
#include "CEntityVariantCallback.h"

class CIntObjClientVariantHandler : public ICypherVariantHolderHandler
{
public:
	CIntObjClientVariantHandler(CIntObjClient* pIntObj)
		:m_pIntObj(pIntObj)
	{}
	~CIntObjClientVariantHandler()
	{
		m_pIntObj = NULL;
	}

private:
	void OnMemberAdded(CCypherVariant *pVariant){}
	void OnDeleteMember(CCypherVariant *pVariant){}
	void OnClearTable(CCypherVariant *pVariant){}

	void OnValueChanged(CCypherVariant *pVariant)
	{
		if(strcmp(pVariant->GetName(), "BottomSize") == 0)
		{
			float fBottomSize = pVariant->GetNumber<float>();
			m_pIntObj->SetBottomSize(fBottomSize);
		}
	}
private:
	CIntObjClient* m_pIntObj;
};


CIntObjClient::CIntObjClient(CCoreObjectFollower* pCoreObj, uint32 uID)
	: m_pCallbackHandler(NULL)
	, m_bSyncDataReceived(false)
{
	CIntObjClientAdapter *pAdapter = new CIntObjClientAdapter( this );
	CEntityClient::Init<CIntObjClientTraits, CIntObjClientAdapter >(pCoreObj, uID, pAdapter);
	m_pRenderObj = CClientRenderScene::GetInst()->CreateRenderObject(new CRenderTime(pCoreObj->GetDistortedTime()));
	m_pRenderObj->SetModelHandler(class_cast<CIntObjClientAdapter*>(pCoreObj->GetHandler()));
	m_pRenderObj->SetNodeHandler(class_cast<CIntObjClientAdapter*>(pCoreObj->GetHandler()));
	OnRefreshRenderObjPos(pCoreObj->GetPixelPos());

	m_pBottomSizeVariantHandler = new CIntObjClientVariantHandler(this);
	m_pCoreObj->SetSyncVariantHandler(m_pBottomSizeVariantHandler, eCTI_Agile);
	
	Ver(const_cast<CEntityClientManager*>(CEntityClientManager::GetInst())->AddGameEntity(this, GetEntityID()));
}

CIntObjClient::~CIntObjClient()
{
	if (m_pRenderObj)
	{
		CRenderScene* pRenderScene = CClientRenderScene::GetInst();
		if (pRenderScene->GetSelectedObj()==m_pRenderObj)
		{
			pRenderScene->SetSelectedObj(NULL);
		}
		m_pRenderObj->SetModelHandler(NULL);
		m_pRenderObj->SetNodeHandler(NULL);
		pRenderScene->DestroyRenderObject(m_pRenderObj);
	}
	m_pRenderObj=NULL;
	SetCallbackHandler(NULL);
	SafeDelete(m_pBottomSizeVariantHandler);
	const_cast<CEntityClientManager*>(CEntityClientManager::GetInst())->RemoveGameEntity(this);
}

CActorIntObjClient* CIntObjClient::GetActor()const 
{ 
	return class_cast<CActorIntObjClient*>(CEntityClient::GetActor()); 
}

CIntObjClient* CIntObjClient::GetIntObj(CCoreObjectFollower* pCoreObj)
{
	if(pCoreObj == NULL) return NULL;
	ICoreObjectFollowerHandler* pHandler = pCoreObj->GetHandler() ? pCoreObj->GetHandler() : NULL;
	if ( pHandler == NULL )
		return NULL;

	int32 eType = (int32)(pHandler->GetTag());
	if (eType == eCOHT_Obj)
		return class_cast<CIntObjClientAdapter*>(pHandler)->GetIntObj();
	return NULL;
}

CIntObjClient* CIntObjClient::GetIntObjByID(uint32 uEntityID)
{
	return	CEntityClientManager::GetInst()->GetIntObject(uEntityID);
}

void CIntObjClient::SetCallbackHandler(IIntObjCallbackHandler *pCallbackHandler )
{
	AdvDelRef(m_pCallbackHandler);
	m_pCallbackHandler = pCallbackHandler;
	AdvAddRef(m_pCallbackHandler);
}

void CIntObjClient::SetIntObjRenderType()
{
	GetRenderObject()->SetRenderStyle(ROS_LIGHTMAT1);
}

void CIntObjClient::CIntObjDoActor(const TCHAR* cActorName)
{
	if (GetActor()->CIntObjDoActor(cActorName))
	{
		GetCallbackHandler()->OnObjDoActionEnd(this);
	}
}

void CIntObjClient::OnDestroy()
{
	if (GetCallbackHandler())
	{
		if (CheckSyncDataReceived())
		{
			GetCallbackHandler()->OnLeaveSyncAoi(this);
			SetSyncDataReceived(false);
		}

		GetCallbackHandler()->OnDestroy(this);
	}

	delete this;
}

void CIntObjClient::OnRenderObjDestroyed()
{
	m_pRenderObj=NULL;
}

void CIntObjClient::OnRefreshRenderObjPos(const CFPos& PixelPos)
{
	if (NeedUpdateRenderObjPos())
	{
		float fRatio = eGridSpanForObj / eGridSpan;
		Ast( CClientRenderScene::GetInst());
		CRenderScene* pRenderScene = CClientRenderScene::GetInst();
		pRenderScene->SetRenderObjectPosition(*m_pRenderObj,CVector3f(PixelPos.x / fRatio, 0, PixelPos.y / fRatio),CanBeOnTheTerrain());
	}
}

void CIntObjClient::OnPosChanged()
{
}

void CIntObjClient::OnEnteredSyncAoi()
{
	if (CheckSyncDataReceived())
	{
		if (GetCallbackHandler()) 
			GetCallbackHandler()->OnLeaveSyncAoi(this);
	}

	if (GetCallbackHandler()) 
		GetCallbackHandler()->OnIntoSyncAoi(this);

	uint8 uActionDir = GetSyncVariant(eCTI_Agile)->GetMember("Actor")->GetMember("ActionDir")->GetNumber<uint8>();
	GetActor()->CIntObjSetDir(uActionDir);

	SetSyncDataReceived(true);
}

void CIntObjClient::OnLeftedSyncAoi()
{
	if (GetCallbackHandler()) 
		GetCallbackHandler()->OnLeaveSyncAoi(this);
	
	SetSyncDataReceived(false);
}

void CIntObjClient::OnBeSelected()
{
	if(CanBeSelected())CClientRenderScene::GetInst()->SetSelectedObj(GetRenderObject());
}

