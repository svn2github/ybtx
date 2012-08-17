#include "stdafx.h"
#include "CSyncMetaSceneServer.h"
#include "CSyncMetaSceneMgrServer.h"
#include "TMetaSceneMgr.inl"
#include "TWhole.inl"
#include "CAppServer.h"
#include "CMetaSceneServer.h"
#include "IPathFinding.h"

template class TMetaSceneMgr<CSyncTraitsServer>;
template class TWhole<CSyncMetaSceneMgrServer,CSyncMetaSceneServer>;

CSyncMetaSceneMgrServer::CSyncMetaSceneMgrServer(void)
{
	
}

CSyncMetaSceneMgrServer::~CSyncMetaSceneMgrServer(void)
{
	
}

CSyncMetaSceneServer* CSyncMetaSceneMgrServer::CreateMetaScene(const CMetaSceneServer* pMetaScene)
{
	if( CAppServer::Inst()->DoingQuit() )
		GenErr("Can't create MetaScene while application is doing quit.");

	CSyncMetaSceneServer* pSyncMetaScene = Parent_t::CreateMetaScene(pMetaScene->GetSceneName(), pMetaScene->GetRegionVersionID());
	pSyncMetaScene->m_nMagicID = pMetaScene->m_nMagicID;
	pSyncMetaScene->m_nVersion = pMetaScene->m_nVersion;
	pSyncMetaScene->m_uGridWidth = pMetaScene->m_uGridWidth;
	pSyncMetaScene->m_bCave = pMetaScene->m_bCave;
	pSyncMetaScene->m_nSubRegionNum = pMetaScene->m_nSubRegionNum;
	pSyncMetaScene->m_uRegionNum = pMetaScene->m_uRegionNum;
	pSyncMetaScene->m_uSubWidthInRegion = pMetaScene->m_uSubWidthInRegion;
	pSyncMetaScene->m_uWidthInRegion = pMetaScene->m_uWidthInRegion;
	pSyncMetaScene->m_uHeightInRegion = pMetaScene->m_uHeightInRegion;
	pSyncMetaScene->m_uWidthInGrid = pMetaScene->m_uWidthInGrid;
	pSyncMetaScene->m_uHeightInGrid = pMetaScene->m_uHeightInGrid;
	pSyncMetaScene->m_uWidthInPixel = pMetaScene->m_uWidthInPixel;
	pSyncMetaScene->m_uHeightInPixel = pMetaScene->m_uHeightInPixel;
	pSyncMetaScene->m_pMapPath = CreateMap(pSyncMetaScene->m_uWidthInRegion, pSyncMetaScene->m_uHeightInRegion, pMetaScene->m_pMapPath->GetBarrierMatrix());
	pSyncMetaScene->m_pMapPath->SetSharedData(pMetaScene->m_pMapPath);
	return pSyncMetaScene;
}

void CSyncMetaSceneMgrServer::DestroyMetaScene(CSyncMetaSceneServer* pScene)
{
	Parent_t::DestroyMetaScene(pScene);
}

