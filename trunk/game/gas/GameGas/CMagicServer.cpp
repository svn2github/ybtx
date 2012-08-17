#include "stdafx.h"
#include "CMagicServer.h"
#include "CFighterDictator.h"
#include "CAoiDimMgr.h"
#include "CFilterOpServer.h"
#include "CCoreSceneServer.h"
#include "PtrlGas2GacCPPDef.h"
#include "CSkillInstServer.h"
#include "CCoreObjectMediator.h"
#include "CConnServer.h"
#include "BaseHelper.h"
#include "ErrLogHelper.h"
#include "CSyncVariantServerHolder.h"
#include "CMagicMgrServer.h"
#include "CCfgCalc.inl"

CMagicServer::CMagicServer(CSkillInstServer* pInst,CFighterDictator* pFighter, uint32 uTargetEntityID)	
:m_eSizeType(eAIST_Magic)
,m_eEyeSightType(eAIET_Magic)
,m_uCoreObjID(0)
,m_pAoiDimMgr(NULL)
,m_uTargetEntityID(uTargetEntityID)
,m_uCheckID(0)
{
	m_From.SetHolder(this);
	SetFighter(pFighter);
	m_uCheckID=pFighter->GetMagicMgr()->AddMagic(this);
	//pInst->RevertInstType();
	if (pInst->GetType()==eIT_MagicStateInst)
	{
		//m_pSkillInst=new CSkillInstServer(pInst->GetCreatorID(),pInst->GetSkillName(),pInst->GetSkillLevel(),pInst->GetAttackType());
		m_pSkillInst=CSkillInstServer::CreateLocal(pInst, NULL);
	}
	else
	{
		m_pSkillInst = pInst;
		m_pSkillInst->AddRef();
	}
	m_pSkillInst->SetIsMagic(true);
	m_pSkillInst->SetIsArea(true);
	//else
	//{
	//	m_pSkillInst=pInst;
	//}

}

CMagicServer::~CMagicServer(void)
{
	m_pSkillInst->SetIsMagic(false);
	m_pSkillInst->DelRef();
	SafeDelete(m_pAoiDimMgr);
	if(GetFighter())GetFighter()->GetMagicMgr()->RemoveMagic(m_uCheckID);
}

void CMagicServer::OnDestroy()
{
	if(GetCoreObj())GetCoreObj()->SetHandler(NULL);
	m_uCoreObjID=0;
}

void CMagicServer::SetFighter(CFighterDictator* pFighter)
{
	pFighter ? m_From.Attach(pFighter->GetRefFighterByMagic()) : m_From.Detach();
}

CCoreObjectDictator* CMagicServer::GetCoreObj()const
{
	CCoreObjectServer* pObj = CCoreObjectServer::GetObjectServer(m_uCoreObjID);
	return pObj?pObj->CastToObjDictator():NULL;
}
uint32 CMagicServer::GetGlobalID()const
{
	if (GetCoreObj())
		return GetCoreObj()->GetGlobalID();
	return 0;
}

void CMagicServer::SetEyeSight(float fEyeSight)
{
	//GetCoreObj()->SetEyeSight(fEyeSight); 
	Ast(m_pAoiDimMgr);
	CCoreObjectServer* pCoreObj = GetCoreObj();
	Ast(pCoreObj);
	m_pAoiDimMgr->SetEyeSight(fEyeSight, pCoreObj);
}

void CMagicServer::SetBarrierSize(float fSize)
{
	CCoreObjectDictator* pCoreObj = GetCoreObj();
	Ast(pCoreObj);
	EBarrierType eBarrierType;
	pCoreObj->SetBarrier(fSize, pCoreObj->GetBarrierType(eBarrierType) ? eBarrierType : eBT_NoBarrier);
}

void CMagicServer::DeleteObj(CCoreSceneServer* pScene)
{
	CCoreObjectDictator* pObj = GetCoreObj();
	if (pObj&&pScene)
	{
		pObj->SetHandler(NULL);
		pScene->DestroyObject(pObj);
		m_uCoreObjID=0;
	}
}

void CMagicServer::SetTargetEntityID(uint32 uEntityID)
{
	m_uTargetEntityID = uEntityID;
	if (uEntityID!=0)
	{
		CSyncVariantServerHolder* pViewRootVariant = GetCoreObj()->GetViewVariantHolder(eCTI_Static);
		pViewRootVariant->SetNumber("Attribute/TargetEntityID", m_uTargetEntityID);
	}
}

void CMagicServer::OnEvent(const void* pData)
{
	if (GetFighter())
		GetFighter()->Notify(eCE_CoreEvent,const_cast<void*>(pData));
}
