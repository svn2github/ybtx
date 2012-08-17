#include "stdafx.h"
#include "BaseHelper.h"
#include "CCoreObjectServer.h"
#include "CSyncCoreObjectServer.h"
#include "TIDPtrMap.inl"
#include "CCoreObjectMediator.h"
#include "CCoreSceneServer.h"
#include "CMetaSceneServer.h"
#include "CCoreTrMirrorBufferMgr.h"
#include "CoreCommon.h"
#include "CAsynCorePos.h"
#include "CAsynCoreQuadObject.h"
#include "CCoreTrMirrorBufferMgr.h"
#include "CPixelPath.h"
#include "IObjPosObserverHandler.h"
#include "TimeHelper.h"
#include "CoreObjectCommon.h"
#include "ICoreObjectServerHandler.h"
#include "PatternRef.inl"
#include "TSqrAllocator.inl"
#include "CCoreServerObjJob.h"
#include "CCoreServerJob.inl"
#include "CAsynDistortedTimeObjServer.h"
#include "CAsynDistortedTimeSceneServer.h"

template class TPtRefer<CCoreObjectServer, ICoreObjectServerHandler>;

namespace sqr
{
	struct MovedDistArg
		:public CCoreObjMallocObject
	{
		MovedDistArg():m_fBeginDist(0),m_pPath(NULL),m_fReachDist(0){}
		~MovedDistArg(){ SafeRelease(m_pPath); }
		uint64			m_tcpMoveBegan;	
		float			m_fBeginDist;
		CPixelPath*		m_pPath;
		float			m_fReachDist;
	};
}

CCoreObjectServer* CCoreObjectServer::GetObjectServer(uint32 uObjGlobalID)
{
	CCoreObjectServer* pCoreObj;
	return GetObjectMap().Get(pCoreObj, uObjGlobalID) ? pCoreObj : NULL;
}

CCoreObjectServer::CCoreObjectServer(ICoreObjectServerHandler* pHandler, CCoreSceneServer* pScene)
:m_pScene(pScene)
,m_pSynCoreObj(NULL)
,m_pPosData(NULL)
,m_pQuadObject(NULL)
,m_bIsMoving(false)
,m_fSpeed(0)
,m_uGPCNFlagRefCount(0)
,m_eLifeState(eAOLS_Running)
,m_bIsTracing(false)
,m_uLastMoveID(0)
,m_uTraceSessionID(0)
,m_bPosSet(false)
,m_pDistortedTimeObj(pScene->CreateDistortedTimeObj())
{
	m_Handler.SetHolder(this);
	SetHandler(pHandler);
	m_uID = GetObjectMap().Add(this);
	GetScene()->AddObject(this);	
	m_pMovedDistArg = new MovedDistArg;

	m_mapDim2AoiData[0].fSize = 0.5f;
	m_mapDim2AoiData[0].fStealth = 0;
	m_mapDim2AoiData[0].uViewGroup = 0;
}

CCoreObjectServer::~CCoreObjectServer(void)
{
	delete m_pMovedDistArg;
	Ver(GetObjectMap().Del(m_uID));

	ReleasePosData();
	ReleaseQuadObject();
	GetScene()->DelObject(this);
	GetScene()->DestroyDistortedTimeObj(m_pDistortedTimeObj);
}

CCoreObjectServer::MapObject& CCoreObjectServer::GetObjectMap()
{
	static MapObject ms_mapObject;
	return ms_mapObject;
}

uint32 CCoreObjectServer::GetGlobalID()const
{
	return m_uID;
}

uint32 CCoreObjectServer::GetLocalID()const
{
	return GetGlobalID();
}

IDistortedTime* CCoreObjectServer::GetDistortedTime()
{
	return m_pDistortedTimeObj;
}

bool CCoreObjectServer::BeginDestroyObject()
{
	if(eAOLS_Destroying == m_eLifeState)
		return false;

	DisbindFromParent();
	DisbindAllChildren();

	(new CCoreObjDestoryObjJob(GetGlobalID()))->Add();
	m_eLifeState = eAOLS_Destroying;
	return true;
}

bool CCoreObjectServer::BeginTransferObject(CCoreSceneServer* pScene,const CFPos& PixelPos)
{
	if( eAOLS_Running != m_eLifeState  )
		return false;

	DisbindFromParent();
	DisbindAllChildren();

	(new CCoreObjTransferObjJob(GetGlobalID(), pScene->GetGlobalID(), PixelPos))->Add();
	m_eLifeState = eAOLS_Transfering;
	if(!CastToObjMediator())
	{
		ResetMovedDistArg(NULL, 0);
		m_bIsMoving = false;
	}
	return true;
}
	
void CCoreObjectServer::TransferDistortedTimeObjTo(CCoreSceneServer* pScene)
{
	m_pDistortedTimeObj->TransferTo(pScene->m_pDistortedTimeScene);
}

void CCoreObjectServer::QueryObject(vector<CCoreObjectServer*>& vecObj, float fRadius, const CFPos& OffsetPos) const
{
	GetScene()->QueryObject(vecObj, const_cast<CCoreObjectServer*>(this), OffsetPos, fRadius);
}


void CCoreObjectServer::SetQuerySize(float fSize)
{
	if(!m_pQuadObject)
	{
		CreateQuadObject(GetPixelPos());
	}

	/*
		因为四叉树本来就有延时，所以我们size还是运到丛线程去设置
	*/
	(new CCoreObjSetCoreSizeJob(GetGlobalID(), fSize))->Add();
}

float CCoreObjectServer::GetQuerySize() const
{
	if(m_pQuadObject)
		return m_pQuadObject->GetSize();
	
	return 0;
}

void CCoreObjectServer::ResetMovedDistArg(const CPixelPath* pPath, float fReachDist)
{
	m_pMovedDistArg->m_tcpMoveBegan = GetDistortedTime()->GetDistortedProcessTime();
	m_pMovedDistArg->m_fBeginDist = 0;
	SafeRelease(m_pMovedDistArg->m_pPath);
	if(pPath)
	{
		m_pMovedDistArg->m_pPath = pPath->Clone();
		m_pMovedDistArg->m_fReachDist = fReachDist * eGridSpanForObj;
	}
}

const CCoreObjectServer::AoiData& CCoreObjectServer::GetAoiData(uint32 uDimension) const
{
	MapDim2AoiData_t::const_iterator iter = m_mapDim2AoiData.find(uDimension);
	if(iter == m_mapDim2AoiData.end())
		GenErr("该维度上面没有相关的AOI信息");

	return iter->second;
}

void CCoreObjectServer::SetViewEyeSight( float fEyeSight, uint32 uDimension )
{
	Ast (uDimension > 0);
	m_mapDim2AoiData[uDimension].fEyeSight = fEyeSight;
	(new CCoreObjSetEyeSightJob(GetGlobalID(), fEyeSight, uDimension))->Add();
}

float CCoreObjectServer::GetViewEyeSight(uint32 uDimension)const
{
	Ast (uDimension > 0);
	return GetAoiData(uDimension).fEyeSight;
}

void CCoreObjectServer::SetViewSize( float fSize, uint32 uDimension )
{
	Ast (uDimension > 0);
	m_mapDim2AoiData[uDimension].fSize = fSize;
	(new CCoreObjSetSizeJob(GetGlobalID(), fSize, uDimension))->Add();
}

float CCoreObjectServer::GetViewSize(uint32 uDimension ) const
{
	Ast (uDimension > 0);
	return GetAoiData(uDimension).fSize;
}

void CCoreObjectServer::SetViewStealth( float fStealth, uint32 uDimension )
{
	Ast (uDimension > 0);
	m_mapDim2AoiData[uDimension].fStealth = fStealth;
	(new CCoreObjSetStealthJob(GetGlobalID(), fStealth, uDimension))->Add();
}

float CCoreObjectServer::GetViewStealth(uint32 uDimension)const
{
	Ast (uDimension > 0);
	return GetAoiData(uDimension).fStealth;
}

void CCoreObjectServer::SetViewKeenness(float Keenness, uint32 uDimension)
{
	Ast (uDimension > 0);
	m_mapDim2AoiData[uDimension].fKeenness = Keenness;
	(new CCoreObjSetKeennessJob(GetGlobalID(), Keenness, uDimension))->Add();
}

float CCoreObjectServer::GetViewKeenness(uint32 uDimension)const
{
	Ast (uDimension > 0);
	return GetAoiData(uDimension).fKeenness;
}

void CCoreObjectServer::SetViewGroup(uint32 uGroupID, uint32 uDimension)
{
	Ast (uDimension > 0);
	m_mapDim2AoiData[uDimension].uViewGroup = uGroupID;
	(new CCoreObjSetViewGroupJob(GetGlobalID(), uGroupID, uDimension))->Add();
}

uint32 CCoreObjectServer::GetViewGroup(uint32 uDimension)const
{
	Ast (uDimension > 0);
	return GetAoiData(uDimension).uViewGroup;
}

void CCoreObjectServer::PostAoiMsgToSelf(const void* pContext)
{
	(new CCoreObjPostAoiMsgToSelfJob(GetGlobalID(), pContext))->Add();
}

void CCoreObjectServer::PostEvent(const void* pContext)
{
	(new CCoreObjPostEventJob(GetGlobalID(), pContext))->Add();
}

ESetPosResult CCoreObjectServer::SetGridPos(const CPos& GridPos)
{
	return SetPixelPos(CFPos(GetPixelOfGrid(GridPos.x),GetPixelOfGrid(GridPos.y)));
}

ESetPosResult CCoreObjectServer::SetPixelPos(const CFPos& PixelPos)
{
	if( IsMoving() )
		return eSPR_IsMoving;

	CMetaSceneServer* pMetaScene=GetScene()->GetMetaScene();
	const int32 nRatio = eGridSpanForObj / eGridSpan;

	if( float( pMetaScene->GetWidthInPixel() * nRatio ) > PixelPos.x
		&&
		float( pMetaScene->GetHeightInPixel() * nRatio ) > PixelPos.y )
	{
		m_bPosSet = true;
		m_Pos = PixelPos;
		(new CCoreObjSetPixelPosJob(GetGlobalID(), PixelPos))->Add();

		return eSPR_Success;
	}

	return eSPR_OutOfBound;
}

ESetSpeedResult CCoreObjectServer::SetSpeed(float fSpeed)
{
	if( fSpeed<0 )
		return eSSR_InvalidSpeed;

	if( !IsMoving() )
		return eSSR_IsNotMoving;

	if( m_fSpeed == fSpeed )
		return eSSR_Success;


	const uint64 uCurTime = GetDistortedTime()->GetDistortedProcessTime();
	
	const float fMovedDist 
		= sqr::CalMoveStep(GetSpeed(), uint32(uCurTime - m_pMovedDistArg->m_tcpMoveBegan))
		+ m_pMovedDistArg->m_fBeginDist;

	m_pMovedDistArg->m_tcpMoveBegan = uCurTime;
	m_pMovedDistArg->m_fBeginDist = fMovedDist;

	(new CCoreObjSetSpeedJob(GetGlobalID(), fSpeed))->Add();
	m_fSpeed = fSpeed;

	return eSSR_Success;
}

bool CCoreObjectServer::IsMoving()const
{
	return m_bIsMoving;
}

uint32 CCoreObjectServer::GetLastMoveID()const
{
	return m_uLastMoveID;
}

EMoveToResult CCoreObjectServer::MoveTo(const CFPos& PixelPosDes,float fSpeed,EFindPathType eFindPathType,EBarrierType eBarrierType,float fReachDist)
{
	if(m_eLifeState == eAOLS_Transfering)
		return eMTR_NotAllowed;

	EMoveToResult eResult = CheckMoving(PixelPosDes, eFindPathType, eBarrierType);
	if(eResult != eMTR_Success)
		return eResult;

	uint64 uCurTime = GetDistortedTime()->GetDistortedProcessTime();
	float fMovedDist = sqr::CalMoveStep(GetSpeed(), uint32(uCurTime - m_pMovedDistArg->m_tcpMoveBegan)) + 
m_pMovedDistArg->m_fBeginDist;
	CFPos BeginPos;
	if(m_pMovedDistArg->m_pPath)
	{
		const float fPathLen = m_pMovedDistArg->m_pPath->GetPathPixelLength();
		const float fCutPathLen = max(0.0f, fPathLen - m_pMovedDistArg->m_fReachDist);
		fMovedDist = min(fMovedDist, fCutPathLen);
		//cout << "path original_len " <<  fPathLen << " cutlen " << fCutPathLen << " fReachDist " << fReachDist << " fMovedDist " << fMovedDist << endl;
		m_pMovedDistArg->m_pPath->GetPixelPosOfPath(BeginPos, fMovedDist);
	}
	else
	{
		BeginPos = GetPixelPos();
	}

	CPixelPath* pPixelPath=GetScene()->
		CreatePath(BeginPos,PixelPosDes,eFindPathType,eBarrierType); 

	if(!pPixelPath)
		return eMTR_FindPathFailed;

	if(m_bIsTracing)
		StopTracing();

	//cout << GetGlobalID() << " bp" << BeginPos.x << "," << BeginPos.y << " dp" << PixelPosDes.x << "," << PixelPosDes.y << " fmd" << fMovedDist << " begindist " << m_pMovedDistArg->m_fBeginDist << " time" << uint32(m_pMovedDistArg->m_tcpMoveBegan.GetElapse()) << " speed" << GetSpeed() << endl;
	ResetMovedDistArg(pPixelPath, fReachDist);
	m_bIsMoving = true;
	IncreaseMoveToSessionID();
	m_fSpeed = fSpeed;

	(new CCoreObjMoveToJob(GetGlobalID(), fSpeed, eFindPathType, eBarrierType, fReachDist, fMovedDist, pPixelPath))->Add();

	return eMTR_Success;
}

EMoveToResult CCoreObjectServer::MoveToLimit(const CFPos& PixelPosDes, float fSpeed, EFindPathType eFindPathType, EBarrierType eBarrierType,float fReachDist, int32 nMaxStep)
{
	if(m_eLifeState == eAOLS_Transfering)
		return eMTR_NotAllowed;

	EMoveToResult eResult = CheckMoving(PixelPosDes, eFindPathType, eBarrierType, nMaxStep);
	if(eResult != eMTR_Success)
		return eResult;

	uint64 uCurTime = GetDistortedTime()->GetDistortedProcessTime();
	float fMovedDist = sqr::CalMoveStep(GetSpeed(), uint32(uCurTime - m_pMovedDistArg->m_tcpMoveBegan)) + 
m_pMovedDistArg->m_fBeginDist;
	CFPos BeginPos;
	if(m_pMovedDistArg->m_pPath)
	{
		const float fPathLen = m_pMovedDistArg->m_pPath->GetPathPixelLength();
		const float fCutPathLen = max(0.0f, fPathLen - m_pMovedDistArg->m_fReachDist);
		fMovedDist = min(fMovedDist, fCutPathLen);
		m_pMovedDistArg->m_pPath->GetPixelPosOfPath(BeginPos, fMovedDist);
	}
	else
	{
		BeginPos = GetPixelPos();
	}

	CPixelPath* pPixelPath=GetScene()->
		CreatePath(BeginPos,PixelPosDes,eFindPathType,eBarrierType, nMaxStep);

	if(!pPixelPath)
		return eMTR_FindPathFailed;

	if(m_bIsTracing)
		StopTracing();

	ResetMovedDistArg(pPixelPath, fReachDist);
	m_bIsMoving = true;
	IncreaseMoveToSessionID();
	m_fSpeed = fSpeed;

	(new CCoreObjMoveToLimitJob(GetGlobalID(), fSpeed, eFindPathType, eBarrierType, fReachDist, fMovedDist, pPixelPath, nMaxStep))->Add();

	return eMTR_Success;
}

EMoveToResult CCoreObjectServer::MoveToInPath(const CPixelPath* pPixelPath, float fSpeed, EFindPathType eFindPathType, EBarrierType eBarrierType, float fReachDist)
{
	if(m_eLifeState == eAOLS_Transfering)
		return eMTR_NotAllowed;

	if(!pPixelPath)
		return eMTR_FindPathFailed;

	if(m_bIsTracing)
		StopTracing();

	StopMoving();	//必须保证对象处于静止状态

	ResetMovedDistArg(pPixelPath, fReachDist);
	m_bIsMoving = true;
	IncreaseMoveToSessionID();
	m_fSpeed = fSpeed;

	(new CCoreObjMoveToInPathJob(GetGlobalID(), pPixelPath->Clone(), fSpeed, eFindPathType, eBarrierType,  fReachDist))->Add();

	return eMTR_Success;
}

EStopMovingResult CCoreObjectServer::StopMoving()
{
	if(!IsMoving())
		return eSMR_IsNotMoving;

	(new CCoreObjStopMovingJob(GetGlobalID()))->Add();

	//cout << GetGlobalID() << " StopMoving" << endl;
	ResetMovedDistArg(NULL, 0);
	m_bIsMoving = false;
	return eSMR_Success;
}

void CCoreObjectServer::Watch(IWatchHandler* pHandler, CCoreObjectServer* pTarget, float fPixelDistThreshold)
{
	pHandler->m_uID = pHandler->m_uID == 0 ? GetNextPosObserverHandlerID() : pHandler->m_uID;
	if(m_mapPosObserverHandler.insert(make_pair<uint32, IObjPosObserverHandler*>(pHandler->m_uID, pHandler)).second)
		(new CCoreObjWatchJob(GetGlobalID(), pHandler->m_uID, pTarget->GetGlobalID(), fPixelDistThreshold))->Add();
	//GetSynCoreObj()->Watch(pHandler, pTarget->GetSynCoreObj(), fPixelDistThreshold);
	//cout<<GetGlobalID()<<"  Wathch "<<pTarget->GetGlobalID()<<endl;
}

void CCoreObjectServer::StopWatching(IWatchHandler* pHandler)
{
	m_mapPosObserverHandler.erase(pHandler->m_uID);
	(new CCoreObjStopWatchingJob(GetGlobalID(), pHandler->m_uID))->Add();
	//cout<<GetGlobalID()<<"  stopwatch"<<endl;
}

void CCoreObjectServer::Trace(CCoreObjectServer* pTarget, float fSpeed, EFindPathType eFindPathType, EBarrierType eBarrierType, float fReachDist)
{
	//EMoveToResult eResult = CheckMoving(pTarget->GetPixelPos(), eFindPathType, eBarrierType);
	//if(eResult != eMTR_Success)
	//{
	//	//send coremsg
	//	return ;
	//}

	//CPixelPath* pPixelPath=GetScene()->
	//	CreatePath(GetPixelPos(),pTarget->GetPixelPos(),eFindPathType,eBarrierType);

	//if(!pPixelPath)
	//{
	//	//send coremsg
	//	return ;
	//}

	//不进行任何的trace检测,逻辑线程调用一次trace,就一定会往引擎线程发送一个trace job,能不能成功trace由引擎线程说了算
	//StopTracing();
	//StopMoving();

	(new CCoreObjTraceJob(GetGlobalID(), pTarget->GetGlobalID(), fSpeed, eFindPathType, eBarrierType, fReachDist))->Add();
	m_bIsMoving = true;
	m_bIsTracing = true;
	IncreaseTraceSessionID();
	SetSpeed(fSpeed);
	return;
}

void CCoreObjectServer::StopTracing()
{
	if(!m_bIsTracing)
		return;

	(new CCoreObjStopTracingJob(GetGlobalID()))->Add();
	//cout << "StopTracing" << endl;
	//ResetMovedDistArg();
	m_bIsMoving = false;
	m_bIsTracing = false;
}

bool CCoreObjectServer::Running()const
{
	return eAOLS_Running==m_eLifeState;
}

bool CCoreObjectServer::Destroying()const
{
	return eAOLS_Destroying==m_eLifeState;
}

bool CCoreObjectServer::Transfering()const
{
	return eAOLS_Transfering==m_eLifeState;
}

void CCoreObjectServer::LogMsg(const string& log)
{
	//GetSynCoreObj()->LogMsg(log);
}

bool CCoreObjectServer::IsTracing()const
{
	return m_bIsTracing;
}

CCoreSceneServer* CCoreObjectServer::GetScene()const
{
	return m_pScene;
}

void CCoreObjectServer::SetHandler(ICoreObjectServerHandler* pHandler)
{
	AdvDelRef(GetHandler());
	m_Handler.Detach();
	if(pHandler)
	{
		m_Handler.Attach(pHandler->m_RefsByCoreObj);
		AdvAddRef(GetHandler());
	}

	//if(GetSynCoreObj() && !(GetSynCoreObj()->GetHandler()))
	//{
	//	if(CastToObjDictator())
	//		GetSynCoreObj()->SetHandler(new ISyncCoreObjectDictatorHandler);
	//	if(CastToObjMediator())
	//		GetSynCoreObj()->SetHandler(new ISyncCoreObjectMediatorHandler);
	//}
}

ICoreObjectServerHandler* CCoreObjectServer::GetHandler()const
{
	return m_Handler.Get();
}

void CCoreObjectServer::GetGridPos(CPos& GridPos)const
{
	PixelPos2GridPos(GridPos,GetPixelPos());
}

const CFPos& CCoreObjectServer::GetPixelPos()const
{
	if(m_bPosSet)
	{
		return m_Pos;	
	}
	else
	{
		return m_pPosData->GetPos();
	}
}

void CCoreObjectServer::GetPixelPos(CFPos& pos)
{
	pos = GetPixelPos();
}

float CCoreObjectServer::GetSpeed()const
{
	if(!IsMoving())
		return 0.0f;
	return m_fSpeed;
}


uint32 CCoreObjectServer::CreatePosData(const CFPos& pos)
{
	m_pPosData = new CAsynCorePos(pos, CCoreTrMirrorBufferMgr::Inst()->GetPosBufferMgr());
	return m_pPosData->GetSlot();
}

void CCoreObjectServer::ReleasePosData()
{
	if(m_pPosData)
	{
		m_pPosData->Release();
		m_pPosData = NULL;
	}
}

void CCoreObjectServer::CreateQuadObject(const CFPos& pos)
{
	m_pQuadObject = new CAsynCoreQuadObject(GetGlobalID(), pos, 
		CCoreTrMirrorBufferMgr::Inst()->GetQuadBufferMgr());
	(new CCoreObjCreateQuadObjJob(GetGlobalID(), m_pQuadObject->GetSlot(), pos))->Add();
}

void CCoreObjectServer::ReleaseQuadObject()
{
	if(m_pQuadObject)
	{
		m_pQuadObject->Release();
		m_pQuadObject = NULL;
	}
}

EMoveToResult CCoreObjectServer::CheckMoving(const CFPos& PixelPosDes,EFindPathType eFindPathType,EBarrierType eBarrierType,int32 nMaxStep)
{
	CFPos PixelPosBegin = GetPixelPos();
	if( PixelPosBegin.x==PixelPosDes.x && PixelPosBegin.y==PixelPosDes.y )
		return eMTR_SuperPosition;

	return eMTR_Success;
}

uint32 CCoreObjectServer::GetNextPosObserverHandlerID()
{
	static uint32 ls_uID;
	return ++ls_uID;
}

void CCoreObjectServer::ReSetCoreScene(CCoreSceneServer* pCoreScene)
{
	m_pScene = pCoreScene;
}

void CCoreObjectServer::IncreaseMoveToSessionID()
{
	++m_uLastMoveID;
}

void CCoreObjectServer::IncreaseTraceSessionID()
{
	++m_uTraceSessionID;
}




