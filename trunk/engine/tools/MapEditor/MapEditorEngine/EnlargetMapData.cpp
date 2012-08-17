//------------------------------------------------------------------------------
//  CEnlargetMapData.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "EnlargetMapData.h"
#include "GraphicCommon.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "Operator.h"
#include "Operator.inl"
#include "WaterOperator.h"
#include "TerrainMeshObject.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "Engine.h"
#include "Render.h"
#include "River.h"
#include "CMapEditModelObjPro.h"
#include "LocalVersion.h"
#include "SceneRegionMgr.h"
#include "SceneRegion.h"
#include "LoadMapMgr.h"
#include "TraceHelper.h"
#include "CEditDataScene.h"
#include "SyncSystem.h"
#include "CEditContext.h"



namespace sqr
{
	CEnlargetMapData::CEnlargetMapData()
	{
	}

	void  CEnlargetMapData::SaveAsEnlargeMapCoder(string strTitle, DWORD dwWidth, DWORD dwDepth)
	{
		this->ClearInfo();
		this->CreateTransformTerrainMesh(strTitle, dwWidth, dwDepth);

		if( !CLoadSavedMapMgr::HasInst() )
			CLoadSavedMapMgr::Create();

		CLoadSavedMapMgr::GetInst()->SetProcessCallback(progress);
		CLoadSavedMapMgr::GetInst()->Save();

		///delete terrain and water
		CTerrainMesh *pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetTerrain();
		if( pTerrain )
		{
			CTerrainMesh *pWater = NULL;
			pWater = pTerrain->GetWater();
			if( pWater )
			{
				delete pWater;
				pWater = NULL;
			}

			delete pTerrain;
			pTerrain = NULL;

			CMapEditApp::GetInst()->SetTerrainMesh(NULL);
		}
		CMapEditApp::GetInst()->SetTerrainMesh(terrain);
	}

	void CEnlargetMapData::CreateTransformTerrainMesh( string strTitle, DWORD dwWidth, DWORD dwDepth )
	{
		string strMapName("");
		size_t npos = strTitle.rfind("\\");
		if( npos != -1 )
			strMapName = strTitle.substr(npos+1, strTitle.length());
		npos = strMapName.rfind(".");
		if( npos != -1 )
			strMapName = strMapName.substr(0, npos);

		if( strMapName.empty() )
		{
			MessageBox(NULL,"需要切割的地图名为空，请重新选择","提示",MB_OK);
			return;
		}

		// rgn mask
		int   newGridCnt = 0, SceneLightCount = 0;
		DWORD newVertexCnt = 0;

		DWORD dwMapWidth = dwWidth * REGION_SPACE;
		DWORD dwMapDepth = dwDepth * REGION_SPACE;
		newGridCnt		 = dwMapWidth * dwMapDepth;
		newVertexCnt     = (dwMapWidth + 1) * (dwMapDepth + 1);

		//////////////////////////////////////////////////////////////////////////

		CMapEditApp::GetInst()->GetDataScene()->SetWidth( uint16(dwWidth) );
		CMapEditApp::GetInst()->GetDataScene()->SetHeight( uint16(dwDepth) );
		CMapEditApp::GetInst()->GetDataScene()->Create();
		CSyncSystem::BeginSyncFun();

		CMapEditApp::GetInst()->InitNewRenderScene();
		CMapEditApp::GetInst()->GetRenderScene()->InitSceneRes();

		CTerrainMesh *pTerrainNew = NULL;
		pTerrainNew = new CTerrainMesh; 
		pTerrainNew->Create(dwWidth,dwDepth,m_strDefaultTerrainTexName,false,false);
		if( pTerrainNew == NULL )
			return;
		CTerrainMesh * pWaterNew = new CTerrainMesh; 
		pWaterNew->Create(dwWidth,dwDepth,m_strDefaultWaterTexName,true,false);
		pTerrainNew->SetWater(pWaterNew);
		pTerrainNew->m_strMapName = strMapName;

		Ast(newVertexCnt == pTerrainNew->GetVertexCount());
		int nNewVertexIndex = 0, nOldVertexIndex = 0;
		DWORD dwNewGridIndex = 0, dwOldGridIndex = 0;

		//////////////////////////////////////////////////////////////////////////
		///顶点信息
		uint8 uReSizeMapType = terrain->GetResizeMapType();
		for ( DWORD n = 0; n < newVertexCnt; ++n )
		{
			DWORD dwVertexIndexOld = 0, oColor = VERTEX_COLOR, sColor = VERTEX_COLOR;
			float fPositionY = 0.0f;
			CVector3f vNormal(0.0, 1.0f, 0.0f);
			short sLogicHeight = 0;

			if ( terrain->IsOverlappingByVertexIndex(dwWidth, dwDepth, n, dwVertexIndexOld, uReSizeMapType) )
			{
				SVertex &VertexOld = terrain->GetVertex(dwVertexIndexOld);
				SVertex &VertexNew = pTerrainNew->GetVertex(n);

				VertexNew.oColor	   = VertexOld.oColor;
				VertexNew.sColor	   = VertexOld.sColor;
				VertexNew.vPosition.y  = VertexOld.vPosition.y;
				VertexNew.vNormal	   = VertexOld.vNormal;
				VertexNew.fLogicHeight = short(VertexOld.fLogicHeight);

				pWaterNew->GetVertex(n).vPosition.y = VertexNew.vPosition.y + WATER_LAYER_BASE;
			}
		}
		//////////////////////////////////////////////////////////////////////////

		// grids
		string strName = "";
		SGrid * pGridOld = NULL;
		float delta = 0.001f;
		float U[2][4] =
		{
			{delta, 0.5f, 0.5f, delta},
			{0.5f, 1.0f - delta, 1.0f - delta, 0.5f}
		};
		float V[2][4] =
		{
			{1.0f - delta, 1.0f - delta, 0.5f, 0.5f},
			{0.5f, 0.5f, delta, delta}
		};

		map<int, int>::iterator iiter;
		bool   bOldMapStart = true;
		int	   nOldSatrtGridInNewIndex = 0;
		for (int i = 0; i < newGridCnt; ++i )
		{
			int x = i % dwMapWidth;
			int z = i / dwMapWidth;
			int nX = x % 2;
			int nZ = z % 2;

			DWORD dwGridIndexOld = 0;
			int   nOldDivideRegionIndex = 0, nNewDivideRegionIndex = 0;

			SGrid * pGridNew = &pTerrainNew->GetGrid(i);

			pGridNew->dwGridIndex = i;
			if ( terrain->IsOverlappingByGridIndex(dwWidth, dwDepth, i, dwGridIndexOld, uReSizeMapType) )
			{
				if( bOldMapStart )
				{
					nOldSatrtGridInNewIndex = i;
					bOldMapStart = false;
				}

				pGridOld = &terrain->GetGrid(dwGridIndexOld);
				nOldDivideRegionIndex = pGridOld->m_nDivideRegionIndex;

				pGridNew->bGridHide = pGridOld->bGridHide;
				pGridNew->nBlockType = pGridOld->nBlockType;

				pGridNew->nodeInfo.strTextureNames[0] = pGridOld->nodeInfo.strTextureNames[0];
				pGridNew->nodeInfo.strTextureNames[1] = pGridOld->nodeInfo.strTextureNames[1];

				pGridNew->bCliffLayerOne = pGridOld->bCliffLayerOne;
				pGridNew->nMaterialIndex		  = pGridOld->nMaterialIndex;
				pGridNew->bMatNotEffectByTileSets = pGridOld->bMatNotEffectByTileSets;///强行设置该格子不受图素包设置影响，默认为受影响

				for (int m = 0; m < 4; ++m)
				{
					pGridNew->uv[0][m]		  = pGridOld->uv[0][m];
					pGridNew->uv[1][m]		  = pGridOld->uv[1][m];
					pGridNew->subGridTypes[m] = pGridOld->subGridTypes[m];
				}

				pGridNew->vCenter.y				= pGridOld->vCenter.y;
				pGridNew->vecCenterObjects		= pGridOld->vecCenterObjects;
				pGridNew->bEffectByVertexColor	= pGridOld->bEffectByVertexColor;

				this->mapRevertSplitMapGridIndex[dwGridIndexOld] = i;
				this->m_OldMapRegionIndexMap[nOldDivideRegionIndex] = pGridNew->m_nDivideRegionIndex;

				pGridNew->bSide					= pGridOld->bSide;

				for ( vector< CTObjectPtr >::iterator iter = pGridNew->vecCenterObjects.begin(); iter != pGridNew->vecCenterObjects.end(); ++iter)
				{
					CTObjectPtr p = (*iter);

					CEditModelGroup *pModelGroup = p->GetModelGroup();
					CMapEditObject *pMapEditObject = NULL;
					pMapEditObject = pModelGroup->GetSelfPresentation(0);

					int nOldModelCenterGridIndex = pMapEditObject->sModelSavePro.nCenterGridIndex;
					iiter = this->mapRevertSplitMapGridIndex.find(nOldModelCenterGridIndex);
					if( iiter != this->mapRevertSplitMapGridIndex.end() )
						pMapEditObject->sModelSavePro.nCenterGridIndex = iiter->second;
				}
			}
			else
			{
				pGridNew->nodeInfo.strTextureNames[0] = m_strDefaultTerrainTexName;
				pGridNew->nodeInfo.strTextureNames[1] = m_strDefaultTerrainTexName;

				for (int m = 0; m < 4; ++m)
				{
					pGridNew->uv[0][m].x = U[nX][m];
					pGridNew->uv[0][m].y = V[nZ][m];
				}
			}
		}

		vector<int> vecGridIndicesRiver = this->CreateRiverInfo(pWaterNew);
		CMapEditApp::GetInst()->SetTerrainMesh(pTerrainNew);
		pTerrainNew->InitTerrain((CEditTerrainMgr*)CMapEditApp::GetInst()->GetDataScene()->GetTerrainMgr());

		size_t size = vecGridIndicesRiver.size();
		for (size_t i = 0; i < size; ++i )
		{
			pWaterNew->AdjustWaterMeshVisibility(vecGridIndicesRiver[i],true);
		}

		this->ChangeRegionLightInfo();
	}

	void CEnlargetMapData::ClearInfo()
	{
		m_OldMapRegionIndexMap.clear();
		mapRevertSplitMapGridIndex.clear();
	}

	vector<int> CEnlargetMapData::CreateRiverInfo( CTerrainMesh *pTransformWater )
	{
		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
		int newGridCnt = pTransformWater->GetGridCount();
		vector<int> vecGridIndicesRiver;
		//////////////////////////////////////////////////////////////////////////
		// water
		CTerrainMesh * pWater = terrain->GetWater();
		set<int> setGridIndicesVisibleWater = pWater->GetGridIndicesVisibleWater();
		map< CRiver*, vector<int> > mapRivers;
		map<int, int>::iterator splitRevertGriditer;

		for ( set<int>::iterator iter = setGridIndicesVisibleWater.begin(); iter != setGridIndicesVisibleWater.end(); )
		{
			int nOldGridIndex = *iter;
			SGrid & gridW = pWater->GetGrid(nOldGridIndex);

			++iter;

			Ast(gridW.bVisible);
			if ( gridW.pRiver )
			{
				setGridIndicesVisibleWater.erase(nOldGridIndex);

				mapRivers[gridW.pRiver].push_back(nOldGridIndex);
			}
		}

		// visible and occupied
		for ( map< CRiver*, vector<int> >::iterator iterMapRivers = mapRivers.begin(); iterMapRivers != mapRivers.end(); ++iterMapRivers )
		{
			CRiver * pRiver = iterMapRivers->first;

			string strItemName = pRiver->GetItemName();
			CMapEditApp::GetInst()->GetWaterOperator()->RebirthRiver(strItemName);

			for (size_t i = 0; i < iterMapRivers->second.size(); ++i  )
			{
				int nNewGridIndex = 0;///切割后格子索引
				int nOldGridIndex = iterMapRivers->second[i];///切割前格子索引

				///得到被切割地图对应索引，将该格子得到，取得水的高度值
				splitRevertGriditer = this->mapRevertSplitMapGridIndex.find(nOldGridIndex);
				if( splitRevertGriditer != this->mapRevertSplitMapGridIndex.end() )
					nNewGridIndex = splitRevertGriditer->second;
				else
				{
					nNewGridIndex = 0;
					continue;
				}

				vecGridIndicesRiver.push_back(nNewGridIndex);

				SGrid &gridW          = pWater->GetGrid(nOldGridIndex);
				SGrid &gridTransformW = pTransformWater->GetGrid(nNewGridIndex);
				gridTransformW.pRiver = pRiver;
				gridTransformW.vCenter.y = gridW.vCenter.y;

				for ( int j = 0; j < 4; ++j )
				{
					int nVertexIndex = gridW.dwVertexIndex[j];
					SVertex &vertex = pWater->GetVertex(nVertexIndex);

					int nTransformVertexIndex = gridTransformW.dwVertexIndex[j];
					SVertex &Transformvertex  = pTransformWater->GetVertex(nTransformVertexIndex);

					Transformvertex.vPosition.y = vertex.vPosition.y;
					Transformvertex.oColor		= vertex.oColor;
				}
			}
		}

		set<int>::iterator end = setGridIndicesVisibleWater.end();
		int nOldGridIndex = 0, nNewGridIndex = 0;
		for ( set<int>::iterator beg = setGridIndicesVisibleWater.begin(); beg != end; ++beg )
		{
			nOldGridIndex = *beg;
			splitRevertGriditer = this->mapRevertSplitMapGridIndex.find(nOldGridIndex);
			if( splitRevertGriditer != this->mapRevertSplitMapGridIndex.end() )
			{
				nNewGridIndex = splitRevertGriditer->second;
				if( nNewGridIndex < newGridCnt )
					setGridIndicesVisibleWater.erase(nOldGridIndex);
			}
		}

		// unoccupied but visible
		size_t nCnt = setGridIndicesVisibleWater.size();
		for ( set<int>::iterator iter = setGridIndicesVisibleWater.begin(); iter != setGridIndicesVisibleWater.end(); ++iter )
		{
			int nNewGridIndex = 0;///切割后格子索引
			int nOldGridIndex = *iter;///切割前格子索引

			///得到被切割地图对应索引，将该格子得到，取得水的高度值
			splitRevertGriditer = this->mapRevertSplitMapGridIndex.find(nOldGridIndex);
			if( splitRevertGriditer != this->mapRevertSplitMapGridIndex.end() )
				nNewGridIndex = splitRevertGriditer->second;
			else
			{
				nNewGridIndex = 0;
				continue;
			}

			vecGridIndicesRiver.push_back(nNewGridIndex);

			SGrid & gridW		  = pWater->GetGrid(nOldGridIndex);
			SGrid &gridTransformW = pTransformWater->GetGrid(nNewGridIndex);
			gridTransformW.vCenter.y = gridW.vCenter.y;

			for ( int j = 0; j < 4; ++j )
			{
				int nVertexIndex = gridW.dwVertexIndex[j];
				SVertex & vertex = pWater->GetVertex(nVertexIndex);

				int nTransformVertexIndex = gridTransformW.dwVertexIndex[j];
				SVertex & Transformvertex = pTransformWater->GetVertex(nTransformVertexIndex);

				Transformvertex.vPosition.y = vertex.vPosition.y;
				Transformvertex.oColor		= vertex.oColor;
			}
		}

		return vecGridIndicesRiver;
	}

	void CEnlargetMapData::ChangeRegionLightInfo()
	{
		map<int, int>::iterator iter;
		map<string, SceneRegion*>::iterator beg;
		for ( beg = CSceneRegionMgr::GetInst()->m_mapRegion.begin(); beg != CSceneRegionMgr::GetInst()->m_mapRegion.end(); ++beg )
		{
			SceneRegion *sceneRegion = beg->second;

			set<int> newRegionUnitList;
			int rgnUnit = 0;
			for ( set<int>::iterator unit = sceneRegion->_RegionUnitList.begin(); unit != sceneRegion->_RegionUnitList.end(); ++unit )
			{
				rgnUnit = *unit;

				iter = this->m_OldMapRegionIndexMap.find(rgnUnit);
				if( iter != this->m_OldMapRegionIndexMap.end() )
					rgnUnit = iter->second;

				newRegionUnitList.insert(rgnUnit);
			}

			sceneRegion->_RegionUnitList.clear();
			sceneRegion->_RegionUnitList = newRegionUnitList;
		}
	}

	void CEnlargetMapData::SetTerrainMesh( CTerrainMesh* t )
	{
		Ast(NULL != t);
		this->terrain = t;
		m_strDefaultTerrainTexName = terrain->GetTerrainDefaultTextureName();
		size_t npos = m_strDefaultTerrainTexName.rfind("\\");
		m_strDefaultWaterTexName = m_strDefaultTerrainTexName.substr(0, npos+1);
		m_strDefaultWaterTexName = m_strDefaultWaterTexName + "water.dds";
	}

	//新增加的拓展地形的方法
	void CEnlargetMapData::NewSaveAsEnlargeMapCoder(DWORD dwWidth, DWORD dwDepth,int offW,int offL)
	{
		this->ClearInfo();
		CTerrainMesh *newTMesh=this->ResizeTerrainMesh(dwWidth,dwDepth,offW,offL);
		//if( !CLoadSavedMapMgr::HasInst() )
		//	CLoadSavedMapMgr::Create();
		//CLoadSavedMapMgr::GetInst()->SetProcessCallback(progress);
		//CLoadSavedMapMgr::GetInst()->Save();
		
		///删除旧的网格
		if( terrain )
		{
			CTerrainMesh *pWater = terrain->GetWater();
			if( pWater )
			{
				delete pWater;
				pWater = NULL;
			}
			delete terrain;
			terrain = newTMesh;
		}
	}

	CTerrainMesh* CEnlargetMapData::ResizeTerrainMesh(DWORD dwWidth, DWORD dwDepth,int offW,int offL)
	{
		//string strMapName("");
		//size_t npos = strTitle.rfind("\\");
		//if( npos != -1 )
		//	strMapName = strTitle.substr(npos+1, strTitle.length());
		//npos = strMapName.rfind(".");
		//if( npos != -1 )
		//	strMapName = strMapName.substr(0, npos);

		//if( strMapName.empty() )
		//{
		//	MessageBox(NULL,"需要切割的地图名为空，请重新选择","提示",MB_OK);
		//	return;
		//}

		// 		
		//旧Mesh尺寸
		DWORD oldTWidth = terrain->GetWidth();
		DWORD oldTDepth = terrain->GetDepth();
		//新Mesh尺寸
		DWORD dwMapWidth = dwWidth * REGION_SPACE;
		DWORD dwMapDepth = dwDepth * REGION_SPACE;
		DWORD newGridCnt		 = dwMapWidth * dwMapDepth;
		DWORD newVertexCnt     = (dwMapWidth + 1) * (dwMapDepth + 1);
		//创建新的Mesh
		CTerrainMesh *pTerrainNew = NULL;
		pTerrainNew = new CTerrainMesh; 
		pTerrainNew->Create(dwWidth,dwDepth,m_strDefaultTerrainTexName,false,false);
		if( pTerrainNew == NULL )
			return NULL;
		CTerrainMesh * pWaterNew = new CTerrainMesh; 
		pWaterNew->Create(dwWidth,dwDepth,m_strDefaultWaterTexName,true,false);
		pTerrainNew->SetWater(pWaterNew);
		pTerrainNew->InitTerrain((CEditTerrainMgr*)CMapEditApp::GetInst()->GetDataScene()->GetTerrainMgr());

		Ast(newVertexCnt == pTerrainNew->GetVertexCount());
		int nNewVertexIndex = 0, nOldVertexIndex = 0;
		DWORD dwNewGridIndex = 0, dwOldGridIndex = 0;

		//旧Mesh在新Mesh下影射的区域
		int oldMinX=offW*REGION_SPACE;
		int oldMaxX=offW*REGION_SPACE+oldTWidth;
		int oldMinZ=offL*REGION_SPACE;
		int oldMaxZ=offL*REGION_SPACE+oldTDepth;

		//替换顶点信息
		for ( DWORD n = 0; n < newVertexCnt; ++n )
		{
			DWORD dwVertexIndexOld = 0, oColor = VERTEX_COLOR, sColor = VERTEX_COLOR;
			float fPositionY = 0.0f;
			CVector3f vNormal(0.0, 1.0f, 0.0f);
			short sLogicHeight = 0;

			int VertexX=n%(dwMapWidth + 1);
			int VertexZ=n/(dwMapWidth + 1);

			if(VertexX>=oldMinX && oldMinX<=oldMaxX && VertexZ>=oldMinZ && VertexZ<=oldMaxZ)
			{
				dwVertexIndexOld=(VertexX-offW*REGION_SPACE)+(VertexZ-offL*REGION_SPACE)*oldTWidth;
				SVertex &VertexOld = terrain->GetVertex(dwVertexIndexOld);
				SVertex &VertexNew = pTerrainNew->GetVertex(n);

				VertexNew.oColor	   = VertexOld.oColor;
				VertexNew.sColor	   = VertexOld.sColor;
				VertexNew.vPosition.y  = VertexOld.vPosition.y;
				VertexNew.vNormal	   = VertexOld.vNormal;
				VertexNew.fLogicHeight = short(VertexOld.fLogicHeight);
				pWaterNew->GetVertex(n).vPosition.y = VertexNew.vPosition.y + WATER_LAYER_BASE;
			}
		}
		//替换表格信息
		//纹理坐标
		float delta = 0.001f;
		float U[2][4] =
		{
			{delta, 0.5f, 0.5f, delta},
			{0.5f, 1.0f - delta, 1.0f - delta, 0.5f}
		};
		float V[2][4] =
		{
			{1.0f - delta, 1.0f - delta, 0.5f, 0.5f},
			{0.5f, 0.5f, delta, delta}
		};
		//map<int, int>::iterator iiter;
		for (DWORD i = 0; i < newGridCnt; ++i )
		{
			int x = i % dwMapWidth;
			int z = i / dwMapWidth;
			int nX = x % 2;
			int nZ = z % 2;
			DWORD dwGridIndexOld = 0;
			int   nOldDivideRegionIndex = 0, nNewDivideRegionIndex = 0;
			SGrid * pGridNew = &pTerrainNew->GetGrid(i);
			pGridNew->dwGridIndex = i;

			int tmpGirdX=x-offW*REGION_SPACE;
			int tmpGridZ=z-offL*REGION_SPACE;

			if(tmpGirdX<(int)oldTWidth && tmpGirdX>=0 && tmpGridZ>=0 && tmpGridZ<(int)oldTDepth)
			{
				dwGridIndexOld=tmpGirdX+tmpGridZ*oldTWidth;

				SGrid* pGridOld = &terrain->GetGrid(dwGridIndexOld);
				nOldDivideRegionIndex = pGridOld->m_nDivideRegionIndex;
				pGridNew->bGridHide = pGridOld->bGridHide;
				pGridNew->nBlockType = pGridOld->nBlockType;
				pGridNew->nodeInfo.strTextureNames[0] = pGridOld->nodeInfo.strTextureNames[0];
				pGridNew->nodeInfo.strTextureNames[1] = pGridOld->nodeInfo.strTextureNames[1];
				pGridNew->bCliffLayerOne = pGridOld->bCliffLayerOne;
				pGridNew->nMaterialIndex		  = pGridOld->nMaterialIndex;
				pGridNew->bMatNotEffectByTileSets = pGridOld->bMatNotEffectByTileSets;///强行设置该格子不受图素包设置影响，默认为受影响
				for (int m = 0; m < 4; ++m)
				{
					pGridNew->uv[0][m]		  = pGridOld->uv[0][m];
					pGridNew->uv[1][m]		  = pGridOld->uv[1][m];
					pGridNew->subGridTypes[m] = pGridOld->subGridTypes[m];
				}
				pGridNew->vCenter.y				= pGridOld->vCenter.y;
				pGridNew->vecCenterObjects		= pGridOld->vecCenterObjects;
				pGridNew->bEffectByVertexColor	= pGridOld->bEffectByVertexColor;
				this->mapRevertSplitMapGridIndex[dwGridIndexOld] = i;
				this->m_OldMapRegionIndexMap[nOldDivideRegionIndex] = pGridNew->m_nDivideRegionIndex;
				pGridNew->bSide					= pGridOld->bSide;

				//模型信息
				//for ( vector< CTObjectPtr >::iterator iter = pGridNew->vecCenterObjects.begin(); iter != pGridNew->vecCenterObjects.end(); ++iter)
				//{
				//	CTObjectPtr p = (*iter);

				//	CEditModelGroup *pModelGroup = p->GetModelGroup();
				//	CMapEditObject *pMapEditObject = NULL;
				//	pMapEditObject = pModelGroup->GetSelfPresentation(0);

				//	int nOldModelCenterGridIndex = pMapEditObject->sModelSavePro.nCenterGridIndex;
				//	iiter = this->mapRevertSplitMapGridIndex.find(nOldModelCenterGridIndex);
				//	if( iiter != this->mapRevertSplitMapGridIndex.end() )
				//		pMapEditObject->sModelSavePro.nCenterGridIndex = iiter->second;
				//}
			}
			else
			{
				pGridNew->nodeInfo.strTextureNames[0] = m_strDefaultTerrainTexName;
				pGridNew->nodeInfo.strTextureNames[1] = m_strDefaultTerrainTexName;

				for (int m = 0; m < 4; ++m)
				{
					pGridNew->uv[0][m].x = U[nX][m];
					pGridNew->uv[0][m].y = V[nZ][m];
				}
			}
		}

		//水面信息
		//vector<int> vecGridIndicesRiver = this->CreateRiverInfo(pWaterNew);
		//pTerrainNew->InitTerrain((CEditTerrainMgr*)CMapEditApp::GetInst()->GetDataScene()->GetTerrainMgr());
		//size_t size = vecGridIndicesRiver.size();
		//for (size_t i = 0; i < size; ++i )
		//{
		//	pWaterNew->AdjustWaterMeshVisibility(vecGridIndicesRiver[i],true);
		//}
		//区域光
		//this->ChangeRegionLightInfo();

		return pTerrainNew;
	}

//!namespace
}