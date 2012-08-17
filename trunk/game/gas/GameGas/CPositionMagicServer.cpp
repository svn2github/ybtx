#include "stdafx.h"
#include "CPositionMagicServer.h"
#include "CPositionMagicCfgServer.h"
#include "CMagicServer.inl"
#include "CCoreSceneServer.h"
#include "CFighterDictator.h"
#include "CMagicEffServer.h"
#include "CMagicMgrServer.h"
#include "CCharacterDictator.h"
#include "CMagicEffDataMgrServer.h"
#include "CoreCommon.h"
#include "CFilterOpServer.h"
#include "CEntityServerManager.h"
#include "CGenericTarget.h"
#include "CSkillInstServer.h"
#include "CCoreObjectDictator.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"


CPositionMagicServer::CPositionMagicServer(CSkillInstServer* pInst, const CPositionMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, const CFPos& pos)
:CMagicServer(pInst,pFrom,0)
,m_pCfg(new CPositionMagicCfgServerSharedPtr(pCfg))
,m_Pos(pos)
,m_uCount(0)
,m_uTime(1)
,m_bSingleObjCount(false)
{
	m_pCancelEffData = new CEffDataByGlobalIDMgr;
	GetFighter()->Attach(this, eCE_Offline);
	GetFighter()->Attach(this, eCE_Die);
	GetFighter()->Attach(this, eCE_ChangeMapEnd);
	GetFighter()->Attach(this, eCE_FinishDuel);
	m_fRadian			= float( GetCfgSharedPtr()->GetRadius()->GetDblValue(pFrom,pInst->GetSkillLevel() - 1) );
	m_uAllTime			= GetCfgSharedPtr()->GetTime()->GetIntValue(pFrom,pInst->GetSkillLevel() - 1);
	m_uTouchCount		= GetCfgSharedPtr()->GetAmountLimit()->GetIntValue(pFrom,pInst->GetSkillLevel() - 1);
	
	CPositionMagicServer* pPosMagicServer = GetFighter()->GetMagicMgr()->GetPositionMagic();
	if (pPosMagicServer != NULL)
	{
		pPosMagicServer->UnRegTick();
		pPosMagicServer->Cancel();
	}
	
	CCoreObjectDictator* pObj = CreateCoreObj<CPositionMagicServer>(pos);
	SetEyeSight(m_fRadian);
	pObj->SetQuerySize(1.0f);

	pObj->PostAoiMsgToSelf(NULL);

	if (GetCfgSharedPtr()->GetMutexType())
	{
		GetFighter()->GetMagicMgr()->SetPositionMagic(this);
	}
}

CPositionMagicServer::~CPositionMagicServer()
{
	UnRegTick();
	DeleteObj(GetFighter()->GetScene());
	m_setEntityID.clear();

	if(m_pCancelEffData)
	{
		delete m_pCancelEffData;
		m_pCancelEffData = NULL;
	}
	SafeDelete(m_pCfg);
}

void CPositionMagicServer::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	if(pSubject == GetFighter())
	{
		switch(uEvent)
		{
		case eCE_Die:
		case eCE_Offline:
		case eCE_FinishDuel:
			Cancel();
			break;
		case eCE_ChangeMapEnd:
			CCoreSceneServer* pScene = reinterpret_cast<CCoreSceneServer*>(pArg);
			DeleteObj(pScene);
			Cancel();
			break;
		}
	}
}

void CPositionMagicServer::OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	CCoreObjectServer* pCoreObj = CCoreObjectServer::GetObjectServer(uObjGlobalId);
	if (!pCoreObj || !pCoreObj->GetHandler()) 
		return;

	size_t uType = (size_t)(pCoreObj->GetHandler()->GetTag());
	CCharacterDictator* pCharacter=NULL;
	if(uType == (uint32)(eCOHT_Character))
	{
		pCharacter =CCharacterDictator::GetCharacterByCoreObj(pCoreObj);
	}
	CFighterDictator* pFromFighter = GetFighter();
	uint32 uCheckID = GetCheckID();
	CFighterDictator* pTo = pCharacter?pCharacter->GetFighter():NULL;
	DoMagicEff(pTo);
	if (!pFromFighter->GetMagicMgr()->IsValid(uCheckID))
	{
		return;
	}
	if(!Registered())
	{
		if (GetCfgSharedPtr()->GetDotMagicEff() != NULL )
		{
			GetFighter()->Detach(this, eCE_Die);
			if(m_bSingleObjCount)
				return;
			const CMagicEffServerSharedPtr& pMagicEffServer = GetCfgSharedPtr()->GetDotMagicEff();
			CGenericTarget Target(pTo);
			VecEntityID_t vecEntityID;
			FilterLimit filterLimit;
			MakeFilterLimit<CPositionMagicServer>(filterLimit);
			CFilterOpServer::FilterOperateObject(m_pSkillInst, vecEntityID, GetCfgSharedPtr()->GetOperateObject(), filterLimit, GetFighter(), &Target);
			VecEntityID_t::iterator it_obj = vecEntityID.begin();
			for (; it_obj != vecEntityID.end(); ++it_obj)
			{
				CCharacterDictator* pCharacter = CEntityServerManager::GetInst()->GetCharacter(*it_obj);
				if (NULL==pCharacter)
				{
					continue;
				}
				CFighterDictator* pToFighter=pCharacter->GetFighter();
				if(pMagicEffServer)
					pMagicEffServer->Do(m_pSkillInst, GetFighter(), pToFighter);		
				if (!pFromFighter->GetMagicMgr()->IsValid(uCheckID))
				{
					return;
				}
				if(GetCfgSharedPtr()->GetSingleObject())
				{
					m_bSingleObjCount = true;
				}
			}
		}
	}
}


void CPositionMagicServer::OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	CCoreObjectServer* pCoreObj = CCoreObjectServer::GetObjectServer(uObjGlobalId);
	if (!pCoreObj || !pCoreObj->GetHandler()) 
		return;
	size_t uType = (size_t)(pCoreObj->GetHandler()->GetTag());
	CCharacterDictator* pCharacter=NULL;
	if(uType == (uint32)(eCOHT_Character))
	{
		pCharacter =CCharacterDictator::GetCharacterByCoreObj(pCoreObj);
	}
	CFighterDictator* pTo = pCharacter?pCharacter->GetFighter():NULL;
	CancelMagicEff(pTo);
}

void CPositionMagicServer::OnAoiMsgFromSelf(const void* pContext)
{
	RegTick(1000);

	GetFighter()->Attach(this, eCE_Die);
	if (!GetFighter()->CppIsAlive())
	{
		Cancel();
	}
}


void CPositionMagicServer::DoMagicEff(CFighterDictator* pTo)
{
	if (pTo==NULL)
	{
		return;
	}
	const CMagicEffServerSharedPtr& pMagicEffServer = GetCfgSharedPtr()->GetTouchMagicEff();
	CGenericTarget Target(pTo);
	VecEntityID_t vecEntityID;
	FilterLimit filterLimit;
	MakeFilterLimit<CPositionMagicServer>(filterLimit);
	CFilterOpServer::FilterOperateObject(m_pSkillInst, vecEntityID, GetCfgSharedPtr()->GetOperateObject(), filterLimit, GetFighter(), &Target);
	VecEntityID_t::iterator it_obj = vecEntityID.begin();
	for (; it_obj != vecEntityID.end(); ++it_obj)
	{
		CCharacterDictator* pCharacter = CEntityServerManager::GetInst()->GetCharacter(*it_obj);
		if (NULL==pCharacter)
		{
			continue;
		}
		CFighterDictator* pToFighter=pCharacter->GetFighter();
		uint32 uEntityID = pToFighter->GetEntityID();
		CFighterDictator* pFromFighter = GetFighter();
		uint32 uCheckID = GetCheckID();
		if(pMagicEffServer)
			pMagicEffServer->Do(m_pSkillInst, GetFighter(), pToFighter, m_pCancelEffData->AddEffData(uEntityID, GetFighter()));
		if (!pFromFighter->GetMagicMgr()->IsValid(uCheckID))
		{
			return;
		}
		m_setEntityID.insert(uEntityID);
		++m_uCount;
	}
}

void CPositionMagicServer::CancelMagicEff(CFighterDictator* pTo)
{
	if (pTo==NULL)
	{
		return;
	}
	uint32 uEntityID = pTo->GetEntityID();
	if(m_setEntityID.find(uEntityID) != m_setEntityID.end())
	{
		if(GetCfgSharedPtr()->GetTouchMagicEff())
		{
			GetCfgSharedPtr()->GetTouchMagicEff()->Cancel(m_pSkillInst, pTo, m_pCancelEffData->GetEffData(uEntityID));
			m_pCancelEffData->DelEffData(uEntityID);
		}
		m_setEntityID.erase(uEntityID);
	}
}


void CPositionMagicServer::RegTick( uint32 uTime )
{
	GetFighter()->RegistDistortedTick(this, uTime);
}

void CPositionMagicServer::UnRegTick()
{
	GetFighter()->UnRegistDistortedTick(this);
}

void CPositionMagicServer::OnTick()
{
	CFighterDictator* pFromFighter = GetFighter();
	uint32 uCheckID = GetCheckID();
	if (m_uTime < m_uAllTime)
	{
		if (GetCfgSharedPtr()->GetDotMagicEff() != NULL && m_uTime % GetCfgSharedPtr()->GetDotInterval() == 0)
		{
			SetEntityID_t::iterator it_coreObj = m_setEntityID.begin();
			for(; it_coreObj != m_setEntityID.end(); ++it_coreObj)
			{		
		
				CCharacterDictator* pChar = CCharacterDictator::GetCharacterByID(*it_coreObj);
				if (pChar)
				{
					GetCfgSharedPtr()->GetDotMagicEff()->Do(m_pSkillInst, GetFighter(), pChar->GetFighter());
					if (!pFromFighter->GetMagicMgr()->IsValid(uCheckID))
					{
						return;
					}
					if(GetCfgSharedPtr()->GetSingleObject())
					{
						break;
					}
				}	
			}		
		}
		++m_uTime;
	} 
	else
	{
		if (GetCfgSharedPtr()->GetFinalMagicEff() != NULL)
		{
			SetEntityID_t::iterator it_coreObj = m_setEntityID.begin();
			for(; it_coreObj != m_setEntityID.end(); ++it_coreObj)
			{		
				CCharacterDictator* pChar = CCharacterDictator::GetCharacterByID(*it_coreObj);
				if (pChar)
				{
					GetCfgSharedPtr()->GetFinalMagicEff()->Do(m_pSkillInst, GetFighter(), pChar->GetFighter());
					if (!pFromFighter->GetMagicMgr()->IsValid(uCheckID))
					{
						return;
					}
					if(GetCfgSharedPtr()->GetSingleObject())
					{
						break;
					}
				}	
			}
		}

		Cancel();
	}	
}

void CPositionMagicServer::Cancel()
{
	SetEntityID_t::iterator it_coreMediator= m_setEntityID.begin();
	if (GetCfgSharedPtr()->GetTouchMagicEff() != NULL)
	{
		for(; it_coreMediator!= m_setEntityID.end(); ++it_coreMediator)
		{	
				CCharacterDictator* pChar = CCharacterDictator::GetCharacterByID(*it_coreMediator);
				if(pChar)
				{
					GetCfgSharedPtr()->GetTouchMagicEff()->Cancel(m_pSkillInst, pChar->GetFighter(),
						m_pCancelEffData->GetEffData(*it_coreMediator));	
				}
				m_pCancelEffData->DelEffData(*it_coreMediator);
		}
	}	
	if (GetCfgSharedPtr()->GetMutexType())
	{
		GetFighter()->GetMagicMgr()->SetPositionMagic(NULL);
	}
	delete this;
}

void CPositionMagicServer::GetTargetVector(VecEntityID_t& vecTarget)
{
	vecTarget.clear();
	SetEntityID_t::iterator itr = m_setEntityID.begin();
	for(; itr!= m_setEntityID.end(); ++itr)
	{	
		CCharacterDictator* pChar = CCharacterDictator::GetCharacterByID(*itr);
		if(pChar)
		{
			vecTarget.push_back(*itr);
		}
	}
}

const string& CPositionMagicServer::GetPostionMagicName()
{
	return GetCfgSharedPtr()->GetName();
}

CPositionMagicCfgServerSharedPtr& CPositionMagicServer::GetCfgSharedPtr()const
{
	return *m_pCfg;
}
