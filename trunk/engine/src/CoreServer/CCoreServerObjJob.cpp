#include "stdafx.h"
#include "CCoreServerObjJob.h"
#include "CCoreServerJob.inl"
#include "CSyncCoreObjectMediator.h"
#include "CSynConnServer.h"
#include "CSyncCoreSceneServer.h"
#include "CCacheVariantServerHolder.h"
#include "CPixelPath.h"
#include "ExpHelper.h"
#include "CCoreServerObjResult.h"
#include "CCoreServerResult.inl"

CCoreServerObjJob::CCoreServerObjJob(uint32 uObjID)
: m_uObjID(uObjID)
{

}

CCoreServerObjJob::~CCoreServerObjJob(void)
{
}

CSyncCoreObjectServer* CCoreServerObjJob::GetCoreObj()const
{
	return CSyncCoreObjectServer::GetObjectServer(m_uObjID);
}

CCoreObjMoveToJob::CCoreObjMoveToJob(uint32 uObjID, float fSpeed, uint32 uFindPathType, uint32 uBarrierType, float fReachDist, float fMovedDist, CPixelPath* pPixelPath)
:CCoreServerObjJob(uObjID)
,m_fSpeed(fSpeed)
,m_uFindPathType(uFindPathType)
,m_uBarrierType(uBarrierType)
,m_fReachDist(fReachDist)
,m_pPixelPath(pPixelPath)
,m_fMovedDist(fMovedDist)
{
}

CCoreObjMoveToJob::~CCoreObjMoveToJob()
{
	CSyncCoreObjectServer* pCoreObj = GetCoreObj();
	if(!pCoreObj)
	{
		m_pPixelPath->Release();
		return;
	}
	pCoreObj->IncreaseMoveToSessionID();
	CFPos posDest;
	m_pPixelPath->GetEndPixelPos(posDest);
	pCoreObj->MoveTo(posDest, m_fSpeed, static_cast<EFindPathType>(m_uFindPathType), static_cast<EBarrierType>(m_uBarrierType), m_fReachDist, m_fMovedDist, m_pPixelPath);
}

CCoreObjMoveToLimitJob::CCoreObjMoveToLimitJob(uint32 uObjID, float fSpeed, uint32 uFindPathType, uint32 uBarrierType, float fReachDist, float fMovedDist, CPixelPath* pPixelPath, int32 iMaxStep)
:CCoreServerObjJob(uObjID)
,m_fSpeed(fSpeed)
,m_uFindPathType(uFindPathType)
,m_uBarrierType(uBarrierType)
,m_fReachDist(fReachDist)
,m_iMaxStep(iMaxStep)
,m_pPixelPath(pPixelPath)
,m_fMovedDist(fMovedDist)
{
}

CCoreObjMoveToLimitJob::~CCoreObjMoveToLimitJob()
{
	CSyncCoreObjectServer* pCoreObj = GetCoreObj();
	if(!pCoreObj)
	{
		m_pPixelPath->Release();
		return;
	}

	pCoreObj->IncreaseMoveToSessionID();
	CFPos posDest;
	m_pPixelPath->GetEndPixelPos(posDest);
	pCoreObj->MoveToLimit(posDest, m_fSpeed, static_cast<EFindPathType>(m_uFindPathType), static_cast<EBarrierType>(m_uBarrierType), m_fReachDist, m_fMovedDist, m_pPixelPath, m_iMaxStep);
}

CCoreObjMoveToInPathJob::CCoreObjMoveToInPathJob(uint32 uObjID, CPixelPath* pPixelPath, float fSpeed, uint32 uFindPathType, uint32 uBarrierType, float fReachDist)
:CCoreServerObjJob(uObjID)
,m_pPixelPath(pPixelPath)
,m_fSpeed(fSpeed)
,m_uFindPathType(uFindPathType)
,m_uBarrierType(uBarrierType)
,m_fReachDist(fReachDist)
{
}

CCoreObjMoveToInPathJob::~CCoreObjMoveToInPathJob()
{
	CSyncCoreObjectServer* pCoreObj = GetCoreObj();
	if(!pCoreObj)
	{
		m_pPixelPath->Release();
		return;
	}
	pCoreObj->IncreaseMoveToSessionID();
	CFPos posDest;
	m_pPixelPath->GetEndPixelPos(posDest);
	pCoreObj->MoveTo(posDest, m_fSpeed, static_cast<EFindPathType>(m_uFindPathType), static_cast<EBarrierType>(m_uBarrierType), m_fReachDist, 0, m_pPixelPath);
}

CCoreObjStopMovingJob::CCoreObjStopMovingJob(uint32 uObjID)
:CCoreServerObjJob(uObjID)
{
}

CCoreObjStopMovingJob::~CCoreObjStopMovingJob()
{
	CSyncCoreObjectServer* pCoreObj = GetCoreObj();
	if(!pCoreObj)
		return;

	pCoreObj->StopMoving();
}

CCoreObjWatchJob::CCoreObjWatchJob(uint32 uObjID, uint32 uHandlerID, uint32 uTargetObjID, float fPixelDistThreshold)
:CCoreServerObjJob(uObjID)
,m_uHandlerID(uHandlerID)
,m_uTargetObjID(uTargetObjID)
,m_fPixelDistThreshold(fPixelDistThreshold)
{
}


CCoreObjWatchJob::~CCoreObjWatchJob()
{
	CSyncCoreObjectServer* pCoreObj = GetCoreObj();
	if(!pCoreObj)
		return;
	pCoreObj->Watch(m_uHandlerID, CSyncCoreObjectServer::GetObjectServer(m_uTargetObjID), m_fPixelDistThreshold);
}

CCoreObjStopWatchingJob::CCoreObjStopWatchingJob(uint32 uObjID, uint32 uHandlerID)
:CCoreServerObjJob(uObjID)
,m_uHandlerID(uHandlerID)
{
}

CCoreObjStopWatchingJob::~CCoreObjStopWatchingJob()
{
	CSyncCoreObjectServer* pCoreObj = GetCoreObj();
	if(!pCoreObj)
		return;

	pCoreObj->StopWatching(m_uHandlerID);
}

CCoreObjTraceJob::CCoreObjTraceJob(uint32 uObjID, uint32 uTargetID, float fSpeed, uint32 uFindPathType, uint32 uBarrierType, float fReachDist)
:CCoreServerObjJob(uObjID)
,m_uTargetID(uTargetID)
,m_fSpeed(fSpeed)
,m_uFindPathType(uFindPathType)
,m_uBarrierType(uBarrierType)
,m_fReachDist(fReachDist)
{
}

CCoreObjTraceJob::~CCoreObjTraceJob()
{
	CSyncCoreObjectServer* pCoreObj = GetCoreObj();
	if(!pCoreObj)
		return;

	pCoreObj->IncreaseTraceSessionID();
	pCoreObj->Trace(CSyncCoreObjectServer::GetObjectServer(m_uTargetID), m_fSpeed, static_cast<EFindPathType>(m_uFindPathType), static_cast<EBarrierType>(m_uBarrierType), m_fReachDist);
}

CCoreObjStopTracingJob::CCoreObjStopTracingJob(uint32 uObjID)
:CCoreServerObjJob(uObjID)
{
}


CCoreObjStopTracingJob::~CCoreObjStopTracingJob()
{
	CSyncCoreObjectServer* pCoreObj = GetCoreObj();
	if(!pCoreObj)
		return;

	pCoreObj->StopMoving();
	pCoreObj->StopTracing();
}
		
CCoreObjSetSpeedJob::CCoreObjSetSpeedJob(uint32 uObjID, float fSpeed)
:CCoreServerObjJob(uObjID)
,m_fSpeed(fSpeed)
{
}

CCoreObjSetSpeedJob::~CCoreObjSetSpeedJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;

	pCoreObj->SetSpeed(m_fSpeed);
}

CCoreObjSetSizeJob::CCoreObjSetSizeJob(uint32 uObjID, float fSize, uint32 uDimension)
:CCoreServerObjJob(uObjID)
,m_fSize(fSize)
,m_uDimension(uDimension)
{
}


CCoreObjSetSizeJob::~CCoreObjSetSizeJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	
	pCoreObj->SetSize(m_fSize, m_uDimension);
}

CCoreObjSetEyeSightJob::CCoreObjSetEyeSightJob(uint32 uObjID, float fEyeSight, uint32 uDimension)
:CCoreServerObjJob(uObjID)
,m_fEyeSight(fEyeSight)
,m_uDimension(uDimension)
{
}

CCoreObjSetEyeSightJob::~CCoreObjSetEyeSightJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	
	pCoreObj->SetEyeSight(m_fEyeSight, m_uDimension);
}

CCoreObjSetStealthJob::CCoreObjSetStealthJob(uint32 uObjID, float fStealth, uint32 uDimension)
:CCoreServerObjJob(uObjID)
,m_fStealth(fStealth)
,m_uDimension(uDimension)
{
}


CCoreObjSetStealthJob::~CCoreObjSetStealthJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	pCoreObj->SetStealth(m_fStealth, m_uDimension);
}

CCoreObjSetKeennessJob::CCoreObjSetKeennessJob(uint32 uObjID, float fKeenness, uint32 uDimension)
:CCoreServerObjJob(uObjID)
,m_fKeenness(fKeenness)
,m_uDimension(uDimension)
{
}


CCoreObjSetKeennessJob::~CCoreObjSetKeennessJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	pCoreObj->SetKeenness(m_fKeenness, m_uDimension);
}

CCoreObjSetViewGroupJob::CCoreObjSetViewGroupJob(uint32 uObjID, uint32 uGroupID, uint32 uDimension)
:CCoreServerObjJob(uObjID)
,m_uGroupID(uGroupID)
,m_uDimension(uDimension)
{
}


CCoreObjSetViewGroupJob::~CCoreObjSetViewGroupJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	pCoreObj->SetViewGroup(m_uGroupID, m_uDimension);
}

CCoreObjPostAoiMsgToSelfJob::CCoreObjPostAoiMsgToSelfJob(uint32 uObjID, const void* pContext)
:CCoreServerObjJob(uObjID)
,m_pContext(pContext)
{
}


CCoreObjPostAoiMsgToSelfJob::~CCoreObjPostAoiMsgToSelfJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	pCoreObj->PostAoiMsgToSelf(m_pContext);
}

CCoreObjPostEventJob::CCoreObjPostEventJob(uint32 uObjID, const void* pContext)
:CCoreServerObjJob(uObjID)
,m_pContext(pContext)
{
}

CCoreObjPostEventJob::~CCoreObjPostEventJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	pCoreObj->PostEvent(m_pContext);
}

CCoreObjSetPixelPosJob::CCoreObjSetPixelPosJob(uint32 uObjID, const CFPos& PixelPos)
:CCoreServerObjJob(uObjID)
,m_PixelPos(PixelPos)
{
}

CCoreObjSetPixelPosJob::~CCoreObjSetPixelPosJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	pCoreObj->SetPixelPos(m_PixelPos);

	(new CCorePixelPosSetResult(m_uObjID))->Add();
}

CCoreObjSetCoreSizeJob::CCoreObjSetCoreSizeJob(uint32 uObjID, float fSize)
:CCoreServerObjJob(uObjID)
,m_fSize(fSize)
{
}


CCoreObjSetCoreSizeJob::~CCoreObjSetCoreSizeJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	pCoreObj->SetQuerySize(m_fSize);
}

CCoreObjBroadCastForViewAoiExtraConnJob::CCoreObjBroadCastForViewAoiExtraConnJob(uint32 uObjID, const void* pData, uint32 uSize, uint32 uExtraConn)
:CCoreServerObjJob(uObjID)
,m_pData(CloneData(pData,uSize))
,m_uSize(uSize)
,m_uExtraConn(uExtraConn)
{
}


CCoreObjBroadCastForViewAoiExtraConnJob::~CCoreObjBroadCastForViewAoiExtraConnJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	CSyncCoreObjectDictator* pDicObj = pCoreObj->CastToObjDictator();
	if(!pDicObj)
		return;

	pDicObj->BroadCastForViewAoiExtraConn(m_pData, m_uSize, CSynConnServer::GetSynConn(m_uExtraConn));
}

CCoreObjBroadCastForSyncAoiExtraConnJob::CCoreObjBroadCastForSyncAoiExtraConnJob(uint32 uObjID, const void* pData, uint32 uSize, uint32 uExtraConn)
:CCoreServerObjJob(uObjID)
,m_pData(CloneData(pData,uSize))
,m_uSize(uSize)
,m_uExtraConn(uExtraConn)
{
}

CCoreObjBroadCastForSyncAoiExtraConnJob::~CCoreObjBroadCastForSyncAoiExtraConnJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	CSyncCoreObjectDictator* pDicObj = pCoreObj->CastToObjDictator();
	if(!pDicObj)
		return;

	pDicObj->BroadCastForSyncAoiExtraConn(m_pData, m_uSize, CSynConnServer::GetSynConn(m_uExtraConn));
}

CCoreObjEnableFollowerDelayJob::CCoreObjEnableFollowerDelayJob(uint32 uObjID, bool bEnableFollowerDelay)
:CCoreServerObjJob(uObjID)
,m_bEnableFollowerDelay(bEnableFollowerDelay)
{
}


CCoreObjEnableFollowerDelayJob::~CCoreObjEnableFollowerDelayJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	CSyncCoreObjectDictator* pDicObj = pCoreObj->CastToObjDictator();
	if(!pDicObj)
		return;
	pDicObj->EnableFollowerDelay(m_bEnableFollowerDelay);
}

CCoreObjSetBarrierJob::CCoreObjSetBarrierJob(uint32 uObjID, float fBarrierSize, uint32 uBarrierType)
:CCoreServerObjJob(uObjID)
,m_fBarrierSize(fBarrierSize)
,m_uBarrierType(uBarrierType)
{
}


CCoreObjSetBarrierJob::~CCoreObjSetBarrierJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	CSyncCoreObjectDictator* pDicObj = pCoreObj->CastToObjDictator();
	if(!pDicObj)
		return;
	pDicObj->SetBarrier(m_fBarrierSize, EBarrierType(m_uBarrierType));
}

CCoreObjSyncBarrierExtraConnJob::CCoreObjSyncBarrierExtraConnJob(uint32 uObjID, uint32 uExtraConn)
:CCoreServerObjJob(uObjID)
,m_uExtraConn(uExtraConn)
{
}


CCoreObjSyncBarrierExtraConnJob::~CCoreObjSyncBarrierExtraConnJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	CSyncCoreObjectDictator* pDicObj = pCoreObj->CastToObjDictator();
	if(!pDicObj)
		return;
	pDicObj->SyncBarrierExtraConn(CSynConnServer::GetSynConn(m_uExtraConn));
}

CCoreObjSetConnectionJob::CCoreObjSetConnectionJob(uint32 uObjID, uint32 uConn)
:CCoreServerObjJob(uObjID)
,m_uConn(uConn)
{
}


CCoreObjSetConnectionJob::~CCoreObjSetConnectionJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	CSyncCoreObjectMediator* pMedObj = pCoreObj->CastToObjMediator();
	if(!pMedObj)
		return;
	pMedObj->SetConnection(CSynConnServer::GetSynConn(m_uConn));
}

CCoreObjSetDirectorMaxSpeedJob::CCoreObjSetDirectorMaxSpeedJob(uint32 uObjID, float fDirectorMaxSpeed)
:CCoreServerObjJob(uObjID)
,m_fDirectorMaxSpeed(fDirectorMaxSpeed)
{
}


CCoreObjSetDirectorMaxSpeedJob::~CCoreObjSetDirectorMaxSpeedJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	CSyncCoreObjectMediator* pMedObj = pCoreObj->CastToObjMediator();
	if(!pMedObj)
		return;
	pMedObj->SetDirectorMaxSpeed(m_fDirectorMaxSpeed);
}

CCoreObjCreateQuadObjJob::CCoreObjCreateQuadObjJob(uint32 uObjID, uint32 uSlot, const CFPos& PixelPos)
:CCoreServerObjJob(uObjID)
,m_uSlot(uSlot)
,m_PixelPos(PixelPos)
{
}


CCoreObjCreateQuadObjJob::~CCoreObjCreateQuadObjJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	pCoreObj->CreateQuadObject(m_uSlot, m_PixelPos);
}

CCoreObjDestoryObjJob::CCoreObjDestoryObjJob(uint32 uObjID)
:CCoreServerObjJob(uObjID)
{
}


CCoreObjDestoryObjJob::~CCoreObjDestoryObjJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	pCoreObj->BeginDestroyObject();
}

CCoreObjTransferObjJob::CCoreObjTransferObjJob(uint32 uObjID, uint32 uTargetSceneID, const CFPos& PixelPos)
:CCoreServerObjJob(uObjID)
,m_uTargetSceneID(uTargetSceneID)
,m_pos(PixelPos)
{
}


CCoreObjTransferObjJob::~CCoreObjTransferObjJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;

	CSyncCoreSceneServer* pTargetSynCoreScene = CSyncCoreSceneServer::GetSynCoreSceneServer(m_uTargetSceneID);
	if(!pTargetSynCoreScene)
		return;

	pCoreObj->BeginTransferObject(pTargetSynCoreScene, m_pos);
}

CCoreSetMoveSpeedCheckArgJob::CCoreSetMoveSpeedCheckArgJob(uint32 uClientTimeStampErrorThreshold, float fMaxStepCommandRate)
:m_uClientTimeStampErrorThreshold(uClientTimeStampErrorThreshold)
,m_fMaxStepCommandRate(fMaxStepCommandRate)
{
}

CCoreSetMoveSpeedCheckArgJob::~CCoreSetMoveSpeedCheckArgJob()
{
	CSyncCoreObjectMediator::SetMoveSpeedCheckArg(m_uClientTimeStampErrorThreshold, m_fMaxStepCommandRate);
}

CCoreObjBindToJob::CCoreObjBindToJob(uint32 uObjID, uint32 uParentObjID)
:CCoreServerObjJob(uObjID)
,m_uParentObjID(uParentObjID)
{
}

CCoreObjBindToJob::~CCoreObjBindToJob()
{
	CSyncCoreObjectServer* pCoreObj = GetCoreObj();
	if(!pCoreObj)
		return;
	CSyncCoreObjectDictator* pDicObj = pCoreObj->CastToObjDictator();
	if(!pDicObj)
		return;
	if(!pDicObj->BindTo(m_uParentObjID))
	{
		//返回false
	}
}

CCoreObjDisbindFromParentJob::CCoreObjDisbindFromParentJob(uint32 uObjID)
:CCoreServerObjJob(uObjID)
{
}

CCoreObjDisbindFromParentJob::~CCoreObjDisbindFromParentJob()
{
	CSyncCoreObjectServer* pCoreObj = GetCoreObj();
	if(!pCoreObj)
		return;
	CSyncCoreObjectDictator* pDicObj = pCoreObj->CastToObjDictator();
	if(!pDicObj)
		return;
	if(!pDicObj->DisbindFromParent())
	{
		//返回false
	}
}

CCoreObjDisbindToChildJob::CCoreObjDisbindToChildJob(uint32 uObjID, uint32 uChildObjID)
:CCoreServerObjJob(uObjID)
,m_uChildObjID(uChildObjID)
{
}

CCoreObjDisbindToChildJob::~CCoreObjDisbindToChildJob()
{
	CSyncCoreObjectServer* pCoreObj = GetCoreObj();
	if(!pCoreObj)
		return;
	CSyncCoreObjectDictator* pDicObj = pCoreObj->CastToObjDictator();
	if(!pDicObj)
		return;
	if(!pDicObj->DisbindChild(m_uChildObjID))
	{
		//返回false
	}
}

CCoreObjDisbindToAllChildJob::CCoreObjDisbindToAllChildJob(uint32 uObjID)
:CCoreServerObjJob(uObjID)
{
}

CCoreObjDisbindToAllChildJob::~CCoreObjDisbindToAllChildJob()
{
	CSyncCoreObjectServer* pCoreObj = GetCoreObj();
	if(!pCoreObj)
		return;
	CSyncCoreObjectDictator* pDicObj = pCoreObj->CastToObjDictator();
	if(!pDicObj)
		return;
	if(!pDicObj->DisbindAllChildren())
	{
		//返回false
	}
}

CAsynCoreObjHaltedJob::CAsynCoreObjHaltedJob(uint32 uObjID)
:CCoreServerObjJob(uObjID)
{
}

CAsynCoreObjHaltedJob::~CAsynCoreObjHaltedJob()
{
	CSyncCoreObjectServer* pSyncCoreObj = GetCoreObj();
	if (pSyncCoreObj)
	{
		pSyncCoreObj->OnAoiObjectHalted();
	}
}
