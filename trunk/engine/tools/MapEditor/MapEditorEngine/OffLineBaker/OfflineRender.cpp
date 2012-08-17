#include "stdafx.h"
#include "OfflineRender.h"
#include "BaseHelper.h"
#include "CMapEditApp.h"
#include "Render.h"
#include "CMapEditObject.h"
#include "TerrainMesh.h"
#include "Operator.h"
#include "Operator.inl"
#include "ModelGroup.h"
#include "CEditModel.h"
#include "CLightSystem.h"
#include "CRenderGroup.h"
#include "Algorithm.h"
#include "SceneRegionMgr.h"
#include "CEditTerrainMgr.h"
#include "StaticIlluminator.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "TerrainMeshGrid.h"
#include "CModelDiffuseColor.h"
#include "EditStateOpertiaonMgr.h"
#include "CColor4.inl"

namespace sqr
{

//------------------------------------------------------------------------------
OfflineRender::OfflineRender()
{

}

//------------------------------------------------------------------------------
OfflineRender::~OfflineRender()
{
	m_GridColor.clear();

	mPointLight.clear();
	mSpotLight.clear();
}

//------------------------------------------------------------------------------
void OfflineRender::CalGridColorParam()
{
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	mSunlightDir           = -CSceneEnvMgr::GetInst()->GetDirDir();

	LightColor color;

	int nGridCnt = pTerrain->GetGridCount();
	m_GridColor.resize(nGridCnt);
	for ( int m = 0; m < nGridCnt; ++m )
	{
		int nRegionIndex = pTerrain->GetDivideRgnIndexByGridIndex(m);
		LightColor gridColor, maxColor;
		SGrid* pGrid = &pTerrain->GetGrid( m );
		const vector<DWORD>& RegionAll = pTerrain->_RegionAjenctList[nRegionIndex];

		SDivideRegion *pDivideRegion = pTerrain->GetDivideRegion( nRegionIndex );
		if ( pDivideRegion && !pDivideRegion->bNeedCalRegion )
		{
			///区域缩小四分之一
			const string& strRgnName = pDivideRegion->m_strRegionName;
			SceneRegion *pRegion = CSceneRegionMgr::GetInst()->GetSceneRegion(strRgnName);
			if ( pRegion )
			{
				color.Ambient = pRegion->lightParam.globalAmbient;
				color.Diffuse = pRegion->lightParam.globalDiffuse;
				color.Shadow = pRegion->lightParam.globalShadow;
			}
		}
		else
		{
			//计算区域光
			for ( size_t i = 0; i < RegionAll.size(); ++i )
			{
				SDivideRegion *pDivideRegion = pTerrain->GetDivideRegion(RegionAll[i]);
				if ( !pDivideRegion )
					continue;
				const string& strReginName = pDivideRegion->m_strRegionName;

				SceneRegion *pSceneRgn = CSceneRegionMgr::GetInst()->GetSceneRegion(strReginName);
				if ( !pSceneRgn )
					continue;
				float x = pDivideRegion->m_vCenter.x - pGrid->vCenter.x;
				float z = pDivideRegion->m_vCenter.z - pGrid->vCenter.z;
				float dis = sqrt(x * x + z * z);

				static const float compareValue = GRID_SPACE * REGION_DIVIDESPACE;
				float ratio = 1.0f - dis / compareValue;
				if ( ratio <= 0.0f )
					continue;

				gridColor.Ambient += pSceneRgn->lightParam.globalAmbient * ratio;
				gridColor.Diffuse += pSceneRgn->lightParam.globalDiffuse * ratio;
				gridColor.Shadow += pSceneRgn->lightParam.globalShadow * ratio;

				maxColor.Ambient = CColor4::Max(maxColor.Ambient, pSceneRgn->lightParam.globalAmbient);
				maxColor.Diffuse = CColor4::Max(maxColor.Diffuse, pSceneRgn->lightParam.globalDiffuse);
				maxColor.Shadow = CColor4::Max(maxColor.Shadow, pSceneRgn->lightParam.globalShadow);
			}

			///计算当前所在区域的太阳光和环境光颜色
			color.Diffuse = CColor4::Min(gridColor.Diffuse, maxColor.Diffuse);
			color.Ambient = CColor4::Min(gridColor.Ambient, maxColor.Ambient);
			color.Shadow = CColor4::Min(gridColor.Shadow, maxColor.Shadow);
		}

		///实时记录当前格子对应的环境和太阳光
		m_GridColor[m] = color;
	}
}

//------------------------------------------------------------------------------
void OfflineRender::UpdateGridLights( int nGirdIndex )
{
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	vector< CTObjectPtr > pointLightObjects, spotLightObjects;
	SGrid* pGrid = &pTerrain->GetGrid(nGirdIndex);

	//查询场景灯光
	vector< CTObjectPtr >::iterator begin = pGrid->vecCenterObjects.begin();
	vector< CTObjectPtr >::iterator end   = pGrid->vecCenterObjects.end();
	for ( ; begin != end; ++begin )
	{
		const CTObjectPtr& tobject = *begin;
		CEditModelGroup *pModelGroup = tobject->m_pModelGroup;

		if ( pModelGroup->m_SL.m_bIAmbScenePointLight && !pModelGroup->m_SL.m_bDynamic)
			pointLightObjects.push_back( tobject );
		else if ( pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight )
			spotLightObjects.push_back( tobject );
	}

	mPointLight[nGirdIndex] = pointLightObjects;
	mSpotLight[nGirdIndex] = spotLightObjects;
}

//------------------------------------------------------------------------------
void OfflineRender::UpdateTerrainColor(bool shadow)
{
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	CStaticIlluminator* illuminator = CStaticIlluminator::GetInst();
	set<DWORD> bufferindexs;

	sMaterial material = {0};
	CRenderGroup* renderGroup = CMainWindowTarget::GetInst()->GetGroup();
	CLightSystem* lightSystem = renderGroup->GetLightSystem();
	CLight light = *lightSystem->GetGlobalLight();

	for ( int m = 0; m < pTerrain->GetGridCount(); ++m )
	{
		SGrid* pGrid = &pTerrain->GetGrid( m );

		const LightColor& color = m_GridColor[m];
		bufferindexs.insert(pGrid->dwBufferIndex);
		illuminator->SetAmbient(color.Ambient);
		light.SetColor(color.Diffuse);
		illuminator->SetGlobalLight(light);

		for ( int i = 0; i < 4; ++i )
		{
			SVertex &vertex = pTerrain->GetVertex(pGrid->dwVertexIndex[i]);
			///判断是否计算过
			if ( vertex.bHasCalculate )
				continue;

			material.Ambient = vertex.oColor;
			material.Diffuse = vertex.oColor;
			CColor4 lighted;
			if (shadow)
				lighted = color.Shadow;
			else
				lighted = illuminator->Illuminate(material, vertex.vPosition, vertex.vNormal);
			lighted.a = float(vertex.oColor >> 24) / 255.0f;

			///计算后的值给到地编顶点上
			vertex.bColor = lighted.MakeDWordSafe();
			vertex.tColor = vertex.bColor;
			vertex.bHasCalculate = true;
		}
	}

// 	///重写地表顶点颜色buffer
 	pTerrain->ReWriteTerrainBuffer( bufferindexs, 0 );

	for (index_t i = 0; i < pTerrain->GetVertexCount(); ++i)
	{
		SVertex& vertex = pTerrain->GetVertex(i);
		vertex.bHasCalculate = false;
	}
}


//------------------------------------------------------------------------------
void OfflineRender::PreviewTerrainBakingResult(bool use)
{
	CTerrainMesh* terrainMesh = CMapEditApp::GetInst()->GetTerrain();
	if (NULL == terrainMesh)
		return;
	
	CEditTerrainMgr* terrain = terrainMesh->m_pTerrain;

// 	if( bCleanRS )
 	terrain->ClearRSShaderType();

 	TerrainFlag::Mask mask = terrain->GetRenderFlag();
 	if (use)
	{
		terrainMesh->ReWriteTerrainBuffer( terrainMesh->m_TerrainBufferIndexs, 0 );
 		mask |= TerrainFlag::Baking;
	}
 	else
 		mask &= ~TerrainFlag::Baking;
 
 	terrain->SetRenderFlag(mask);

	this->RevertVertexBakingToVertexColor(use);
}

//------------------------------------------------------------------------------
void OfflineRender::UseBakingResult( bool use, bool bShowTerrainBaking, bool bCleanRS )
{
	CTerrainMesh* terrainMesh = CMapEditApp::GetInst()->GetTerrain();
	if (NULL == terrainMesh)
		return;
	// shader switch
	CTerrainMgr* terrain = terrainMesh->m_pTerrain;

	if( bCleanRS )
		terrain->ClearRSShaderType();

	RevertVertexBakingToVertexColor(use);

// #if !VERTEX_COLOR_EDIT_WHEN_BAKED
 	TerrainFlag::Mask mask = terrain->GetRenderFlag();
 	if (use)
 		mask |= TerrainFlag::Baking;
 	else
 		mask &= ~TerrainFlag::Baking;
 
 	terrain->SetRenderFlag(mask);
// 
// #endif
	SQR_TRY
	{
		// model vertex color switch
		for (int gridIndex = 0; gridIndex < terrainMesh->GetGridCount(); ++gridIndex)
		{
			SGrid& grid = terrainMesh->GetGrid(gridIndex);

			for (ModelGroupIndex modelGroupIndex = 0; modelGroupIndex < grid.vecCenterObjects.size(); ++modelGroupIndex)
			{
				CEditModelGroup* modelGroup = grid.vecCenterObjects[modelGroupIndex]->m_pModelGroup;
				if ( modelGroup->m_SL.m_bIAmbScenePointLight || modelGroup->m_SpotLight.m_bIAmbSceneSpotLight || modelGroup->m_pSceneAudio )
					continue;

				for (vector<CMapEditObject*>::iterator itm = modelGroup->m_vecSelfPresentation.begin();
						itm != modelGroup->m_vecSelfPresentation.end(); ++itm)
				{
					CMapEditObject* meModel = *itm;

					//if ( true == use )
					{
						CVertexDiffuseColor::GetInst()->ClearModelDiffuse(modelGroupIndex, *meModel);


						ModelGroupColorMap* modelColorMap = grid.GetModelColorMap(modelGroupIndex);
						if (NULL == modelColorMap || modelColorMap->empty())
							goto NEXT;

						CEditModel& model = *meModel;
						ModelGroupColorMap::iterator ito = modelColorMap->find(meModel->GetName().c_str());
						Ast(modelColorMap->end() != ito);
						vector<PieceClassColor>& pieceColors = ito->second;

						if( pieceColors.empty() )
						{
							string log = meModel->GetName();
							log += "该模型piece为空，请联系做该模型同学\n否则导出的烘培文件在游戏中有问题,模型好后重新生成烘培文件";
							MessageBox(NULL,log.c_str(),"提示",MB_OK);
							continue;
						}

						index_t renderPieceIndex = 0;
						for (CEditModel::RenderPieceMap::iterator itr = model.GetRenderPieces().begin();
							itr != model.GetRenderPieces().end(); ++itr, ++renderPieceIndex)
						{
							CRenderPiece& renderPiece = itr->second;
							PieceClassColor& pieceColor = pieceColors[renderPieceIndex];
							if( pieceColor.pieceColorMap.empty() )
							{
								string log = meModel->GetName();
								log += "该模型的piece可能没有烘培值";
								MessageBox(NULL,log.c_str(),"提示",MB_OK);
								continue;
							}

							for (index_t pieceIndex = 0; pieceIndex < renderPiece.m_Pieces.size(); ++pieceIndex)
							{
								model.ModifyMesh(itr->first.c_str(), NULL, 0,MO_DELETE, pieceIndex);
								if (use)
								{
									model.ModifyMesh(itr->first.c_str(),
										pieceColor.pieceColorMap[pieceIndex]->GetVB(),
										0,
										MO_ADD, 
										pieceIndex,											
										pieceColor.pieceColorMap[pieceIndex]->GetVB()->getNumVertices());
								}
							}
						}
					}
// 					else
// 					{
// 						CVertexDiffuseColor::GetInst()->ApplyModelDiffuse(modelGroupIndex, *meModel);
// 					}

				NEXT:
					continue;
				}
			}
		}
	}
	SQR_CATCH(exp)
	{
		string str = exp.ErrorMsg();
		str += "使用烘培失败";
		MessageBox(NULL, str.c_str(), "错误", MB_OK);

		return;
	}
	SQR_TRY_END;
}

//------------------------------------------------------------------------------
void
OfflineRender::AddLightsToIlluminator(bool terrainOnly)
{
	CRenderGroup* renderGroup = CMainWindowTarget::GetInst()->GetGroup();
	CLightSystem* lightSystem = renderGroup->GetLightSystem();
	CStaticIlluminator* illuminator = CStaticIlluminator::GetInst();
	illuminator->ClearAllLights();

	CLight tempLight;
	// point lights
	for (map<int32, vector<CTObjectPtr> >::iterator it = this->mPointLight.begin();
	        it != this->mPointLight.end(); ++it)
	{
		vector<CTObjectPtr>& lights = it->second;
		for (vector<CTObjectPtr>::iterator itl = lights.begin();
		        itl != lights.end(); ++itl)
		{
			CTObjectPtr& tobject = *itl;
			SceneLight& light = tobject->m_pModelGroup->m_SL;
			if (!light.m_bIAmbScenePointLight)
				continue;

#ifdef USE_OLD_LIGHT_MODEL
			if (terrainOnly && !light.m_bEffectTerrain)
				continue;
#else
			///场景光源全部影响地表
			if (terrainOnly)
				continue;
#endif
			
			SceneLightToCLight(tempLight, light);
			// position
			CMapEditObject* pEModel = tobject->m_pModelGroup->m_vecSelfPresentation[0];
			CMatrix world;
			pEModel->GetFinalMatrix(world);
			tempLight.SetPosition(world.m[3]);
			// add to illuminator
			illuminator->AddPointLight(tempLight);
		}
	}
	// spot lights
	for (map<int32, vector<CTObjectPtr> >::iterator it = this->mSpotLight.begin();
	        it != this->mSpotLight.end(); ++it)
	{
		vector<CTObjectPtr>& lights = it->second;
		for (vector<CTObjectPtr>::iterator itl = lights.begin();
		        itl != lights.end(); ++itl)
		{
			CTObjectPtr& tobject = *itl;
			SceneSpotLight& light = tobject->m_pModelGroup->m_SpotLight;
			if (!light.m_bIAmbSceneSpotLight)
				continue;

#ifdef USE_OLD_LIGHT_MODEL
			if (terrainOnly && !light.m_bEffectTerrain)
				continue;
#else
			///场景光源全部影响地表
			if (terrainOnly)
				continue;
#endif
			
			SceneLightToCLight(tempLight, light);
			// position
			tempLight.SetPosition(tobject->m_pModelGroup->m_SpotLightPos);
			// dirction
			tempLight.setDIRECTION(tobject->m_pModelGroup->m_SpotLight.m_Direction);
			// add to illuminator
			illuminator->AddSpotLight(tempLight);
		}
	}
}

void OfflineRender::RevertVertexBakingToVertexColor(bool use)
{
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	set<DWORD> bufferindexs;
	int nGridCnt = pTerrain->GetGridCount();
	for ( int m = 0; m < nGridCnt; ++m )
	{
		SGrid* pGrid = &pTerrain->GetGrid( m );
		bufferindexs.insert(pGrid->dwBufferIndex);
		for ( int i = 0; i < 4; ++i )
		{
			SVertex &vertex = pTerrain->GetVertex(pGrid->dwVertexIndex[i]);
			
			vertex.bColor = use ? vertex.tColor : vertex.oColor;
		}
	}

	///重写地表顶点颜色buffer
	pTerrain->ReWriteTerrainBuffer( bufferindexs, 0 );
}
}
