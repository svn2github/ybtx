#include "StdAfx.h"
#include "LightMapBaker.h"
#include "ShadowMapBakerPool.h"
#include "CWindowTarget.h"
#include "CGraphic.h"
#include "CRenderPipeline.h"
#include "LightMapWriter.h"
#include "CLightMapFilter.h"
#include "OfflineRender.h"

//#include "SceneEnvMgr.h"
//#include "Scene.h"
#include "CMapEditCamera.inl"
#include "CCameraManager.h"

#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "CTerrain.h"
#include "TerrainMeshRegion.h"
#include "TerrainMeshObject.h"
#include "ModelGroup.h"
#include "CMapEditObject.h"
#include "CEditTerrainMgr.h"
#include "CEditModel.h"
#include <DepathGroupModelInfo.h>
#include "Console.h"


namespace sqr
{

CLightMapBaker::CLightMapBaker(void)
	: tileWidth(0)
	, tileDepth(0)
	, LightMapCamera(0)
{
}

CLightMapBaker::~CLightMapBaker(void)
{
}

void CLightMapBaker::Setup()
{
	CBakerBase::Setup();
	CTerrainMesh* terrainMesh = CMapEditApp::GetInst()->GetTerrain();
	this->terrain = terrainMesh->m_pTerrain;

	m_LightMapWriter = new CLMFileWriter();
	LightMapCamera = new CCamera();

	m_LightMapTarget = CGraphic::GetInst()->CreateRenderTarget();
	m_LightMapTarget->Release();                 //RTT显存正确释放补丁
	m_LightMapTarget->SetWidth(512);
	m_LightMapTarget->SetHeight(512);
	m_LightMapTarget->SetFormat(TFMT_X8R8G8B8);
	m_LightMapTarget->Create();
	m_LightMapTarget->SetClearColor(CColor::White);
	m_LightMapTarget->Clear();

	tileWidth = terrainMesh->GetWidthInRegion() / TILE_WIDTH_IN_REGION;
	tileDepth = terrainMesh->GetDepthInRegion() / TILE_WIDTH_IN_REGION;
}

// 销毁
void CLightMapBaker::Discard()
{
	CShadowMapBakingPool::GetInst()->ClearShadowBuf();

 	delete LightMapCamera;
	m_LightMapTarget=NULL;
	m_LightMapWriter=NULL;

	CBakerBase::Discard();
}

void CLightMapBaker::OnPrepare()
{
	//阴影过滤器切换和设置
	CRenderGroup* tempRenderGroup=CMainWindowTarget::GetInst()->GetGroup();
	oldShadowType = tempRenderGroup->GetShadowType();
	m_pShadowFilter = new CLightMapFilter(tempRenderGroup);
	Ast(m_pShadowFilter!=NULL);
	tempRenderGroup->SetShadowFilter(m_pShadowFilter);
}

void CLightMapBaker::TilePrepare(index_t x,index_t z)
{
	CBakerBase::TilePrepare(x,z);
	int tileIndex=x+z*tileWidth;
	CAxisAlignedBox box=GetTileBox(x,z);

	//准备深度图
	if( !CShadowMapBakingPool::GetInst()->AddShadowBuf(tileIndex,box) )
	{
		return ;
	}
	ShadowMapBuf tempBuf;
	if( !CShadowMapBakingPool::GetInst()->GetShadowBuf(tileIndex,tempBuf) )
	{
		return ;
	}
	
	if(tempBuf.Flag==FLAG_UPDATE)
	{	
		Ast(m_pShadowFilter!=NULL);
		m_pShadowFilter->SetShadowTexOut(tempBuf.ShadowTexture);
		m_pShadowFilter->SetMatrixBuildOut(tempBuf.MatrixBuild);	
		m_pShadowFilter->ResetBox();
		m_pShadowFilter->AddReceiverBox(tempBuf.Box);
		m_pShadowFilter->EnableRenderOut();
		//相机切换
		CRenderGroup* tempRenderGroup=CMainWindowTarget::GetInst()->GetGroup();
		CCamera* oldCamera = tempRenderGroup->GetCamera();
		ChangeCamera(x,z);
		//渲染
		RenderTile(x,z);
		//相机还原
		tempRenderGroup->SetCamera(oldCamera);
		//阴影过滤器设置和还原
		m_pShadowFilter->DisableRenderOut();
		m_pShadowFilter->ResetBox();	

		CShadowMapBakingPool::GetInst()->SetShadowMapBufFlag(tileIndex,FLAG_USE);

		//string namestr;
		//char buf[4];
		//namestr="e:/Depth";
		//itoa(x,buf,10);
		//namestr.append(buf);
		//namestr+="_";
		//itoa(z,buf,10);
		//namestr.append(buf);
		//namestr+=".dds";
		//tempBuf.ShadowTexture->SaveToFile(namestr);
	}
}

void CLightMapBaker::OnProcess(index_t x, index_t z)
{
	CShadowMapBakingPool::GetInst()->ResetShadowBufFlag();
	//过滤器还原
	CRenderGroup* tempRenderGroup=CMainWindowTarget::GetInst()->GetGroup();
	tempRenderGroup->SetShadowType(oldShadowType);
	m_pShadowFilter=NULL;
	CBakerBase::OnProcess(x,z);
}

void  CLightMapBaker::ChangeCamera(index_t x, index_t z)
{
	int tileIndex=x+z*tileWidth;

	CRenderGroup* tempRenderGroup=CMainWindowTarget::GetInst()->GetGroup();
	LightMapCamera->SetOrthoProject(true);
	LightMapCamera->SetOrthoSize(float(LIGHT_MAP_SIZE_IN_PIXEL),float(LIGHT_MAP_SIZE_IN_PIXEL));
	CVector3f pos,target;
	CTerrainMesh* terrainMesh = CMapEditApp::GetInst()->GetTerrain();
	CAxisAlignedBox& aabb = GetTileBox(x,z);
	pos.x = LIGHT_MAP_SIZE_IN_PIXEL * (x + 0.5f);
	pos.z = LIGHT_MAP_SIZE_IN_PIXEL * (z + 0.5f);
	pos.y=  aabb.getMaximum().y+100;
	target=pos;
	target.y-=100;
	
	LightMapCamera->setFixedYawAxis(true,CVector3f(0,0,1));
	LightMapCamera->SetPosition(pos);
	LightMapCamera->lookAt(target);
	
	LightMapCamera->setNearDist(50);
	LightMapCamera->setFarDist( pos.y-aabb.getMinimum().y +100 );
	LightMapCamera->getUP();
	tempRenderGroup->SetCamera(LightMapCamera);
}

void CLightMapBaker::RenderTile(index_t x,index_t z)
{
	int width=10;
	int CurTerrainIndex=x+z*tileWidth;

	CTerrainMesh* terrainMesh = CMapEditApp::GetInst()->GetTerrain();
	DWORD		  dwGridCntInTileRegion = terrainMesh->GetGridCountInTileRegion();
	DWORD		  dwTileRegionCnt		= terrainMesh->GetTileRegionCount();

	STileRegion *pTileRegion = terrainMesh->GetTileRegion(CurTerrainIndex);
	if(!pTileRegion)
	{
		return ;
	}

	map<string, int> depathModelIndexs;
	map<string, int>::iterator depathModelIndexIter;

	for (DWORD i = 0; i < dwGridCntInTileRegion; ++i )
	{
		DWORD nGridIndex = pTileRegion->vGridIndices[i];
		const SGrid &grid = terrainMesh->GetGrid(nGridIndex);

		DepathModelInfoMap::iterator iter = terrainMesh->m_DepathModelMap.find(nGridIndex);
		if ( iter != terrainMesh->m_DepathModelMap.end() )
		{
			const DepathModelInfoArray& depathModelInfoList = iter->second;
			size_t depathModelSize = depathModelInfoList.size();
			for ( size_t mm = 0; mm < depathModelSize; ++mm )
			{
				const DepathModelInfoPtr& pDepathModelInfo = depathModelInfoList[mm];
				int nOldGridIndex = pDepathModelInfo->nOriginGridIndex;
				const SGrid &oldgrid = terrainMesh->GetGrid(nOldGridIndex);
				size_t gridModelNum = oldgrid.vecCenterObjects.size();
				if( pDepathModelInfo->nGridModelIndex < gridModelNum )
				{
					CTObject * p = oldgrid.vecCenterObjects[pDepathModelInfo->nGridModelIndex];
					CEditModelGroup *pModelGroup = NULL;
					pModelGroup = p->GetModelGroup();

					if( pModelGroup )
					{
						size_t nGroupIndex = pDepathModelInfo->nGroupModelIndex;
						if( nGroupIndex < pModelGroup->m_vecSelfPresentation.size() )
						{
							pModelGroup->m_vecSelfPresentation[nGroupIndex]->Render();
						}

					}
				}
			}
		}

		if ( !grid.vecCenterObjects.empty() )
		{
			int ngridIndex = grid.dwGridIndex;
			int ngridsetIndex = 0;

			for (vector< CTObjectPtr >::const_iterator iter= grid.vecCenterObjects.begin(); iter != grid.vecCenterObjects.end(); ++iter)
			{
				CTObject * p = (*iter);
				CEditModelGroup *pModelGroup = NULL;
				pModelGroup = p->GetModelGroup();

				///是复合图素就不处理，上面已处理过
				if ( pModelGroup->GetIsSubUnit() )
					continue;

				size_t size = pModelGroup->m_vecSelfPresentation.size();
				for( size_t n = 0; n < size; ++n )
				{
					pModelGroup->m_vecSelfPresentation[n]->Render();
				}
			}
		}
	}
	CGraphic::GetInst()->ClearStack();
	CGraphic::GetInst()->RenderGroup();
}

CAxisAlignedBox CLightMapBaker::GetTileBox(index_t x,index_t z)
{
	int terrainIndex=x+z*tileWidth;

	CTerrainMesh* terrainMesh = CMapEditApp::GetInst()->GetTerrain();
	DWORD		  dwGridCntInTileRegion = terrainMesh->GetGridCountInTileRegion();
	DWORD		  dwTileRegionCnt		= terrainMesh->GetTileRegionCount();

	CAxisAlignedBox& aabb = ((TerrainNode*)terrain->GetNode(URegionID(x,z)))->m_TAAB;
	CAxisAlignedBox tempAABB = aabb;

	STileRegion *pTileRegion = terrainMesh->GetTileRegion(terrainIndex);
	for (DWORD i = 0; i < dwGridCntInTileRegion; ++i )
	{
		DWORD nGridIndex = pTileRegion->vGridIndices[i];
		SGrid &grid = terrainMesh->GetGrid(nGridIndex);

			if ( !grid.vecCenterObjects.empty() )
			{
				int ngridIndex = grid.dwGridIndex;
				int ngridsetIndex = 0;

				for (vector< CTObjectPtr >::iterator iter= grid.vecCenterObjects.begin(); iter != grid.vecCenterObjects.end(); ++iter)
				{
					CTObject * p = (*iter);
					CEditModelGroup *pModelGroup = NULL;
					pModelGroup = p->GetModelGroup();
					vector<CMapEditObject*>::iterator MapEditObjIter;	
					MapEditObjIter=pModelGroup->m_vecSelfPresentation.begin();
					for(;MapEditObjIter!=pModelGroup->m_vecSelfPresentation.end();MapEditObjIter++)
					{
						tempAABB.mergeBox((*MapEditObjIter)->GetAABB());
					}
				}
			}
	}
	return tempAABB;
}



}
