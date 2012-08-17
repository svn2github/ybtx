//------------------------------------------------------------------------------
//  CMapBaker.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "MapBaker.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "Render.h"
#include "Engine.h"
#include "CMapEditCamera.inl"
#include "CCameraManager.h"
#include "CGraphic.h"
#include "CLightMapFilter.h"
#include "CEditTerrainMgr.h"
#include "CTerrain.h"
#include "TerrainBaker.h"
#include "LogicalHeightBaker.h"

namespace sqr
{
	CMapBaker::CMapBaker()
			: terrainMesh(NULL)
			, camera(NULL)
			, renderGroup(NULL)
			, progress(NULL)
			, zOffset(0)
			, terrainOffset(0)
	{
	}

	CMapBaker::~CMapBaker()
	{
	}

	//------------------------------------------------------------------------------
	void
	CMapBaker::Setup()
	{
		this->terrainMesh = CMapEditApp::GetInst()->GetTerrain();
		this->camera	  = CCameraManager::GetInst()->GetMapEditCamera();
		this->renderGroup = CMainWindowTarget::GetInst()->GetGroup();
	}

	//------------------------------------------------------------------------------
	void
	CMapBaker::Discard()
	{
		this->terrainMesh = NULL;
		this->camera = NULL;
		this->renderGroup = NULL;

		this->progress = NULL;
	}

	//------------------------------------------------------------------------------
	void
	CMapBaker::SetProcessor( )
	{
		int tt = 0;
	}

	//------------------------------------------------------------------------------
	void CMapBaker::SetBakerMgr(CBakerMgrPtr bakermgr)
	{
		Ast(bakermgr!=NULL);
		BakerMgr=bakermgr;
	}

	//------------------------------------------------------------------------------
	void
	CMapBaker::Generate()
	{
		SQR_TRY
		{	
			Ast(BakerMgr!=NULL);
			CEditTerrainMgr* terrain = this->terrainMesh->m_pTerrain;
			int width = this->terrainMesh->GetWidthInRegion() / TILE_WIDTH_IN_REGION;
			int depth = this->terrainMesh->GetDepthInRegion() / TILE_WIDTH_IN_REGION;

			///’“≥ˆ∏¥∫œÕºÀÿ
			this->terrainMesh->DepathMapGroupModel();
			
			BakerMgr->SetTileSize(width,depth);
			BakerMgr->Setup();
		
			for (int j = 0; j < depth; ++j)
			{
				for (int i = 0; i < width; ++i)
				{
					BakerMgr->OnProcess(i,j);
					float cur = float(j*width+i + 1) / float(width*depth);
					this->progress->SetProgress(cur);
				}
			}
			
			BakerMgr->Discard();
			BakerMgr=NULL;
		}
		SQR_CATCH(exp)
		{
			string str = exp.ErrorMsg();
			str += "∫Ê≈‡…˙≥… ß∞‹";
			MessageBox(NULL, str.c_str(), "¥ÌŒÛ", MB_OK);

			return;
		}
		SQR_TRY_END;
	}


	//------------------------------------------------------------------------------
	void
	CMapBaker::GenerateByTileRegions(set<int> regions)
	{
		SQR_TRY
		{
			Ast(BakerMgr!=NULL);
			CEditTerrainMgr* terrain = this->terrainMesh->m_pTerrain;
			int width = this->terrainMesh->GetWidthInRegion() / TILE_WIDTH_IN_REGION;
			int depth = this->terrainMesh->GetDepthInRegion() / TILE_WIDTH_IN_REGION;
			int Count = width * depth;
			int num   = 0;

			CRenderGroup* tempRenderGroup=CMainWindowTarget::GetInst()->GetGroup();
			CCamera* oldCamera = tempRenderGroup->GetCamera();
			EShadowType oldShadowType = tempRenderGroup->GetShadowType();

			///’“≥ˆ∏¥∫œÕºÀÿ
			this->terrainMesh->DepathMapGroupModel();

			BakerMgr->SetTileSize(width,depth);
			BakerMgr->Setup();

			set<int>::iterator beg = regions.begin();
			set<int>::iterator end = regions.end();
			for ( beg; beg != end; ++beg  )
			{
				int bufferIndex = *beg;
				int	j = bufferIndex % width;
				int	i = bufferIndex / width;

				BakerMgr->OnProcess(i,j);
				float cur = float(++num) / float(Count);
				this->progress->SetProgress(cur);
			}

			BakerMgr->Discard();
			BakerMgr=NULL;

			CCamera* changeCamera = tempRenderGroup->GetCamera();
			tempRenderGroup->SetCamera(oldCamera);
		}
		SQR_CATCH(exp)
		{
			string str = exp.ErrorMsg();
			str += "∫Ê≈‡…˙≥… ß∞‹";
			MessageBox(NULL, str.c_str(), "¥ÌŒÛ", MB_OK);

			return;
		}
		SQR_TRY_END;
	}
}
