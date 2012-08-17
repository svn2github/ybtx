#include "stdafx.h"
#include "CServantServerMgr.h"
#include "CTriggerEvent.h"
#include "CCharacterMediator.h"
#include "TServantMgr.inl"
#include "CFighterDictator.h"
#include "NpcInfoMgr.h"
#include "CAllStateMgr.h"

CServantServerMgr::CServantServerMgr(CCharacterDictator* pOwner)
:TServantMgr<CCharacterDictator>(pOwner)
{
}

CServantServerMgr::~CServantServerMgr()
{
	ServantIDMapType& ServantMap = this->GetServantIDMapType();
	if (ServantMap.size() != 0 && GetOwner()->CastToCharacterMediator())
	{
		string strErrType = "mediator do not clear and save all servant when destory !";
		LogErr(strErrType.c_str());
	}
}

void CServantServerMgr::OnRemoveServant( CCharacterDictator* pServant )
{
	if (pServant)
	{
		ENpcType eType = pServant->GetNpcType();
		CCharacterDictator* pMaster = pServant->GetMaster();
		Ast(pMaster);
		CFighterDictator* pMasterFighter = pMaster->GetFighter();
		Ast(pMasterFighter);
		pMasterFighter->OnDestroyServant(eType, pServant->GetEntityID());
		CTriggerEvent::MessageEvent(eTST_Trigger, pServant->GetFighter(), NULL, eSET_RemoveFromMaster);
		//pServant->GetFighter()->GetAllStateMgr()->ClearAll();
	}
	OnServantChanged();
}

//死亡
void CServantServerMgr::ClearAllServantByMasterDie()
{
	ServantIDMapType& ServantMap = this->GetServantIDMapType();
	ServantIDMapType::iterator it = ServantMap.begin();
	for(;it!=ServantMap.end();)
	{
		CCharacterDictator* pServant = CCharacterDictator::GetCharacterByID(it->first);
		ServantMap.erase(it++);
		if(pServant && pServant->GetFighter())
		{
			CTriggerEvent::MessageEvent(eTST_Trigger, pServant->GetFighter(), NULL, eSET_RemoveFromMaster);
			pServant->GetFighter()->GetAllStateMgr()->ClearAll();
			CCharacterDictator* pMaster = pServant->GetMaster();
			if (pMaster)
				pMaster->GetFighter()->OnDestroyServant(pServant->GetNpcType(), pServant->GetEntityID());
			pServant->OnMasterDestroyed();
			ServantResByMasterDie(pServant);
		}
	}
	TServantMgr<CCharacterDictator>::ClearAllServant();
	OnServantChanged();
}

void CServantServerMgr::ClearAllServant()
{
	ServantIDMapType& ServantMap = this->GetServantIDMapType();
	ServantIDMapType::iterator it = ServantMap.begin();
	for(;it!=ServantMap.end();)
	{
		CCharacterDictator* pServant = CCharacterDictator::GetCharacterByID(it->first);
		ServantMap.erase(it++);
		if(pServant && pServant->GetFighter())
		{
			CTriggerEvent::MessageEvent(eTST_Trigger, pServant->GetFighter(), NULL, eSET_RemoveFromMaster);
			pServant->GetFighter()->GetAllStateMgr()->ClearAll();
			GetOwner()->GetFighter()->OnDestroyServant(pServant->GetNpcType(), pServant->GetEntityID());
			pServant->OnBeginDestory(false,0,0,true);
		}
	}
	TServantMgr<CCharacterDictator>::ClearAllServant();
}

//同服异服切场景
void CServantServerMgr::SaveAndClearAllServant(bool bSaveServant)
{
	uint32 uServantNum = GetServantNum();
	if (uServantNum != 0 && GetOwner()->CastToCharacterMediator() && bSaveServant)
		SaveAllServantInfo();

	ClearAllServant();
	//如果不存则清空数据库里面的召唤兽
	if (!bSaveServant)
		SaveAllServantInfo();
}

void CServantServerMgr::SaveAllServantInfo()
{
	if (GetOwner()->CastToCharacterMediator())
	{
		GetOwner()->CastToCharacterMediator()->SaveServantInfo();
	}
}

void CServantServerMgr::OnServantChanged()
{
	if (GetOwner()->CastToCharacterMediator())
	{
		GetOwner()->CastToCharacterMediator()->SaveServantInfoOnServantChange();
	}
}

void CServantServerMgr::ServantResByMasterDie(CCharacterDictator* pServant)
{
	ENpcType eType = pServant->GetNpcType();
	if (NpcInfoMgr::BeKillServantByMasterDestory(eType))
		pServant->OnBeginDestory();
}

