#include "stdafx.h"
#include "CCoreServerSceneJob.h"
#include "CCoreServerJob.inl"
#include "CSyncCoreSceneServer.h"
#include "CSyncCoreObjectServer.h"
#include "CSyncCoreSceneMgrServer.h"
#include "CDimensionInfo.h"

CCoreCreateCalcObjJob::CCoreCreateCalcObjJob(uint32 uSceneID, uint32 uObjID, const CFPos& PixelPos, uint32 uSlot)
:m_uSceneID(uSceneID)
,m_uObjID(uObjID)
,m_PixelPos(PixelPos)
,m_uSlot(uSlot)
{
}


CCoreCreateCalcObjJob::~CCoreCreateCalcObjJob()
{
	CSyncCoreSceneServer* pScene = CSyncCoreSceneServer::GetSynCoreSceneServer(m_uSceneID);
	if(!pScene)
		return;
	pScene->CreateObjectCalculator(m_uObjID, m_PixelPos, m_uSlot);
}

CCoreCreateDicObjJob::CCoreCreateDicObjJob(uint32 uSceneID, uint32 uObjID, const CFPos& PixelPos,uint32 uVarDefID, uint32 uSlot)
:m_uSceneID(uSceneID)
,m_uObjID(uObjID)
,m_PixelPos(PixelPos)
,m_uVarDefID(uVarDefID)
,m_uSlot(uSlot)
{
}

CCoreCreateDicObjJob::~CCoreCreateDicObjJob()
{
	CSyncCoreSceneServer* pScene = CSyncCoreSceneServer::GetSynCoreSceneServer(m_uSceneID);
	if(!pScene)
		return;
	pScene->CreateObjectDictator(m_uObjID, m_PixelPos, m_uVarDefID, m_uSlot);
}

CCoreCreateMedObjJob::CCoreCreateMedObjJob(uint32 uSceneID, uint32 uObjID, const CFPos& PixelPos, uint32 uVarDefID, uint32 uSlot, float fDirectorMaxSpeed)
:m_uSceneID(uSceneID)
,m_uObjID(uObjID)
,m_PixelPos(PixelPos)
,m_uVarDefID(uVarDefID)
,m_uSlot(uSlot)
,m_fDirectorMaxSpeed(fDirectorMaxSpeed)
{
}


CCoreCreateMedObjJob::~CCoreCreateMedObjJob()
{
	CSyncCoreSceneServer* pScene = CSyncCoreSceneServer::GetSynCoreSceneServer(m_uSceneID);
	if(!pScene)
		return;
	pScene->CreateObjectMediator(m_uObjID, m_PixelPos, m_uVarDefID, m_uSlot, m_fDirectorMaxSpeed);
}

CCoreCreateCoreSceneJob::CCoreCreateCoreSceneJob(uint32 uSceneID, CSyncMetaSceneServer* pMetaScene,const char* szSyncAoiFileName, CDimensionInfo* DimInfo, uint32 uSlot)
:m_uSceneID(uSceneID)
,m_pMetaScene(pMetaScene)
,m_sSyncAoiFileName(CloneString(szSyncAoiFileName))
,m_uSlot(uSlot)
{
	m_DimensionInfo = new (AllocMem(sizeof(CDimensionInfo))) CDimensionInfo(*DimInfo);
}

CCoreCreateCoreSceneJob::~CCoreCreateCoreSceneJob()
{
	CSyncCoreSceneMgrServer* pSynCoreSceneMgr = CSyncCoreSceneMgrServer::Inst();
	if (pSynCoreSceneMgr)
	{
		pSynCoreSceneMgr->CreateCoreScene(m_uSceneID, m_pMetaScene, m_sSyncAoiFileName, m_DimensionInfo, m_uSlot);
	}

	m_DimensionInfo->~CDimensionInfo();
}

CCoreDestroyCoreSceneJob::CCoreDestroyCoreSceneJob(uint32	uSceneID)
:m_uSceneID(uSceneID)
{

}

CCoreDestroyCoreSceneJob::~CCoreDestroyCoreSceneJob()
{
	CSyncCoreSceneMgrServer* pSynCoreSceneMgr = CSyncCoreSceneMgrServer::Inst();
	CSyncCoreSceneServer* pSynCoreScene = CSyncCoreSceneServer::GetSynCoreSceneServer(m_uSceneID);
	if (pSynCoreSceneMgr && pSynCoreScene)
	{
		pSynCoreSceneMgr->DestroyCoreScene(pSynCoreScene);
	}
}


CCoreSendSceneCmdJob::CCoreSendSceneCmdJob(uint32 uSceneID, const void* pData, uint32 uSize)
:m_uSceneID(uSceneID)
,m_pData(CloneData(pData,uSize))
,m_uSize(uSize)
{
}

CCoreSendSceneCmdJob::~CCoreSendSceneCmdJob()
{
	CSyncCoreSceneServer* pSynCoreScene = CSyncCoreSceneServer::GetSynCoreSceneServer(m_uSceneID);
	if(pSynCoreScene)
	{
		pSynCoreScene->Send(m_pData, m_uSize);
	}
}

CCoreSetTimeDistortedRatioJob::CCoreSetTimeDistortedRatioJob(uint32	uSceneID, float fRatio)
:m_uSceneID(uSceneID)
,m_fRatio(fRatio)
{
}

CCoreSetTimeDistortedRatioJob::~CCoreSetTimeDistortedRatioJob()
{
	CSyncCoreSceneServer* pSynCoreScene = CSyncCoreSceneServer::GetSynCoreSceneServer(m_uSceneID);
	if(pSynCoreScene)
	{
		pSynCoreScene->SetTimeDistortedRatio(m_fRatio);
	}
}
