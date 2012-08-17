#include "stdafx.h"
#include "CEntityServer.h"
#include "CActorServer.h"
#include "IActorServerHandler.h"
#include "CCoreObjectDictator.h"
#include "CCoreObjectMediator.h"
#include "CFighterDictator.h"
#include "IFighterServerHandler.h"
#include "CAoiDimMgr.h"
#include "CMoveServerManager.h"
#include "CCoreSceneServer.h"
#include "PtrlGas2GacCPPDef.h"
#include "PtrlGac2GasCPPDef.h"
#include "PatternRef.inl"
#include "CMoveMagicServer.h"
#include "CoreCommon.h"
#include "CSizeChangeMgr.h"
#include "CCharacterDictator.h"
#include "IDistortedTime.h"

template class TPtBiRef<CEntityServer,IMoveManager>;

CEntityServer::CEntityServer()
:m_pFighter(NULL)
,m_uEntityID(0)
,m_pCoreObj(NULL)
,m_pActor(NULL)
,m_pAoiDimMgr(NULL)
,m_uSetActiveCounter(0)
,m_bMoveManagerHaveOwner(false)
,m_bValid(true)
,m_fBottomSize(0)
,m_pVariantBottomSize(NULL)
,m_pVariantAttribute(NULL)
,m_pVariantShowInClient(NULL)
{
	m_RefsByMoveMgr.SetHolder(this);
}

CEntityServer::~CEntityServer()
{
	IActorServerHandler* pHandler=GetActor()->GetHandler();
	if(pHandler)
	{
		m_pCoreObj->GetHandler();
		m_pCoreObj->SetHandler(NULL);
	}
	delete pHandler;
	delete GetActor();
	delete m_pAoiDimMgr;
}

void CEntityServer::PostAoiMsgToSelf(const void* pContext)
{
	m_pCoreObj->PostAoiMsgToSelf(pContext);
}

uint32 CEntityServer::GetGlobalID()const
{
	return GetCoreObj()->GetGlobalID(); 
}

void CEntityServer::SetEyeSight(float fEyeSight)
{ 
	Ast(m_pAoiDimMgr);
	m_pAoiDimMgr->SetEyeSight(fEyeSight,GetCoreObj());
}

void CEntityServer::SetSecondEyeSight(float fEyeSight)
{
	Ast(m_pAoiDimMgr);
	m_pAoiDimMgr->SetSecondEyeSight(fEyeSight,GetCoreObj());
}

void CEntityServer::SetDoubleEyeSight(float fEyeSight, float fSecondEyeSight)
{
	Ast(m_pAoiDimMgr);
	m_pAoiDimMgr->SetDoubleEyeSight(fEyeSight, fSecondEyeSight,GetCoreObj());
}

void CEntityServer::SetSize(float fSize) 
{ 
	Ast(m_pAoiDimMgr);
	m_fBaseSize = fSize;
	m_pAoiDimMgr->SetSize(float(m_fBaseSize * GetStateZoomRate()),GetCoreObj());

#ifdef COUT_SIZE_CHANGE_INFO
	cout << "[" << GetProcessTime() << "]Entity[" << GetEntityID() << "]设置Size(" << m_fBaseSize << " * " << GetStateZoomRate() << " = " << m_fBaseSize * GetStateZoomRate() << endl;
#endif

}


void CEntityServer::SetKeenness(float fKeenness) 
{
	Ast(m_pAoiDimMgr);
	m_pAoiDimMgr->SetKeenness(fKeenness,GetCoreObj());
}

void CEntityServer::SetStealth(float fStealth) 
{
	Ast(m_pAoiDimMgr);
	m_pAoiDimMgr->SetStealth(fStealth,GetCoreObj());
}

void CEntityServer::SetViewGroup(uint32 uViewGroup) 
{
	Ast(m_pAoiDimMgr);
	m_pAoiDimMgr->SetViewGroup(uViewGroup,GetCoreObj());
}

void CEntityServer::SetEyeSightType(EAoiItemEyeSightType eType)
{
	if(eType == eAIET_Player && !m_pCoreObj->CastToObjMediator())
		GenErr("非Mediator对象不允许设置第0层AOI！");
	Ast(m_pAoiDimMgr);
	m_pAoiDimMgr->SetEyeSightType(eType,GetCoreObj());
}

void CEntityServer::SetSizeType(EAoiItemSizeType eType)
{
	Ast(m_pAoiDimMgr);
	m_pAoiDimMgr->SetSizeType(eType,GetCoreObj());
}

void CEntityServer::SetAoiType(EAoiItemEyeSightType eEyeSightType, EAoiItemSizeType eSizeType)
{
	if(eEyeSightType == eAIET_Player && !m_pCoreObj->CastToObjMediator())
		GenErr("非Mediator对象不允许设置第0层AOI！");
	Ast(m_pAoiDimMgr);
	m_pAoiDimMgr->SetAoiType(eEyeSightType, eSizeType, GetCoreObj());
}
void CEntityServer::SetObjectAoiType(EObjectAoiType eObjectAoiType)
{
	Ast(m_pAoiDimMgr);
	m_pAoiDimMgr->SetObjectAoiType(eObjectAoiType, GetCoreObj());
}
void CEntityServer::UniteObjectAoiType(EObjectAoiType eObjectAoiType)
{
	Ast(m_pAoiDimMgr);
	m_pAoiDimMgr->UniteObjectAoiType(eObjectAoiType, GetCoreObj());
}
void CEntityServer::InitAoiType(EAoiItemEyeSightType eEyeSightType, EAoiItemSizeType eSizeType)
{
	if(eEyeSightType == eAIET_Player && !m_pCoreObj->CastToObjMediator())
		GenErr("非Mediator对象不允许设置第0层AOI！");
	Ast(m_pAoiDimMgr);
	m_pAoiDimMgr->InitAoiType(eEyeSightType, eSizeType);
}

void CEntityServer::SetObjInValidByDestory()
{
	m_bValid = false;
}

bool CEntityServer::GetObjValidState()
{
	return m_bValid;
}

float CEntityServer::GetSize()const
{
	Ast(m_pAoiDimMgr);
	return m_pAoiDimMgr->GetSize();
}

float CEntityServer::GetEyeSight()const
{
	Ast(m_pAoiDimMgr);
	return m_pAoiDimMgr->GetEyeSight();
}

float CEntityServer::GetSecondEyeSight()const
{
	Ast(m_pAoiDimMgr);
	return m_pAoiDimMgr->GetSecondEyeSight();
}

float CEntityServer::GetStealth()const
{
	Ast(m_pAoiDimMgr);
	return m_pAoiDimMgr->GetStealth();
}

float CEntityServer::GetKeenness()const
{
	Ast(m_pAoiDimMgr);
	return m_pAoiDimMgr->GetKeenness();
}

uint32 CEntityServer::GetEntityID() const
{
	return m_uEntityID;
}

CCoreSceneServer* CEntityServer::GetScene() const
{
	Ast(m_pCoreObj);
	return GetCoreObj()->GetScene();
}

void CEntityServer::GetGridPos(CPos& GridPos)const
{
	Ast(m_pCoreObj);
	GetCoreObj()->GetGridPos(GridPos);
}

ESetPosResult CEntityServer::SetGridPos(const CPos& GridPos)
{
	Ast(m_pCoreObj);
	if (GetCoreObj()->CastToObjMediator())
	{
		DisableDirectorMoving(true);
		ESetPosResult eResult= GetCoreObj()->SetGridPos(GridPos);
		DisableDirectorMoving(false);
		return eResult;
	}
	return GetCoreObj()->SetGridPos(GridPos);
}

ESetPosResult CEntityServer::SetPixelPos(const CFPos& PixelPos)
{
	Ast(m_pCoreObj);
	if (GetCoreObj()->CastToObjMediator())
	{
		DisableDirectorMoving(true);
		ESetPosResult eResult= GetCoreObj()->SetPixelPos(PixelPos);
		DisableDirectorMoving(false);
		return eResult;
	}
	return GetCoreObj()->SetPixelPos(PixelPos);
}

const CFPos& CEntityServer::GetPixelPos()const
{
	Ast(m_pCoreObj);
	return GetCoreObj()->GetPixelPos();
}

void CEntityServer::GetPixelPos(CFPos& pos)const
{
	Ast(m_pCoreObj);
	pos = GetCoreObj()->GetPixelPos();
}

ISend* CEntityServer::GetIS(uint32 uRange)const
{
	Ast(m_pCoreObj);
	return GetCoreObj()->GetIS(uRange);
}

bool CEntityServer::IsMoving()const
{
	Ast(m_pCoreObj);
	return GetCoreObj()->IsMoving();
}

EMoveToResult CEntityServer::MoveToInLinePath(const CFPos& PixelPosDes,float fPixelSpeed,EFindPathType eFindPathType,EBarrierType eBarrierType)
{
	Ast(m_pCoreObj);
	if (GetCoreObj()->CastToObjMediator())
	{
		if (!IsExistMoveMgr())
		{
			CMoveServerManager* pMoveMgr=new CMoveServerManager(this);
			EMoveToResult eResult=pMoveMgr->MoveTo(PixelPosDes,fPixelSpeed,eFindPathType,eBarrierType);
			return eResult;
		}
		return eMTR_NotAllowed;
	}
	return GetCoreObj()->MoveTo(PixelPosDes,fPixelSpeed,eFindPathType,eBarrierType,0.0f);
}

EMoveToResult CEntityServer::MoveToInAStarPath(const CFPos& PixelPosDes, float fPixelSpeed, EFindPathType eFindPathType, EBarrierType eBarrierType, float fReachDist, int32 iMaxStep)
{
	Ast(m_pCoreObj)
	return GetCoreObj()->MoveToLimit(PixelPosDes, fPixelSpeed, eFindPathType, eBarrierType, fReachDist, iMaxStep);
}

EMoveToResult CEntityServer::MoveToInCustomPath(const vector<CFPos>& vecPos, float fPixelSpeed, float fReachDist)
{
	Ast(m_pCoreObj)
	Ast(vecPos.size() >= 2);
	CPixelPath* pPath = m_pCoreObj->GetScene()->CreatePath(vecPos);
	return m_pCoreObj->MoveToInPath(pPath, fPixelSpeed, eFPT_Custom, eBT_HighBarrier, fReachDist);
}

ESetSpeedResult CEntityServer::SetCoreObjSpeed( float fSpeed )
{
	Ast(m_pCoreObj);
	if (IsExistMoveMgr())
	{
		return eSSR_NotAllowed;
	}
	return GetCoreObj()->SetSpeed(fSpeed);
}

EStopMovingResult CEntityServer::StopMoving()
{
	Ast(m_pCoreObj);
	return GetCoreObj()->StopMoving();
}

CSyncVariantServer* CEntityServer::GetSyncVariant(ECodeTableIndex eIndex) const
{
	Ast(m_pCoreObj);
	return GetCoreObj()->GetSyncVariant(eIndex);
}

CSyncVariantServer* CEntityServer::GetViewVariant(ECodeTableIndex eIndex) const
{
	Ast(m_pCoreObj);
	return GetCoreObj()->GetViewVariant(eIndex);
}

CSyncVariantServerHolder* CEntityServer::GetSyncVariantHolder(ECodeTableIndex eIndex) const
{
	Ast(m_pCoreObj);
	return GetCoreObj()->GetSyncVariantHolder(eIndex);
}

CSyncVariantServerHolder* CEntityServer::GetViewVariantHolder(ECodeTableIndex eIndex) const
{
	Ast(m_pCoreObj);
	return GetCoreObj()->GetViewVariantHolder(eIndex);
}

bool CEntityServer::IsInViewAoi(uint32 uGlobalID) 
{
	Ast(m_pCoreObj);
	vector<CCoreObjectServer* > vecObject;
	CFPos pixelPos = GetPixelPos();
	if(GetScene())GetScene()->QueryObject(vecObject, pixelPos, GetEyeSight());
	for(vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{
		if ((*it)->GetGlobalID() == uGlobalID)
			return true;
	}
	return false;
}
uint32 CEntityServer::GetAoiMediatorNum() 
{
	Ast(m_pCoreObj);
	uint32 uNum = 0;
	vector<CCoreObjectServer* > vecObject;
	CFPos pixelPos = GetPixelPos();
	if(GetScene())GetScene()->QueryObject(vecObject, pixelPos, GetEyeSight());
	for(vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{
		if ((*it)->GetType() == eSOT_Mediator)
			uNum++;
	}
	return uNum;
}

void CEntityServer::DisableDirectorMoving(bool bActive)
{
	CCoreObjectMediator* pMedObj = GetCoreObj()->CastToObjMediator();
	if(pMedObj)
	{
		if(bActive)
		{
			++m_uSetActiveCounter;
			if(m_uSetActiveCounter == 1)
			{
				pMedObj->SetDirectorMaxSpeed(0);
			}
		}
		else
		{
			if(m_uSetActiveCounter == 0)
			{
				return;
			}
			--m_uSetActiveCounter;
			if(m_uSetActiveCounter == 0)
			{
				CFighterDictator* pFighter = GetFighter();
				float fDirectorMaxSpeed = CCoreObjectMediator::GetDefaultDirectorMaxSpeed();

				if(pFighter && pFighter->m_RunSpeed.Get(pFighter) > 0)
				{
					fDirectorMaxSpeed = pFighter->m_RunSpeed.Get(pFighter);
				}
				else
				{
					LogErr("主角最大速度被设置为0了");
					CConnServer* pConn = pMedObj->GetConnection();
					if(pConn)
						LogErr("主角最大速度被设置为0了");
				}

				pMedObj->SetDirectorMaxSpeed(fDirectorMaxSpeed);
			}
		}
	}
}

void CEntityServer::SetDirectorMaxSpeed(float fDirectorMaxSpeed)
{
	CCoreObjectMediator* pMedObj = GetCoreObj()->CastToObjMediator();
	if(pMedObj && pMedObj->GetDirectorMaxSpeed() > 0)
	{
		pMedObj->SetDirectorMaxSpeed(fDirectorMaxSpeed);
	}
}

bool CEntityServer::DirectorMovingIsDisabled()const
{
	Ast(m_pCoreObj);
	if (!GetCoreObj()->CastToObjMediator())
	{
		return true;
	}
	else
	{
		return GetCoreObj()->CastToObjMediator()->GetDirectorMaxSpeed() == 0;
	}
}

CConnServer* CEntityServer::GetConnection() const
{
	Ast(m_pCoreObj);
	if (!GetCoreObj()->CastToObjMediator())
	{
		return NULL;
	}
	else
	{
		return GetCoreObj()->CastToObjMediator()->GetConnection();
	}
}

void CEntityServer::SetConnection(CConnServer* pConn)
{
	Ast(m_pCoreObj);
	if (GetCoreObj()->CastToObjMediator())
	{
		GetCoreObj()->CastToObjMediator()->SetConnection(pConn);
	}
}

void CEntityServer::SetDynamicBarrier(EBarrierType eBarrierType)
{
	Ast(m_pCoreObj);
	m_pCoreObj->SetBarrier(eBarrierType <= eBT_NoBarrier ? 0 : GetBottomSize(), eBarrierType);
}

CMoveServerManager* CEntityServer::GetMoveManager()
{
	if (!m_bMoveManagerHaveOwner)
	{
		m_bMoveManagerHaveOwner=true;
		
		CMoveServerManager* MoveServerManager=class_cast<CMoveServerManager*>(m_RefsByMoveMgr.Get());
		return MoveServerManager;
		
	}
	return NULL;
}

CBaseMoveMagicServer* CEntityServer::GetMoveMagic()
{
	return dynamic_cast<CBaseMoveMagicServer*>(m_RefsByMoveMgr.Get());;
}

bool CEntityServer::IsValid(const CMoveServerManager* pMoveMgrKey)
{
	if(pMoveMgrKey&&pMoveMgrKey==m_RefsByMoveMgr.Get()) return true;
	return false;
}

bool CEntityServer::IsExistMoveMgr()
{
	if(NULL!=m_RefsByMoveMgr.Get()) return true;
	return false;
}

void CEntityServer::PostCoreEvent(const void* pCoreEvent)
{
	Ast(m_pCoreObj);
	GetCoreObj()->PostEvent(pCoreEvent);
}

void CEntityServer::LogEntityMsg(const string& log)
{
	if(m_pCoreObj)
	{
		GetCoreObj()->LogMsg(log);
	}
}

float CEntityServer::GetEntityDist(const CEntityServer* pEntity)const
{
	Ast(m_pCoreObj);
	Ast(pEntity);
	Ast(pEntity->GetCoreObj());

	float fDistAdjustment = (GetBottomSize() + pEntity->GetBottomSize() ) * eGridSpan;
	return max(0.0f, m_pCoreObj->Distance(pEntity->GetCoreObj()) - fDistAdjustment);
}

float CEntityServer::GetEntityDistInGrid(const CEntityServer* pEntity)const
{
	return GetEntityDist(pEntity) / eGridSpanForObj;
}

void CEntityServer::SetBarrierSize(float fSize)
{
	Ast(m_pCoreObj);
	EBarrierType eBarrierType;
	m_pCoreObj->SetBarrier(float(m_fBaseBottomSize * GetStateZoomRate()), m_pCoreObj->GetBarrierType(eBarrierType) ? eBarrierType : eBT_NoBarrier);

#ifdef COUT_SIZE_CHANGE_INFO
	cout << "[" << GetProcessTime() << "]Entity[" << GetEntityID() << "]设置BarrierSize(" << m_fBaseBottomSize << " * " << GetStateZoomRate() << " = " << m_fBaseBottomSize * GetStateZoomRate() << "\n)";
#endif

}

void CEntityServer::StateSetSize()
{
	Ast(m_pAoiDimMgr);
	m_pAoiDimMgr->SetSize(float(m_fBaseSize * GetStateZoomRate()), GetCoreObj());

#ifdef COUT_SIZE_CHANGE_INFO
	cout << "[" << GetProcessTime() << "]Entity[" << GetEntityID() << "]的状态设置Size(" << m_fBaseSize << " * " << GetStateZoomRate() << " = " << m_fBaseSize * GetStateZoomRate() << "\n)";
#endif

}

void CEntityServer::StateSetBottomSize()
{
	Ast(m_pCoreObj);
	IntSetBottomSize(float(m_fBaseBottomSize * GetStateZoomRate()));
	EBarrierType eBarrierType = m_pCoreObj->GetBarrierType(eBarrierType) ? eBarrierType : eBT_NoBarrier;
	float fBarrierSize = eBarrierType <= eBT_NoBarrier ? 0 : GetBottomSize();
	m_pCoreObj->SetBarrier(fBarrierSize, eBarrierType);

#ifdef COUT_SIZE_CHANGE_INFO
	cout << "[" << GetProcessTime() << "]Entity[" << GetEntityID() << "]的状态设置BarrierSize(" << m_fBaseBottomSize << " * " << GetStateZoomRate() << " = " << m_fBaseBottomSize * GetStateZoomRate() << "\n)";
#endif

}

void CEntityServer::EnableFollowerDelay(bool bEnable)
{
	m_pCoreObj->EnableFollowerDelay(bEnable);
}

int32 CEntityServer::GetFollowerDelay()const
{
	if(GetCoreObj()->IsFollowerDelayEnabled())
		return GetCoreObj()->GetFollowerDelay();
	return 0;
}


void CEntityServer::InitPlaySizeForNpcSleepDim()
{
	Ast (m_pAoiDimMgr);
	m_pAoiDimMgr->InitPlaySizeForNpcSleepDim(GetCoreObj());
}

void CEntityServer::InitNpcEyeSizeForNpcSleepDim(bool bSleep)
{
	Ast (m_pAoiDimMgr);
	m_pAoiDimMgr->InitNpcEyeSizeForNpcSleepDim(bSleep,GetCoreObj());
}

void CEntityServer::Watch(IWatchHandler* pHandler, CEntityServer* pTarget, float fPixelDistThreshold)
{
	Ast(GetCoreObj());
	//cout << "watch " << fPixelDistThreshold << " " << GetBottomSize() << " " << pTarget->GetBottomSize() << endl;
	GetCoreObj()->Watch(pHandler, pTarget->GetCoreObj(), fPixelDistThreshold + (GetBottomSize() + pTarget->GetBottomSize()) * eGridSpan);
}

void CEntityServer::StopWatching(IWatchHandler* pHandler)
{
	Ast(GetCoreObj());
	GetCoreObj()->StopWatching(pHandler);
}

void CEntityServer::Trace(CEntityServer* pTarget, float fSpeed
	, EFindPathType eFindPathType, EBarrierType eBarrierType, float fReachDist)
{
	Ast(GetCoreObj());
	GetCoreObj()->Trace(pTarget->GetCoreObj(), fSpeed, eFindPathType, eBarrierType, fReachDist);
}

void CEntityServer::StopTracing()
{
	Ast(GetCoreObj());
	GetCoreObj()->StopTracing();
}

uint32 CEntityServer::GetLastMoveID()
{
	Ast(GetCoreObj());
	return GetCoreObj()->GetLastMoveID();
}

void CEntityServer::RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc)
{
	Ast(GetCoreObj());
	return GetCoreObj()->GetDistortedTime()->RegistDistortedTick(pTick, uCyc);
}

void CEntityServer::UnRegistDistortedTick(CDistortedTick* pTick)
{
	Ast(GetCoreObj());
	return GetCoreObj()->GetDistortedTime()->UnregistDistortedTick(pTick);
}

uint64 CEntityServer::GetDistortedProcessTime()
{
	Ast(GetCoreObj());
	return GetCoreObj()->GetDistortedTime()->GetDistortedProcessTime();
}

uint64 CEntityServer::GetDistortedFrameTime()
{
	Ast(GetCoreObj());
	return GetCoreObj()->GetDistortedTime()->GetDistortedFrameTime();
}

bool CEntityServer::IsTracing()
{
	Ast (GetCoreObj());
	return GetCoreObj()->IsTracing();
}

uint32 CEntityServer::GetCoreObjID()
{
	if(GetCoreObj())
		return GetCoreObj()->GetGlobalID();
	else
		return 0;
}

bool CEntityServer::BindTo(CEntityServer* pEntity)
{
	if(GetCoreObj() && pEntity && pEntity->GetCoreObj())
	{
		return GetCoreObj()->BindTo(pEntity->GetCoreObjID());
	}
	return false;
}

void CEntityServer::DisbindAllChildren()
{
	GetCoreObj()->DisbindAllChildren();
}

void CEntityServer::IntSetBottomSize(float fBottomSize)
{
	m_fBottomSize = fBottomSize;
	CCoreObjectDictator* pCoreObj = GetCoreObj();
	Ast(pCoreObj);
	pCoreObj->SetQuerySize(fBottomSize);
	if(!m_pVariantBottomSize)
	{
		CSyncVariantServer* pVariant = pCoreObj->GetSyncVariant(eCTI_Agile);
		pVariant = pVariant->GetMember("Attribute1");
		if(!pVariant)
			pVariant = pCoreObj->GetSyncVariant(eCTI_Agile)->SetMember("Attribute1");
		m_pVariantBottomSize = pVariant->SetMember("BottomSize");
	}
	m_pVariantBottomSize->SetNumber<float>(fBottomSize);
}

void CEntityServer::SetBottomSize(float fBottomSize)
{
	if(fBottomSize < 0)
		GenErr("error::BottomSize is minus");

	m_fBaseBottomSize = fBottomSize;
	IntSetBottomSize(fBottomSize);

	//还需要设置在魔法,光环层的size
	//
}

float CEntityServer::GetBottomSize()const
{
	return m_fBottomSize;
}

CSyncVariantServer* CEntityServer::GetViewAttributeVariant()const
{
	Ast(m_pCoreObj);

	if(!m_pVariantAttribute)
	{
		CSyncVariantServer* pRoot = m_pCoreObj->GetViewVariant(eCTI_Static);
		m_pVariantAttribute = pRoot->GetMember("Attribute");
	}
		
	return m_pVariantAttribute;
}

CSyncVariantServer* CEntityServer::GetViewShowInClientVariant()const
{
	Ast(m_pCoreObj);
	if(!m_pVariantShowInClient)
	{
		CSyncVariantServer* pAttribtue = GetViewAttributeVariant();
		m_pVariantShowInClient = pAttribtue->GetMember("ShowInClient");
	}

	return m_pVariantShowInClient;
}

