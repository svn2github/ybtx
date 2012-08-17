#include "stdafx.h"
#include "CEntityClient.h"
#include "CActorClient.h"
#include "IActorClientHandler.h"
#include "CCoreObjectFollower.h"
#include "CCoreObjectDirector.h"
#include "CCharacterDirector.h"
#include "CRenderSystemClient.h"
#include "CSyncVariantClient.h"
#include "CCypherVariantCodeTable.h"
#include "CIntObjClient.h"
#include "CoreCommon.h"
#include "IDistortedTime.h"
#include "TIntObjAdapterClient.h"
#include "IActorIntObjHandler.h"
#include "TEntityAdapterClient.inl"
#include "IIntObjClientHandler.h"
#include "TCharacterAdapterClient.h"
#include "ICharacterDirectorHandler.h"
#include "IFighterDirectorHandler.h"
#include "ICharacterFollowerHandler.h"
#include "IActorDirectorHandler.h"
#include "IRenderObjClientHandler.h"

CEntityClient::CEntityClient()
:m_uEntityID(0)
,m_uCoreObjID(0)
,m_pCoreObj(NULL)
,m_pActor(NULL)
,m_pFighter(NULL)
,m_pRenderObj(NULL)
,m_bCanBeSelected(true)
,m_bLieOnTheTerrain(true)
,m_bUpdateRenderObjPos(true)
,m_fBottomSize(0)
,m_pVariantBottomSize(NULL)
{
}

CEntityClient::~CEntityClient()
{
	CCoreObjectFollower* pCoreObj = CCoreObjectFollower::GetObjectFollower(m_uCoreObjID);
	if(pCoreObj)
	{
		for (uint32 i=0; i<(uint32)eCTI_End; i++)
		{
			pCoreObj->SetViewVariantHandler(NULL, (ECodeTableIndex)i);
		}
		for (uint32 i=0; i<(uint32)eCTI_End; i++)
		{
			pCoreObj->SetSyncVariantHandler(NULL, (ECodeTableIndex)i);
		}
		m_pCoreObj->SetHandler(NULL);
	}

	delete GetActor();
	delete GetActor()->GetHandler();
}

EClientObjectType CEntityClient::GetType()const
{
	if(m_pCoreObj == NULL)
		return eCOT_Director;
	return GetCoreObj()->GetType();
}

uint32 CEntityClient::GetLocalID()const
{
	if(m_pCoreObj == NULL)
		return NULL;
	return GetCoreObj()->GetLocalID();
}

CEntityClient* CEntityClient::GetSelectedEntity(CCoreSceneClient* pCoreScene)
{
	if (!CClientRenderScene::HasInst())
	{
		return NULL;
	}

	CRenderScene* pRenderScene = CClientRenderScene::GetInst();
	CRenderObject* pRenderObj = pRenderScene->GetSelectedObj();

	if(!pRenderObj||!pRenderObj->GetModelHandler())
		return NULL;

	CIntObjClientAdapter* pIntObjAdapter = dynamic_cast<CIntObjClientAdapter*>(pRenderObj->GetModelHandler());
	if (pIntObjAdapter)
	{
		return pIntObjAdapter->GetEntity();
	}
	CCharacterDirectorAdapter* pCharacterDirectorAdapter = dynamic_cast<CCharacterDirectorAdapter*>(pRenderObj->GetModelHandler());
	if (pCharacterDirectorAdapter)
	{
		return pCharacterDirectorAdapter->GetEntity();
}
	CCharacterFollowerAdapter* pCharacterFollowerAdapter = dynamic_cast<CCharacterFollowerAdapter*>(pRenderObj->GetModelHandler());
	if (pCharacterFollowerAdapter)
	{
		return pCharacterFollowerAdapter->GetEntity();
	}

		return NULL;
}

void  CEntityClient::GetPixelPos(CFPos& PixelPos)const
{
	if(m_pCoreObj != NULL)
		GetCoreObj()->GetPixelPos(PixelPos);
	else
	{
		PixelPos.x=0;
		PixelPos.y=0;
	}
}

const CFPos&  CEntityClient::GetPixelPos()const
{
	if(m_pCoreObj == NULL)
		GenErr("m_pCoreObj == NULL");
	return GetCoreObj()->GetPixelPos();
}

void  CEntityClient::GetGridPos(CPos& GridPos)const
{
	if(m_pCoreObj != NULL)
		GetCoreObj()->GetGridPos(GridPos);
	else
		GenErr("m_pCoreObj == NULL");
}
void CEntityClient::GetDirection(CDir& dir)const
{
	m_pRenderObj->GetDirection(dir);
}

void CEntityClient::SetCanBeSelected(bool IsCanBeSelected)
{
	m_bCanBeSelected = IsCanBeSelected;
	if (GetRenderObject())
		GetRenderObject()->SetCanBeSelect(IsCanBeSelected);
}

bool CEntityClient::CanBeSelected()const
{
	return m_bCanBeSelected;
}
		
void CEntityClient::SetLieOnTheTerrain(bool bLieOn)
{
	m_bLieOnTheTerrain = bLieOn;
}

bool CEntityClient::CanBeOnTheTerrain()const
{
	return m_bLieOnTheTerrain;;
}

void CEntityClient::SetUpdateRenderObjPos( bool bUpdate )
{
	m_bUpdateRenderObjPos=bUpdate;
}

bool CEntityClient::NeedUpdateRenderObjPos()const
{
	return m_bUpdateRenderObjPos;
}

const CSyncVariantClient* CEntityClient::GetSyncVariant(ECodeTableIndex eIndex) const
{
	if(m_pCoreObj == NULL)
		GenErr("m_pCoreObj == NULL");
	return GetCoreObj()->GetSyncVariant(eIndex);
}

const CSyncVariantClient* CEntityClient::GetViewVariant(ECodeTableIndex eIndex) const
{
	if(m_pCoreObj == NULL)
		GenErr("m_pCoreObj == NULL");
	return GetCoreObj()->GetViewVariant(eIndex);
}
const CSyncVariantClientHolder* CEntityClient::GetSyncVariantHolder(ECodeTableIndex eIndex) const
{
	if(m_pCoreObj == NULL)
		GenErr("m_pCoreObj == NULL");
	return GetCoreObj()->GetSyncVariantHolder(eIndex);
}
const CSyncVariantClientHolder* CEntityClient::GetViewVariantHolder(ECodeTableIndex eIndex) const
{
	if(m_pCoreObj == NULL)
		GenErr("m_pCoreObj == NULL");
	return GetCoreObj()->GetViewVariantHolder(eIndex);
}

uint32 CEntityClient::GetSyncVariantID(const TCHAR* szPath) const
{
	if(m_pCoreObj == NULL)
		GenErr("m_pCoreObj == NULL");
	return uint32(GetCoreObj()->GetSyncVariant(eCTI_Static)->GetCode()->GetIDByPath(szPath));
}

uint32 CEntityClient::GetViewVariantID(const TCHAR* szPath) const
{
	if(m_pCoreObj == NULL)
		GenErr("m_pCoreObj == NULL");
	return uint32(GetCoreObj()->GetViewVariant(eCTI_Static)->GetCode()->GetIDByPath(szPath));
}

bool CEntityClient::IsPassive()
{
	if(m_pCoreObj == NULL)
		GenErr("m_pCoreObj == NULL");
	return GetCoreObj()->IsPassive();
}  

CPixelPath* CEntityClient::GetClonedMovePath()const
{
	if (!GetCoreObj())
	{
		return NULL;
	}
	if(GetCoreObj()->GetType() == eCOT_Director)
	{
		CCoreObjectDirector* pObj =  class_cast<CCoreObjectDirector*>(GetCoreObj());
		if (!pObj->IsPassive())
		{
			return pObj->GetClonedMovePath();
		}
	}
	return GetCoreObj()->GetClonedMovePath();
}

bool CEntityClient::IsMoving()
{
	return GetCoreObj()&&GetCoreObj()->IsMoving();
}

void CEntityClient::GetServerPixelPos(CFPos& PixelPos)const
{
	if(m_pCoreObj != NULL)	
		GetCoreObj()->GetServerPixelPos(PixelPos);
	else
		GenErr("m_pCoreObj == NULL");
}

CCoreSceneClient* CEntityClient::GetScene()const
{
	if(m_pCoreObj == NULL)
		return NULL;
	Ast(GetCoreObj()->GetScene());
	return GetCoreObj()->GetScene();
}

float CEntityClient::GetCoreSpeed()const 
{
	if(m_pCoreObj == NULL)
		return 0;
	return GetCoreObj()->GetSpeed();
}

float CEntityClient::GetEntityDist(const CEntityClient* pEntity)const
{
	Ast(m_pCoreObj);
	Ast(pEntity);
	Ast(pEntity->CoreObjisExist());

	float fDist = m_pCoreObj->Distance(pEntity->GetCoreObj());
	float fDistAdjustment = (GetBottomSize() + pEntity->GetBottomSize()) * eGridSpanForObj;
	return max(0, fDist - fDistAdjustment);
}

bool CEntityClient::GetCurPixelWayPoint(CFPos& PixelPos)const
{
	Ast(m_pCoreObj);
	return GetCoreObj()->GetCurPixelWayPoint(PixelPos);
}

float CEntityClient::GetEntityDistInGrid(const CEntityClient* pEntity)const
{
	return GetEntityDist(pEntity) / eGridSpanForObj;
}

void CEntityClient::LogEntityMsg(const string& log)
{
	if(m_pCoreObj)
	{
		m_pCoreObj->LogMsg(log);
	}
}

void CEntityClient::RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc)
{
	GetDistortedTime()->RegistDistortedTick(pTick, uCyc);
}

void CEntityClient::UnRegistDistortedTick(CDistortedTick* pTick)
{
	GetDistortedTime()->UnregistDistortedTick(pTick);
}

uint64 CEntityClient::GetDistortedProcessTime()
{
	return GetDistortedTime()->GetDistortedProcessTime();
}

uint64 CEntityClient::GetDistortedFrameTime()
{
	return GetDistortedTime()->GetDistortedFrameTime();
}

uint64 CEntityClient::GetDistortedServerFrameTime()
{
	return GetDistortedTime()->GetDistortedServerFrameTime();
}

IDistortedTime* CEntityClient::GetDistortedTime()const
{
	Ast(m_pCoreObj);
	return GetCoreObj()->GetDistortedTime();
}

void CEntityClient::SetBottomSize(float fBottomSize)
{
	Ast(!(fBottomSize < 0));
	m_fBottomSize = fBottomSize;
}

float CEntityClient::GetBottomSize()const
{
	return m_fBottomSize;
}

