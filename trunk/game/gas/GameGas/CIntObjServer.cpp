#include <iostream>
#include "stdafx.h"
#include "PtrlGas2GacCPPDcl.h"
#include "PtrlGas2GacCPPDef.h"
#include "CCoreObjectServer.h"
#include "CIntObjServer.h"
#include "CCoreSceneServer.h"
#include "CGlobalObjVarDefCfg.h"
#include "TEntityTraitsServer.h"
#include "TIntObjAdapterServer.h"
#include "TIntObjAdapterServer.inl"
#include "CEntityServer.inl"
#include "IActorIntObjHandler.h"
#include "CEntityServerManager.h"
#include "IIntObjCallbackHandlerServer.h"
#include "CFighterDictator.h"
#include "CCharacterDictator.h"
#include "CGenericTarget.h"
#include "TEntityAdapterServer.inl"
#include "CCoreObjectDictator.h"
#include "IFighterServerHandler.h"
#include "PatternRef.inl"
#include "CAllStateMgr.h"
#include "BaseHelper.h"

template class TPtRefer<CIntObjServer,CCharacterDictator>;

void IIntObjServerHandler::OnAoiMsgFromSelf(const void* pContext)
{
	CIntObjServer *pIntObj = class_cast<CIntObjServerAdapter*>(this)->GetEntity();
	Ast(pIntObj);
	pIntObj->OnAoiMsgFromSelf();
}	


CIntObjServer::CIntObjServer(CCoreObjectDictator *pCoreObj)
:m_sObjName("")
,m_eTrapSeeType(ETrapSeeType_None)
,m_bSeeAllNpc(false)
{
	uint32 uID;
	const_cast<CEntityServerManager*>(CEntityServerManager::GetInst())->AddGameEntity(this, uID);

	CIntObjServerAdapter *pAdapter = new CIntObjServerAdapter( this );
	CEntityServer::Init<CCIntObjTraits, CIntObjServerAdapter >(pCoreObj, uID, pAdapter);

	m_Owner.SetHolder(this);
	m_CallbackHandler.SetHolder(this);

	CSyncVariantServer* pViewVariantAttribute = GetViewVariant()->SetMember("Attribute");
	m_pViewVariantAttribute_EntityID = pViewVariantAttribute->SetMember("EntityID");
	m_pViewVariantAttribute_Type = pViewVariantAttribute->SetMember("Type");
}

CIntObjServer::~CIntObjServer()
{
	//ÓÐÊ±ÐòÒÀÀµ
	if (m_pFighter)
	{
		m_pFighter->Notify(eCE_Offline);
		delete m_pFighter;
		m_pFighter = NULL;
	}
	const_cast<CEntityServerManager*>(CEntityServerManager::GetInst())->RemoveGameEntity(this);
	SetCallbackHandler(NULL);
	m_Owner.Detach();
}

CIntObjServer* CIntObjServer::GetIntObjServerByID(uint32 uEntityID)
{
	return CEntityServerManager::GetInst()->GetIntObject(uEntityID);
}

void CIntObjServer::SetCallbackHandler( IIntObjCallbackHandlerServer* pCallbackHandler)
{
	AdvAddRef(pCallbackHandler);
	AdvDelRef(m_CallbackHandler.Get());
	pCallbackHandler ? m_CallbackHandler.Attach(pCallbackHandler->GetRefByIntObj()) : m_CallbackHandler.Detach();
}

IIntObjCallbackHandlerServer* CIntObjServer::GetCallbackHandler() const
{
	return m_CallbackHandler.Get();
}


void CIntObjServer::Release()
{
	delete this;
}

CIntObjServer* CIntObjServer::GetIntObjServer(CCoreObjectServer* pCoreObj)
{
	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler() ? pCoreObj->GetHandler() : NULL;
	if ( pHandler == NULL )
		return NULL;

	size_t eType = (size_t)(pHandler->GetTag());
	if (eType == eCOHT_Obj)
		return class_cast<CIntObjServerAdapter*>(pHandler)->GetEntity();

	return NULL;
}

void CIntObjServer::OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	CCoreObjectServer *pCoreObj = CCoreObjectServer::GetObjectServer(uObjGlobalId);
	if ( !pCoreObj )
		return;
	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
	if ( !pHandler )
		return;
	size_t eType = (size_t)(pHandler->GetTag());
	if (eType == eCOHT_Character)
	{
		CCharacterDictator*pCharacter = CCharacterDictator::GetCharacterByCoreObj(pCoreObj);
		if (!pCharacter)
			return;
		switch(m_eTrapSeeType)
		{
		case ETrapSeeType_Npc:
			{
				if (pCharacter->GetNpcAIHandler())
				{
					const string& sNpcName = pCharacter->GetNpcName();
					SetNameIter iter = m_setNpcName.find(sNpcName);
					if (m_bSeeAllNpc || iter != m_setNpcName.end())
						GetCallbackHandler()->OnIntoTrapViewAoi( GetEntityID(), this, pCharacter);
				}
			}
			break;
		case ETrapSeeType_Player:
			{
				if (pCharacter->CastToCharacterMediator())
				{
					GetCallbackHandler()->OnIntoTrapViewAoi( GetEntityID(), this, pCharacter);
				}
			}
			break;
		case ETrapSeeType_All:
			{
				GetCallbackHandler()->OnIntoTrapViewAoi( GetEntityID(), this, pCharacter);
			}
			break;
		case ETrapSeeType_None:
			{
				string strErrType = " IntObj See Relation err : OnCaughtViewSightOf £¡";
				ostringstream strm;
				strm<<" IntObj Name : "<<m_sObjName<<endl;
				LogErr(strErrType, strm.str());
			}
			break;
		}
	}
}

void CIntObjServer::OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	CCoreObjectServer *pCoreObj = CCoreObjectServer::GetObjectServer(uObjGlobalId);
	if ( !pCoreObj) 
		return;

	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
	if ( !pHandler)
		return;
	size_t eType = (size_t)(pHandler->GetTag());
	if (eType == eCOHT_Character)
	{
		CCharacterDictator*pCharacter = CCharacterDictator::GetCharacterByCoreObj(pCoreObj);
		if (!pCharacter)
			return;
		switch(m_eTrapSeeType)
		{
		case ETrapSeeType_Npc:
			{
				if (pCharacter->GetNpcAIHandler())
				{
					const string& sNpcName = pCharacter->GetNpcName();
					SetNameIter iter = m_setNpcName.find(sNpcName);
					if (m_bSeeAllNpc || iter != m_setNpcName.end())
						GetCallbackHandler()->OnLeaveTrapViewAoi( GetEntityID(), this, pCharacter);
				}
			}
			break;
		case ETrapSeeType_Player:
			{
				if (pCharacter->CastToCharacterMediator())
				{
					GetCallbackHandler()->OnLeaveTrapViewAoi( GetEntityID(), this, pCharacter);
				}
			}
			break;
		case ETrapSeeType_All:
			{
				GetCallbackHandler()->OnLeaveTrapViewAoi( GetEntityID(), this, pCharacter);
			}
			break;
		case ETrapSeeType_None:
			{
				string strErrType = " IntObj See Relation err : OnLostViewSightOf £¡";
				ostringstream strm;
				strm<<" IntObj Name : "<<m_sObjName<<endl;
				LogErr(strErrType, strm.str());
			}
			break;
		}
	}
}

CCharacterDictator*	CIntObjServer::GetOwner()
{
	return m_Owner.Get();
}

void CIntObjServer::SetOwner(CCharacterDictator* pMaster)
{
	if(pMaster)
		m_Owner.Attach(pMaster->GetRefsByIntObj());
	else
		m_Owner.Detach();
}

void CIntObjServer::SetObjName(const TCHAR* sName)
{
	m_sObjName = sName;
}

CActorIntObj* CIntObjServer::GetActor() const 
{ 
	return class_cast<CActorIntObj*>(CEntityServer::GetActor()); 
}

void CIntObjServer::SetAndSyncActionDir(uint8 uDir)
{
	CActorIntObj* pActor = GetActor();
	Ast(pActor);
	pActor->GetVariantActionDir()->SetNumber(uDir);
}

uint8 CIntObjServer::GetActionDir() const
{
	CActorIntObj* pActor = GetActor();
	Ast(pActor);
	return (uint32)pActor->GetVariantActionDir()->GetNumber<uint8>();
}

void CIntObjServer::OnAoiMsgFromSelf()
{
	uint32 uiEntityID = this->GetEntityID();
	GetCallbackHandler()->OnObjAoiMsgFromSelf( uiEntityID, this);
}

void CIntObjServer::DoPosIntObjSkill(const TCHAR* sName, uint32 uLevel,const CFPos& Pos) 
{
	if (!GetFighter())
		CreateFighter();
	CGenericTarget Target(Pos);
	GetFighter()->DoIntObjSkill(sName,uLevel,&Target);
}

void CIntObjServer::DoIntObjSkill(const TCHAR* sName, uint32 uLevel,CEntityServer* pTarget)
{
	if (!GetFighter())
		CreateFighter();
	CGenericTarget Target(pTarget->GetFighter());
	GetFighter()->DoIntObjSkill(sName,uLevel,&Target);
}

double CIntObjServer::GetStateZoomRate()
{
	if (GetFighter())
		return GetFighter()->GetAllStateMgr()->GetStateZoomRate();
	else
		return 1.0f;
}

void CIntObjServer::CreateFighter()
{
	if (!m_pFighter)
	{
		m_pFighter = new CFighterDictator;
		m_pFighter->Init();
		m_pFighter->SetHandler(class_cast<CIntObjServerAdapter*>(GetCoreObj()->GetHandler()));
	}
}

void CIntObjServer::SetIntObjSeeType(ETrapSeeType eSeeType)
{
	m_eTrapSeeType = eSeeType;
	switch(eSeeType)
	{
	case ETrapSeeType_None:
		SetObjectAoiType(EAoi_IntObjSeeNone);
		break;
	case ETrapSeeType_Player:
		SetObjectAoiType(EAoi_TrapSeePlayer);
		break;
	case ETrapSeeType_Npc:
		SetObjectAoiType(EAoi_TrapSeeNpc);
		break;
	case ETrapSeeType_All:
		SetObjectAoiType(EAoi_TrapSeeAll);
		break;
	}
}

void CIntObjServer::InsertNpcSet(const char* sNpcName)
{
	string sName(sNpcName);
	m_setNpcName.insert(sName);
}

void CIntObjServer::OnSetNpcArgEnd()
{
	if (m_setNpcName.size() != 0)
		m_bSeeAllNpc = false;
	else
		m_bSeeAllNpc = true;
}

