#include "stdafx.h"
#include "CTransferableMagicServer.h"
#include "CTransferableMagicCfgServer.h"
#include "CMagicServer.inl"
#include "CCoreSceneServer.h"
#include "CMagicEffServer.h"
#include "CFilterOpServer.h"
#include "CFighterMediator.h"
#include "CEntityServerManager.h"
#include "CGenericTarget.h"
#include "CTempVarServer.h"
#include "CSkillInstServer.h"
#include "CCoreObjectDictator.h"
#include "PtrlGas2GacCPPDef.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"
#include "CMagicMgrServer.h"

//初始化可传递魔法
CTransferableMagicServer::CTransferableMagicServer(CSkillInstServer* pInst, const CTransferableMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, CFighterDictator* pTo)
: CMagicServer( pInst, pFrom, pTo->GetEntityID())
, m_pCfg( new CTransferableMagicCfgServerSharedPtr(pCfg) )
, m_uCount( 0 )
{	
	m_uAmountLimit = GetCfgSharedPtr()->GetAmountLimit()->GetIntValue(GetFighter(), pInst->GetSkillLevel()-1);
	pTo->Attach(this, eCE_Offline);
	pTo->Attach(this, eCE_Die);
	pTo->Attach(this, eCE_ChangeMapEnd);
	GetFighter()->Attach(this, eCE_Offline);
	GetFighter()->Attach(this, eCE_ChangeMapEnd);
	GetFighter()->Attach(this, eCE_FinishDuel);
	
	CFPos posFrom, posTo; 
	CFPos posFromPixel;
	GetFighter()->GetPixelPos(posFromPixel);
	CFighterMediator* pMedFighter = GetFighter()->CastToFighterMediator();
	if(pMedFighter)
	{
		posFrom = pMedFighter->GetFighterPos();
	}
	else
	{
		posFrom = GetFighter()->GetPixelPos();
	}
	posTo = pTo->GetPixelPos();
	CCoreObjectDictator* pObj = CreateCoreObj<CTransferableMagicServer>(posFromPixel);
	pObj->EnableFollowerDelay(false);

	SetTargetEntityID(pTo->GetEntityID());
	m_setEntityID.insert(pTo->GetEntityID());

	//cout << "trans " << nMaxStep << endl;
	pObj->Trace(pTo->GetEntity()->GetCoreObj(), GetCfgSharedPtr()->GetSpeed(), eFPT_HypoLine, eBT_MidBarrier, 0);
}

void CTransferableMagicServer::OnTraceEnded(ETraceEndedType eTEType)
{
	if(eTET_Traced != eTEType)
		OnMoveEnded(eMET_Reached,0);
}

CTransferableMagicServer::~CTransferableMagicServer()
{
	CCoreObjectDictator* pObj = GetCoreObj();
	if(pObj)
		pObj->StopTracing();

	DeleteObj(GetFighter()->GetScene());
	m_setEntityID.clear();
	SafeDelete(m_pCfg);
}

void CTransferableMagicServer::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	CEntityServer* pEntity = CEntityServerManager::GetEntityByID(GetTargetEntityID());
	CFighterDictator* pTo = pEntity ? pEntity->GetFighter() : NULL;
	if(pSubject == pTo)
	{
		switch(uEvent)
		{
		case eCE_Die:
		case eCE_Offline:
		case eCE_ChangeMapEnd:
			pTo->Detach(this, eCE_Offline);
			pTo->Detach(this, eCE_Die);
			pTo->Detach(this, eCE_ChangeMapEnd);
			SetTargetEntityID(0);
			break;
		}	
	}
	else if(pSubject == GetFighter())
	{
		switch(uEvent)
		{
		case eCE_Offline:
		case eCE_FinishDuel:
			delete this;
			break;
		case eCE_ChangeMapEnd:
			{
				CCoreSceneServer* pScene = reinterpret_cast<CCoreSceneServer*>(pArg);
				DeleteObj(pScene);
				delete this;
				break;
			}
		}
	}
	
}

void CTransferableMagicServer::OnMoveEnded(EMoveEndedType eMEType, uint32 uMoveID)
{
	CFighterDictator* pFrom = GetFighter();
	CEntityServer* pEntity = CEntityServerManager::GetEntityByID(GetTargetEntityID());
	CFighterDictator* pTo = pEntity ? pEntity->GetFighter() : NULL;
	if ( !pFrom || !pTo || m_uCount >= m_uAmountLimit )
	{
		delete this;
		return;
	}
	
	uint32 uCheckID = GetCheckID();
	// 执行魔法效果
	if(0 == m_uCount)
	{
		GetCfgSharedPtr()->GetMainMagicEff()->Do(m_pSkillInst, pFrom, pTo);
		if (pFrom->GetMagicMgr()->IsValid(uCheckID))
		{
			int32 iDamage = pFrom->GetTempVarMgr()->GetDamage(); 
			if(iDamage == 0)	
			{
				delete this;
				return;
			}
		}
	}
	else
	{
		GetCfgSharedPtr()->GetSecondMagicEff()->Do(m_pSkillInst, pFrom, pTo);
	}	
	if (!pFrom->GetMagicMgr()->IsValid(uCheckID))
		return;

	// 追踪下个目标
	CGenericTarget Target(pTo);
	CFighterDictator* pNextTarget = NULL;
	CMagicOpTreeServer::VecEntityID vecEntityID;
	FilterLimit filterLimit;
	MakeFilterLimit<CTransferableMagicServer>(filterLimit);
	CFilterOpServer::FilterOperateObject(m_pSkillInst, vecEntityID, GetCfgSharedPtr()->GetOperateObject(), filterLimit, pFrom, &Target);
	CMagicOpTreeServer::VecEntityID::iterator it_obj = vecEntityID.begin();
	for (; it_obj != vecEntityID.end(); ++it_obj)
	{
		if (GetTargetEntityID() == (*it_obj))
			continue;

		SetTargetEntityID(0);
		if (m_setEntityID.count(*it_obj) == 1 && GetCfgSharedPtr()->GetTransType() == eTMT_OneOff)
			continue;
	
		CEntityServer* pTarget = CEntityServerManager::GetEntityByID(*it_obj);
		if (!pTarget)
			continue;

		pNextTarget = pTarget->GetFighter();
		SetTargetEntityID(*it_obj);
		m_setEntityID.insert(*it_obj);
		break;
	}

	if (pNextTarget && m_uCount < m_uAmountLimit && GetTargetEntityID() != 0)
	{
		++m_uCount;
		pNextTarget->Attach(this, eCE_Offline);
		pNextTarget->Attach(this, eCE_Die);
		//cout << "trans " << nMaxStep << endl;
		GetCoreObj()->Trace(pNextTarget->GetEntity()->GetCoreObj(), GetCfgSharedPtr()->GetSpeed(), eFPT_HypoLine, eBT_MidBarrier, 0);
	}
	else
	{
		delete this;
	}
}

CTransferableMagicCfgServerSharedPtr& CTransferableMagicServer::GetCfgSharedPtr()const
{
	return *m_pCfg;
}
