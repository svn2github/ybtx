#include "stdafx.h"
#include "MapCommonDataLoader.h"
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
#include "LocalVersion.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "WaterOperator.h"

void MapCommonDataLoader::SetOperator(COperator * pOper)
{
	pOperator = pOper;
}

void MapCommonDataLoader::SetTerrainMesh(CTerrainMesh *pTerrain)
{
	terrain = pTerrain;
}

MapCommonDataLoader::MapCommonDataLoader()
: pOperator(NULL), terrain(NULL)
{
}

MapCommonDataLoader::~MapCommonDataLoader()
{
	pOperator = NULL;
	terrain   = NULL;
}

void sqr::MapCommonDataLoader::LoadEnvInfo( FILE * fp )
{
#pragma region ENVPARA
	/*为了避免修改格式，CSceneEnvMgr::EEnv_Cnt = 4先硬性修改*/
	for (size_t i = 0; i < 4; ++i)
	{
		// light
		DWORD dwAmbientColor;
		fread(&dwAmbientColor,sizeof(DWORD),1,fp);
		CSceneEnvMgr::GetInst()->SetSceneAmbientColor(dwAmbientColor);

		DWORD dwShadowColor;
		fread(&dwShadowColor, sizeof(DWORD), 1, fp);
		CSceneEnvMgr::GetInst()->SetShadowColor(dwShadowColor);

		DWORD dwSunColor;
		fread(&dwSunColor,sizeof(DWORD),1,fp);
		CSceneEnvMgr::GetInst()->SetSceneDirColor(dwSunColor);

		CVector3f vSunDir;
		fread(&vSunDir,sizeof(CVector3f),1,fp);
		CSceneEnvMgr::GetInst()->SetDirDir(vSunDir);

		DWORD dwPlayerSunColor;
		fread(&dwPlayerSunColor,sizeof(DWORD),1,fp);
		CSceneEnvMgr::GetInst()->SetPlayerDirColor(dwPlayerSunColor);

		DWORD dwPlayerColor;
		fread(&dwPlayerColor,sizeof(DWORD),1,fp);
		CSceneEnvMgr::GetInst()->SetPlayerAmbientColor(dwPlayerColor);

		// fog
		DWORD dwFogColor;
		fread(&dwFogColor,sizeof(DWORD),1,fp);
		CSceneEnvMgr::GetInst()->SetFogColor(dwFogColor);

		float fFogStart;
		fread(&fFogStart,sizeof(float),1,fp);
		CSceneEnvMgr::GetInst()->SetFogStart(fFogStart);

		float fFogEnd;
		fread(&fFogEnd,sizeof(float),1,fp);
		CSceneEnvMgr::GetInst()->SetFogEnd(fFogEnd);

		float fFogDensity;
		fread(&fFogDensity,sizeof(float),1,fp);
		CSceneEnvMgr::GetInst()->SetDensity(fFogDensity);
	}

	float zLightMapOffset = 0.0f;
	fread(&zLightMapOffset,sizeof(zLightMapOffset),1,fp);
	CSceneEnvMgr::GetInst()->SetLightMapZOffset(zLightMapOffset);

#pragma endregion ENVPARA
}

void sqr::MapCommonDataLoader::LoadBackPictureInfo( FILE * fp )
{
	bool bsetbackpic = false;
	fread(&bsetbackpic,sizeof(bool),1,fp);
	if (bsetbackpic)
	{
		CSceneEnvMgr::GetInst()->SetIsBackPicMove(bsetbackpic);
		float move = 0.0f;

		fread(&move,sizeof(float),1,fp);
		CSceneEnvMgr::GetInst()->SetBackPicLeftMoveOffset(move);

		fread(&move,sizeof(float),1,fp);
		CSceneEnvMgr::GetInst()->SetBackPicRightMoveOffset(move);

		fread(&move,sizeof(float),1,fp);
		CSceneEnvMgr::GetInst()->SetBackPicTopMoveOffset(move);

		fread(&move,sizeof(float),1,fp);
		CSceneEnvMgr::GetInst()->SetBackPicBottonMoveOffset(move);


		pOperator->MapBackPicMoveInfo();
	}

	DWORD  dwFileNameLen = 0;
	string str = "";

	fread(&dwFileNameLen,sizeof(DWORD),1,fp);
	str.resize(dwFileNameLen);
	fread((TCHAR*)str.data(),dwFileNameLen,1,fp);
	size_t npos = str.rfind(".jpg");
	if ( npos != string::npos)
	{
		str = str.substr(0, npos) + ".dds";
	}

	CSceneEnvMgr::GetInst()->SetSkyFarBackTextureName(str);

	//back pic center and scale
	CVector2f center;
	fread(&center.x,sizeof(float),1,fp);
	fread(&center.y,sizeof(float),1,fp);

	CSceneEnvMgr::GetInst()->SetBackTextureUVCenter(center);

	float scale = 1.0f;
	fread(&scale,sizeof(float),1,fp);
	scale *= 100.0f;
	CSceneEnvMgr::GetInst()->SetBackTextureScale((int)scale);
}

void sqr::MapCommonDataLoader::LoadWaveInfo( FILE * fp )
{
	CWaterOperator *pWaterOperator = CMapEditApp::GetInst()->GetWaterOperator();
	int nWavePerGrid;
	fread(&nWavePerGrid,sizeof(int),1,fp);
	pWaterOperator->SetWaveDensity(nWavePerGrid);

	int nWaveSize;
	fread(&nWaveSize,sizeof(int),1,fp);
	pWaterOperator->SetWaveSize(nWaveSize);

	int nWaveOff;
	fread(&nWaveOff,sizeof(int),1,fp);
	pWaterOperator->SetWaveOff(nWaveOff);

	float fWaveSpeed;
	fread(&fWaveSpeed,sizeof(float),1,fp);
	pWaterOperator->SetWaveSpeed(fWaveSpeed);

	int nWaveLife = 0;
	fread(&nWaveLife,sizeof(int),1,fp);
	pWaterOperator->SetWaveLife(nWaveLife);

	DWORD dwFileNameLen = 0;
	fread(&dwFileNameLen,sizeof(DWORD),1,fp);
	string strFileNameWave = "";
	strFileNameWave.resize(dwFileNameLen);
	fread((TCHAR*)strFileNameWave.data(),dwFileNameLen,1,fp);

	// .tga -> .dds
	if (strFileNameWave.rfind(".tga") != string::npos)
	{
		strFileNameWave = strFileNameWave.substr(0, strFileNameWave.rfind(".tga"));
		strFileNameWave += ".dds";
	}

	pWaterOperator->SetWaveTexture(strFileNameWave);
	//////////////////////////////////////////////////////////////////////////
}

void sqr::MapCommonDataLoader::LoadEachVertexInfo( FILE * fp, SVertex* pVertex )
{
	short s = 0;

	fread(&pVertex->vPosition.y,sizeof(float),1,fp);

	fread(&pVertex->vNormal,sizeof(CVector3f),1,fp);
	fread(&pVertex->oColor,sizeof(DWORD),1,fp);
	fread(&pVertex->sColor,sizeof(DWORD),1,fp);
	fread(&s,sizeof(short),1,fp);
	pVertex->fLogicHeight = s;
	pVertex->bColor = pVertex->oColor;
	pVertex->tColor = pVertex->oColor;
}