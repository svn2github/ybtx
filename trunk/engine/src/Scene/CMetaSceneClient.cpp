#include "stdafx.h"
#include <float.h>
#include "CGraphic.h"
#include "CMetaGridMgr.h"
#include "CCoreSceneClient.h"
#include "CMetaSceneClient.h"
#include "CMetaRegion.h"
#include "CMetaStaticData.h"
#include "BaseHelper.h"
#include "IProcessCallBack.h"
#include "CMetaSceneMgrClient.h"
#include "TMetaScene.inl"
#include "CPkgFile.h"
#include "TWhole.inl"
#include "TPart.inl"
#include "CShadowedGrids.h"
#include "CAppClient.h"
#include "CBakedColorData.h"
#include "CQuadScene.h"
#include "SyncSystem.h"
#include "CWindowTarget.h"
template class TMetaScene<CTraitsClient>;
template class TWhole<CMetaSceneClient,CCoreSceneClient>;
template class TPart<CMetaSceneClient,CMetaSceneMgrClient>;



#define INVALID -1


#include <Windows.h>

IMap*		CoreSceneManagerClient::CreateIMap(uint32 uWidthInRegion, uint32 uHeightInRegion)
{
	return CreateMap(uWidthInRegion,uHeightInRegion);
}

CMetaSceneClient::CMetaSceneClient(  const char* szSceneName, const uint32 sceneId, CMetaSceneMgrClient* pMgr )
:ParentMetaScene_t( szSceneName,sceneId,pMgr )
, m_pTreeScene(new CQuadTreeScene)
, m_SceneManager(szSceneName,m_pTreeScene,3,3,5)
, m_bUsedByCore(false)
, m_uUsedCountByShell(0)
{
	m_SceneManager.Create();
	m_uWidthInRegion	= m_SceneManager.GetWidthInRegion();
	m_uHeightInRegion	= m_SceneManager.GetHeightInRegion();
	m_uWidthInGrid		= m_SceneManager.GetWidthInGrid();
	m_uHeightInGrid		= m_SceneManager.GetHeightInGrid();
	m_uWidthInPixel		= m_SceneManager.GetWidthInPixel();
	m_uHeightInPixel	= m_SceneManager.GetHeightInPixel();

	m_nSubRegionNum		= m_SceneManager.GetSubRegionNum();
	m_uRegionNum		= m_SceneManager.GetRegionNum();
	m_uSubWidthInRegion	= m_SceneManager.GetWidthInSubRegion();
	m_pMapPath			= m_SceneManager.GetMapPath();

	///判读服务端和客户端版本号是否一致
	if( sceneId!=0&&this->m_nVersionID != sceneId )
	{
		ostringstream strm;
		strm<< "文件ID不匹配,客户端: " << this->m_nVersionID << "  服务端: " << sceneId <<endl;

		MessageBox(NULL, strm.str().c_str(), "提示", MB_OK);
	}

	LoadLinkRegionInfo();
}

CMetaSceneClient::~CMetaSceneClient()
{
	CSyncSystem::BeginSyncFun();
	SafeRelease(m_pMapPath);
}



CoreSceneManagerClient::CoreSceneManagerClient( const SString& SceneName, CTreeScene* pTreeScene , uint32 Wdith , uint32 Height, uint32 RecycleSize ) 
: CSceneManagerClient(SceneName, pTreeScene, CMainWindowTarget::GetInst()->GetGroup(),Wdith,Height,RecycleSize)
{

}