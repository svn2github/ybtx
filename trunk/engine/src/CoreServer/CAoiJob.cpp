#include "stdafx.h"
#include "CAoiJob.h"
#include "CAoiJob.inl"
#include "CAsyAoiViewObj.h"
#include "CAsyAoiScene.h"
#include "ExpHelper.h"
#include "CLogicThreadResult.inl"
#include "CSynAoiScene.h"
#include "CSynAoiViewObj.h"
#include "CSynAoiSyncerObj.h"
#include "CSynAoiSynceeObj.h"

#include "CAoiThreadMgr.h"
#include "CAoiResult.inl"
#include "CDimensionInfo.h"


CSynAoiViewObj* CAoiJob::GetSynObj(uint32 uId)
{
	return CAsyAoiViewObj::GetSynAoiObject(uId);
}

inline void CAoiJob::AddSynObj(uint32 uId, CSynAoiViewObj *pObj)
{
	CAsyAoiViewObj::AddSynAoiObject(uId, pObj);
}

inline void CAoiJob::DelSynObj(uint32 uId)
{
	CAsyAoiViewObj::DelSynAoiObject(uId);
}

inline CSynAoiScene* CAoiJob::GetSynScene(uint32 uId)
{
	return CAsyAoiScene::GetSynAoiScene(uId);
}

inline void CAoiJob::AddSynScene(uint32 uId, CSynAoiScene *pScene)
{
	CAsyAoiScene::AddSynAoiScene(uId, pScene);
}

inline void CAoiJob::DelSynScene(uint32 uId)
{
	CAsyAoiScene::DelSynAoiScene(uId);
}

/************************************************************************/
/*                                                                      
	Set Size
*/
/************************************************************************/
CAoiSetSizeJob::CAoiSetSizeJob(uint32 uId, float fSize, uint32 uDimension)
: CAoiObjWithLayerJob(uId, uDimension), m_fSize(fSize)
{
}


CAoiSetSizeJob::~CAoiSetSizeJob()
{
	GetSynObj(m_uId)->SetSize(m_fSize, m_uDimension);
}

/************************************************************************/
/*                                                                      
Set Eye Sight
*/
/************************************************************************/
CAoiSetEyeSightJob::CAoiSetEyeSightJob(uint32 uId, float fEyeSight, uint32 uDimension)
: CAoiObjWithLayerJob(uId, uDimension), m_fEyeSight(fEyeSight)
{
}


CAoiSetEyeSightJob::~CAoiSetEyeSightJob()
{
	GetSynObj(m_uId)->SetEyeSight(m_fEyeSight, m_uDimension);
}

/************************************************************************/
/*                                                                      
Set Stealth
*/
/************************************************************************/
CAoiSetStealthJob::CAoiSetStealthJob(uint32 uId, float fStealth, uint32 uDimension)
: CAoiObjWithLayerJob(uId, uDimension), m_fStealth(fStealth)
{
}


CAoiSetStealthJob::~CAoiSetStealthJob()
{
	GetSynObj(m_uId)->SetStealth(m_fStealth, m_uDimension);
}

/************************************************************************/
/*                                                                      
Set Keenness
*/
/************************************************************************/
CAoiSetKeennessJob::CAoiSetKeennessJob(uint32 uId, float fKeenness, uint32 uDimension)
: CAoiObjWithLayerJob(uId, uDimension), m_fKeenness(fKeenness)
{
}


CAoiSetKeennessJob::~CAoiSetKeennessJob()
{
	GetSynObj(m_uId)->SetKeenness(m_fKeenness, m_uDimension);
}

/************************************************************************/
/*                                                                      
Set View Group
*/
/************************************************************************/
CAoiSetViewGroupJob::CAoiSetViewGroupJob(uint32 uId, uint32 uViewGroup, uint32 uDimension)
: CAoiObjWithLayerJob(uId, uDimension), m_uViewGroup(uViewGroup)
{
}


CAoiSetViewGroupJob::~CAoiSetViewGroupJob()
{
	GetSynObj(m_uId)->SetViewGroup(m_uViewGroup, m_uDimension);
}

/************************************************************************/
/*                                                                      
Set Grid Pos
*/
/************************************************************************/
CAoiSetPosJob::CAoiSetPosJob(uint32 uId, const CFPos &pos)
: CAoiObjJob(uId), m_Pos(pos)
{
}


CAoiSetPosJob::~CAoiSetPosJob()
{
	GetSynObj(m_uId)->SetPos(m_Pos);
}

CAoiForceSetPosJob::CAoiForceSetPosJob(sqr::uint32 uId, const sqr::CFPos &pos)
:CAoiObjJob(uId), m_Pos(pos)
{
}

CAoiForceSetPosJob::~CAoiForceSetPosJob()
{
	GetSynObj(m_uId)->ForceSetPos(m_Pos);
}


/************************************************************************/
/*                                                                      
Create the aoi object
*/
/************************************************************************/
CAoiCreateObjJob::CAoiCreateObjJob(uint32 uId, uint32 uSceneId, uint32 uCoreObjId,
								   const CFPos& pos, EAoiObjType type)
								   : CAoiObjJob(uId), m_uSceneId(uSceneId), m_uCoreObjId(uCoreObjId)
								   , m_Pos(pos), m_eType(type)
{
}

CAoiCreateObjJob::~CAoiCreateObjJob()
{
	CSynAoiScene *pScene = GetSynScene(m_uSceneId);
	switch(m_eType)
	{
	case eAOT_ViewObj:
		{
			CSynAoiViewObj* pObj = pScene->CreateViewObj(m_Pos, m_uId, m_uCoreObjId);
			AddSynObj(m_uId, pObj);
		}
		break;
	case eAOT_SyncerObj:
		{
			CSynAoiSyncerObj *pObj = pScene->CreateSyncerObj(m_Pos, m_uId, m_uCoreObjId);
			AddSynObj(m_uId, pObj);
		}
		break;
	case eAOT_SynceeObj:
		{
			CSynAoiSynceeObj *pObj = pScene->CreateSynceeObj(m_Pos, m_uId, m_uCoreObjId);
			AddSynObj(m_uId, pObj);
		}
		break;
	default:
		{
			ostringstream oss;
			oss<<"Invalid aoi obj type:"<<m_eType;
			GenErr("CAoiCreateObjJob Error", oss.str());
		}
	}
}

/************************************************************************/
/*                                                                      
Delete the aoi object
*/
/************************************************************************/
CAoiDestroyObjJob::CAoiDestroyObjJob(uint32 uId, uint32 uSceneId)
: CAoiObjJob(uId), m_uSceneId(uSceneId)
{
}


CAoiDestroyObjJob::~CAoiDestroyObjJob()
{
	CSynAoiScene *pScene = GetSynScene(m_uSceneId);
	CSynAoiViewObj *pObj = GetSynObj(m_uId);

	DelSynObj(m_uId);
	pScene->DestroyObj(pObj);


	(new CAoiObjHaltedResult(m_uId))->Add();
	(new CAoiObjHaltedResultToLogic(pObj->GetCoreObjID()))->Add();
}

/************************************************************************/
/*                                                                      
Create the aoi scene
*/
/************************************************************************/
CAoiCreateSceneJob::CAoiCreateSceneJob(uint32 uSceneId, 
									   const char* szSyncAoiFileName, uint32 uScale,  
									   uint32 uGridWidth, uint32 uGridHeight,
									   CDimensionInfo*	DimInfo)
									   : m_uSceneId(uSceneId)
									   , m_uGridWidth(uGridWidth)
									   , m_uGridHeight(uGridHeight)
									   , m_uScale(uScale)
{
	m_szSyncAoiFileName = CloneString(szSyncAoiFileName);
	m_DimensionInfo = new (AllocMem(sizeof(CDimensionInfo)))CDimensionInfo(*DimInfo);
}


CAoiCreateSceneJob::~CAoiCreateSceneJob()
{
	CSynAoiScene *pScene = new CSynAoiScene(m_szSyncAoiFileName, m_uScale,
											m_uGridWidth, m_uGridHeight, m_DimensionInfo);
	AddSynScene(m_uSceneId, pScene);

	m_DimensionInfo->~CDimensionInfo();
}

/************************************************************************/
/*                                                                      
Delete the aoi scene
*/
/************************************************************************/
CAoiDestroySceneJob::CAoiDestroySceneJob(uint32 uSceneId)
: m_uSceneId(uSceneId)
{
}


CAoiDestroySceneJob::~CAoiDestroySceneJob()
{
	GetSynScene(m_uSceneId)->Release();
	DelSynScene(m_uSceneId);

	(new CAoiSceneHaltedResult(m_uSceneId))->Add();
}

/*
	Post Msg to Observee
*/
CAoiPostMsgToVieweeJob::CAoiPostMsgToVieweeJob(uint32 uId, const void* pContext, uint32 uDimension)
: CAoiObjWithLayerJob(uId, uDimension)
, m_pContext(pContext)
{
}


CAoiPostMsgToVieweeJob::~CAoiPostMsgToVieweeJob()
{
	GetSynObj(m_uId)->PostMsgToViewee(m_pContext, m_uDimension);
}

/*
	Post Msg to Observer
*/
CAoiPostMsgToViewerJob::CAoiPostMsgToViewerJob(uint32 uId, const void* pContext, uint32 uDimension)
: CAoiObjWithLayerJob(uId, uDimension)
, m_pContext(pContext)
{
}

CAoiPostMsgToViewerJob::~CAoiPostMsgToViewerJob()
{
	GetSynObj(m_uId)->PostMsgToViewer(m_pContext, m_uDimension);
}

/*
	Post Msg to Syncee
*/
CAoiPostMsgToSynceeJob::CAoiPostMsgToSynceeJob(uint32 uId, const void* pContext)
: CAoiObjJob(uId)
, m_pContext(pContext)
{
}

CAoiPostMsgToSynceeJob::~CAoiPostMsgToSynceeJob()
{
	CSynAoiSyncerObj *pObj = static_cast<CSynAoiSyncerObj*>(GetSynObj(m_uId));
	pObj->PostMsgToSyncee(m_pContext);
}

/*
	Post Msg to Syncer
*/
CAoiPostMsgToSyncerJob::CAoiPostMsgToSyncerJob(uint32 uId, const void* pContext)
: CAoiObjJob(uId)
, m_pContext(pContext)
{
}


CAoiPostMsgToSyncerJob::~CAoiPostMsgToSyncerJob()
{
	CSynAoiSynceeObj *pObj = static_cast<CSynAoiSynceeObj*>(GetSynObj(m_uId));
	pObj->PostMsgToSyncer(m_pContext);
}

/*
	Post Msg to Self
*/
CAoiPostMsgToSelfJob::CAoiPostMsgToSelfJob(uint32 uId, const void* pContext)
: CAoiObjJob(uId)
, m_pContext(pContext)
{
}


CAoiPostMsgToSelfJob::~CAoiPostMsgToSelfJob()
{
	(new CAoiMsgToSelfHandledResult(m_uId, m_pContext))->Add();
}

