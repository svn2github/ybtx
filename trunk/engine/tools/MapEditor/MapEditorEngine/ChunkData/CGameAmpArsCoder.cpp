#include "stdafx.h"
#include "../ChunkData/CGameAmpArsCoder.h"
#include "Render.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "Operator.h"
#include "Operator.inl"
#include "WaterOperator.h"
#include "River.h"
#include "DepathGroupModelInfo.h"
#include "TerrainMeshObject.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "CWindowTarget.h"
#include "CGraphic.h"
#include "CEditTerrainMgr.h"
#include "SceneRegionMgr.h"
#include "SceneRegion.h"
#include "LoadMapMgr.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "CMapEffect.h"
#include "ToolSetModel.h"
#include "LocalVersion.h"
#include "LocalMapResMgr.h"
#include "TraceHelper.h"
#include "CEditDataScene.h"
#include "CEditContext.h"
#include "IoSystem.h"

//////////////////////////////////////////////////////////////////////////
BEGIN_VERSION_MAP(CGameAmbientCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CGameAmbientCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh *pTerrain  = CMapEditApp::GetInst()->GetTerrain();
	COperator    *pOperator = CMapEditApp::GetInst()->GetOperator(); 
	// width depth in region
	WORD wWidth = pTerrain->GetWidthInRegion();
	wdi.Write(&wWidth, sizeof(wWidth));
	
	WORD wDepth = pTerrain->GetDepthInRegion();
	wdi.Write(&wDepth, sizeof(wDepth));

	// REGION_SPACE
	BYTE bRegionSpace = (BYTE)REGION_SPACE;
	wdi.Write(&bRegionSpace, sizeof(bRegionSpace));

	// GRID_SPACE
	BYTE bGridSpace = (BYTE)GRID_SPACE;
	wdi.Write(&bGridSpace, sizeof(bGridSpace));

	//////////////////////////////////////////////////////////////////////////
	static const basic_string <TCHAR>::size_type npos = -1;

	for (size_t i = 0; i < 4; ++i)
	{
		// light
		DWORD dwAmbientColor = CSceneEnvMgr::GetInst()->GetSceneAmbientColor();
		wdi.Write(&dwAmbientColor, sizeof(dwAmbientColor));

		DWORD dwShadowColor = CSceneEnvMgr::GetInst()->GetShadowColor();
		wdi.Write(&dwShadowColor, sizeof(dwShadowColor));

		DWORD dwSunColor = CSceneEnvMgr::GetInst()->GetSceneAmbientColor();
		wdi.Write(&dwSunColor, sizeof(dwSunColor));

		CVector3f vSunDir = CSceneEnvMgr::GetInst()->GetDirDir();
		wdi.Write(&vSunDir, sizeof(vSunDir));

		DWORD dwPlayerSunColor = CSceneEnvMgr::GetInst()->GetPlayerDirColor();
		wdi.Write(&dwPlayerSunColor, sizeof(dwPlayerSunColor));

		DWORD dwPlayerColor = CSceneEnvMgr::GetInst()->GetPlayerAmbientColor();
		wdi.Write(&dwPlayerColor, sizeof(dwPlayerColor));

		// fog
		DWORD dwFogColor = CSceneEnvMgr::GetInst()->GetFogColor();
		wdi.Write(&dwFogColor, sizeof(dwFogColor));

		float fFogStart = CSceneEnvMgr::GetInst()->GetFogStart();
		wdi.Write(&fFogStart, sizeof(fFogStart));

		float fFogEnd = CSceneEnvMgr::GetInst()->GetFogEnd();
		wdi.Write(&fFogEnd, sizeof(fFogEnd));

		float fFogDensity = CSceneEnvMgr::GetInst()->GetDensity();
		wdi.Write(&fFogDensity, sizeof(fFogDensity));
	}

	string strTextureSkyBack = CSceneEnvMgr::GetInst()->GetSkyFarBackTextureName();
	size_t nfind = strTextureSkyBack.find("tile\\天空\\back.dds");
	if ( nfind != -1 )
		strTextureSkyBack = "";

	if( strTextureSkyBack != "" )
	{
		nfind = strTextureSkyBack.rfind(".");
		strTextureSkyBack = strTextureSkyBack.substr(0, nfind);
		strTextureSkyBack = strTextureSkyBack + ".dds";		
		ITexture* pTexture = NULL;
		CRenderDevice* pRender = CRenderDevice::GetInst();
		pTexture = pRender->GetTexture(strTextureSkyBack);
		if( pTexture == NULL )
		{
			string log = strTextureSkyBack + " 背景图不存在";

			MessageBox(NULL,log.c_str(),"错误",MB_OK);
		}
	}

	size_t dwLen = strTextureSkyBack.length();
	wdi.Write(&dwLen,		sizeof(dwLen));
	wdi.Write(strTextureSkyBack.c_str(), dwLen);
	//////////////////////////////////////////////////////////////////////////

	// wave info
	CWaterOperator* pWaterOperator = CMapEditApp::GetInst()->GetWaterOperator(); 
	int nWavePerGrid = pWaterOperator->GetWaveDensity();
	wdi.Write(&nWavePerGrid, sizeof(nWavePerGrid));

	int nWaveSize = pWaterOperator->GetWaveSize();
	wdi.Write(&nWaveSize, sizeof(nWaveSize));

	int nWaveOff = pWaterOperator->GetWaveOff();
	wdi.Write(&nWaveOff, sizeof(nWaveOff));

	float fWaveSpeed = pWaterOperator->GetWaveSpeedForClient();
	wdi.Write(&fWaveSpeed, sizeof(fWaveSpeed));

	int nWaveLife = pWaterOperator->GetWaveLifeForClient();
	wdi.Write(&nWaveLife, sizeof(nWaveLife));

	string ss("tile/水/wave.dds");
	int n = ss.length();
	wdi.Write(&n,		sizeof(n));
	wdi.Write(ss.c_str(), n);

	ss = "fx/setting/dimple.efx";
	n = ss.length();
	wdi.Write(&n,		sizeof(n));
	wdi.Write(ss.c_str(), n);

	ss = "wave/move";
	n = ss.length();
	wdi.Write(&n,		sizeof(n));
	wdi.Write(ss.c_str(), n);

	ss = "fx/setting/dimple.efx";
	n = ss.length();
	wdi.Write(&n,		sizeof(n));
	wdi.Write(ss.c_str(), n);

	ss = "wave/stand";
	n = ss.length();
	wdi.Write(&n,		sizeof(n));
	wdi.Write(ss.c_str(), n);

	//////////////////////////////////////////////////////////////////////////

	// background music
	string str = CSceneEnvMgr::GetInst()->GetBackGroundMusic();
	DWORD dwFileNameLen = str.length();
	wdi.Write(&dwFileNameLen,		sizeof(dwFileNameLen));
	wdi.Write(str.c_str(), dwFileNameLen);

	int nMusicInterval = 0;
	wdi.Write(&nMusicInterval, sizeof(nMusicInterval));

	//////////////////////////////////////////////////////////////////////////

	///背景图移动速度信息
	bool bsetbackpic = CSceneEnvMgr::GetInst()->GetISBackPicMove();
	wdi.Write(&bsetbackpic, sizeof(bsetbackpic));
	if (bsetbackpic)
	{
		float move = 0.0f;

		move = CSceneEnvMgr::GetInst()->GetBackPicLeftMoveOffset() * 0.00001f;
		wdi.Write(&move, sizeof(move));

		move = CSceneEnvMgr::GetInst()->GetBackPicRightMoveOffset() * 0.00001f;
		wdi.Write(&move, sizeof(move));

		move = CSceneEnvMgr::GetInst()->GetBackPicTopMoveOffset() * 0.00001f;
		wdi.Write(&move, sizeof(move));

		move = CSceneEnvMgr::GetInst()->GetBackPicBottonMoveOffset() * 0.00001f;
		wdi.Write(&move, sizeof(move));

	}

	//全屏泛光
	float fGaussBrightPass	= CSceneEnvMgr::GetInst()->GetGaussBrightPass();
	float fGaussMultiplier	= CSceneEnvMgr::GetInst()->GetGaussMultiplier();
	float fGaussMean		= CSceneEnvMgr::GetInst()->GetGaussMean();
	float fGaussStdDev		= CSceneEnvMgr::GetInst()->GetGaussStdDev();
	float fGaussExposure	= CSceneEnvMgr::GetInst()->GetGaussExposure();
	float fGaussSam			= CSceneEnvMgr::GetInst()->GetGaussSample();

	wdi.Write(&fGaussBrightPass, sizeof(fGaussBrightPass));
	wdi.Write(&fGaussMultiplier, sizeof(fGaussMultiplier));
	wdi.Write(&fGaussMean,		 sizeof(fGaussMean));
	wdi.Write(&fGaussStdDev,	 sizeof(fGaussStdDev));
	wdi.Write(&fGaussExposure,   sizeof(fGaussExposure));
	wdi.Write(&fGaussSam,		 sizeof(fGaussSam));

	//////////////////////////////////////////////////////////////////////////
	//read specular parameter
	DWORD dwColor = 0; 
	float fPower = 0.0f;

	dwColor = CSceneEnvMgr::GetInst()->GetSceneSpecurlarColor();
	wdi.Write(&dwColor, sizeof(dwColor));

	dwColor = CSceneEnvMgr::GetInst()->GetPlayerSpecurlarColor();
	wdi.Write(&dwColor, sizeof(dwColor));

	fPower = CSceneEnvMgr::GetInst()->GetSpecularReflectionPower(0);
	wdi.Write(&fPower, sizeof(fPower));

	fPower = CSceneEnvMgr::GetInst()->GetSpecularReflectionPower(1);
	wdi.Write(&fPower, sizeof(fPower));
	//////////////////////////////////////////////////////////////////////////

	// ambient fx info
	this->SaveAmbientFXInfo(wdi);

	return true;
}

void sqr::CGameAmbientCoder::SaveAmbientFXInfo(WriteDataInf& wdi)
{
	size_t size = CMapEffect::GetInst()->mapAmientUseEffect.size();
	wdi.Write(&size,		sizeof(size));

	map<string, CMapEffect::stAmbientEffectInfo *>::iterator beg = CMapEffect::GetInst()->mapAmientUseEffect.begin();
	map<string, CMapEffect::stAmbientEffectInfo *>::iterator end = CMapEffect::GetInst()->mapAmientUseEffect.end();
	for ( beg; beg != end; ++beg )
	{
		CMapEffect::stAmbientEffectInfo *pAmbinetEffectInfo = beg->second;

		string effectname = pAmbinetEffectInfo->strEffect;
		int    nLen = effectname.length();
	 	wdi.Write(&nLen,			  sizeof(nLen));
	 	wdi.Write(effectname.c_str(), nLen);

		int	   nProbability = pAmbinetEffectInfo->nProbability;
		wdi.Write(&nProbability,	  sizeof(nProbability));
	}
}

bool sqr::CGameAmbientCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	// width depth in region
	WORD wWidthInRegion = 0;
	tData.SafeRead( &wWidthInRegion,	sizeof(wWidthInRegion) );

	WORD wDepthInRegion = 0;
	tData.SafeRead( &wDepthInRegion,	sizeof(wDepthInRegion) );

	CMapEditApp::GetInst()->GetDataScene()->SetWidth( uint16(wWidthInRegion) );
	CMapEditApp::GetInst()->GetDataScene()->SetHeight( uint16(wDepthInRegion) );
	CMapEditApp::GetInst()->GetDataScene()->Create();
	CSyncSystem::BeginSyncFun();

	CMapEditApp::GetInst()->InitNewRenderScene();
	CMapEditApp::GetInst()->GetRenderScene()->InitSceneRes();

	string strEditorResDir = IoSystem::GetInst()->GetRegisterDir("editorRes");
	string strDefaultTerTexName = strEditorResDir + "res\\default.bmp";
	string strDefaultWatTexName = strEditorResDir + "res\\water.dds";
	// create a new terrain
	CTerrainMesh * pTerrain = new CTerrainMesh; 
	pTerrain->Create(wWidthInRegion,wDepthInRegion,strDefaultTerTexName,false,false);
	CMapEditApp::GetInst()->SetTerrainMesh(pTerrain);
	CTerrainMesh * pWater = new CTerrainMesh; 
	pWater->Create(wWidthInRegion,wDepthInRegion,strDefaultWatTexName,true,false);
	pTerrain->SetWater(pWater);
	pTerrain->SetTerrainVersion(CLoadSavedMapMgr::GetInst()->dwLocalVersion);

	CMapEditApp::GetInst()->GetOperator()->InitProcessParams(false,/*version*/0);

	// REGION_SPACE
	BYTE bRegionSpace = 0;
	tData.SafeRead( &bRegionSpace,	sizeof(bRegionSpace) );

	// GRID_SPACE
	BYTE bGridSpace = 0;
	tData.SafeRead( &bGridSpace,	sizeof(bGridSpace) );

	//////////////////////////////////////////////////////////////////////////
	static const basic_string <TCHAR>::size_type npos = -1;

	for (size_t i = 0; i < 4; ++i)
	{
		// light
		DWORD dwAmbientColor = 0;
		tData.SafeRead( &dwAmbientColor,	sizeof(dwAmbientColor) );
		CSceneEnvMgr::GetInst()->SetSceneAmbientColor(dwAmbientColor);

		DWORD dwShadowColor = 0;
		tData.SafeRead( &dwShadowColor,	sizeof(dwShadowColor) );
		CSceneEnvMgr::GetInst()->SetShadowColor(dwShadowColor);

		DWORD dwSunColor = 0;
		tData.SafeRead( &dwSunColor,	sizeof(dwSunColor) );
		CSceneEnvMgr::GetInst()->SetSceneDirColor(dwSunColor);

		CVector3f vSunDir;
		tData.SafeRead( &vSunDir,	sizeof(vSunDir) );
		CSceneEnvMgr::GetInst()->SetDirDir(vSunDir);

		DWORD dwPlayerSunColor = 0;
		tData.SafeRead( &dwPlayerSunColor,	sizeof(dwPlayerSunColor) );
		CSceneEnvMgr::GetInst()->SetPlayerDirColor(dwPlayerSunColor);

		DWORD dwPlayerColor = 0;
		tData.SafeRead( &dwPlayerColor,	sizeof(dwPlayerColor) );
		CSceneEnvMgr::GetInst()->SetPlayerAmbientColor(dwPlayerColor);

		// fog
		DWORD dwFogColor = 0;
		tData.SafeRead( &dwFogColor,	sizeof(dwFogColor) );
		CSceneEnvMgr::GetInst()->SetFogColor(dwFogColor);

		float fFogStart = 0.0f;
		tData.SafeRead( &fFogStart,	sizeof(fFogStart) );
		CSceneEnvMgr::GetInst()->SetFogStart(fFogStart);

		float fFogEnd = 0.0f;
		tData.SafeRead( &fFogEnd,	sizeof(fFogEnd) );
		CSceneEnvMgr::GetInst()->SetFogEnd(fFogEnd);
		
		float fFogDensity = 0.0f;
		tData.SafeRead( &fFogDensity,	sizeof(fFogDensity) );
		CSceneEnvMgr::GetInst()->SetDensity(fFogDensity);
	}

	DWORD dwColor = CSceneEnvMgr::GetInst()->GetSceneAmbientColor();
	CSceneEnvMgr::GetInst()->SetSceneAmbientColor(dwColor);

	dwColor = CSceneEnvMgr::GetInst()->GetSceneDirColor();
	CSceneEnvMgr::GetInst()->SetSceneDirColor(dwColor);

	dwColor = CSceneEnvMgr::GetInst()->GetPlayerDirColor();
	CSceneEnvMgr::GetInst()->SetPlayerDirColor(dwColor);

	dwColor = CSceneEnvMgr::GetInst()->GetPlayerAmbientColor();
	CSceneEnvMgr::GetInst()->SetPlayerAmbientColor(dwColor);

	dwColor = CSceneEnvMgr::GetInst()->GetFogColor();
	CSceneEnvMgr::GetInst()->SetFogColor(dwColor);

	string strTextureSkyBack = "";
	size_t dwLen = 0;
	tData.SafeRead( &dwLen,	  sizeof(dwLen) );
	strTextureSkyBack.resize(dwLen);
	tData.SafeRead( &strTextureSkyBack[0], dwLen );
	CSceneEnvMgr::GetInst()->SetSkyFarBackTextureName(strTextureSkyBack);
	//////////////////////////////////////////////////////////////////////////

	// wave info
	int nWavePerGrid = 0;
	tData.SafeRead( &nWavePerGrid,	  sizeof(nWavePerGrid) );

	int nWaveSize = 0;
	tData.SafeRead( &nWaveSize,		  sizeof(nWaveSize) );

	int nWaveOff = 0;
	tData.SafeRead( &nWaveOff,	  sizeof(nWaveOff) );

	float fWaveSpeed = 0;
	tData.SafeRead( &fWaveSpeed,	  sizeof(fWaveSpeed) );

	int nWaveLife = 0;
	tData.SafeRead( &nWaveLife,	  sizeof(nWaveLife) );

	string ss = "";
	int n = 0;
	tData.SafeRead( &n,	  sizeof(n) );
	ss.resize(n);
	tData.SafeRead( &ss[0], n );

	ss = "";
	tData.SafeRead( &n,	  sizeof(n) );
	ss.resize(n);
	tData.SafeRead( &ss[0], n );

	ss = "";
	tData.SafeRead( &n,	  sizeof(n) );
	ss.resize(n);
	tData.SafeRead( &ss[0], n );

	ss = "";
	tData.SafeRead( &n,	  sizeof(n) );
	ss.resize(n);
	tData.SafeRead( &ss[0], n );

	ss = "";
	tData.SafeRead( &n,	  sizeof(n) );
	ss.resize(n);
	tData.SafeRead( &ss[0], n );

	//////////////////////////////////////////////////////////////////////////

	// background music
	string str = "";
	DWORD dwFileNameLen = 0;
	tData.SafeRead( &dwFileNameLen,	  sizeof(dwFileNameLen) );
	str.resize(dwFileNameLen);
	tData.SafeRead( &str[0], dwFileNameLen );
	CSceneEnvMgr::GetInst()->SetBackGroundMusic(str, false);

	int nMusicInterval = 0;
	tData.SafeRead( &nMusicInterval,	  sizeof(nMusicInterval) );
	//////////////////////////////////////////////////////////////////////////

	///背景图移动速度信息
	bool bsetbackpic = false;
	tData.SafeRead( &bsetbackpic,	  sizeof(bsetbackpic) );
	CSceneEnvMgr::GetInst()->SetIsBackPicMove(bsetbackpic);
	if (bsetbackpic)
	{
		float move = 0.0f;
		tData.SafeRead( &move,	  sizeof(move) );
		move = move / 0.00001f;
		CSceneEnvMgr::GetInst()->SetBackPicLeftMoveOffset(move);

		tData.SafeRead( &move,	  sizeof(move) );
		move = move / 0.00001f;
		CSceneEnvMgr::GetInst()->SetBackPicRightMoveOffset(move);

		tData.SafeRead( &move,	  sizeof(move) );
		move = move / 0.00001f;
		CSceneEnvMgr::GetInst()->SetBackPicTopMoveOffset(move);

		tData.SafeRead( &move,	  sizeof(move) );
		move = move / 0.00001f;
		CSceneEnvMgr::GetInst()->SetBackPicBottonMoveOffset(move);
	}

	//全屏泛光
	float fGaussBrightPass	= CSceneEnvMgr::GetInst()->GetGaussBrightPass();
	float fGaussMultiplier	= CSceneEnvMgr::GetInst()->GetGaussMultiplier();
	float fGaussMean		= CSceneEnvMgr::GetInst()->GetGaussMean();
	float fGaussStdDev		= CSceneEnvMgr::GetInst()->GetGaussStdDev();
	float fGaussExposure	= CSceneEnvMgr::GetInst()->GetGaussExposure();
	float fGaussSam			= CSceneEnvMgr::GetInst()->GetGaussSample();

	tData.SafeRead(&fGaussBrightPass, sizeof(fGaussBrightPass));
	tData.SafeRead(&fGaussMultiplier, sizeof(fGaussMultiplier));
	tData.SafeRead(&fGaussMean,		  sizeof(fGaussMean));
	tData.SafeRead(&fGaussStdDev,	  sizeof(fGaussStdDev));
	tData.SafeRead(&fGaussExposure,   sizeof(fGaussExposure));
	tData.SafeRead(&fGaussSam,		  sizeof(fGaussSam));
	
	//////////////////////////////////////////////////////////////////////////
	//read specular parameter
	dwColor = 0; 
	float fPower = 0.0f;

	tData.SafeRead( &dwColor,	  sizeof(dwColor) );
	CSceneEnvMgr::GetInst()->SetSceneSpecurlarColor(dwColor);

	dwColor = CSceneEnvMgr::GetInst()->GetPlayerSpecurlarColor();
	tData.SafeRead( &dwColor,	  sizeof(dwColor) );
	CSceneEnvMgr::GetInst()->SetPlayerSpecurlarColor(dwColor);
	
	fPower = CSceneEnvMgr::GetInst()->GetSpecularReflectionPower(0);
	tData.SafeRead( &fPower,	  sizeof(fPower) );
	CSceneEnvMgr::GetInst()->SetSpecularReflectionPower(0, fPower);

	fPower = CSceneEnvMgr::GetInst()->GetSpecularReflectionPower(1);
	tData.SafeRead( &fPower,	  sizeof(fPower) );
	CSceneEnvMgr::GetInst()->SetSpecularReflectionPower(1, fPower);
	//////////////////////////////////////////////////////////////////////////

	// ambient fx info
	this->LoadAmbientFXInfo(tData);

	return true;
}

void sqr::CGameAmbientCoder::LoadAmbientFXInfo(CBufFile& tData)
{
	size_t size = 0;
	tData.SafeRead( &size,	  sizeof(size) );

	for ( size_t i = 0; i < size; ++i )
	{
		string effectname("");
		int    nLen = 0;
		tData.SafeRead( &nLen,	  sizeof(nLen) );
		effectname.resize(nLen);
		tData.SafeRead( &effectname[0], nLen );

		int	   nProbability = 0;
		tData.SafeRead( &nProbability,	  sizeof(nProbability) );

		CMapEffect::GetInst()->AddUseAmbientEffect(effectname, nProbability);

		MoldeSaveProperty sModelSavePro;
		
		sqr::ECenter_Type eCenterType = sqr::ECenter_Type::ECT_CENTER;
		CVector3f vGroupOffset;
		vector<string> veCMapEditModelFileNames;
		veCMapEditModelFileNames.push_back("tile/地编专用/model/effectdummy.mod");
		CToolSetModel::GetInst()->CreateActiveObject(false,sModelSavePro.bVisibleByConfigure,sModelSavePro.bShadow,sModelSavePro.bTranslucent, "effectdummy", veCMapEditModelFileNames, false, eCenterType, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
			sModelSavePro.vRotate.y, 0,sModelSavePro.vRotate.z, 0,sModelSavePro.vRotate.x, 0,sModelSavePro.vScale.x,sModelSavePro.vScale.y,sModelSavePro.vScale.z, vGroupOffset, true, true, false, false);
	
		CTObject *p = CMapEditApp::GetInst()->GetOperator()->GetLastAmbientObject();

			//pUnit->m_bAmbientInUse = true;

		CMapEffect::GetInst()->ToolSetModel_LinkEffect(effectname, CMapEffect::GetInst()->m_IEffectGroupForSceneLight, p);
	}
}
//////////////////////////////////////////////////////////////////////////

BEGIN_VERSION_MAP(CGameRegionLightCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CGameRegionLightCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetTerrain();

	///四分之一区域光保存
	size_t sceneSize = CSceneRegionMgr::GetInst()->m_mapRegion.size();
	wdi.Write(&sceneSize, sizeof(sceneSize));

	string strMusicName = "";
	uint32 uLen			= 0;
	map<string, SceneRegion*>::iterator iter;
	map<string, SceneRegion*>::iterator eiter = CSceneRegionMgr::GetInst()->m_mapRegion.end();
	for ( iter = CSceneRegionMgr::GetInst()->m_mapRegion.begin(); iter != eiter; ++iter )
	{
		CColor color;
		SceneRegion *sceneRegion = NULL;
		sceneRegion = iter->second;
		if ( sceneRegion )
		{
			///scene
			color = sceneRegion->lightParam.globalAmbient;
			wdi.Write(&color.R, sizeof(BYTE));
			wdi.Write(&color.G, sizeof(BYTE));
			wdi.Write(&color.B, sizeof(BYTE));
			DebugOut("save scene ambient %d, %d, %d\n",color.R, color.G, color.B);

			color = sceneRegion->lightParam.globalDiffuse;
			wdi.Write(&color.R, sizeof(BYTE));
			wdi.Write(&color.G, sizeof(BYTE));
			wdi.Write(&color.B, sizeof(BYTE));
			DebugOut("save scene diffuse %d, %d, %d\n",color.R, color.G, color.B);

			color = sceneRegion->lightParam.globalSpecular;
			wdi.Write(&color.R, sizeof(BYTE));
			wdi.Write(&color.G, sizeof(BYTE));
			wdi.Write(&color.B, sizeof(BYTE));

			color = sceneRegion->lightParam.globalShadow;
			wdi.Write(&color.R, sizeof(BYTE));
			wdi.Write(&color.G, sizeof(BYTE));
			wdi.Write(&color.B, sizeof(BYTE));
			DebugOut("save shadow %d, %d, %d\n",color.R, color.G, color.B);

			color = sceneRegion->lightParam.fogColor;
			wdi.Write(&color.R, sizeof(BYTE));
			wdi.Write(&color.G, sizeof(BYTE));
			wdi.Write(&color.B, sizeof(BYTE));
			DebugOut("save fog %d, %d, %d\n",color.R, color.G, color.B);

			///player
			color = sceneRegion->lightParam.playerAmbient;
			wdi.Write(&color.R, sizeof(BYTE));
			wdi.Write(&color.G, sizeof(BYTE));
			wdi.Write(&color.B, sizeof(BYTE));
			DebugOut("save player ambient %d, %d, %d\n",color.R, color.G, color.B);

			color = sceneRegion->lightParam.playerDiffuse;
			wdi.Write(&color.R, sizeof(BYTE));
			wdi.Write(&color.G, sizeof(BYTE));
			wdi.Write(&color.B, sizeof(BYTE));
			DebugOut("save player diffuse %d, %d, %d\n",color.R, color.G, color.B);

			color = sceneRegion->lightParam.playerSpecular;
			wdi.Write(&color.R, sizeof(BYTE));
			wdi.Write(&color.G, sizeof(BYTE));
			wdi.Write(&color.B, sizeof(BYTE));

			///fog param
			uint8 uFogMode	  = sceneRegion->GetSceneRgnFogType();
			float fFogStart   = sceneRegion->GetSceneRgnFogStart();
			float fFogEnd	  = sceneRegion->GetSceneRgnFogEnd();
			float fFogDensity = sceneRegion->GetSceneRgnFogDensity();

			wdi.Write(&uFogMode,	sizeof(uFogMode));
			wdi.Write(&fFogStart,	sizeof(fFogStart));
			wdi.Write(&fFogEnd,		sizeof(fFogEnd));
			wdi.Write(&fFogDensity, sizeof(fFogDensity));

			///music name
			strMusicName = sceneRegion->GetSceneRgnMusicName();
			uLen		 = strMusicName.length();

			wdi.Write(&uLen,			     sizeof(uLen));
			wdi.Write(strMusicName.c_str(),  uLen);
		}
	}

	return true;
}

bool sqr::CGameRegionLightCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	size_t sceneSize = 0;
	tData.SafeRead( &sceneSize, sizeof(sceneSize) );

	uint8 uFogMode = 0;
	float fFogStart   = 0.0f;
	float fFogEnd	  = 0.0f;
	float fFogDensity = 0.0f;
	string strMusicName = "";
	uint32 uLen			= 0;

	size_t size = CSceneRegionMgr::GetInst()->m_mapRegion.size(), j = 0;
	map<string, SceneRegion*>::iterator iter;
	for( iter = CSceneRegionMgr::GetInst()->m_mapRegion.begin(); j < sceneSize; ++j, ++iter )
	{
		CColor color;

		SceneRegion *pSceneRegion = iter->second;

		DebugOut("region %s\n", iter->first.c_str());

		tData.SafeRead(&color.R, sizeof(BYTE));
		tData.SafeRead(&color.G, sizeof(BYTE));
		tData.SafeRead(&color.B, sizeof(BYTE));
		pSceneRegion->lightParam.globalAmbient = color;

		tData.SafeRead(&color.R, sizeof(BYTE));
		tData.SafeRead(&color.G, sizeof(BYTE));
		tData.SafeRead(&color.B, sizeof(BYTE));
		pSceneRegion->lightParam.globalDiffuse = color;

		tData.SafeRead(&color.R, sizeof(BYTE));
		tData.SafeRead(&color.G, sizeof(BYTE));
		tData.SafeRead(&color.B, sizeof(BYTE));
		pSceneRegion->lightParam.globalSpecular = color;

		tData.SafeRead(&color.R, sizeof(BYTE));
		tData.SafeRead(&color.G, sizeof(BYTE));
		tData.SafeRead(&color.B, sizeof(BYTE));
		pSceneRegion->lightParam.globalShadow = color;
		
		DebugOut("load shadow %d, %d, %d\n",color.R, color.G, color.B);

		tData.SafeRead(&color.R, sizeof(BYTE));
		tData.SafeRead(&color.G, sizeof(BYTE));
		tData.SafeRead(&color.B, sizeof(BYTE));
		pSceneRegion->lightParam.fogColor = color;

		DebugOut("load fog %d, %d, %d\n",color.R, color.G, color.B);

		///player
		tData.SafeRead(&color.R, sizeof(BYTE));
		tData.SafeRead(&color.G, sizeof(BYTE));
		tData.SafeRead(&color.B, sizeof(BYTE));
		pSceneRegion->lightParam.playerAmbient = color;

		tData.SafeRead(&color.R, sizeof(BYTE));
		tData.SafeRead(&color.G, sizeof(BYTE));
		tData.SafeRead(&color.B, sizeof(BYTE));
		pSceneRegion->lightParam.playerDiffuse = color;

		tData.SafeRead(&color.R, sizeof(BYTE));
		tData.SafeRead(&color.G, sizeof(BYTE));
		tData.SafeRead(&color.B, sizeof(BYTE));
		pSceneRegion->lightParam.playerSpecular = color;

		///fog param
		tData.SafeRead(&uFogMode,	 sizeof(uFogMode));
		pSceneRegion->SetFogType(uFogMode);

		tData.SafeRead(&fFogStart,	 sizeof(fFogStart));
		pSceneRegion->SetFogStart(fFogStart);

		tData.SafeRead(&fFogEnd,	 sizeof(fFogEnd));
		pSceneRegion->SetFogEnd(fFogEnd);

		tData.SafeRead(&fFogDensity, sizeof(fFogDensity));
		pSceneRegion->SetFogDensity(fFogDensity);

		///music
		tData.SafeRead( &uLen,				sizeof(uLen) );
		strMusicName.resize(uLen);
		tData.SafeRead( &strMusicName[0],	uLen );
		pSceneRegion->SetSceneRgnMusicName(strMusicName);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
BEGIN_VERSION_MAP(CGameMapResCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CGameMapResCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();

	map< pair<string,int>, int> mapModels;
	map<string ,int> mapAudios;
	vector< vector<string> > vvModelNames;
	vector<string> veCEditModelAnimationGroupNames;
	vector<string> veCEditModelAnimationSkeNames;
	vector<string> veCEditModelEffectNames;
	vector<string> veCEditModelEffectFileNames;
	vector<string> vecCueNames;
	vector<BYTE> veCEditModelShadow;
	vector<BYTE> veCEditModelTranslucent;
	vector<BYTE> veCEditModelFirstShadow;
	vector<BYTE> veCEditModelShadowZTest;
	vector<BYTE> veCEditModelSelfZTest;
	vector<BYTE> veCEditModelRenderStyle;

	static const basic_string <TCHAR>::size_type npos = -1;

	//water texture streams
	this->MakeAtlasTexture();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	DWORD dwWaterTextureCnt = 0;
	CTerrainMesh * pWater = pTerrain->GetWater();
	size_t nRiverCnt = pWater->GetRiverCount();
	///river num
	wdi.Write(&nRiverCnt, sizeof(nRiverCnt));
	for (size_t i = 0; i < nRiverCnt; ++i )
	{
		CRiver * pRiver = pWater->GetRiver(i);
		vector<string> texturenames = pRiver->GetTextureNams(dwWaterTextureCnt);
	
		string name("");
		int	   dwLen = 0;
		DWORD  texCnt = texturenames.size();
		
		///texturen num of this river
		wdi.Write(&texCnt, sizeof(texCnt));
		for ( DWORD m = 0; m < texCnt; ++m )
		{
			name = texturenames[m];
			dwLen = name.size();

			wdi.Write(&dwLen,		sizeof(dwLen));
			wdi.Write(name.c_str(), dwLen);
		}
		dwWaterTextureCnt += texCnt;
	}

	//////////////////////////////////////////////////////////////////////////

	// model streams
	map<string, int> depathModelIndexs;
	map<string, int>::iterator depathModelIndexIter;
	// generate the model map

	int j = 0;
	int nAudioIndex = 0;
	string str = "";
	int nGridCnt = pTerrain->GetGridCount();
	for (int i = 0; i < nGridCnt; ++i )
	{
		SGrid * pGrid = &pTerrain->GetGrid(i);

		DepathModelInfoMap::iterator iter = pTerrain->m_DepathModelMap.find(i);
		if ( iter != pTerrain->m_DepathModelMap.end() )
		{
			const DepathModelInfoArray& depathModelInfoList = iter->second;
			size_t depathModelSize = depathModelInfoList.size();
			for ( size_t mm = 0; mm < depathModelSize; ++mm )
			{
				const DepathModelInfoPtr& pDepathModelInfo = depathModelInfoList[mm];

				string str = pDepathModelInfo->strItemName;
				int nRenderstyle = (int)pDepathModelInfo->byRenderStyle;

				if ( str == "effectdummy" )
					str = pDepathModelInfo->effectName;

				map< pair<string,int>, int>::iterator iterModel = mapModels.find(make_pair(str,nRenderstyle));

				if ( iterModel == mapModels.end() )
				{
					mapModels[make_pair(str,nRenderstyle)] = j;

					vector<string> v;
					// arp
					if ( pDepathModelInfo->strItemName == "effectdummy" )
						v.push_back("");
					else
						v = pDepathModelInfo->modelNames;
					vvModelNames.push_back(v);

					//////////////////////////////////////////////////////////////////////////
					///anitmation
					// agp
					veCEditModelAnimationGroupNames.push_back(pDepathModelInfo->agpName);
					// ske
					veCEditModelAnimationSkeNames.push_back(pDepathModelInfo->skeName);
					//////////////////////////////////////////////////////////////////////////

					// are
					string temp = pDepathModelInfo->effectName;
					if ( temp != "" )
					{
						veCEditModelEffectFileNames.push_back(pDepathModelInfo->effectFileName);
						veCEditModelEffectNames.push_back(temp);
					}
					else
					{
						veCEditModelEffectFileNames.push_back("");
						veCEditModelEffectNames.push_back("");
					}

					// shadow
					veCEditModelShadow.push_back(pDepathModelInfo->bShadow ? 1 : 0);
					// translucent
					veCEditModelTranslucent.push_back(pDepathModelInfo->bTranslucent ? 1 : 0);
					// first shadow
					veCEditModelFirstShadow.push_back(pDepathModelInfo->bSecondShadow ? 0 : 1);
					// shadow z test
					veCEditModelShadowZTest.push_back(pDepathModelInfo->bZTest ? 1 : 0);
					// self z test
					veCEditModelSelfZTest.push_back(pDepathModelInfo->bSelfZTest ? 1 : 0);

					// render style
					BYTE byRenderStyle = pDepathModelInfo->byRenderStyle;
					if ( byRenderStyle > 15 )
						::MessageBox(0,"错误 渲染类型太大","",MB_OK);

					veCEditModelRenderStyle.push_back(byRenderStyle);
					pDepathModelInfo->nModelIndex = j;
					depathModelIndexs[str] = j;
					++j;
				}
				else
				{
					depathModelIndexIter = depathModelIndexs.find(str);
					if ( depathModelIndexIter != depathModelIndexs.end() )
						pDepathModelInfo->nModelIndex = depathModelIndexIter->second;
					else
						pDepathModelInfo->nModelIndex = j;
				}
			}
		}
		if ( !pGrid->vecCenterObjects.empty() )
		{
			int ngridIndex = pGrid->dwGridIndex;
			int ngridsetIndex = 0;
			for ( vector< CTObjectPtr >::iterator iter = pGrid->vecCenterObjects.begin(); iter != pGrid->vecCenterObjects.end(); ++iter )
			{
				++ngridsetIndex;
				CTObject * p = (*iter);
				CEditModelGroup *pModelGroup = p->GetModelGroup();
				//////////////////////////////////////////////////////////////////////////
				if ( pModelGroup->GetIsSubUnit() )
					continue;

				if( pModelGroup->m_pSceneAudio )
				{
					string strAudioName = pModelGroup->GetSceneCueName();
					map<string, int>::iterator iterAudio = mapAudios.find(strAudioName);
					if( iterAudio == mapAudios.end() )
					{
						mapAudios[strAudioName] = nAudioIndex;
						vecCueNames.push_back(strAudioName);
						p->SetResCueIndex(nAudioIndex);

						++nAudioIndex;
					}
					else
						p->SetResCueIndex(iterAudio->second);

					continue;
				}

				//////////////////////////////////////////////////////////////////////////

				string str = pModelGroup->GetItemName();

				if ( str == "effectdummy" )
					pModelGroup->GetEffectName(str);

				int nRenderstyle = pModelGroup->GetRenderStyle();

				map< pair<string,int>, int>::iterator iterModel = mapModels.find(make_pair(str,nRenderstyle));

				if ( iterModel == mapModels.end() )
				{
					mapModels[make_pair(str,nRenderstyle)] = j;

					vector<string> v;

					// arp
					if ( pModelGroup->GetItemName() != "effectdummy" )
					{
						v = pModelGroup->GetModelFileNames();
					}
					else
					{
						v.push_back("");
					}

					vvModelNames.push_back(v);

					//////////////////////////////////////////////////////////////////////////
					///anitmation
					// agp
					veCEditModelAnimationGroupNames.push_back(pModelGroup->GetAnimationGroupName());
					// ske
					veCEditModelAnimationSkeNames.push_back(pModelGroup->GetAnimationSkeName());
					//////////////////////////////////////////////////////////////////////////

					// are
					string temp = "", effectname = "";

					if ( pModelGroup->GetEffectFileName(temp) )
					{
						veCEditModelEffectFileNames.push_back(temp);
						if( pModelGroup->GetEffectName(effectname) )
							veCEditModelEffectNames.push_back(effectname);
					}
					else
					{
						veCEditModelEffectFileNames.push_back("");
						veCEditModelEffectNames.push_back("");
					}

					// shadow
					veCEditModelShadow.push_back(p->IsCastShadow() ? 1 : 0);
					// translucent
					veCEditModelTranslucent.push_back(0);
					// first shadow
					veCEditModelFirstShadow.push_back(p->GetModelGroup()->GetShadowSecondPass() ? 0 : 1);
					// shadow z test
					veCEditModelShadowZTest.push_back(0 ? 1 : 0);
					// self z test
					veCEditModelSelfZTest.push_back(0 ? 1 : 0);

					// render style
					BYTE byRenderStyle = pModelGroup->GetRenderStyle();

					if ( byRenderStyle > 15 )
					{
						::MessageBox(0,"错误 渲染类型太大","",MB_OK);
					}

					veCEditModelRenderStyle.push_back(byRenderStyle);

					p->SetResSetIndex(j);

					++j;
				}
				else
				{
					p->SetResSetIndex(iterModel->second);
				}
			}
		}
	}

	// component cnt
	size_t modelsszie = vvModelNames.size();
	wdi.Write(&modelsszie, sizeof(modelsszie));

	for (size_t i = 0; i < modelsszie; ++i )
	{
		const vector<string>& vec = vvModelNames[i];

		BYTE byIndividualCnt = 0;

		for ( size_t k = 0; k < vec.size(); ++k )
		{
			// mod
			if ( !vec[k].empty() )
				++byIndividualCnt;
		}

		// ani
		if ( !veCEditModelAnimationGroupNames[i].empty() ) 
			++byIndividualCnt;

		// effect
		if ( !veCEditModelEffectFileNames[i].empty() )
			++byIndividualCnt;

		BYTE by = veCEditModelRenderStyle[i];

		///rs index
		wdi.Write(&by, sizeof(by));

		// write cnt模型+动作+特效个数总和，根据标志进行判断是模型，还是动作或者是特效
		wdi.Write(&byIndividualCnt, sizeof(byIndividualCnt));

		//DebugOut("i = %d, cnt = %d\n", i, byIndividualCnt);

		// write mod
		size_t sizemodel = vec.size();
		for ( size_t j2 = 0; j2 < sizemodel; ++j2 )
		{
			//fwrite("ARP",4,1,fpResSet);
			if ( !vec[j2].empty() )
			{
				wdi.Write(&"MOD", 4);

				string str = vec[j2];
				int n = str.length();
				wdi.Write(&n,			sizeof(n));
				wdi.Write(str.c_str(),  n);

				//DebugOut("mod %s\n", str.c_str());
			}
		}

		// write ani
		if ( !veCEditModelAnimationGroupNames[i].empty() )
		{
			//fwrite("SKE",4,1,fpResSet);
			wdi.Write(&"SKE", 4);
			
			string str = veCEditModelAnimationGroupNames[i];
			int n = str.length();
			wdi.Write(&n,			sizeof(n));
			wdi.Write(str.c_str(),  n);

			//DebugOut("ske %s\n", str.c_str());
		}

		// write effect
		if ( !veCEditModelEffectFileNames[i].empty() )
		{
			//fwrite("EFX",4,1,fpResSet);
			wdi.Write(&"EFX", 4);
		
			string str = veCEditModelEffectFileNames[i];
			int n = str.length();
			wdi.Write(&n,			sizeof(n));
			wdi.Write(str.c_str(),  n);

			//DebugOut("effectfull %s\n", str.c_str());

			str = veCEditModelEffectNames[i];
			n = str.length();
			wdi.Write(&n,			sizeof(n));
			wdi.Write(str.c_str(),  n);

			//DebugOut("effectname %s\n", str.c_str());
		}
	}

	//////////////////////////////////////////////////////////////////////////
	///audio info
	if( nAudioIndex != mapAudios.size() )
	{
		MessageBox(NULL,"客户端音源信息有问题","提示",MB_OK);
	}

	string strAudio = "";
	int nLen = 0;

	size_t size = vecCueNames.size();
	wdi.Write(&size, sizeof(size));
	for ( size_t i = 0; i < size; ++i )
	{
		strAudio = vecCueNames[i];
		nLen = strAudio.length();
	
		wdi.Write(&nLen,			sizeof(nLen));
		wdi.Write(strAudio.c_str(), nLen);
	}
	//////////////////////////////////////////////////////////////////////////

	return true;
}

bool sqr::CGameMapResCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	vector<string> vecRiverTexNames;

	size_t nRiverCnt = 0;
	tData.SafeRead( &nRiverCnt, sizeof(nRiverCnt) );

	for( size_t i = 0; i < nRiverCnt; ++i )
	{
		string name("");
		int    nLen = 0;
		DWORD  texCnt = 0;
		tData.SafeRead( &texCnt, sizeof(texCnt) );

		for ( DWORD i = 0; i < texCnt; ++i )
		{		
			tData.SafeRead( &nLen,	  sizeof(nLen) );
			name.resize(nLen);
			tData.SafeRead( &name[0], nLen );

			CLoadMapResInfo::GetInst()->m_vecWaterTextureNames.push_back(name);
		}
	}
	
	vector<CLoadMapResInfo::CTerrainSetUnit> &SetUnit = CLoadMapResInfo::GetInst()->m_SetUnit;

	int nSetUnitNum = 0;
	tData.SafeRead( &nSetUnitNum, sizeof(nSetUnitNum) );
	SetUnit.resize(nSetUnitNum);

	for( int i = 0; i < nSetUnitNum; i++ )
	{
		bool bARA = false;
		uint8 nUnitPartNum;
		uint8 nRSIndex;
	
		tData.SafeRead( &nRSIndex, sizeof(nRSIndex) );
		tData.SafeRead( &nUnitPartNum, sizeof(nUnitPartNum) );

		CLoadMapResInfo::CREATE_ITEM AnimationItem;
		vector<CLoadMapResInfo::CREATE_ITEM> EffectItem;

		//DebugOut("i = %d, cnt = %d\n", i, nUnitPartNum);

		for( uint8 j = 0; j < nUnitPartNum; j++ )
		{
			string name("");
			int    nLen = 0;
			CLoadMapResInfo::CREATE_ITEM CreateItem;

			tData.SafeRead( &CreateItem.dwResType, sizeof(CreateItem.dwResType) );
		
			tData.SafeRead( &nLen,	  sizeof(nLen) );
			name.resize(nLen);
			tData.SafeRead( &name[0], nLen );
			CreateItem.szFileName = name;

			//DebugOut("%s\n", name.c_str());

			switch( CreateItem.dwResType )
			{
				case MAKEFOURCC( 'M', 'O', 'D', 0 ):
				CreateItem.szClassName = "";
				CreateItem.szParam.assign( (char*)&nRSIndex, sizeof( nRSIndex ) );
				SetUnit[i].m_UnitPart.push_back( CLoadMapResInfo::OBJECT_CREATE_PARAM() );
				SetUnit[i].m_UnitPart[ SetUnit[i].m_UnitPart.size() - 1 ].Items.push_back( CreateItem );
				break;
			case MAKEFOURCC( 'S', 'K', 'E', 0 ):
				CreateItem.szClassName = "RFT_ARA";
				bARA = true;
				AnimationItem = CreateItem;
				break;
			case MAKEFOURCC( 'E', 'F', 'X', 0 ):
				CreateItem.szClassName = "RFT_ARE";
				tData.SafeRead( &nLen,	  sizeof(nLen) );
				name.resize(nLen);
				tData.SafeRead( &name[0], nLen );
				//DebugOut("effect %s\n", name.c_str());
				CreateItem.szParam = name;
				EffectItem.push_back( CreateItem );
				break;
			default:
				break;
			}
		}

		if( SetUnit[i].m_UnitPart.size() == 0 )
			SetUnit[i].m_UnitPart.push_back( CLoadMapResInfo::OBJECT_CREATE_PARAM() );

		if( bARA )
		{
			for( int nArp = 0; nArp < (int)SetUnit[i].m_UnitPart.size(); ++nArp )
				SetUnit[i].m_UnitPart[nArp].Items.push_back( AnimationItem );
		}

		for( size_t nFx = 0; nFx < EffectItem.size(); ++nFx )
			SetUnit[i].m_UnitPart[0].Items.push_back( EffectItem[nFx] );
	}

	int nAudioUnitNum = 0;
	tData.SafeRead( &nAudioUnitNum,	sizeof(nAudioUnitNum) );
	CLoadMapResInfo::GetInst()->m_CueNameUnit.resize( nAudioUnitNum );

	for( int i = 0; i < nAudioUnitNum; ++i )
	{
		string name("");
		int nLen = 0;

		tData.SafeRead( &nLen,	  sizeof(nLen) );
		name.resize(nLen);
		tData.SafeRead( &name[0], nLen );

		CLoadMapResInfo::GetInst()->m_CueNameUnit[i] = name;
	}

	return true;
}

void sqr::CGameMapResCoder::MakeAtlasTexture()
{
	string path;

	CRenderDevice* pRender = CRenderDevice::GetInst();
	//拼接贴图
	map<string, DWORD>::iterator it = pRender->m_TerrainTexIndexs.begin();
	map<string, DWORD>::iterator ite = pRender->m_TerrainTexIndexs.end();
	ITexture* pMainTexture;
	ITexture* pTexture;
	////这些东西写在这里，是因为这个X文件太长了，翻到最上面再翻下来太烦了

	if (!SUCCEEDED(CGraphic::GetInst()->CreateEmptyTexture(
		EXPAND_MAX_SIZE,EXPAND_MAX_SIZE,1, TF_ARGB32,&pMainTexture)))
	{
		string log = "拼接过程中创建主纹理失败";
		MessageBox(NULL,log.c_str(),"错误",MB_OK);
	}

	for (;it!=ite;++it)
	{
		pTexture = pRender->GetTexture(it->first);
		if (NULL==pTexture)
		{
			string log = "拼接过程:"+it->first+"纹理无法找到";
			MessageBox(NULL,log.c_str(),"错误",MB_OK);
			continue;
		}
		//pTempTexture = (LPDIRECT3DTEXTURE9)pTexture->GetBuffer();
		if (NULL==pTexture->GetBuffer())
		{
			string log = "拼接过程:"+it->first+"纹理无法找到";
			MessageBox(NULL,log.c_str(),"错误",MB_OK);
			continue;
		}
		UINT HotIndex = it->second;
		UINT H = (HotIndex/ROW_SIZE)*EXPAND_TEXTURE_SIZE;
		UINT W = (HotIndex%ROW_SIZE)*EXPAND_TEXTURE_SIZE;
		//D3DSURFACE_DESC desc;
		//pTempTexture->GetLevelDesc(0,&desc);
		if (pTexture->GetWidth()!=TEXTURE_SIZE||pTexture->GetHeight()!=TEXTURE_SIZE)
		{
			string log = "拼接过程:"+it->first+"尺寸不合法,所有贴图纹理应为128X128";
			MessageBox(NULL,log.c_str(),"错误",MB_OK);
		}

		CIRect rectT,rectO;
		_TEXLOCKED_RECT lockT,lockO;
		rectO.top = rectO.left = 0;
		rectO.bottom = rectO.right = TEXTURE_SIZE;
		rectT.top = H;
		rectT.bottom = H + EXPAND_TEXTURE_SIZE;
		rectT.left = W;
		rectT.right = W + EXPAND_TEXTURE_SIZE;
		pMainTexture->Lock(0,&lockT,&rectT,0,LOCK_DISCARD);
		pTexture->Lock(0,&lockO,&rectO,0,LOCK_READONLY);
		TCHAR* pTar = (TCHAR*)lockT.pBits;
		TCHAR* pOrc = (TCHAR*)lockO.pBits;

		for (UINT i=0;i<EXPAND_SIZE;++i,pTar+=lockT.Pitch)
		{
			for ( UINT j=0;j<EXPAND_SIZE;++j)
			{
				memcpy(pTar+PixelSize*j,pOrc,PixelSize);
				memcpy(pTar+lockO.Pitch+PixelSize*(j+EXPAND_SIZE),pOrc+lockO.Pitch-PixelSize,PixelSize);
			}
			memcpy(pTar+PixelSize*EXPAND_SIZE,pOrc,lockO.Pitch);
		}

		for (UINT i = 0;i<TEXTURE_SIZE;++i,pTar+=lockT.Pitch,pOrc+=lockO.Pitch)
		{

			for ( UINT j=0;j<EXPAND_SIZE;++j)
			{
				memcpy(pTar+PixelSize*j,pOrc,PixelSize);
				memcpy(pTar+lockO.Pitch+PixelSize*(j+EXPAND_SIZE),pOrc+lockO.Pitch-PixelSize,PixelSize);
			}
			memcpy(pTar+PixelSize*EXPAND_SIZE,pOrc,lockO.Pitch);
		}
		pOrc-=lockO.Pitch;
		for (UINT i=0;i<EXPAND_SIZE;++i,pTar+=lockT.Pitch)
		{
			for ( UINT j=0;j<EXPAND_SIZE;++j)
			{
				memcpy(pTar+PixelSize*j,pOrc,PixelSize);
				memcpy(pTar+lockO.Pitch+PixelSize*(j+EXPAND_SIZE),pOrc+lockO.Pitch-PixelSize,PixelSize);
			}
			memcpy(pTar+PixelSize*EXPAND_SIZE,pOrc,lockO.Pitch);
		}

		pTexture->Unlock(0);
		pMainTexture->Unlock(0);
	}

	string FileName = "map\\" + path + "_c.tex";
	if( CMapEditApp::GetInst()->GetTerrain()->m_pTerrain->m_Texture )
		CMapEditApp::GetInst()->GetTerrain()->m_pTerrain->m_Texture->SaveToFile(FileName.c_str(),IFF_JPG);
	//D3DXSaveTextureToFile(FileName.c_str(),D3DXIFF_JPG,pTerrain->m_pTerrain->m_Texture->GetBuffer(),&pale);
}

class CAnyObject;


CGameAmpArsFormat::CGameAmpArsFormat(void)
: CFileFormatMgr( 'TMFT', 0 )
{
	InitAmpArsCode();
}

CGameAmpArsFormat::~CGameAmpArsFormat(void)
{ /*do nothing~~~*/ }

void sqr::CGameAmpArsFormat::InitAmpArsCode()
{
	RegisterCoder(new CGameAmbientCoder);
	RegisterCoder(new CGameRegionLightCoder);
	RegisterCoder(new CGameMapResCoder);
}

bool sqr::CGameAmpArsFormat::LoadAmpArsFile( const string& strFile )
{
	CAnyObject* pObj = (CAnyObject*)(0xeefefeff); //指向要读取/存储的对象
	return Load( strFile, pObj );
}

bool sqr::CGameAmpArsFormat::SaveAmpArsFile( const string& strFile )
{
	CAnyObject* pObj = (CAnyObject*)(0xeefefeff); //指向要读取/存储的对象
	return Save( strFile, pObj ); 
}