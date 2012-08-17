#include "stdafx.h"
#include "CAureMagicServer.h"
#include "CAureMagicCfgServer.h"
#include "CCoreObjectDictator.h"
#include "CCoreSceneServer.h"
#include "CSkillInstServer.h"
#include "CFighterMediator.h"
#include "CMagicEffServer.h"
#include "CCharacterDictator.h"
#include "CMagicServer.inl"
#include "CBulletMagicServer.h"
#include "CShockWaveMagicServer.h"
#include "CTransferableMagicServer.h"
#include "CMagicEffDataMgrServer.h"
#include "CTeamServerManager.h"
#include "CAoiDimMgr.h"
#include "CFilterOpServer.h"
#include "CEntityServerManager.h"
#include "CGenericTarget.h"
#include "CMagicMgrServer.h"
#include "BaseHelper.h"
#include "CAppConfigServer.h"
#include "TSqrAllocator.inl"

bool CAureMagicServer::CancelTheExistedStance(const CAureMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom)
{
	MapAureMagic_t mapAureMagic = pFrom->GetMagicMgr()->GetAureMagicMap();
	MapAureMagic_t::iterator it = mapAureMagic.find(pCfg->GetStanceType());

	if ((it != mapAureMagic.end()) && (it->second->GetCfgSharedPtr()->GetName() == pCfg->GetName()))
	{
		it->second->Cancel();
		return true;
	}

	CMagicMgrServer::SetAureMagic_t& setInvisibleAureMagic = pFrom->GetMagicMgr()->GetAureMagicSet();
	CMagicMgrServer::SetAureMagic_t::iterator it_invisible_aure = setInvisibleAureMagic.begin();
	for(; it_invisible_aure != setInvisibleAureMagic.end(); ++ it_invisible_aure)
	{
		if((*it_invisible_aure)->GetCfgSharedPtr()->GetName() == pCfg->GetName())
		{
			(*it_invisible_aure)->Cancel();
			return true;
		}
	}
	return false;
}

bool CAureMagicServer::HasAureMagic(const CAureMagicCfgServerSharedPtr& pCfg, const CFighterDictator* pFrom)
{
	MapAureMagic_t& mapAureMagic = pFrom->GetMagicMgr()->GetAureMagicMap();
	MapAureMagic_t::iterator it = mapAureMagic.find(pCfg->GetStanceType());

	if ((it != mapAureMagic.end()) && (it->second->GetCfgSharedPtr()->GetName()==pCfg->GetName()))
	{
		return true;
	}

	CMagicMgrServer::SetAureMagic_t& setInvisibleAureMagic = pFrom->GetMagicMgr()->GetAureMagicSet();
	CMagicMgrServer::SetAureMagic_t::iterator it_invisible_aure = setInvisibleAureMagic.begin();
	for(; it_invisible_aure != setInvisibleAureMagic.end(); ++ it_invisible_aure)
	{
		if((*it_invisible_aure)->GetCfgSharedPtr()->GetName() == pCfg->GetName())
			return true;
	}
	return false;
}

void CAureMagicServer::CancelAllAureMagic(CFighterDictator* pFrom)
{
	MapAureMagic_t& mapAureMagic = pFrom->GetMagicMgr()->GetAureMagicMap();
	while (!mapAureMagic.empty())
	{
		MapAureMagic_t::iterator it = mapAureMagic.begin();
		it->second->Cancel();
	}
}

void CAureMagicServer::CancelAllInvisibleAureMagic(CFighterDictator* pFrom)
{
	CMagicMgrServer::SetAureMagic_t& setAureMagic = pFrom->GetMagicMgr()->GetAureMagicSet();
	while (!setAureMagic.empty())
	{
		CMagicMgrServer::SetAureMagic_t::iterator it = setAureMagic.begin();
		(*it)->Cancel();
	}
}

void CAureMagicServer::CancelAllAureMagicByType(CFighterDictator* pFrom, EStanceType eStanceType)
{
	MapAureMagic_t& mapAureMagic = pFrom->GetMagicMgr()->GetAureMagicMap();
	MapAureMagic_t::iterator it = mapAureMagic.find(eStanceType);
	if (it != mapAureMagic.end())
	{
		it->second->Cancel();
	}
}

CAureMagicServer::CAureMagicServer(CSkillInstServer* pInst, const CAureMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom,CFighterDictator* pTo)
:CMagicServer(pInst, pFrom,pTo->GetEntityID())
,m_pCfg(new CAureMagicCfgServerSharedPtr(pCfg))
	,m_bCancelling(false)
	,m_bInTeam(true)
{
	MapAureMagic_t& mapAureMagic = pFrom->GetMagicMgr()->GetAureMagicMap();
	MapAureMagic_t::iterator it =mapAureMagic.find(GetCfgSharedPtr()->GetStanceType());
	if (it!=mapAureMagic.end()&&GetCfgSharedPtr()->GetStanceType()!=eSS_InvisibleAure)
	{
		it->second->Cancel();
	}
	m_pCancelEffData = new CEffDataByGlobalIDMgr;
	m_pSkillInst->AddRef();
	GetFighter()->Attach(this, eCE_Offline);
	//GetFighter()->Attach(this, eCE_FinishDuel);
	if (GetCfgSharedPtr()->GetStanceType()!=eSS_Stance&&GetCfgSharedPtr()->GetStanceType()!=eSS_MutexStance)
	{
		GetFighter()->Attach(this, eCE_BeforeDie);
	}
	GetFighter()->Attach(this, eCE_ChangeMapEnd);

	EAgileType AgileType;
	if (GetCfgSharedPtr()->GetStanceType()==eSS_Form)
	{
		AgileType=pCfg->GetAgileType();
	}
	else
	{
		AgileType=m_eHoldAglie;
	}
	if (GetCfgSharedPtr()->GetMagicEff() && GetCfgSharedPtr()->GetOperateObject() != eOOT_Enemy&&GetFighter()&&GetCfgSharedPtr()->GetStanceType() != eSS_CascadeAure)
	{
		uint32 uEntityID = GetFighter()->GetEntityID();
		uint32 uCheckID = GetCheckID();
		CMagicEffDataMgrServer* pEffDataMgr = m_pCancelEffData->AddEffData(uEntityID, GetFighter());
		if(!pEffDataMgr)
		{
			stringstream str;
			str << "姿态魔法[" << pCfg->GetName() << "]魔法效果[" << GetCfgSharedPtr()->GetMagicEff()->GetName() << "]";
			LogErr(" CAureMagicServer::CAureMagicServer执行Do的pEffDataMgr指针为空", str.str());
		}
		GetCfgSharedPtr()->GetMagicEff()->Do(m_pSkillInst, GetFighter(), GetFighter(), pEffDataMgr);
		if (!pFrom->GetMagicMgr()->IsValid(uCheckID))
		{
			return;
		}
		m_setEntityID.insert(uEntityID);
	}
	if (IsAure())
	{
		CFPos pos;
		float fRadius = float(GetCfgSharedPtr()->GetRadius()->GetDblValue(pFrom,pInst->GetSkillLevel()-1));
		GetFighter()->GetPixelPos(pos);
		if (GetCfgSharedPtr()->GetStanceType() == eSS_CascadeAure)
		{
			m_eEyeSightType = eAIET_RefMagic;
		}
		CCoreObjectDictator* pCoreObj = CreateCoreObj<CAureMagicServer>(pos);
		//if (pFrom->IsMoving())
		//{
		//	uint32 uBaseCycTime = CAppConfigServer::Inst()->GetBaseTickCyc();
		//	CAppServer::Inst()->RegisterTick(this, uBaseCycTime);
		//}
		if(pCoreObj)
		{
			CCharacterDictator* pFromChar = GetFighter()->GetCharacter();
			if(pFromChar)
			{
				pCoreObj->BindTo(pFromChar->GetCoreObjID());
			}	
		}

		SetEyeSight(fRadius);
		//GetFighter()->Attach(this, eCE_MoveBegan);
		//GetFighter()->Attach(this, eCE_MoveEnd);
		if (GetFighter()->GetCharacter()->GetTeamID()!=0)
		{
			m_bInTeam = true;
		}
		CTeamServerManager::CreateTeamManager().AddHandler(this);
	}
	if (GetCfgSharedPtr()->GetStanceType()==eSS_InvisibleAure)
	{
		pFrom->GetMagicMgr()->GetAureMagicSet().insert(this);
	}
	else
	{
		mapAureMagic.insert(pair<EStanceType, CAureMagicServer*>(GetCfgSharedPtr()->GetStanceType(), this));
	}


	//GetFighter()->GetMagicMgr()->OnMagicSkill(GetCfgSharedPtr()->GetId(),eMGT_Aure,true,uAureObjID);
}

CAureMagicServer::~CAureMagicServer()
{
	if ((int32)GetCfgSharedPtr()->GetStanceType()==(int32)eSS_InvisibleAure)
	{
		GetFighter()->GetMagicMgr()->GetAureMagicSet().erase(this);
	}
	else
	{
		MapAureMagic_t& mapAureMagic = GetFighter()->GetMagicMgr()->GetAureMagicMap();
		MapAureMagic_t::iterator it =mapAureMagic.find(GetCfgSharedPtr()->GetStanceType());
		if (it!=mapAureMagic.end()&&it->second==this)
		{
			mapAureMagic.erase(it);
		}
	}

	GetFighter()->UnRegistDistortedTick(this);
	m_pSkillInst->DelRef();
	if (m_pCancelEffData)
	{
		delete m_pCancelEffData;
		m_pCancelEffData = NULL;
	}
	SafeDelete(m_pCfg);
}
void CAureMagicServer::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	if (pSubject == GetFighter())
	{
		switch (uEvent)
		{
		//case eCE_MoveBegan:
		//	{
		//		OnTick();
		//		uint32 uBaseCycTime = CAppConfigServer::Inst()->GetBaseTickCyc();
		//		CAppServer::Inst()->RegisterTick(this, uBaseCycTime);
		//		break;
		//	}
		//case eCE_MoveEnd:
		//	CAppServer::Inst()->UnRegisterTick(this);
		//	break;
		case eCE_BeforeDie:
		case eCE_Offline:
		//case eCE_FinishDuel:
			Cancel();
			break;
		case eCE_ChangeMapEnd:
			if (GetCoreObj()&&IsAure())
			{
				CCharacterDictator* pFromChar = GetFighter()->GetCharacter();
				CCoreSceneServer* pScene = reinterpret_cast<CCoreSceneServer*>(pArg);
				pScene->TransferObject(GetCoreObj(),pFromChar->GetScene(), pFromChar->GetPixelPos());
			break;
			}
		}
	}
}
void CAureMagicServer::Cancel()
{
	//uint32 uCheckID = GetCheckID();
	//CFighterDictator* pFighterFrom = GetFighter();
	if(m_bCancelling)
		return;
	m_bCancelling = true;

	SetEntityID_t::iterator it_setEntityID= m_setEntityID.begin();
	for (; it_setEntityID!= m_setEntityID.end(); ++it_setEntityID)
	{
		CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(*it_setEntityID);
		if (pCharacter)
		{
			CFighterDictator* pTo=pCharacter->GetFighter();

			if (GetCfgSharedPtr()->GetMagicEff())
			{
				GetCfgSharedPtr()->GetMagicEff()->Cancel(m_pSkillInst,pTo, m_pCancelEffData->GetEffData(*it_setEntityID));
				//if(!pFighterFrom->GetMagicMgr()->IsValid(uCheckID))
				//	return;
			}
		}
		m_pCancelEffData->DelEffData(*it_setEntityID);
	}
	if (IsAure())
	{
		CTeamServerManager::CreateTeamManager().RemoveHander(this);
		DeleteObj(GetFighter()->GetScene());
	}
	//GetFighter()->GetMagicMgr()->OnMagicSkill(GetCfgSharedPtr()->GetId(),eMGT_Aure,false,uAureObjID);
	delete this;
}

void CAureMagicServer::OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	CCoreObjectServer* pCoreObj = CCoreObjectServer::GetObjectServer(uObjGlobalId);
	if (!pCoreObj || !pCoreObj->GetHandler())
		return;

	CCharacterDictator* pCharacter=NULL;
	uint32 uOwnerID = GetFighter()->GetEntityID();
	switch ((size_t)pCoreObj->GetHandler()->GetTag())
	{
	case eCOHT_Character:
		{
			if (GetCfgSharedPtr()->GetStanceType() == eSS_CascadeAure)
				return;
			pCharacter =CCharacterDictator::GetCharacterByCoreObj(pCoreObj);
		}
		break;
	case eCOHT_Magic:
		{
			CMagicServer* pMagic = class_cast<CMagicServer*>(pCoreObj->GetHandler());
			if (GetCfgSharedPtr()->GetStanceType() == eSS_Shield)
			{
				if (GetFighter() == pMagic->GetFighter())
					return;

				switch (pMagic->GetMagicType())
				{
				case eMGT_Bullet:
					{
						CBulletMagicServer* pBulletMagic=class_cast<CBulletMagicServer*>(pMagic);
						pBulletMagic->GetSkillInst()->SetCreatorID(uOwnerID);
						CFPos pos;
						pCoreObj->GetPixelPos(pos);
						CFighterMediator* pMedFighter = GetFighter()->CastToFighterMediator();
						if(pMedFighter)
							pMedFighter->SetFighterPos(pos);
						new CBulletMagicServer(pBulletMagic->GetSkillInst(), pBulletMagic->GetCfgSharedPtr(), GetFighter(),pBulletMagic->GetFighter());
						delete pBulletMagic;
						break;
					}
				case eMGT_ShockWave:
					{
						CShockWaveMagicHandlerServer* pShockWaveMagic=class_cast<CShockWaveMagicHandlerServer*>(pMagic);
						pShockWaveMagic->GetSkillInst()->SetCreatorID(uOwnerID);
						CFPos pos,TargetPos;
						pCoreObj->GetPixelPos(pos);
						CFighterMediator* pMedFighter = GetFighter()->CastToFighterMediator();
						if(pMedFighter)
							pMedFighter->SetFighterPos(pos);
						pShockWaveMagic->GetFighter()->GetPixelPos(TargetPos);
						new CShockWaveMagicServer(pShockWaveMagic->GetSkillInst(), pShockWaveMagic->GetCfgSharedPtr(), GetFighter(),TargetPos);
						delete pShockWaveMagic;
						break;
					}
				case eMGT_Transferable:
					{
						CTransferableMagicServer* pTransferableMagic=class_cast<CTransferableMagicServer*>(pMagic);
						CFPos pos;
						pCoreObj->GetPixelPos(pos);
						CFighterMediator* pMedFighter = GetFighter()->CastToFighterMediator();
						if(pMedFighter)
							pMedFighter->SetFighterPos(pos);
						pTransferableMagic->GetSkillInst()->SetCreatorID(uOwnerID);
						new CTransferableMagicServer(pTransferableMagic->GetSkillInst(), pTransferableMagic->GetCfgSharedPtr(), GetFighter(),pTransferableMagic->GetFighter());
						delete pTransferableMagic;
						break;
					}
				default:
					break;
				}
			}
			else if (GetCfgSharedPtr()->GetStanceType() == eSS_CascadeAure && pMagic->GetMagicType() == eMGT_Aure)
			{
				CAureMagicServer* pStanceMagic = class_cast<CAureMagicServer*>(pMagic);
				if (pStanceMagic->GetCfgSharedPtr()->GetName() != GetCfgSharedPtr()->GetName())
					return;
				pCharacter=CCharacterDictator::GetCharacterByID(pStanceMagic->GetSkillInst()->GetCreatorID());
			}
		}
		break;
	}

	CFighterDictator* pTo = pCharacter?pCharacter->GetFighter():NULL;
	DoMagicEff(pTo);
}

void CAureMagicServer::OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	CCoreObjectServer* pCoreObj = CCoreObjectServer::GetObjectServer(uObjGlobalId);
	if (!pCoreObj || !pCoreObj->GetHandler())
		return;
	CCharacterDictator* pCharacter =NULL;
	size_t uType = (size_t)(pCoreObj->GetHandler()->GetTag());
	if (uType == (uint32)(eCOHT_Character)&&GetCfgSharedPtr())
	{
		pCharacter =CCharacterDictator::GetCharacterByCoreObj(pCoreObj);
	}
	else if (uType == eCOHT_Magic && GetCfgSharedPtr()->GetStanceType() == eSS_CascadeAure)
	{
		CMagicServer* pMagic=class_cast<CMagicServer*>(pCoreObj->GetHandler());
		if (pMagic->GetMagicType() != eMGT_Aure)
		{
			return;
		}
		CAureMagicServer* pStanceMagic = class_cast<CAureMagicServer*>(pMagic);
		if (pStanceMagic->GetCfgSharedPtr()->GetName() != GetCfgSharedPtr()->GetName())
			return;
		pCharacter= CCharacterDictator::GetCharacterByID(pStanceMagic->GetSkillInst()->GetCreatorID());
	}
	CFighterDictator* pTo = pCharacter?pCharacter->GetFighter():NULL;
	CancelMagicEff(pTo);
}

void CAureMagicServer::OnTransfered(CCoreSceneServer* pOldScene, const CFPos& OldPos)
{
	CCharacterDictator* pFromChar = GetFighter()->GetCharacter();
	if(pFromChar)
	{
		GetCoreObj()->BindTo(pFromChar->GetCoreObjID());
	}	
}

void CAureMagicServer::DoMagicEff(CFighterDictator* pTo)
{
	if (pTo==NULL||(pTo->GetEntityID() == GetFighter()->GetEntityID()))
	{
		return;
	}
	CGenericTarget Target(pTo);
	CMagicOpTreeServer::VecEntityID vecEntityID;
	FilterLimit filterLimit;
	MakeFilterLimit<CAureMagicServer>(filterLimit);
	CFilterOpServer::FilterOperateObject(m_pSkillInst, vecEntityID, GetCfgSharedPtr()->GetOperateObject(), filterLimit, GetFighter(), &Target);
	CMagicOpTreeServer::VecEntityID::iterator it_obj = vecEntityID.begin();
	for (; it_obj != vecEntityID.end(); ++it_obj)
	{
		CCharacterDictator* pCharacter = CEntityServerManager::GetInst()->GetCharacter(*it_obj);
		if (NULL==pCharacter||(pCharacter->GetEntityID() == GetFighter()->GetEntityID()))
		{
			continue;
		}
		CFighterDictator* pToFighter=pCharacter->GetFighter();
		uint32 uEntityID = pToFighter->GetEntityID();
		
		CFighterDictator* pFromFighter = GetFighter();
		uint32 uCheckID = GetCheckID();
		if (GetCfgSharedPtr()->GetMagicEff())
		{
			CMagicEffDataMgrServer* pEffDataMgr = m_pCancelEffData->AddEffData(uEntityID, GetFighter());
			if(!pEffDataMgr&&GetCfgSharedPtr()->GetStanceType() != eSS_CascadeAure)
			{
				stringstream str;
				str << "姿态魔法[" << GetCfgSharedPtr()->GetName() << "]魔法效果[" << GetCfgSharedPtr()->GetMagicEff()->GetName() << "]";
				LogErr(" CAureMagicServer::DoMagicEff执行Do的pEffDataMgr指针为空", str.str());
			}
			GetCfgSharedPtr()->GetMagicEff()->Do(m_pSkillInst, GetFighter(), pToFighter, pEffDataMgr);
		}
		if (!pFromFighter->GetMagicMgr()->IsValid(uCheckID))
		{
			return;
		}
		m_setEntityID.insert(uEntityID);
	}
}

void CAureMagicServer::CancelMagicEff(CFighterDictator* pTo)
{
	if (pTo==NULL||(pTo->GetEntityID() == GetFighter()->GetEntityID()))
	{
		return;
	}
	uint32 uEntityID = pTo->GetEntityID();
	if (m_setEntityID.find(uEntityID) != m_setEntityID.end())
	{
		if (GetCfgSharedPtr()->GetMagicEff())
		{
			GetCfgSharedPtr()->GetMagicEff()->Cancel(m_pSkillInst, pTo, m_pCancelEffData->GetEffData(uEntityID));
			m_pCancelEffData->DelEffData(uEntityID);
		}
		m_setEntityID.erase(uEntityID);
	}
}

//void CAureMagicServer::OnTick()
//{
//	static uint32 uTick = 0;
//	++uTick;
//	if(uTick % 10 == 0)
//	{
//		CCoreObjectDictator* pObj = GetCoreObj();
//		if(pObj)
//		{
//			//pObj->SetPixelPos(GetFighter()->GetPixelPos());
//			CFPos pos;
//			pObj->GetPixelPos(pos);
//			cout << "光环坐标 = " << pos.x << ", " << pos.y << endl;
//		}
//	}
//}

bool CAureMagicServer::IsAure()const
{
	if (GetCfgSharedPtr()->GetStanceType()==eSS_Aure||GetCfgSharedPtr()->GetStanceType()==eSS_Shield||
	        GetCfgSharedPtr()->GetStanceType() == eSS_CascadeAure || GetCfgSharedPtr()->GetStanceType() == eSS_InvisibleAure)
		return true;
	return false;
}

void CAureMagicServer::OnAddTeamMember(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID)
{
	OnUpdate(uTeamID);
}

void CAureMagicServer::OnRemoveTeamMember(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID)
{
	OnUpdate(uTeamID);
}

void CAureMagicServer::OnCreateTeam(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID)
{
	OnUpdate(uTeamID);
}

void CAureMagicServer::OnDestroyTeam(uint32 uTeamID,ETeamType eTeamType)
{
	OnUpdate(uTeamID);
}

void CAureMagicServer::OnUpdate(uint32 uTeamID)
{
	CCharacterDictator* pCharacter = GetFighter()->GetCharacter();
	if (GetCoreObj()&&(pCharacter&&(uTeamID == pCharacter->GetTeamID()||(m_bInTeam&&0 == pCharacter->GetTeamID()))))
	{
		float fRadius = float(GetCfgSharedPtr()->GetRadius()->GetDblValue(GetFighter(),m_pSkillInst->GetSkillLevel()-1));
		SetEyeSight(0);
		SetEyeSight(fRadius);
		m_bInTeam = pCharacter->GetTeamID()?true:false;
	}
}

CAureMagicCfgServerSharedPtr& CAureMagicServer::GetCfgSharedPtr()const
{
	return *m_pCfg;
}
