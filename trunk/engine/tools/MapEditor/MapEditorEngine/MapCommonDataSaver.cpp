#include "stdafx.h"
#include "MapCommonDataSaver.h"
#include "TerrainMesh.h"
#include "Operator.h"
#include "Operator.inl"
#include "CMapEditApp.h"
#include "WaterOperator.h"
#include "ExpHelper.h"
#include "LocalVersion.h"
#include "TerrainMeshGrid.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"

void CMapCommonDataSaver::SetOperator(COperator * pOper)
{
	pOperator = pOper;
}

void CMapCommonDataSaver::SetTerrainMesh(CTerrainMesh *pTerrain)
{
	terrain = pTerrain;
}

CMapCommonDataSaver::CMapCommonDataSaver()
: pOperator(NULL), terrain(NULL), dwNewMapWidth(0)
{
}

CMapCommonDataSaver::~CMapCommonDataSaver()
{
	pOperator = NULL;
	terrain   = NULL;
}

void sqr::CMapCommonDataSaver::SaveEnvInfo( FILE * fp )
{
	//////////////////////////////////////////////////////////////////////////
	//环境信息写入
	for (size_t i = 0; i < 4; ++i)
	{
		// light
		DWORD dwAmbientColor = CSceneEnvMgr::GetInst()->GetSceneAmbientColor();
		fwrite(&dwAmbientColor,sizeof(DWORD),1,fp);

		DWORD dwShadowColor = CSceneEnvMgr::GetInst()->GetShadowColor();
		fwrite(&dwShadowColor, sizeof(DWORD), 1, fp);

		DWORD dwSunColor = CSceneEnvMgr::GetInst()->GetSceneAmbientColor();
		fwrite(&dwSunColor,sizeof(DWORD),1,fp);

		CVector3f vSunDir = CSceneEnvMgr::GetInst()->GetDirDir();
		fwrite(&vSunDir,sizeof(CVector3f),1,fp);

		DWORD dwPlayerSunColor = CSceneEnvMgr::GetInst()->GetPlayerDirColor();
		fwrite(&dwPlayerSunColor,sizeof(DWORD),1,fp);

		DWORD dwPlayerColor = CSceneEnvMgr::GetInst()->GetPlayerAmbientColor();
		fwrite(&dwPlayerColor,sizeof(DWORD),1,fp);

		// fog
		DWORD dwFogColor = CSceneEnvMgr::GetInst()->GetFogColor();
		fwrite(&dwFogColor,sizeof(DWORD),1,fp);

		float fFogStart = CSceneEnvMgr::GetInst()->GetFogStart();
		fwrite(&fFogStart,sizeof(float),1,fp);

		float fFogEnd = CSceneEnvMgr::GetInst()->GetFogEnd();
		fwrite(&fFogEnd,sizeof(float),1,fp);

		float fFogDensity = CSceneEnvMgr::GetInst()->GetDensity();
		fwrite(&fFogDensity,sizeof(float),1,fp);
	}

	///lightmap z offset
	float zLightMapOffset = CSceneEnvMgr::GetInst()->GetLightMapZOffset();
	fwrite(&zLightMapOffset,sizeof(zLightMapOffset),1,fp);
}

void sqr::CMapCommonDataSaver::SaveBackPictureInfo( FILE * fp )
{
	bool bsetbackpic = CSceneEnvMgr::GetInst()->GetISBackPicMove();
	fwrite(&bsetbackpic,sizeof(bool),1,fp);
	if (bsetbackpic)
	{
		float move = 0.0f;

		move = CSceneEnvMgr::GetInst()->GetBackPicLeftMoveOffset();
		fwrite(&move,sizeof(float),1,fp);

		move = CSceneEnvMgr::GetInst()->GetBackPicRightMoveOffset();
		fwrite(&move,sizeof(float),1,fp);

		move = CSceneEnvMgr::GetInst()->GetBackPicTopMoveOffset();
		fwrite(&move,sizeof(float),1,fp);

		move = CSceneEnvMgr::GetInst()->GetBackPicBottonMoveOffset();
		fwrite(&move,sizeof(float),1,fp);

	}

	string strTextureSkyBack = CSceneEnvMgr::GetInst()->GetSkyFarBackTextureName();

	int nLen = strTextureSkyBack.length();
	fwrite(&nLen,sizeof(int),1,fp);
	fwrite(strTextureSkyBack.c_str(),nLen,1,fp);

	//back pic center and scale
	CVector2f center = CSceneEnvMgr::GetInst()->GetBackTextureUVCenter();
	fwrite(&center.x,sizeof(float),1,fp);
	fwrite(&center.y,sizeof(float),1,fp);

	float scale = CSceneEnvMgr::GetInst()->GetBackTextureScale();
	fwrite(&scale,sizeof(float),1,fp);
}

void sqr::CMapCommonDataSaver::SaveWaveInfo( FILE * fp )
{
	CWaterOperator *pWaterOperator = CMapEditApp::GetInst()->GetWaterOperator();
	int nWavePerGrid = pWaterOperator->GetWaveDensity();
	fwrite(&nWavePerGrid,sizeof(int),1,fp);

	int nWaveSize = pWaterOperator->GetWaveSize();
	fwrite(&nWaveSize,sizeof(int),1,fp);

	int nWaveOff = pWaterOperator->GetWaveOff();
	fwrite(&nWaveOff,sizeof(int),1,fp);

	float fWaveSpeed = pWaterOperator->GetWaveSpeed();
	fwrite(&fWaveSpeed,sizeof(float),1,fp);

	int nWaveLife = pWaterOperator->GetWaveLife();
	fwrite(&nWaveLife,sizeof(int),1,fp);

	//////////////////////////////////////////////////////////////////////////
	//wavetexture name
	string strWaveName = pWaterOperator->GetWaveTextureName();

	//////////////////////////////////////////////////////////////////////////
	//strWaveName = GetAlphabetic(strWaveName.c_str());
	//////////////////////////////////////////////////////////////////////////

	int iWaveNameLen = strWaveName.length();
	fwrite(&iWaveNameLen,sizeof(int),1,fp);
	fwrite(strWaveName.c_str(),iWaveNameLen,1,fp);

	//////////////////////////////////////////////////////////////////////////
}

void sqr::CMapCommonDataSaver::SaveMainRoadWeightInfo( FILE * fp )
{
	size_t size = terrain->m_MainRoadWeight.size();
	int iNameLen = 0, weight = 0;
	string texname = "";
	fwrite(&size,sizeof(size_t),1,fp);
	map<string, int>::iterator texiter = terrain->m_MainRoadWeight.begin();
	for ( size_t i = 0; i < size; ++i, ++texiter)
	{
		texname = texiter->first;
		weight = texiter->second;
		iNameLen = texname.length();
		fwrite(&iNameLen,sizeof(int),1,fp);
		fwrite(texname.c_str(),iNameLen,1,fp);
		fwrite(&weight,sizeof(int),1,fp);
	}
}

void sqr::CMapCommonDataSaver::SaveGridModeProVersion(FILE * fp)
{
	uint32 uModelVersion = MODELPRO_POSITION;	
	fwrite(&uModelVersion,sizeof(uModelVersion),1,fp);

	uint32 uGridVersion  = GRIDPRO_ENFORCELOWBLOCK;	
	fwrite(&uGridVersion,sizeof(uGridVersion),1,fp);
}

void sqr::CMapCommonDataSaver::SaveEachVertexInfo( FILE * fp, const float fPositionY, const CVector3f& vNormal, 
												  const DWORD oColor, const DWORD sColor, const short sLogicHeight )
{
	fwrite(&fPositionY,   sizeof(fPositionY),	1, fp);
	fwrite(&vNormal,	  sizeof(vNormal),		1, fp);
	fwrite(&oColor,		  sizeof(oColor),		1, fp);
	fwrite(&sColor,		  sizeof(sColor),		1, fp);
	fwrite(&sLogicHeight, sizeof(sLogicHeight), 1, fp);
}

void sqr::CMapCommonDataSaver::SaveTerrainGridTexIndex(FILE * fp, const int nGridCnt, map<string, int16>& terrainTextureNames)
{
	string strName = "";
	int    iNameLen = 0;
	terrainTextureNames.clear();///用于记录地图中用到的所有纹理的名字，保存各层纹理信息则保存index就行
	int16 nTextureIndex = 0;

	size_t terrainTextureCnt = 0;
	long lterrainTextureCntOffset = ftell(fp);
	fwrite(&terrainTextureCnt,sizeof(terrainTextureCnt),1,fp);
	for ( int i = 0; i < nGridCnt; ++i )
	{
		SGrid *pGrid = &terrain->GetGrid(i);
		strName = pGrid->nodeInfo.strTextureNames[0];

		if ( terrainTextureNames.find(strName) == terrainTextureNames.end() )
		{
			terrainTextureNames[strName] = nTextureIndex;

			iNameLen = strName.length();
			fwrite(&iNameLen,sizeof(int),1,fp);
			fwrite(strName.c_str(),iNameLen,1,fp);
			fwrite(&nTextureIndex,sizeof(nTextureIndex),1,fp);

			++nTextureIndex;
		}
		strName = pGrid->nodeInfo.strTextureNames[1];
		if ( terrainTextureNames.find(strName) == terrainTextureNames.end() )
		{
			terrainTextureNames[strName] = nTextureIndex;

			iNameLen = strName.length();
			fwrite(&iNameLen,sizeof(int),1,fp);
			fwrite(strName.c_str(),iNameLen,1,fp);
			fwrite(&nTextureIndex,sizeof(nTextureIndex),1,fp);

			++nTextureIndex;
		}

	}

	terrainTextureCnt = terrainTextureNames.size();
	long terrainTextureNow = ftell(fp);
	fseek(fp,lterrainTextureCntOffset,SEEK_SET);
	fwrite(&terrainTextureCnt,sizeof(terrainTextureCnt),1,fp);
	fseek(fp,terrainTextureNow,SEEK_SET);
}

void sqr::CMapCommonDataSaver::SaveEachGridInfo(FILE * fp, SGrid* pGrid, int& SceneLightCount, int& nSpotLightCount, bool bResize, bool bSplit)
{
	bool   bEffectBased = false, bEffecIsExit = false;
	string strName = "", strEffectName = "", strCueName = "";
	map<int, int>::iterator iiter;

	/////////////////////////////////////////////////////////////////
	fwrite(&pGrid->bCliffLayerOne, sizeof(bool), 1, fp);

	fwrite(&pGrid->bMatNotEffectByTileSets, sizeof(bool), 1, fp);

	fwrite(&pGrid->bCliffLayerOne, sizeof(bool), 1, fp);

	//纹理坐标信息,第1层纹理坐标的信息
	for (int i = 0; i < 4; ++i)
	{
		fwrite(&pGrid->uv[0][i].x, sizeof(float), 1, fp);
		fwrite(&pGrid->uv[0][i].y, sizeof(float), 1, fp);
	}

	fwrite(&pGrid->vCenter.y,sizeof(float),1,fp);

	///物体是否受顶点色影响
	//bool bEffectByVertexColor = true;
	fwrite(&pGrid->bEffectByVertexColor,sizeof(bool),1,fp);

	///保存顶点色是否影响场景物体标志
	fwrite(&pGrid->bCliffLayerOne, sizeof(bool), 1, fp);

	//格子上的模型信息
	// central  object
	if ( !pGrid->vecCenterObjects.empty() )
	{
		DWORD dw = 0;
		long lCntOffset = ftell(fp);
		fwrite(&dw,sizeof(DWORD),1,fp);

		for ( vector< CTObjectPtr >::iterator iter = pGrid->vecCenterObjects.begin(); iter != pGrid->vecCenterObjects.end(); ++iter)
		{
			CTObjectPtr p = (*iter);

			CEditModelGroup *pModelGroup = p->GetModelGroup();

			///login model 跳过
			if( pModelGroup->m_bLoginModel )
			{
				nLoginGridIndexs.insert(pGrid->dwGridIndex);
				continue;
			}

			if( pModelGroup->m_bReginEffect )
			{
				continue;
			}

			if( pModelGroup->m_pSceneAudio )
				continue;

			// effect bound?
			bEffectBased = pModelGroup->GetIsEffectBased();
			bEffecIsExit = pModelGroup->GetEffectName(strEffectName);
			if ( bEffectBased && bEffecIsExit )
			{
				//记录场景点光源个数---
				if (pModelGroup->m_SL.m_bIAmbScenePointLight)
					++SceneLightCount;

				//记录场景聚光灯个数---
				if (pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight)
					++nSpotLightCount;

				//---
				bool b = true;
				fwrite(&b,sizeof(bool),1,fp);
				DWORD dwLen = strEffectName.size();
				fwrite(&dwLen,sizeof(int),1,fp);
				fwrite(strEffectName.c_str(),dwLen,1,fp);
			}
			else
			{
				bool b = false;

				fwrite(&b,sizeof(bool),1,fp);
			}

			dw++;

			string strItemName = pModelGroup->GetItemName();

			DWORD dwLen = strItemName.size();
			fwrite(&dwLen,sizeof(int),1,fp);
			fwrite(strItemName.c_str(),dwLen,1,fp);

			//////////////////////////////////////////////////////////////////////////
			///save as modelgroup
			int nAtten = pModelGroup->GetAtten();
			fwrite(&nAtten,sizeof(int),1,fp);

			///nAffectRadio
			int nRatio = pModelGroup->GetAffectRatio();
			fwrite( &nRatio, sizeof(int), 1, fp);

			///save model property
			CMapEditObject *pMapEditObject = NULL;
			pMapEditObject = pModelGroup->GetSelfPresentation(0);
			if( pModelGroup == NULL )
			{
				MessageBox(NULL,"没有模型，不保存","",MB_OK);
				return;
			}

			CVector3f offset;
			pMapEditObject->GetImageOffsetToAdjustedCenterGrid(offset);
			MoldeSaveProperty sModelSavePro = pMapEditObject->sModelSavePro;
			sModelSavePro.vPosition = offset;

			if( bResize && !bSplit )
			{
				int nOldGridIndex = pMapEditObject->sModelSavePro.nCenterGridIndex;
				iiter = pOperator->m_OldMapGridIndexMap.find(nOldGridIndex);
				if( iiter != pOperator->m_OldMapGridIndexMap.end() )
				{
					int nNewGridIndex = iiter->second; 
					sModelSavePro.nCenterGridIndex = nNewGridIndex;

					int nOldX = 0, nOldZ = 0, nNewX = 0, nNewZ = 0;
					nOldX = nOldGridIndex % terrain->GetWidth(); // 列
					nOldZ = nOldGridIndex / terrain->GetWidth(); // 行
					nNewX = nNewGridIndex % dwNewMapWidth; // 列
					nNewZ = nNewGridIndex / dwNewMapWidth; // 行

					CVector3f oldpos = sModelSavePro.vPosition;
					CVector3f offset(oldpos.x - nOldX * GRID_SPACE, oldpos.y, oldpos.z - nOldZ * GRID_SPACE);
					CVector3f newpos = CVector3f(offset.x + nNewX * GRID_SPACE, offset.y, offset.z + nNewZ * GRID_SPACE);
					sModelSavePro.vPosition = newpos;
					//sModelSavePro.vUserOffsetEx = offset - CVector3f(32.0f,0.0f,32.0f);
				}
			}
			else
			{
				if( !bResize && bSplit )
				{
					int nOldGridIndex = pMapEditObject->sModelSavePro.nCenterGridIndex;
					iiter = pOperator->m_SplitOldMapGridIndexMap.find(nOldGridIndex);
					if( iiter != pOperator->m_SplitOldMapGridIndexMap.end() )
					{
						int nNewGridIndex = iiter->second; 
						sModelSavePro.nCenterGridIndex = nNewGridIndex;

						int nOldX = 0, nOldZ = 0, nNewX = 0, nNewZ = 0;
						nOldX = nOldGridIndex % dwNewMapWidth; // 列
						nOldZ = nOldGridIndex / dwNewMapWidth; // 行
						nNewX = nNewGridIndex % dwNewMapWidth; // 列
						nNewZ = nNewGridIndex / dwNewMapWidth; // 行

						CVector3f oldpos = pMapEditObject->sModelSavePro.vPosition;
						CVector3f offset(oldpos.x - nOldX * GRID_SPACE, oldpos.y, oldpos.z - nOldZ * GRID_SPACE);
						CVector3f newpos = CVector3f(offset.x + nNewX * GRID_SPACE, offset.y, offset.z + nNewZ * GRID_SPACE);
						sModelSavePro.vPosition = newpos;
					}
				}
			}

			fwrite( &sModelSavePro, sizeof(sModelSavePro),  1, fp);
		}

		long lNow = ftell(fp);
		fseek(fp,lCntOffset,SEEK_SET);
		fwrite(&dw,sizeof(DWORD),1,fp);
		fseek(fp,lNow,SEEK_SET);
	}
	else
	{
		DWORD dw = 0;
		fwrite(&dw,sizeof(int),1,fp);
	}
}