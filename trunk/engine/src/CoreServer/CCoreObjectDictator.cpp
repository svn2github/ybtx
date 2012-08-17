#include "stdafx.h"
#include "CCoreObjectDictator.h"
#include "CSyncCoreObjectDictator.h"
#include "CConnServer.h"
#include "CoreCommon.h"
#include "CConnServer.inl"
#include "CSyncVariantServerHolder.h"
#include "ICoreObjectMediatorHandler.h"
#include "CObjVarDefCfg.h"
#include "CObjVarDef.h"
#include "CLogCoreObject.h"
#include "CAppConfigServer.h"
#include "TSendAdapter.h"
#include "CObjVarDefCfg.h"
#include "CCoreSceneServer.h"
#include "CCoreObjectMediator.h"
#include "ErrLogHelper.h"
#include "ExpHelper.h"
#include "CMetaSceneServer.h"
#include "CoreObjectCommon.h"
#include "CCoreServerObjJob.h"
#include "CCoreServerJob.inl"
#include "CBindObjectServer.h"
#include "TBindObject.inl"
#include "BaseHelper.h"

//template<typename ImpConn_t>
//void ConnSendCoreCmd(const ImpConn_t* pCmd);

CCoreObjectDictator::CCoreObjectDictator(ICoreObjectDictatorHandler* pHandler, CCoreSceneServer* pScene, uint32 uVarDefID)
:CCoreObjectServer(pHandler, pScene)
,m_bEnableFollowerDelay(true)
,m_fBarrierSize(0.0f)
,m_eBarrierType(eBT_NoBarrier)
,m_pBindObj(NULL)
{
	CObjVarDef* pDef=GetVarDefCfg()->Get(uVarDefID);
	CCypherVariantCodeTable* pCodeTable;
	
	m_pBindObj = new CBindObjectServer(this);
	
	for (uint32 i=0;i<(uint32)pDef->GetSyncCodeTableCount();i++)
	{
		pCodeTable=pDef?pDef->GetSyncCodeTable((ECodeTableIndex)i):NULL;
		m_vecSyncVariantHolder.push_back(new CSyncVariantServerHolder(uVarDefID, pCodeTable, this, true, (ECodeTableIndex)i));
	}
	for (uint32 i=0;i<(uint32)pDef->GetViewCodeTableCount();i++)
	{
		pCodeTable=pDef?pDef->GetViewCodeTable((ECodeTableIndex)i):NULL;
		m_vecViewVariantHolder.push_back(new CSyncVariantServerHolder(uVarDefID, pCodeTable, this, false, (ECodeTableIndex)i));
	}
}

CCoreObjectDictator::~CCoreObjectDictator(void)
{
	ClearBarrier();
	delete m_pBindObj;
	for (uint32 i=0; i<m_vecSyncVariantHolder.size(); i++)
	{
		delete m_vecSyncVariantHolder[i];
	}
	m_vecSyncVariantHolder.clear();
	for (uint32 i=0; i<m_vecViewVariantHolder.size(); i++)
	{
		delete m_vecViewVariantHolder[i];
	}
	m_vecViewVariantHolder.clear();
}

ICoreObjectDictatorHandler* CCoreObjectDictator::GetHandler()const
{
	ICoreObjectServerHandler* pHandler = CCoreObjectServer::GetHandler();
	return pHandler ? pHandler->CastToObjDicHandler() : NULL;
}

EServerObjectType CCoreObjectDictator::GetType()const
{
	return eSOT_Dictator;
}

CCoreObjectMediator* CCoreObjectDictator::CastToObjMediator()
{
	return NULL;
}

CCoreObjectDictator* CCoreObjectDictator::CastToObjDictator()
{
	return this;
}

ISend* CCoreObjectDictator::GetIS(uint32 uRange)const
{
	//return GetSynCoreObjDic()->GetIS(uRange);
	static TSendAdapter<CCoreObjectDictator,&CCoreObjectDictator::BroadCastForSyncAoi>	gs_SBC;
	static TSendAdapter<CCoreObjectDictator,&CCoreObjectDictator::BroadCastForViewAoi>	gs_VBC;

	if( uRange == 0 )
	{
		gs_SBC.SetTarget(this);
		return &gs_SBC;
	}
	gs_VBC.SetTarget(this);
	return &gs_VBC;
}

void CCoreObjectDictator::BroadCastForViewAoi(const void* pData,uint32 uSize)const
{
	BroadCastForViewAoiExtraConn(pData, uSize, NULL);
}

void CCoreObjectDictator::BroadCastForSyncAoi(const void* pData,uint32 uSize)const
{
	BroadCastForSyncAoiExtraConn(pData, uSize, NULL);
}

void CCoreObjectDictator::BroadCastForViewAoiExtraConn(const void* pData,uint32 uSize,CConnServer* pExtraConn)const
{
	(new CCoreObjBroadCastForViewAoiExtraConnJob(GetGlobalID(), pData, uSize, pExtraConn ? pExtraConn->GetGlobalID() : 0))->Add();
}

void CCoreObjectDictator::BroadCastForSyncAoiExtraConn(const void* pData,uint32 uSize, CConnServer* pExtraConn)const
{
	(new CCoreObjBroadCastForSyncAoiExtraConnJob(GetGlobalID(), pData, uSize, pExtraConn ? pExtraConn->GetGlobalID() : 0))->Add();
}

void CCoreObjectDictator::OnViewRelationChanged(CCoreObjectServer* pObjServer,bool bSee, uint32 uDimension)
{
	if(!pObjServer)
	{
		ostringstream strm;
		strm << "bSee,uDim " << bSee << "," << uDimension;
		LogErr("err:ViewAOIChg", strm.str());
		return;
	}

	ICoreObjectServerHandler* pHandler = pObjServer->GetHandler();
	if(pHandler)
	{
		if(bSee)
			pHandler->CastToObjDicHandler()->OnCaughtViewSightOf(GetGlobalID(), uDimension);	
		else
			pHandler->CastToObjDicHandler()->OnLostViewSightOf(GetGlobalID(), uDimension);	
	}
}

CSyncVariantServer* CCoreObjectDictator::GetSyncVariant(ECodeTableIndex eIndex)const
{
	if ((uint32)eIndex >= m_vecSyncVariantHolder.size())
		return NULL;
	return m_vecSyncVariantHolder[eIndex]->GetRootVariant();
}

CSyncVariantServer* CCoreObjectDictator::GetViewVariant(ECodeTableIndex eIndex)const
{
	if ((uint32)eIndex >= m_vecViewVariantHolder.size())
		return NULL;
	return m_vecViewVariantHolder[eIndex]->GetRootVariant();
}

CSyncVariantServerHolder* CCoreObjectDictator::GetSyncVariantHolder(ECodeTableIndex eIndex)const
{
	if ((uint32)eIndex >= m_vecSyncVariantHolder.size())
		return NULL;
	return m_vecSyncVariantHolder[eIndex];
}

CSyncVariantServerHolder* CCoreObjectDictator::GetViewVariantHolder(ECodeTableIndex eIndex)const
{
	if ((uint32)eIndex >= m_vecViewVariantHolder.size())
		return NULL;
	return m_vecViewVariantHolder[eIndex];
}


CObjVarDefCfg* CCoreObjectDictator::GetVarDefCfg()
{
	//static CObjVarDefCfg ls_DefCfg;
	//return &ls_DefCfg;
	return CSyncCoreObjectDictator::GetVarDefCfg();
}

void CCoreObjectDictator::EnableFollowerDelay(bool bEnable)
{
	if ( m_bEnableFollowerDelay == bEnable )
		return;

	m_bEnableFollowerDelay = bEnable;

	(new CCoreObjEnableFollowerDelayJob(GetGlobalID(), m_bEnableFollowerDelay))->Add();
}

uint32 CCoreObjectDictator::GetFollowerDelay()
{
	return CAppConfigServer::Inst()->GetFollowerDelay();
}

bool CCoreObjectDictator::IsFollowerDelayEnabled()const
{
	return m_bEnableFollowerDelay;
}

void CCoreObjectDictator::SyncBarrier()const
{
	SyncBarrierExtraConn(NULL);
}

void CCoreObjectDictator::SyncBarrierExtraConn(CConnServer* pExtraConn)const
{
	(new CCoreObjSyncBarrierExtraConnJob(GetGlobalID(), pExtraConn?pExtraConn->GetGlobalID():0))->Add();
}

void CCoreObjectDictator::SetBarrier(float fBarrierSize, EBarrierType eBarrierType)
{
	if(fBarrierSize < 0)
		GenErr("error::BarrierSize is minus");
	if(fBarrierSize > 0 && eBarrierType <= eBT_NoBarrier)
		GenErr("error::BarrierType is illegal");

	//SetQuerySize(fBarrierSize * eGridSpan);

	UpdateSceneBarrier(eBT_NoBarrier);
	m_fBarrierSize = fBarrierSize;
	UpdateSceneBarrier(m_eBarrierType);

	if(eBarrierType != m_eBarrierType)
	{
		if(UpdateSceneBarrier(eBarrierType))
		{
			m_eBarrierType = eBarrierType;
		}
	}
	(new CCoreObjSetBarrierJob(GetGlobalID(), fBarrierSize, eBarrierType))->Add();
}

bool CCoreObjectDictator::HasBarrier()const
{
	return m_fBarrierSize > 0;
}

bool CCoreObjectDictator::GetBarrierType(EBarrierType& eBarrierType)const
{
	if(m_fBarrierSize <= 0)
		return false;

	eBarrierType = m_eBarrierType;
	return true;
}

uint8 CCoreObjectDictator::GetBarrierSizeInGrid()const
{
	//return m_fBarrierSize == 0.0f ? 0 : uint8(m_fBarrierSize) + 1;
	return uint8(m_fBarrierSize * 2);
}

float CCoreObjectDictator::GetBarrierSize()const
{
	return m_fBarrierSize;
}

float CCoreObjectDictator::Distance(const CCoreObjectDictator* pCoreObj)const
{
	Ast(pCoreObj);
	CFPos posSelf, posTarget;
	posSelf = GetPixelPos();
	posTarget = pCoreObj->GetPixelPos();
	float fDist = posSelf.Dist(posTarget);
	return fDist;
	//cout << "CCoreObjectDictator::Distance fOrgDist = " << fDist << " selfBS = " << GetBarrierSize() <<  " TargetBS = " << pCoreObj->GetBarrierSize() << endl;
	//float fOffSet = (GetBarrierSize() + pCoreObj->GetBarrierSize()) * eGridSpanForObj;
	//fDist -= fOffSet;
	//return max(0.0f, fDist);
}

void CCoreObjectDictator::LogMsg(const string& log)
{
	ostringstream strm;
	strm << GetGlobalID() << "\t" << log;
	CLogCoreObject::Inst().LogMsg(strm.str());
	//GetSynCoreObjDic()->LogMsg(log);
}

bool CCoreObjectDictator::UpdateSceneBarrier(EBarrierType eBarrierType)
{
	if(GetBarrierSizeInGrid() <= 0)
		return false;

	BarrierArea barrierArea;
	CMetaSceneServer* pMetaScene = m_pScene->GetMetaScene();
	CalcBarrierArea(barrierArea, GetPixelPos(), GetBarrierSizeInGrid(), pMetaScene->GetWidthInGrid(), pMetaScene->GetHeightInGrid());

	m_pScene->SetDynamicBarrierInfo(GetGlobalID(), barrierArea, eBarrierType);

	return true;
}

void CCoreObjectDictator::ClearBarrier()
{
	SetBarrier(0, m_eBarrierType);
}

void CCoreObjectDictator::SetSyncSize(float fSize)
{
	m_pBindObj->SetSize(fSize);
	SetRealSyncSize(fSize);
}

void CCoreObjectDictator::SetSyncStealth( float fStealth)
{
	m_pBindObj->SetStealth(fStealth);
	SetRealSyncStealth(fStealth);
}

void CCoreObjectDictator::SetSyncGroup(uint32 uGroupID)
{
	m_pBindObj->SetViewGroup(uGroupID);
	SetRealSyncGroup(uGroupID);
}

void CCoreObjectDictator::SetRealSyncSize(float fSize)
{
	m_mapDim2AoiData[0].fSize = fSize;
	(new CCoreObjSetSizeJob(GetGlobalID(), fSize, 0))->Add();
}

void CCoreObjectDictator::SetRealSyncStealth( float fStealth)
{
	m_mapDim2AoiData[0].fStealth = fStealth;
	(new CCoreObjSetStealthJob(GetGlobalID(), fStealth, 0))->Add();
}

void CCoreObjectDictator::SetRealSyncGroup(uint32 uGroupID)
{
	m_mapDim2AoiData[0].uViewGroup = uGroupID;
	(new CCoreObjSetViewGroupJob(GetGlobalID(), uGroupID, 0))->Add();
}

float CCoreObjectDictator::GetSyncSize() const
{
	if(m_pBindObj->IsBindingOrBoundObj())
		return m_pBindObj->GetVirtualSize();
	return GetAoiData(0).fSize;
}

float CCoreObjectDictator::GetSyncStealth() const
{
	if(m_pBindObj->IsBindingOrBoundObj())
		return m_pBindObj->GetVirtualStealth();
	return GetAoiData(0).fStealth;
}

uint32 CCoreObjectDictator::GetSyncGroup() const
{
	if(m_pBindObj->IsBindingOrBoundObj())
		return m_pBindObj->GetVirtualViewGroup();
	return GetAoiData(0).uViewGroup;
}

float CCoreObjectDictator::GetRealSyncSize() const
{
	return GetAoiData(0).fSize;
}

float CCoreObjectDictator::GetRealSyncStealth() const
{
	return GetAoiData(0).fStealth;
}

uint32 CCoreObjectDictator::GetRealSyncGroup() const
{
	return GetAoiData(0).uViewGroup;
}


bool CCoreObjectDictator::BindTo(uint32 uParentObjID)
{
	if(!GetObjectServer(uParentObjID))
		return false;
	if(m_pBindObj->BindTo(uParentObjID))
	{
		//CCoreServerThreadMgr::Inst()->AddCoreObjBindToJob(GetGlobalID(), uParentObjID);
		(new CCoreObjBindToJob(GetGlobalID(), uParentObjID))->Add();
		return true;
	}
	return false;
}

bool CCoreObjectDictator::DisbindFromParent()
{
	if(m_pBindObj->DisbindFromParent())
	{
		//CCoreServerThreadMgr::Inst()->AddCoreObjDisbindFromParentJob(GetGlobalID());
		(new CCoreObjDisbindFromParentJob(GetGlobalID()))->Add();
		return true;
	}
	return false;
}

//考虑组合复杂度，暂时不提供向下单个解绑
bool CCoreObjectDictator::DisbindChild(uint32 uChildObjID)
{
	if(!GetObjectServer(uChildObjID))
		return false;
	if(m_pBindObj->DisbindChild(uChildObjID))
	{
		//CCoreServerThreadMgr::Inst()->AddCoreObjDisbindToChildJob(GetGlobalID(), uChildObjID);
		(new CCoreObjDisbindToChildJob(GetGlobalID(), uChildObjID))->Add();
		return true;
	}
	return false;
}

bool CCoreObjectDictator::DisbindAllChildren()
{
	if(m_pBindObj->DisbindAllChildren())
	{
		//CCoreServerThreadMgr::Inst()->AddCoreObjDisbindToAllChildJob(GetGlobalID());
		(new CCoreObjDisbindToAllChildJob(GetGlobalID()))->Add();
		return true;
	}
	return false;
}

void CCoreObjectDictator::OnBindTo(uint32 uParentID, bool bRet)
{
	//以下从BindTo移过来
	if(bRet)
	{
		CCoreObjectDictator* pParent = GetObject(uParentID);
		if(pParent)
		{
			CFPos fParentPos;
			pParent->GetPixelPos(fParentPos);
			SetPixelPos(fParentPos);
		}
	}
}

void CCoreObjectDictator::OnDisbindParentSingle()
{
	m_pBindObj->OnDisbindParentSingle();
}

void CCoreObjectDictator::OnDisbindChildSingle()
{
	m_pBindObj->OnDisbindChildSingle();
}

void CCoreObjectDictator::OnBeforeBindTo(uint32 uParentID, bool bRet)
{
	m_pBindObj->OnBeforeBindTo(uParentID, bRet);
	//从BindTo移过来的
	if(bRet && IsMoving())
		StopMoving();
}

CCoreObjectDictator* CCoreObjectDictator::GetObject(uint32 uID)
{
	CCoreObjectServer* pDic = CCoreObjectServer::GetObjectServer(uID);
	if(pDic)
	{
		return pDic->CastToObjDictator();
	}
	else
	{
		return NULL;
	}
}

