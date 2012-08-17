#include "stdafx.h"
#include "../ChunkData/CEditorAmpArsCoder.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "WaterOperator.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "ToolMainPlayer.h"
#include "CCameraManager.h"
#include "CMapEditCamera.h"
#include "LoadMapMgr.h"
#include "SceneRegionMgr.h"
#include "SceneRegion.h"
#include "LocalVersion.h"
#include "LocalMapResMgr.h"
#include "ToolRecord.h"



//////////////////////////////////////////////////////////////////////////
BEGIN_VERSION_MAP(CEditorAmbientCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()


bool sqr::CEditorAmbientCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	//version
	DWORD dwVersion = LOCAL_VERSION;
	wdi.Write(&dwVersion, sizeof(dwVersion));

	// main player
	this->SaveToolMainPlayerInfo(wdi);

	// camera
	CMapEditCamera * pCamera = CCameraManager::GetInst()->GetMapEditCamera();
	CVector3f vView = pCamera->GetTarget();
	wdi.Write(&vView, sizeof(vView));

	//fov
	float fov = pCamera->GetFovY();
	wdi.Write(&fov, sizeof(fov));

	//yaw
	float yaw = pCamera->GetYaw();
	wdi.Write(&yaw, sizeof(yaw));

	//pitch
	float pitch = pCamera->GetPitch();
	wdi.Write(&pitch, sizeof(pitch));

	//back pic center and scale
	CVector2f center = CSceneEnvMgr::GetInst()->GetBackTextureUVCenter();
	wdi.Write(&center, sizeof(center));

	float scale = CSceneEnvMgr::GetInst()->GetBackTextureScale();
	wdi.Write(&scale, sizeof(scale));

	this->SaveOperationInfo(wdi);

	///wave
	this->SaveWaveInfo(wdi);

	///version
	this->SaveGridModeProVersion(wdi);

	return true;
}

void sqr::CEditorAmbientCoder::SaveOperationInfo( WriteDataInf& wdi )
{
	//////////////////////////////////////////////////////////////////////////
	///一些控件的开关信息
	bool flag = false;

	//是否全局光效
	flag = CSceneEnvMgr::GetInst()->GetIsTurnOnGaussTarget();
	wdi.Write(&flag, sizeof(flag));

	//是否开启主角光效
	flag = false;
	wdi.Write(&flag, sizeof(flag));

	//是否开启雾效
	flag = CSceneEnvMgr::GetInst()->GetDoesEnableFog();
	wdi.Write(&flag, sizeof(flag));

	//是否开启物件透明
	flag = false;
	wdi.Write(&flag, sizeof(flag));

	//是否开启全局视角阴影
	flag = CSceneEnvMgr::GetInst()->GetIsLightMapShadow();
	wdi.Write(&flag, sizeof(flag));

	//是否开启标准视角阴影
	flag = CSceneEnvMgr::GetInst()->GetIsRealtimeShadow();
	wdi.Write(&flag, sizeof(flag));
	//////////////////////////////////////////////////////////////////////////
}

void sqr::CEditorAmbientCoder::SaveToolMainPlayerInfo( WriteDataInf& wdi )
{
	string strMainPlayerModel = CToolMainPlayer::GetInst()->GetMainPlayerModel();
	int dwFileNameLen = strMainPlayerModel.length();
	wdi.Write(&dwFileNameLen,				sizeof(dwFileNameLen));
	wdi.Write(strMainPlayerModel.c_str(),   dwFileNameLen);

	string strMainPlayerAni = CToolMainPlayer::GetInst()->GetMainPlayerAni();
	dwFileNameLen = strMainPlayerAni.length();
	wdi.Write(&dwFileNameLen,			    sizeof(dwFileNameLen));
	wdi.Write(strMainPlayerAni.c_str(),     dwFileNameLen);
}

void sqr::CEditorAmbientCoder::SaveWaveInfo( WriteDataInf& wdi )
{
	CWaterOperator *pWaterOperator = CMapEditApp::GetInst()->GetWaterOperator();

	int nWavePerGrid = pWaterOperator->GetWaveDensity();
	wdi.Write(&nWavePerGrid, sizeof(nWavePerGrid));

	int nWaveSize = pWaterOperator->GetWaveSize();
	wdi.Write(&nWaveSize, sizeof(nWaveSize));

	int nWaveOff = pWaterOperator->GetWaveOff();
	wdi.Write(&nWaveOff, sizeof(nWaveOff));

	float fWaveSpeed = pWaterOperator->GetWaveSpeed();
	wdi.Write(&fWaveSpeed, sizeof(fWaveSpeed));

	int nWaveLife = pWaterOperator->GetWaveLife();
	wdi.Write(&nWaveLife, sizeof(nWaveLife));

	//////////////////////////////////////////////////////////////////////////
	//wavetexture name
	string strWaveName = pWaterOperator->GetWaveTextureName();
	int iWaveNameLen = strWaveName.length();
	wdi.Write(&iWaveNameLen,		sizeof(iWaveNameLen));
	wdi.Write(strWaveName.c_str(),  iWaveNameLen);
}

void sqr::CEditorAmbientCoder::SaveGridModeProVersion(WriteDataInf& wdi)
{
	uint32 uModelVersion = MODELPRO_POSITION;	
	wdi.Write(&uModelVersion, sizeof(uModelVersion));

	uint32 uGridVersion  = GRIDPRO_ENFORCELOWBLOCK;	
	wdi.Write(&uGridVersion, sizeof(uGridVersion));
}

bool sqr::CEditorAmbientCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	//version
	DWORD dwVersion;
	tData.SafeRead( &dwVersion,	sizeof(dwVersion) );
	CLoadSavedMapMgr::GetInst()->dwLocalVersion = dwVersion;

	// main player
	this->LoadToolMainPlayerInfo(tData);

	// camera
	CMapEditCamera * pCamera = CCameraManager::GetInst()->GetMapEditCamera();
	
	CVector3f vView;
	tData.SafeRead( &vView,	sizeof(vView) );
	pCamera->SetNewTarget(vView);

	//fov
	float fov = 0.0f;
	tData.SafeRead( &fov,	sizeof(fov) );
	pCamera->SetFovY(fov);

	//yaw
	float yaw = 0.0f;
	tData.SafeRead( &yaw,	sizeof(yaw) );

	//pitch
	float pitch = pCamera->GetPitch();
	tData.SafeRead( &pitch,	sizeof(pitch) );

	pCamera->SetPitchAndYaw(pitch,yaw);

	CVector2f center;
	tData.SafeRead( &center,	sizeof(center) );
	CSceneEnvMgr::GetInst()->SetBackTextureUVCenter(center);

	float scale = 0.0f;
	tData.SafeRead( &scale,	sizeof(scale) );
	scale *= 100.0f;
	CSceneEnvMgr::GetInst()->SetBackTextureScale((int)scale);

	//////////////////////////////////////////////////////////////////////////
	this->LoadOperationInfo(tData);
	//////////////////////////////////////////////////////////////////////////

	///wave
	this->LoadWaveInfo(tData);

	///version
	this->LoadGridModeProVersion(tData);

	return true;
}

void sqr::CEditorAmbientCoder::LoadOperationInfo( CBufFile& tData )
{
	///一些控件的开关信息
	bool flag = false;

	//是否全局光效
	tData.SafeRead( &flag,	sizeof(flag) );
	CSceneEnvMgr::GetInst()->SetIsTurnOnGaussTarget(flag);

	//是否开启主角光效
	tData.SafeRead( &flag,	sizeof(flag) );

	//是否开启雾效
	tData.SafeRead( &flag,	sizeof(flag) );
	CSceneEnvMgr::GetInst()->SetDoesEnableFog(flag);

	//是否开启物件透明
	tData.SafeRead( &flag,	sizeof(flag) );

	//是否开启全局视角阴影
	tData.SafeRead( &flag,	sizeof(flag) );
	CSceneEnvMgr::GetInst()->ShowLightMap(false);

	//是否开启标准视角阴影
	tData.SafeRead( &flag,	sizeof(flag) );
	CSceneEnvMgr::GetInst()->ShowRealtimeShadow(flag);
}

void sqr::CEditorAmbientCoder::LoadToolMainPlayerInfo( CBufFile& tData )
{
	string strMainPlayerModel;
	int dwFileNameLen = 0;
	tData.SafeRead( &dwFileNameLen,	  sizeof(dwFileNameLen) );
	strMainPlayerModel.resize(dwFileNameLen);
	tData.SafeRead( &strMainPlayerModel[0], dwFileNameLen );
	CToolMainPlayer::GetInst()->SetMainPlayerModel(strMainPlayerModel);

	string strMainPlayerAni;
	dwFileNameLen = 0;
	tData.SafeRead( &dwFileNameLen,	  sizeof(dwFileNameLen) );
	strMainPlayerAni.resize(dwFileNameLen);
	tData.SafeRead( &strMainPlayerAni[0], dwFileNameLen );
	CToolMainPlayer::GetInst()->SetMainPlayerAni(strMainPlayerAni);
}

void sqr::CEditorAmbientCoder::LoadWaveInfo( CBufFile& tData )
{
	CWaterOperator *pWaterOperator = CMapEditApp::GetInst()->GetWaterOperator();

	int nWavePerGrid = 0;
	tData.SafeRead( &nWavePerGrid,	  sizeof(nWavePerGrid) );
	pWaterOperator->SetWaveDensity(nWavePerGrid);

	int nWaveSize = 0;
	tData.SafeRead( &nWaveSize,	  sizeof(nWaveSize) );
	pWaterOperator->SetWaveSize(nWaveSize);

	int nWaveOff = 0;
	tData.SafeRead( &nWaveOff,	  sizeof(nWaveOff) );
	pWaterOperator->SetWaveOff(nWaveOff);

	float fWaveSpeed = 0.0f;
	tData.SafeRead( &fWaveSpeed,	  sizeof(fWaveSpeed) );
	pWaterOperator->SetWaveSpeed(fWaveSpeed);

	int nWaveLife = 0;
	tData.SafeRead( &nWaveLife,	  sizeof(nWaveLife) );
	pWaterOperator->SetWaveLife(nWaveLife);

	//////////////////////////////////////////////////////////////////////////
	//wavetexture name
	string strWaveName = "";
	int iWaveNameLen = 0;
	tData.SafeRead( &iWaveNameLen,	  sizeof(iWaveNameLen) );
	strWaveName.resize(iWaveNameLen);
	tData.SafeRead( &strWaveName[0], iWaveNameLen );
	pWaterOperator->SetWaveTexture(strWaveName);
}

void sqr::CEditorAmbientCoder::LoadGridModeProVersion(CBufFile& tData)
{
	uint32 uModelVersion = MODELPRO_POSITION;	
	tData.SafeRead( &CLoadSavedMapMgr::GetInst()->uModelProVersion, sizeof(uint32) );

	uint32 uGridVersion  = GRIDPRO_ENFORCELOWBLOCK;	
	tData.SafeRead( &CLoadSavedMapMgr::GetInst()->uGridProVersion, sizeof(uint32) );
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
BEGIN_VERSION_MAP(CEditorTileResoureCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()


bool sqr::CEditorTileResoureCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	///图素包集合信息
	this->SaveResoureceSetInfo(wdi);
	return true;
}

bool sqr::CEditorTileResoureCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	this->LoadResoureceSetInfo(tData);
	
	return true;
}

void sqr::CEditorTileResoureCoder::SaveResoureceSetInfo(WriteDataInf& wdi)
{
//	///setview
//#ifdef NEW_MAPEDITOR
//	vector<string> sceneSesetnames = CMapEditorManage::GetInst()->GetMapEditorFrame()->GetResSetView()->GetResSetsName();
//#else
//	vector<string> sceneSesetnames = ((CMainFrame*)AfxGetMainWnd())->GetResSetView()->GetResSetsName();
//#endif
//	string name("");
//	size_t nLen = 0;
//
//	size_t size = sceneSesetnames.size();
//	wdi.Write(&size, sizeof(size));
//	for ( size_t i = 0; i < size; ++i )
//	{
//		name = sceneSesetnames[i];
//		nLen = name.size();
//
//		wdi.Write(&nLen,		sizeof(nLen));
//		wdi.Write(name.c_str(), nLen);
//	}
//
//	///loginview
//#ifdef NEW_MAPEDITOR
//	vector<string> loginResetnames = CMapEditorManage::GetInst()->GetMapEditorFrame()->GetLoginView()->GetResSetsName();
//#else
//	vector<string> loginResetnames = ((CMainFrame*)AfxGetMainWnd())->GetLoginView()->GetResSetsName();
//#endif
//	
//	size = loginResetnames.size();
//	wdi.Write(&size, sizeof(size));
//	for ( size_t i = 0; i < size; ++i )
//	{
//		name = loginResetnames[i];
//		nLen = name.size();
//
//		wdi.Write(&nLen,		sizeof(nLen));
//		wdi.Write(name.c_str(), nLen);
//	}
}

void sqr::CEditorTileResoureCoder::LoadResoureceSetInfo(CBufFile& tData)///地图所用图素包集合
{
//	size_t size = 0;
//
//	///setview
//#ifdef NEW_MAPEDITOR
//	CMapEditorManage::GetInst()->GetMapEditorFrame()->GetResSetView()->Clear();	
//#else
//	((CMainFrame*)AfxGetMainWnd())->GetResSetView()->Clear();
//#endif
//	
//	tData.SafeRead( &size, sizeof(size) );
//	if( size != 0 )
//	{
//		string name("");
//		size_t nLen = 0;
//
//		for ( size_t i = 0; i < size; ++i )
//		{
//			tData.SafeRead( &nLen,	  sizeof(nLen) );
//			name.resize(nLen);
//			tData.SafeRead( &name[0], nLen );
//
//
//			name = "./src/ResourceSet/" + name;
//#ifdef NEW_MAPEDITOR
//	CMapEditorManage::GetInst()->GetMapEditorFrame()->GetResSetView()->AddResSetFromFile(name);
//#else
//	((CMainFrame*)AfxGetMainWnd())->GetResSetView()->AddResSetFromFile(name);
//#endif
//		}
//	}
//
//	///loginview
//#ifdef NEW_MAPEDITOR
//	CMapEditorManage::GetInst()->GetMapEditorFrame()->GetLoginView()->Clear();
//#else
//	((CMainFrame*)AfxGetMainWnd())->GetLoginView()->Clear();
//#endif
//	
//	tData.SafeRead( &size, sizeof(size) );
//	if( size != 0 )
//	{
//		string name("");
//		size_t nLen = 0;
//
//		for ( size_t i = 0; i < size; ++i )
//		{
//			tData.SafeRead( &nLen,	  sizeof(nLen) );
//			name.resize(nLen);
//			tData.SafeRead( &name[0], nLen );
//
//
//			name = "./src/ResourceSet/" + name;
//#ifdef NEW_MAPEDITOR
//	;//CMapEditorManage::GetInst()->GetMapEditorFrame()->GetLoginView()->AddResSetFromFile(name);
//#else
//	((CMainFrame*)AfxGetMainWnd())->GetLoginView()->AddResSetFromFile(name);
//#endif
//		}
//	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
BEGIN_VERSION_MAP(CEditorRegionLightCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()


bool sqr::CEditorRegionLightCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	size_t sceneSize = CSceneRegionMgr::GetInst()->m_mapRegion.size();
	wdi.Write(&sceneSize,			     sizeof(sceneSize));

	string strRegionName = "";
	uint32 uLen			= 0;
	map<string, SceneRegion*>::iterator iter;
	map<string, SceneRegion*>::iterator eiter = CSceneRegionMgr::GetInst()->m_mapRegion.end();
	for ( iter = CSceneRegionMgr::GetInst()->m_mapRegion.begin(); iter != eiter; ++iter )
	{
		strRegionName = iter->first;
		uLen = strRegionName.length();

		wdi.Write(&uLen,			     sizeof(uLen));
		wdi.Write(strRegionName.c_str(), uLen);

		int rgnUnit = 0;
		set<int> RegionUnitList = iter->second->_RegionUnitList;

		size_t size = RegionUnitList.size();
		wdi.Write(&size,			     sizeof(size));

		for ( set<int>::iterator unit = RegionUnitList.begin(); unit != RegionUnitList.end(); ++unit )
		{
			rgnUnit = *unit;
			wdi.Write(&rgnUnit,			 sizeof(rgnUnit));
		}
	}

	return true;
}

bool sqr::CEditorRegionLightCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	
	size_t sceneSize = 0;
	tData.SafeRead( &sceneSize, sizeof(sceneSize) );

	string strRegionName = "";
	uint32 uLen			= 0;

	for( size_t j = 0; j < sceneSize; ++j )
	{
		tData.SafeRead( &uLen,	  sizeof(uLen) );
		strRegionName.resize(uLen);
		tData.SafeRead( &strRegionName[0], uLen );

		int rgnUnit = 0;
		set<int> RegionUnitList;
		size_t size = 0;
		tData.SafeRead( &size,	  sizeof(size) );

		for ( size_t m = 0; m < size; ++m )
		{
			tData.SafeRead( &rgnUnit, sizeof(rgnUnit) );
			RegionUnitList.insert(rgnUnit);
		}

		SceneRegion *pCurSceneRegion = new SceneRegion;

		pCurSceneRegion->_RegionUnitList = RegionUnitList;
		CSceneRegionMgr::GetInst()->AddSceneRegion(strRegionName, pCurSceneRegion);
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////

BEGIN_VERSION_MAP(CEditorMapResCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CEditorMapResCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	string strName = "";
	int    iNameLen = 0;
	int	   nGridCnt = pTerrain->GetGridCount();
	
	map<string, int16> &terrainTextureNames = CLoadMapResInfo::GetInst()->m_mapTerrainTextureNames;
	vector<string>	   vecTerNameList;
	terrainTextureNames.clear();///用于记录地图中用到的所有纹理的名字，保存各层纹理信息则保存index就行
	int16 nTextureIndex = 0;

	for ( int i = 0; i < nGridCnt; ++i )
	{
		SGrid *pGrid = &pTerrain->GetGrid(i);
		strName = pGrid->nodeInfo.strTextureNames[0];

		if ( terrainTextureNames.find(strName) == terrainTextureNames.end() )
		{
			terrainTextureNames[strName] = nTextureIndex;
			vecTerNameList.push_back(strName);

			++nTextureIndex;
		}
		strName = pGrid->nodeInfo.strTextureNames[1];
		if ( terrainTextureNames.find(strName) == terrainTextureNames.end() )
		{
			terrainTextureNames[strName] = nTextureIndex;
			vecTerNameList.push_back(strName);

			++nTextureIndex;
		}

	}

	size_t terrainTextureCnt = terrainTextureNames.size();
	wdi.Write(&terrainTextureCnt, sizeof(terrainTextureCnt));

	map<string, int16>::iterator iter = terrainTextureNames.begin();
	for ( size_t i = 0; i < terrainTextureCnt; ++i, ++iter )
	{
		strName       = vecTerNameList[i];

		iter = terrainTextureNames.find(strName);
		nTextureIndex = iter->second;
		iNameLen = strName.length();
		
		wdi.Write(&iNameLen,	   sizeof(iNameLen));
		wdi.Write(strName.c_str(), iNameLen);
		wdi.Write(&nTextureIndex,  sizeof(nTextureIndex));

		//DebugOut("Save %s, %d\n", strName.c_str(), nTextureIndex);
	}

	return true;
}

bool sqr::CEditorMapResCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	map<string, int16> &terrainTextureNames = CLoadMapResInfo::GetInst()->m_mapTerrainTextureNames;
	map<int16, string> &mapIndexTextureNames = CLoadMapResInfo::GetInst()->m_mapIndexTextureNames;
	terrainTextureNames.clear();

	size_t terrainTextureCnt = 0;
	tData.SafeRead( &terrainTextureCnt, sizeof(terrainTextureCnt) );

	string strName = "";
	int    iNameLen = 0;
	int16 nTextureIndex = 0;

	for (size_t i = 0; i < terrainTextureCnt; ++i )
	{
		tData.SafeRead( &iNameLen,	 sizeof(iNameLen) );
		strName.resize(iNameLen);
		tData.SafeRead( &strName[0], iNameLen );

		tData.SafeRead( &nTextureIndex,	 sizeof(nTextureIndex) );

		terrainTextureNames[strName] = nTextureIndex;
		mapIndexTextureNames[nTextureIndex] = strName;

		//DebugOut("Load %s, %d\n", strName.c_str(), nTextureIndex);
	}
	
	size_t size = CLoadMapResInfo::GetInst()->m_mapTerrainTextureNames.size();
	Ast(size == terrainTextureCnt);
	return true;
}
//////////////////////////////////////////////////////////////////////////
BEGIN_VERSION_MAP(CEditorRecordCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CEditorRecordCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CToolRecord::GetInst()->SaveRecordInfo(&wdi);
	return true;
}

bool sqr::CEditorRecordCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	CToolRecord::GetInst()->LoadRecordInfo(&tData);
	
	return true;
}
//////////////////////////////////////////////////////////////////////////

class CAnyObject;

CEditorAmpArsFormat::CEditorAmpArsFormat(void)
: CFileFormatMgr( 'SMFT', 0 )
{
	InitAmpArsCode();
}

CEditorAmpArsFormat::~CEditorAmpArsFormat(void)
{ /*do nothing~~~*/ }

void sqr::CEditorAmpArsFormat::InitAmpArsCode()
{
	RegisterCoder(new CEditorAmbientCoder);
	RegisterCoder(new CEditorMapResCoder);
	RegisterCoder(new CEditorRegionLightCoder);
	RegisterCoder(new CEditorTileResoureCoder);
	RegisterCoder(new CEditorRecordCoder);
}

bool sqr::CEditorAmpArsFormat::LoadAmpArsFile( const string& strFile )
{
	CAnyObject* pObj = (CAnyObject*)(0xeefefeff); //指向要读取/存储的对象
	return Load( strFile, pObj );
}

bool sqr::CEditorAmpArsFormat::SaveAmpArsFile( const string& strFile )
{
	CAnyObject* pObj = (CAnyObject*)(0xeefefeff); //指向要读取/存储的对象
	return Save( strFile, pObj ); 
}