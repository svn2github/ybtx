//------------------------------------------------------------------------------
//  CChangeMoveMapData.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "ChangeMoveMapData.h"
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
#include "TraceHelper.h"
#include "CMapEditModelObjPro.h"
#include "LocalVersion.h"
#include "SceneAudioMgr.h"
#include "SceneAudio.h"
#include "SceneRegionMgr.h"
#include "SceneRegion.h"
#include "LoadMapMgr.h"

namespace sqr
{
	CChangeMoveMapData::CChangeMoveMapData()
	{
		m_nMapXMoveOffset = m_nMapZMoveOffset = 0;
	}

	void CChangeMoveMapData::MoveAsMap( const int nStartReginX, const int nStartReginZ, const int nOffsetReginWidth, const int nOffsetReginDepth )
	{
		this->ClearInfo();

		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();

		int nGridCount = terrain->GetGridCount();
		int nVertexCount = terrain->GetVertexCount();
		int nMapWidth  = terrain->GetWidth();
		int nNewGridIndex = 0, nOldGridIndex = 0;//nOldGridIndex为原始地表格子索引，nNewGridIndex为偏移后地表格子索引
		int nOldVertexIndex = 0, nNewVertexIndex = 0;
		int nXOffsetGridCount = nOffsetReginWidth * REGION_SPACE;
		int nZOffsetGridCount = nOffsetReginDepth * REGION_SPACE;
		int nStartRegionGridStartX   = nStartReginX * REGION_SPACE;
		int nStartRegionGridStartZ   = nStartReginZ * REGION_SPACE;
		map<int, SGrid> mapTempTerrainGridInfo;///将所有老格子上的信息传先暂时保存下来
		map<int, SVertex> mapTempTerrainVertexInfo;///将所有老顶点上的信息传先暂时保存下来
		map<int, SGrid> mapTempWaterGridInfo;///将所有老格子上的信息传先暂时保存下来
		map<int, SVertex> mapTempWaterVertexInfo;///将所有老顶点上的信息传先暂时保存下来

		CTerrainMesh *pWater = terrain->GetWater();

		for( int m = 0; m < nZOffsetGridCount; ++m )
		{
			int nOldDepth = nStartRegionGridStartZ + m;
			int nNewDepth = nStartRegionGridStartZ + m + m_nMapZMoveOffset;
			for ( int n = 0; n < nXOffsetGridCount; ++n )
			{
				int nOldGridIndex = n + nStartRegionGridStartX + nOldDepth * nMapWidth;
				int nNewGridIndex = n + nStartRegionGridStartX + m_nMapXMoveOffset + nNewDepth * nMapWidth;

				//DebugOut("old %d, new %d\n", nOldGridIndex, nNewGridIndex);
				mapOldMapGridIndex[nOldGridIndex] = nNewGridIndex;
				
				if( terrain->IsValid(nOldGridIndex) && terrain->IsValid(nNewGridIndex) )
				{
					SGrid OldTerrainGrid = terrain->GetGrid(nOldGridIndex);
					SGrid OldWaterGrid   = pWater->GetGrid(nOldGridIndex);
					SGrid *pNewGrid      = &terrain->GetGrid(nNewGridIndex);

					mapTempTerrainGridInfo[nOldGridIndex] = OldTerrainGrid;
					mapTempWaterGridInfo[nOldGridIndex]   = OldWaterGrid;
					
					for( int i = 0; i < 4; ++i )
					{
						nOldVertexIndex = OldTerrainGrid.dwVertexIndex[i];
						nNewVertexIndex = pNewGrid->dwVertexIndex[i];
						mapOldMapVertexIndex[nOldVertexIndex] = nNewVertexIndex;

						SVertex TerrainVertex = terrain->GetVertex(nOldVertexIndex);
						mapTempTerrainVertexInfo[nOldVertexIndex] = TerrainVertex;
						SVertex WaterVertex = pWater->GetVertex(nOldVertexIndex);
						mapTempWaterVertexInfo[nOldVertexIndex] = WaterVertex;
					}
				}
			}
		}

		map<int, SVertex>::iterator vertexiter;
		map<int, int>::iterator vertexbeg = mapOldMapVertexIndex.begin();
		map<int, int>::iterator vertexend = mapOldMapVertexIndex.end();
		set<int> newHasMoveVertexIndexs;//记录哪些顶点移动了，移动后新顶点的索引
		for ( vertexbeg; vertexbeg != vertexend; ++vertexbeg )
		{
			nOldVertexIndex = vertexbeg->first;
			nNewVertexIndex = vertexbeg->second;
			newHasMoveVertexIndexs.insert(nNewVertexIndex);

			vertexiter = mapTempTerrainVertexInfo.find(nOldVertexIndex);
			if( vertexiter != mapTempTerrainVertexInfo.end() )
			{
				if (terrain->IsValidVertexIndex(nNewVertexIndex))
				{
					SVertex Vertex = vertexiter->second;
					SVertex &newvertex = terrain->GetVertex(nNewVertexIndex);

					newvertex.vPosition.y	 = Vertex.vPosition.y;
					newvertex.oColor	     = Vertex.oColor;
					newvertex.sColor		 = Vertex.sColor;
					newvertex.vNormal		 = Vertex.vNormal;
					newvertex.fLogicHeight   = Vertex.fLogicHeight;

					pWater->GetVertex(nNewVertexIndex).vPosition.y = newvertex.vPosition.y + WATER_LAYER_BASE;

					if( newHasMoveVertexIndexs.find(nOldVertexIndex) == newHasMoveVertexIndexs.end() )
					{
						SVertex &oldvertex = terrain->GetVertex(nOldVertexIndex);
						oldvertex.vPosition.y	 = 0.0f;
						oldvertex.oColor		 = VERTEX_COLOR;
						oldvertex.sColor		 = 0;
						oldvertex.vNormal		 = CVector3f(0.0f,1.0f,0.0f);
						oldvertex.fLogicHeight	 = 0.0f;
						pWater->GetVertex(nOldVertexIndex).vPosition.y = WATER_LAYER_BASE;
					}
				}
			}
		}

		string strDefaultTerrainTexName = terrain->GetTerrainDefaultTextureName();

		map<int, int>::iterator beg = mapOldMapGridIndex.begin();
		map<int, int>::iterator end = mapOldMapGridIndex.end();
		map<int, SGrid>::iterator iter;
		set<int> newHasMoveGridIndexs;//记录哪些格子移动了，移动后新格子的索引
		set<int>::iterator iiter;
		for ( beg; beg != end; ++beg )
		{
			nOldGridIndex = beg->first;
			nNewGridIndex = beg->second;
			newHasMoveGridIndexs.insert(nNewGridIndex);

			iter = mapTempTerrainGridInfo.find(nOldGridIndex);
			if( iter != mapTempTerrainGridInfo.end() )
			{
				if( terrain->IsValid(nNewGridIndex) )
				{
					SGrid oldGrid = iter->second;
					SGrid &grid = terrain->GetGrid(nNewGridIndex);
					grid.ClearGridInfo();
					{
						grid.pRiver				  = oldGrid.pRiver;
						grid.pWave				  = oldGrid.pWave;
						grid.vecCenterObjects	  = oldGrid.vecCenterObjects;
	
						grid.bReverse			  = oldGrid.bReverse;
						grid.bEffectByVertexColor = oldGrid.bEffectByVertexColor;
						grid.bCliffLayerOne		  = oldGrid.bCliffLayerOne;
						grid.bGridHide			  = oldGrid.bGridHide;
						grid.nGridSeekRoadWeight  = oldGrid.nGridSeekRoadWeight;
						grid.m_bSetExactBlock	  = oldGrid.m_bSetExactBlock;
						grid.nMaterialIndex		  = oldGrid.nMaterialIndex;
						grid.nBlockType			  = oldGrid.nBlockType;

						grid.bMatNotEffectByTileSets = oldGrid.bMatNotEffectByTileSets;///强行设置该格子不受图素包设置影响，默认为受影响

						grid.nMaterialIndex			 = oldGrid.nMaterialIndex;
						grid.bSide					 = oldGrid.bSide;

						for ( int i = 0; i < 4; ++i )
						{
							grid.subGridTypes[i] = oldGrid.subGridTypes[i];
							grid.nodeInfo.strTextureNames[i] = oldGrid.nodeInfo.strTextureNames[i];
						}

						grid.vCenter.y = oldGrid.vCenter.y;
						grid.pRiver = oldGrid.pRiver;

						for ( vector< CTObjectPtr >::iterator iter = oldGrid.vecCenterObjects.begin(); iter != oldGrid.vecCenterObjects.end(); ++iter )
						{
							CEditModelGroup *pModelGroup = NULL;
							pModelGroup = (*iter)->GetModelGroup();

							string strEffectName = "";
							string strCueName = pModelGroup->GetSceneCueName();
							string strItemName = pModelGroup->GetItemName();
							bool bEffectBased = pModelGroup->GetIsEffectBased();
							bool bEffecIsExit = pModelGroup->GetEffectName(strEffectName);
							int nAtten = pModelGroup->GetAtten();
							int nRatio = pModelGroup->GetAffectRatio();
							bool bPointLight = false, bSpotLight = false;

							if (pModelGroup->m_SL.m_bIAmbScenePointLight)
								bPointLight = true;

							if (pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight)
								bSpotLight = true;

							///一段时间的播放长度最小最大值
							DWORD dwPlayLenMin = pModelGroup->GetPlayTimeLenMin();
							DWORD dwPlayLenMax = pModelGroup->GetPlayTimeLenMax();

							///二段播放间隔长度最小最大值
							DWORD dwPlayIntervalMin = pModelGroup->GetPlayIntervalMin();
							DWORD dwPlayIntervalMax = pModelGroup->GetPlayIntervalMax();

							CMapEditObject *pMapEditObject = NULL;
							pMapEditObject = pModelGroup->GetSelfPresentation(0);
							if( pModelGroup == NULL )
							{
								MessageBox(NULL,"没有模型，不保存","",MB_OK);
								return;
							}

							MoldeSaveProperty sModelSavePro = pMapEditObject->sModelSavePro;
							sModelSavePro.nCenterGridIndex = nNewGridIndex;

							if ( bEffectBased && bEffecIsExit )
							{
								pOperator->RebirthIndividualEffect(strEffectName, sModelSavePro.nCenterGridIndex, sModelSavePro.vRotate.y, sModelSavePro.nRotationCnt,
									sModelSavePro.vRotate.z, sModelSavePro.nPitchCnt, sModelSavePro.vRotate.x, sModelSavePro.nYawCnt, sModelSavePro.vScale.x, sModelSavePro.vScale.y, sModelSavePro.vScale.z, &sModelSavePro.vUserOffsetEx, sModelSavePro.vPosition, 0, NULL, 0);
				
							}
							else // effect
							{
								if( strCueName != "" )
								{
									EAUTO_GENERATION_TYPE eAutoGenerationType = EAGT_NONE;
									BYTE byRenderStyle = 0;
									bool bShadow = true;
									bool bReceiveShadow = false;
									BYTE bTransState = 0;
									int nMirrorType = 0, nRatio = 0;
									bool bTrans = true;
									CVector3f vUserOffsetEx2 = CVector3f(sModelSavePro.vUserOffsetEx.y,sModelSavePro.vUserOffsetEx.x,sModelSavePro.vUserOffsetEx.z);

									pOperator->Rebirth(strItemName, sModelSavePro.nCenterGridIndex, sModelSavePro.vRotate.y, sModelSavePro.nRotationCnt,
										sModelSavePro.vRotate.z, sModelSavePro.nPitchCnt, sModelSavePro.vRotate.x, sModelSavePro.nYawCnt, sModelSavePro.vScale.x, sModelSavePro.vScale.y, sModelSavePro.vScale.z, &vUserOffsetEx2, 0, NULL, 0,0,1.0f,
										eAutoGenerationType,byRenderStyle,nAtten,false,bShadow,bTrans,bTransState,nMirrorType,nRatio, "", strItemName, strCueName);
								}
								else
									pOperator->Rebirth(&sModelSavePro, strItemName, "", nRatio, strItemName, strCueName);
							}

							size_t size = grid.vecCenterObjects.size();
							CEditModelGroup *pNewModelGroup = NULL;
							CTObjectPtr p = grid.vecCenterObjects[size-1];
							pNewModelGroup = p->GetModelGroup();
							if( pNewModelGroup )
							{
								if( bPointLight )
								{
									pNewModelGroup->m_SL = pModelGroup->m_SL;
									pNewModelGroup->SetIsLightEffect(true);

									iiter = terrain->m_DynamicLightGrids.find(nOldGridIndex);
									if ( iiter != terrain->m_DynamicLightGrids.end() )
										terrain->m_DynamicLightGrids.erase(iiter);
									terrain->m_DynamicLightGrids.insert(nNewGridIndex);
								}
								else
								{
									if( bSpotLight )
									{
										pNewModelGroup->m_SpotLight = pModelGroup->m_SpotLight;
										pNewModelGroup->SetIsLightEffect(true);
									}
									else
									{
										if( strCueName != "" )
										{
											if( pModelGroup->m_pSceneAudio )
											{
												if( pNewModelGroup->m_pSceneAudio == NULL )
													pNewModelGroup->m_pSceneAudio = new SceneAudio;

												pNewModelGroup->SetSceneCueName(strCueName);

												DWORD time = pModelGroup->GetPlayTimeLenMin();
												pNewModelGroup->SetPlayTimeLenMin(time);

												time = pModelGroup->GetPlayTimeLenMax();
												pNewModelGroup->SetPlayTimeLenMax(time);

												time = pModelGroup->GetPlayIntervalMin();
												pNewModelGroup->SetPlayIntervalMin(time);

												time = pModelGroup->GetPlayIntervalMax();
												pNewModelGroup->SetPlayIntervalMax(time);

											}
										}
									}
								}
							}
						}
					}

					if( newHasMoveGridIndexs.find(nOldGridIndex) == newHasMoveGridIndexs.end() )
					{
						SGrid &oldgrid = terrain->GetGrid(nOldGridIndex);
						oldgrid.pWave = NULL;
						oldgrid.ClearGridInfo();
						oldgrid.nodeInfo.strTextureNames[0] = strDefaultTerrainTexName;
					}
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		///water
		set<int> setGridIndicesVisibleWater = pWater->GetGridIndicesVisibleWater();
		map< CRiver*, vector<int> > mapRivers;
		for ( set<int>::iterator iter = setGridIndicesVisibleWater.begin(); iter != setGridIndicesVisibleWater.end();)
		{
			int nGridIndex = *iter;
			++iter;

			SGrid & gridW = pWater->GetGrid(nGridIndex);
			Ast(gridW.bVisible);
			if ( gridW.pRiver )
			{
				mapRivers[gridW.pRiver].push_back(nGridIndex);
			}
		}

		CWaterOperator* pWaterOperator = CMapEditApp::GetInst()->GetWaterOperator();

		vector<int> vecGridIndicesRiver;
		for ( map< CRiver*, vector<int> >::iterator iterMapRivers = mapRivers.begin(); iterMapRivers != mapRivers.end(); ++iterMapRivers )
		{
			CRiver * pRiver = iterMapRivers->first;
			string strItemName = pRiver->GetItemName();
			pWaterOperator->RebirthRiver(strItemName);

			for (size_t i = 0; i < iterMapRivers->second.size(); ++i  )
			{
				int nOldGridIndex = iterMapRivers->second[i];///移动前的格子索引
				int nNewGridIndex = mapOldMapGridIndex[nOldGridIndex];///移动后的格子索引

				iter = mapTempTerrainGridInfo.find(nOldGridIndex);
				if( iter != mapTempTerrainGridInfo.end() )
				{
					if( terrain->IsValid(nNewGridIndex) )
					{
						SGrid &gridOldW = mapTempWaterGridInfo[nOldGridIndex];
						SGrid &gridNewW = pWater->GetGrid(nNewGridIndex);

						gridOldW.pRiver = NULL;
						vecGridIndicesRiver.push_back(nNewGridIndex);

						pWater->EraseGridIndexVisibleWater(nOldGridIndex);
						//pWater->InsertGridIndexVisibleWater(nNewGridIndex);

						for ( int j = 0; j < 4; ++j )
						{
							int nOldVertexIndex = gridOldW.dwVertexIndex[j];
							SVertex &OldVertex  = mapTempWaterVertexInfo[nOldVertexIndex];

							int nNewVertexIndex = gridNewW.dwVertexIndex[j];
							SVertex &NewVertex  = pWater->GetVertex(nNewVertexIndex);

							NewVertex.vPosition.y = OldVertex.vPosition.y;
							NewVertex.oColor	  = OldVertex.oColor;

							//OldVertex.vPosition.y = WATER_LAYER_BASE;
							//OldVertex.oColor	  = WATER_COLOR;
							//OldVertex.vNormal	  = CVector3f(0.0, 1.0f, 0.0f);
						}
					}
				}
			}
		}
		size_t size = vecGridIndicesRiver.size();
		for (size_t i = 0; i < size; ++i )
		{
			pWater->AdjustWaterMeshVisibility(vecGridIndicesRiver[i],true);
		}

		this->ChangeRegionLightInfo();
		//////////////////////////////////////////////////////////////////////////

		terrain->ReWriteTerrainBuffer(terrain->m_TerrainBufferIndexs, 0);
		terrain->ReWriteTerrainBuffer(terrain->m_TerrainBufferIndexs, 1);
		terrain->ReWriteTerrainBuffer(terrain->m_TerrainBufferIndexs, 2);
		terrain->ReWriteTerrainBuffer(terrain->m_TerrainBufferIndexs, 3);
		terrain->ReWriteTerrainBuffer(terrain->m_TerrainBufferIndexs, 4);
		terrain->ReWriteTerrainBuffer(terrain->m_TerrainBufferIndexs, 5);
	}

	void CChangeMoveMapData::ChangeRegionLightInfo()
	{
		map<int, int>::iterator beg;
		map<string, SceneRegion*>::iterator iter = CSceneRegionMgr::GetInst()->m_mapRegion.begin();
		for ( iter; iter != CSceneRegionMgr::GetInst()->m_mapRegion.end(); ++iter )
		{
			SceneRegion *sceneRegion = iter->second;

			set<int> newRegionUnitList;
			int rgnUnit = 0;
			for ( set<int>::iterator unit = sceneRegion->_RegionUnitList.begin(); unit != sceneRegion->_RegionUnitList.end(); ++unit )
			{
				rgnUnit = *unit;

				beg = this->m_OldMapRegionIndexMap.find(rgnUnit);
				if( beg != this->m_OldMapRegionIndexMap.end() )
					rgnUnit = beg->second;

				newRegionUnitList.insert(rgnUnit);
			}

			sceneRegion->_RegionUnitList.clear();
			sceneRegion->_RegionUnitList = newRegionUnitList;
		}
	}

	void CChangeMoveMapData::ClearInfo()
	{
		mapOldMapGridIndex.clear();
		mapOldMapVertexIndex.clear();
		m_OldMapRegionIndexMap.clear();
	}
}
