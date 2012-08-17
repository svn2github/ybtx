//------------------------------------------------------------------------------
//  CLocalMapLoader.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "LocalMapLoader.h"
#include "GraphicCommon.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "Operator.h"
#include "Operator.inl"
#include "WaterOperator.h"
#include "CMapEffect.h"
#include "TerrainMeshObject.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "Engine.h"
#include "Render.h"
#include "River.h"
#include "TerrainMeshRegion.h"
#include "CMapEditCamera.inl"
#include "CCameraManager.h"
#include "CmdMgr.h"
#include "SceneRegionMgr.h"
#include "WireBox.h"
#include "OffLineBaker/OfflineRender.h"
#include "LocalMapAudioLoader.h"
#include "LocalMapLoginLoader.h"
#include "IoSystem.h"
#include "TraceHelper.h"
#include "CMapEditModelObjPro.h"
#include "CodeCvs.h"
#include "LocalVersion.h"
#include "MapCommonDataLoader.h"
#include "Console.h"
#include "CWindowTarget.h"
#include "CGraphic.h"
#include "ToolSetTileBlock.h"
#include "ToolRecord.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "ToolMainPlayer.h"
#include "LoadMapMgr.h"
#include "EditStateOpertiaonMgr.h"
#include "OffLineBaker/CModelDiffuseColor.h"
#include "CEditDataScene.h"
#include "CEditContext.h"
#include "EditorExp.h"
#include "MapGUIManager.h"
#include "TSqrAllocator.inl"

// 重构部分
#include "PathManager/CPathManager.h"


namespace sqr
{
	void FindSubString(string& mainstr, const string& substr)
	{
		string::iterator iter = search(mainstr.begin( ), mainstr.end( ), substr.begin( ), substr.end( ));

		int npos = iter - mainstr.begin();

		if( npos != -1 )
			mainstr = mainstr.substr(npos + substr.length() + 1, mainstr.length());
	}

	//------------------------------------------------------------------------------
	CLocalMapLoader::CLocalMapLoader()
		: progress(NULL)
	{
		pOperator = NULL;

		uModelProVersion = 0;
		uGridProVersion  = 0;

		string strDir = CPathSetImp::GetInst()->GetPath(0);
		m_strDefaultTerrainTexName = strDir + "default.bmp";
		m_strDefaultWaterTexName   = strDir + "water.dds";
	}

	void CLocalMapLoader::NewLoadAsLocal(const string& path)
	{
		locale::global(locale(""));

		pOperator = CMapEditApp::GetInst()->GetOperator();
		if ( pOperator == NULL )
			return;
		CMapEditApp::GetInst()->SetEditingMesh(sqr::EEM_TERRAIN);
		//CMapGUIManager::GetInst()->InitMiniMap();
		CCmdMgr::GetInst()->Clear();
		//////////////////////////////////////////////////////////////////////////
		DWORD  dwVersion = 0;
		string strFileName = path;
		use_facet<ctype<TCHAR> >( loc ).tolower
			( &strFileName[0], &strFileName[strFileName.length()] );

		FILE * fp = NULL;
		fp = fopen(strFileName.c_str(),"rb");
		if (! fp )
		{
			string log = strFileName + "无法打开";
			MessageBox(NULL,log.c_str(),"请换其他地图",MB_OK);

#ifdef USE_OLD_RESOURCEDIR_NOTCODER

#else
			if( !CLoadSavedMapMgr::HasInst() )
				CLoadSavedMapMgr::Create();

			CLoadSavedMapMgr::GetInst()->SetProcessCallback(progress);
			CLoadSavedMapMgr::GetInst()->Load(strFileName);
#endif
		}
		else
		{
			// header
			TCHAR szHeader[4] = "";
			fread(szHeader,4,1,fp);
			if ( *((DWORD*)szHeader) != MAKEFOURCC('L','O','C','L') )
			{
				MessageBox(NULL,"文件类型不符","错误",MB_OK);
				fclose(fp);
				return;
			}

			ErrObject.clear();

			// version
			fread(&dwVersion,sizeof(DWORD),1,fp);
			if ( dwVersion < LOCAL_VERSION )
			{
				DWORD diff = LOCAL_VERSION - dwVersion;

#ifdef USE_OLD_RESOURCEDIR_NOTCODER
				if( diff == 3 )
					LoadAsOOldLocal(fp, dwVersion, strFileName);
				else 
				{
					if( diff == 2 )
						LoadAsOldLocal(fp, dwVersion, strFileName);
					else if( diff == 1 )
						LoadAsNewLocal(fp, dwVersion, strFileName);
				}
#else
				if( diff == 3 )
					LoadAsOOldLocal(fp, dwVersion, strFileName);
				else 
				{
					if( diff == 2 )
						LoadAsOldLocal(fp, dwVersion, strFileName);
					else if( diff == 1 )
						LoadAsNewLocal(fp, dwVersion, strFileName);

					// 加载audio文件
					CLocalMapAudioLoaderPtr audioloader = new CLocalMapAudioLoader();
					audioloader->SetTerrainMesh(CMapEditApp::GetInst()->GetTerrain());
					audioloader->LoadAsSceneAudio(path);

					// 加载login文件
					CLocalMapLoginLoaderPtr loginloader = new CLocalMapLoginLoader();
					loginloader->SetTerrainMesh(CMapEditApp::GetInst()->GetTerrain());
					loginloader->LoadAsSceneLogin(path);
				}
#endif

			}
			else//采用新版本的读取方式
			{
#ifdef USE_OLD_RESOURCEDIR_NOTCODER
				CEditStateOpertiaonMgr::GetInst()->SetIsCalObjectWorOffset(false);
				LoadAsNewLocal(fp, dwVersion, strFileName);
				CEditStateOpertiaonMgr::GetInst()->SetIsCalObjectWorOffset(true);
#else
				if( !CLoadSavedMapMgr::HasInst() )
					CLoadSavedMapMgr::Create();

				CLoadSavedMapMgr::GetInst()->SetProcessCallback(progress);
				CLoadSavedMapMgr::GetInst()->Load(strFileName);

				CLoadSavedMapMgr::GetInst()->dwLocalVersion = dwVersion;
#endif
			}

			// load ambient fx model 
			CMapEffect::GetInst()->LoadAmbientFXModel();

#ifdef USE_OLD_RESOURCEDIR_NOTCODER
			{
				// 加载audio文件
				CLocalMapAudioLoaderPtr audioloader = new CLocalMapAudioLoader();
				audioloader->SetTerrainMesh(CMapEditApp::GetInst()->GetTerrain());
				audioloader->LoadAsSceneAudio(path);

				// 加载login文件
				CLocalMapLoginLoaderPtr loginloader = new CLocalMapLoginLoader();
				loginloader->SetTerrainMesh(CMapEditApp::GetInst()->GetTerrain());
				loginloader->LoadAsSceneLogin(path);
			}
#else
			continue;
#endif
		}

		CVector3f sunDir = CSceneEnvMgr::GetInst()->GetDirDir();
		LogConsole("Sun Direction is: " << sunDir.x << " " << sunDir.y << " "<< sunDir.z << "\n" );

		CVector3f fixdir = CVector3f(0.390625f, -0.70347214f, -0.59375f);
		if ( sunDir != fixdir )
		{
			LogConsole("Fix Direction is: " << fixdir.x << " " << fixdir.y << " "<< fixdir.z << "\n" );
		}

		//判断是否能够保存，不能及给出提示
		if ( !CMapEditApp::GetInst()->m_bSave )
		{
			string log = CMapEditApp::GetInst()->savelog;
			log += "无法加载";

			::MessageBox( 0, log.c_str(), "无法保存地图", MB_ICONINFORMATION);
		}

		LoadFaildModelMap.clear();

		//////////////////////////////////////////////////////////////////////////
		CSceneEnvMgr::GetInst()->InitiaRgnDefault();

		CTerrainMesh *pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		// 加载烘焙文件
		//pTerrain->LoadAsColor(progress);

		///sh1文件 shadow.sh1
		CSceneEnvMgr::GetInst()->LoadShadowedColor();

		// clear again
		CCmdMgr::GetInst()->Clear();

		//CTimer::GetInst()->ResumeUpdate();
		//CTimer::GetInst()->Start();
		//CMapGUIManager::GetInst()->Finalize();

		//////////////////////////////////////////////////////////////////////////
		///将当前未保存的local文件copy一份，以免保存后的local文件打开失败
		string autopath = IoSystem::GetInst()->GetFullPath("home:Auto");
		string strOpenBackUp = pTerrain->m_strMapName;

		strOpenBackUp = autopath + "\\" + strOpenBackUp + "_LastOpen.Local";
		CopyFile(strFileName.c_str(), strOpenBackUp.c_str(), false);
		//////////////////////////////////////////////////////////////////////////

		//CSceneManager * pScene = CSceneManager::GetInst();
		//pScene->RenderAll();
		CVertexDiffuseColor::GetInst()->BakeMapDiffuse();
	}

	//------------------------------------------------------------------------------
	void
		CLocalMapLoader::LoadAsLocal(const string& path)
	{
		locale::global(locale(""));
		//Ast(NULL != this->progress);

		pOperator = CMapEditApp::GetInst()->GetOperator();
		if ( pOperator == NULL )
			return;
		CMapEditApp::GetInst()->SetEditingMesh(sqr::EEM_TERRAIN);
		//CMapGUIManager::GetInst()->InitMiniMap();
		//CCmdMgr::GetInst()->Clear();
		//////////////////////////////////////////////////////////////////////////
		DWORD  dwVersion = 0;
		string strFileName = path;
		use_facet<ctype<TCHAR> >( loc ).tolower
			( &strFileName[0], &strFileName[strFileName.length()] );

		FILE * fp = NULL;
		fp = fopen(strFileName.c_str(),"rb");
		if (! fp )
		{
			string log = strFileName + "无法打开";
			MessageBox(NULL,log.c_str(),"请换其他地图",MB_OK);

#ifdef USE_OLD_RESOURCEDIR_NOTCODER

#else
			if( !CLoadSavedMapMgr::HasInst() )
				CLoadSavedMapMgr::Create();

			CLoadSavedMapMgr::GetInst()->SetProcessCallback(progress);
			CLoadSavedMapMgr::GetInst()->Load(strFileName);
#endif
		}
		else
		{
			// header
			TCHAR szHeader[4] = "";
			fread(szHeader,4,1,fp);
			if ( *((DWORD*)szHeader) != MAKEFOURCC('L','O','C','L') )
			{
				MessageBox(NULL,"文件类型不符","错误",MB_OK);
				fclose(fp);
				return;
			}

			ErrObject.clear();

			// version
			fread(&dwVersion,sizeof(DWORD),1,fp);
			if ( dwVersion < LOCAL_VERSION )
			{
				DWORD diff = LOCAL_VERSION - dwVersion;

#ifdef USE_OLD_RESOURCEDIR_NOTCODER
				if( diff == 3 )
					LoadAsOOldLocal(fp, dwVersion, strFileName);
				else 
				{
					if( diff == 2 )
						LoadAsOldLocal(fp, dwVersion, strFileName);
					else if( diff == 1 )
						LoadAsNewLocal(fp, dwVersion, strFileName);
				}
#else
				if( diff == 3 )
					LoadAsOOldLocal(fp, dwVersion, strFileName);
				else 
				{
					if( diff == 2 )
						LoadAsOldLocal(fp, dwVersion, strFileName);
					else if( diff == 1 )
						LoadAsNewLocal(fp, dwVersion, strFileName);

					// 加载audio文件
					CLocalMapAudioLoaderPtr audioloader = new CLocalMapAudioLoader();
					audioloader->SetTerrainMesh(CMapEditApp::GetInst()->GetTerrain());
					audioloader->LoadAsSceneAudio(path);

					// 加载login文件
					CLocalMapLoginLoaderPtr loginloader = new CLocalMapLoginLoader();
					loginloader->SetTerrainMesh(CMapEditApp::GetInst()->GetTerrain());
					loginloader->LoadAsSceneLogin(path);
				}
#endif

			}
			else//采用新版本的读取方式
			{
#ifdef USE_OLD_RESOURCEDIR_NOTCODER
				CEditStateOpertiaonMgr::GetInst()->SetIsCalObjectWorOffset(false);
				LoadAsNewLocal(fp, dwVersion, strFileName);
				CEditStateOpertiaonMgr::GetInst()->SetIsCalObjectWorOffset(true);
#else
				if( !CLoadSavedMapMgr::HasInst() )
					CLoadSavedMapMgr::Create();

				CLoadSavedMapMgr::GetInst()->SetProcessCallback(progress);
				CLoadSavedMapMgr::GetInst()->Load(strFileName);

				CLoadSavedMapMgr::GetInst()->dwLocalVersion = dwVersion;
#endif
			}

			// load ambient fx model 
			CMapEffect::GetInst()->LoadAmbientFXModel();

#ifdef USE_OLD_RESOURCEDIR_NOTCODER
			{
				// 加载audio文件
				CLocalMapAudioLoaderPtr audioloader = new CLocalMapAudioLoader();
				audioloader->SetTerrainMesh(CMapEditApp::GetInst()->GetTerrain());
				audioloader->LoadAsSceneAudio(path);

				// 加载login文件
				CLocalMapLoginLoaderPtr loginloader = new CLocalMapLoginLoader();
				loginloader->SetTerrainMesh(CMapEditApp::GetInst()->GetTerrain());
				loginloader->LoadAsSceneLogin(path);
			}
#else
			continue;
#endif
		}

		CVector3f sunDir = CSceneEnvMgr::GetInst()->GetDirDir();
		LogConsole("Sun Direction is: " << sunDir.x << " " << sunDir.y << " "<< sunDir.z << "\n" );

		CVector3f fixdir = CVector3f(0.390625f, -0.70347214f, -0.59375f);
		if ( sunDir != fixdir )
		{
			LogConsole("Fix Direction is: " << fixdir.x << " " << fixdir.y << " "<< fixdir.z << "\n" );
		}

		//判断是否能够保存，不能及给出提示
		if ( !CMapEditApp::GetInst()->m_bSave )
		{
			string log = CMapEditApp::GetInst()->savelog;
			log += "无法加载";

			::MessageBox( 0, log.c_str(), "无法保存地图", MB_ICONINFORMATION);
		}

		LoadFaildModelMap.clear();

		//////////////////////////////////////////////////////////////////////////
		CSceneEnvMgr::GetInst()->InitiaRgnDefault();

		CTerrainMesh *pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		// 加载烘焙文件
		//pTerrain->LoadAsColor(progress);

		///sh1文件 shadow.sh1
		CSceneEnvMgr::GetInst()->LoadShadowedColor();

		// clear again
		CCmdMgr::GetInst()->Clear();

		//CTimer::GetInst()->ResumeUpdate();
		//CTimer::GetInst()->Start();
		//CMapGUIManager::GetInst()->Finalize();

		//////////////////////////////////////////////////////////////////////////
		///将当前未保存的local文件copy一份，以免保存后的local文件打开失败
		string autopath = IoSystem::GetInst()->GetFullPath("home:Auto");
		string strOpenBackUp = pTerrain->m_strMapName;

		strOpenBackUp = autopath + "\\" + strOpenBackUp + "_LastOpen.Local";
		CopyFile(strFileName.c_str(), strOpenBackUp.c_str(), false);
		//////////////////////////////////////////////////////////////////////////

		//CSceneManager * pScene = CSceneManager::GetInst();
		//pScene->RenderAll();
		CVertexDiffuseColor::GetInst()->BakeMapDiffuse();
	}

	//------------------------------------------------------------------------------
	void
		CLocalMapLoader::LoadAsOOldLocal(FILE * fp, DWORD& version, string strFileName)
	{
		//Ast(NULL != progress);
		if ( !fp )
			return;

		CRenderDevice * pRender = NULL;
		pRender = CRenderDevice::GetInst();
		if ( pRender == NULL )
			return;

		///游戏客户端服务器端文件ID号
		DWORD dwGameClientID = 0, dwGameServerID = 0;
		fread(&dwGameClientID,sizeof(DWORD),1,fp);
		fread(&dwGameServerID,sizeof(DWORD),1,fp);

		if ( dwGameServerID != dwGameClientID )
			dwGameClientID = dwGameServerID;

		LogConsole("Old Old Version: " << version << "  GameClientID: " << dwGameClientID << "  GameServerID: " << dwGameServerID << "\n" );

		// background music

		DWORD dwFileNameLen;
		string str;
		fread(&dwFileNameLen,sizeof(DWORD),1,fp);
		str.resize(dwFileNameLen);
		fread((TCHAR*)str.data(),dwFileNameLen,1,fp);

		CSceneEnvMgr::GetInst()->SetBackGroundMusic(str, false);
		///大家希望一进去不听背景音乐，觉得不好听
		CSceneEnvMgr::GetInst()->StopBackMusic();

		// mainplayer
		CToolMainPlayer::GetInst()->Load(fp);

		//////////////////////////////////////////////////////////////////////////

		// camera
		CVector3f vView;
		fread(&vView,sizeof(CVector3f),1,fp);
		if ( vView.y < 0 )
		{
			vView.y = 447.676605f;
		}
		CMapEditCamera * pCamera = CCameraManager::GetInst()->GetMapEditCamera();
		pCamera->SetNewTarget(vView);

		//fov
		float fov = 0.0f;
		fread(&fov,sizeof(float),1,fp);

		//yaw
		float yaw = 0.0f;
		fread(&yaw,sizeof(float),1,fp);

		//pitch
		float pitch = 0.0f;
		fread(&pitch,sizeof(float),1,fp);

		pCamera->SetFovY(fov);
		pCamera->SetPitchAndYaw(pitch,yaw);

		//////////////////////////////////////////////////////////////////////////
#pragma region ENVPARA
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

#pragma endregion ENVPARA

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

		fread(&dwFileNameLen,sizeof(DWORD),1,fp);
		str.resize(dwFileNameLen);
		fread((TCHAR*)str.data(),dwFileNameLen,1,fp);

		// .tga -> .dds
		use_facet<ctype<TCHAR> >( loc ).tolower
			( &str[0], &str[str.length()] );

		if (str.rfind(".tga") != string::npos)
		{
			str = str.substr(0, str.rfind(".tga")) + ".dds";
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

		int fogMode;
		fread(&fogMode,sizeof(int),1,fp);
		CSceneEnvMgr::GetInst()->SetVertexFog((sqr::CSceneEnvMgr::FogModeType)fogMode);

		//全屏泛光
		CSceneEnvMgr::GetInst()->LoadGaussParamsInfo(fp);

		//////////////////////////////////////////////////////////////////////////
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

		int nWaveLife;
		fread(&nWaveLife,sizeof(int),1,fp);
		pWaterOperator->SetWaveLife(nWaveLife);

		fread(&dwFileNameLen,sizeof(DWORD),1,fp);
		string strFileNameWave;
		strFileNameWave.resize(dwFileNameLen);
		fread((TCHAR*)strFileNameWave.data(),dwFileNameLen,1,fp);

		// .tga -> .dds
		use_facet<ctype<TCHAR> >( loc ).tolower
			( &strFileNameWave[0], &strFileNameWave[strFileNameWave.length()] );
		if (strFileNameWave.rfind(".tga") != string::npos)
		{
			strFileNameWave = strFileNameWave.substr(0, strFileNameWave.rfind(".tga"));
			strFileNameWave += ".dds";
		}

		//////////////////////////////////////////////////////////////////////////

		// ambient fx info
		CMapEffect::GetInst()->ReadAmbientFXInfo(fp);
		pWaterOperator->SetWaveTexture(strFileNameWave);

		//////////////////////////////////////////////////////////////////////////

		// width depth
		DWORD dwWidth;
		DWORD dwDepth;
		fread(&dwWidth,sizeof(DWORD),1,fp);
		fread(&dwDepth,sizeof(DWORD),1,fp);

		LogConsole("Map Size is: " << dwWidth << " " << dwDepth << "\n" );

		// create a new terrain
		CTerrainMesh *pLastTerrain = NULL;
		pLastTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		CMapEditApp::GetInst()->GetDataScene()->SetWidth( uint16(dwWidth) );
		CMapEditApp::GetInst()->GetDataScene()->SetHeight( uint16(dwDepth) );
		CMapEditApp::GetInst()->GetDataScene()->Create();
		CSyncSystem::BeginSyncFun();

		CMapEditApp::GetInst()->InitNewRenderScene();
		CMapEditApp::GetInst()->GetRenderScene()->InitSceneRes();

		CTerrainMesh * pTerrain = new CTerrainMesh; 
		pTerrain->Create(dwWidth,dwDepth,m_strDefaultTerrainTexName,false,false);
		if( pLastTerrain != NULL )
		{
			CMapEditApp::GetInst()->SetTransformTerrainMesh(pTerrain);
		}
		CMapEditApp::GetInst()->SetTerrainMesh(pTerrain);
		pTerrain->SetTerrainVersion(version);

		size_t npos = strFileName.rfind("\\");
		string mapname = "";
		if ( npos != -1 )
		{
			mapname = strFileName.substr(npos+1, strFileName.length());

			npos = mapname.rfind(".");
			if ( npos != -1 )
				mapname = mapname.substr(0, npos);
		}

		pTerrain->m_strMapName = mapname;

		pOperator->InitProcessParams(false,version);

		CTerrainMesh * pWater = new CTerrainMesh; 
		pTerrain->Create(dwWidth,dwDepth,m_strDefaultWaterTexName,true,false);
		pTerrain->SetWater(pWater);

		pTerrain->SetGameGacMapVersion(dwGameClientID);
		pTerrain->SetGameGasMapVersion(dwGameServerID);

		CTObject * p = NULL;
		//////////////////////////////////////////////////////////////////////////
		///read terrain road and sub texture name
		size_t size = 0;
		int iNameLen = 0, weight = 0;
		string texname = "";
		fread(&size,sizeof(size_t),1,fp);
		for ( size_t i = 0; i < size; ++i )
		{
			fread(&iNameLen,sizeof(int),1,fp);
			texname.resize(iNameLen);
			fread((TCHAR*)texname.data(),iNameLen,1,fp);
			fread(&weight,sizeof(int),1,fp);

			pTerrain->m_MainRoadWeight[texname] = weight;
		}

		DWORD dwVertexCnt = pTerrain->GetVertexCount();
		DWORD dwGridCnt	  = pTerrain->GetGridCount();
		float Total = float(dwVertexCnt + dwGridCnt + dwGridCnt/2);
		//progress->SetProgress( 0.0f );

#pragma region VERTEX
		for ( DWORD i = 0; i< dwVertexCnt; ++i )
		{
			SVertex &vertex = pTerrain->GetVertex(i);

			fread(&vertex.vPosition.y,sizeof(float),1,fp);
			pWater->GetVertex(i).vPosition.y = vertex.vPosition.y + WATER_LAYER_BASE;
			fread(&vertex.vNormal,sizeof(CVector3f),1,fp);
			fread(&vertex.oColor,sizeof(DWORD),1,fp);
			fread(&vertex.sColor,sizeof(DWORD),1,fp);
			short s;
			fread(&s,sizeof(short),1,fp);
			vertex.fLogicHeight = s;
			vertex.bColor = vertex.oColor;
			vertex.tColor = vertex.oColor;
			vertex.sColor = VERTEX_SPECULAR_COLOR;

			//progress->SetProgress( float(i)/Total );
		}
#pragma endregion VERTEX

		// grids
		int dwNameLen = 0;
		int dwTexCnt = 0;
		set<string> terrainTexIndexsSet;
		string gridtexname = "";
		size_t nPos = 0;

#pragma region TEXTURE
		///读取地图用到的所有贴图
		map<int16, string> terrainTextureNames;///用于记录地图中用到的所有纹理的名字
		int16 nTextureIndex = 0;
		size_t nTerrainTexCnt = 0;
		fread(&nTerrainTexCnt,sizeof(nTerrainTexCnt),1,fp);

		for ( size_t i = 0; i < nTerrainTexCnt; ++i )
		{
			fread(&dwNameLen,sizeof(int),1,fp);
			gridtexname.resize(dwNameLen);
			fread((TCHAR*)gridtexname.data(),dwNameLen,1,fp);
			fread(&nTextureIndex,sizeof(nTextureIndex),1,fp);

			use_facet<ctype<TCHAR> >( loc ).tolower
				( &gridtexname[0], &gridtexname[gridtexname.length()] );

			if (gridtexname.find(".tga")!=string::npos)
			{
				gridtexname = gridtexname.substr(0, gridtexname.rfind(".tga")) + ".dds";
			}

			if (gridtexname.find("defaultlayerone") != string::npos ||
				gridtexname.find("defaultlayertwo") != string::npos)
			{
				gridtexname = "";
			}

			// 			size_t npos0 = gridtexname.find("tile\\地表纹理");
			// 			if( npos0 != -1 )
			// 			{
			// 				npos0 = gridtexname.find("\\");
			// 				gridtexname = gridtexname.substr(npos0+1, gridtexname.length());
			// 				gridtexname = "..\\..\\tools\\" + gridtexname;
			// 			}

			if( gridtexname.find("default.bmp") == 0 )
				gridtexname = m_strDefaultTerrainTexName;

			terrainTextureNames[nTextureIndex] = gridtexname;
		}
#pragma endregion TEXTURE

		map<string, int16>::iterator texiter;
#pragma region GRID
		for ( DWORD i = 0; i< dwGridCnt; ++i )
		{
			SGrid * pGrid = &pTerrain->GetGrid(i);
			//grid hide mask
			fread(&pGrid->bGridHide,sizeof(bool),1,fp);

			//////////////////////////////////////////////////////////////
			//读取纹理信息
			//1
			fread(&nTextureIndex,sizeof(nTextureIndex),1,fp);
			gridtexname= terrainTextureNames[nTextureIndex] ;
			pGrid->nodeInfo.strTextureNames[0]  = gridtexname;

			if ( terrainTexIndexsSet.find(gridtexname) == terrainTexIndexsSet.end() && gridtexname != "" )
			{
				terrainTexIndexsSet.insert(gridtexname);
				++dwTexCnt;
			}

			//2
			fread(&nTextureIndex,sizeof(nTextureIndex),1,fp);
			gridtexname= terrainTextureNames[nTextureIndex] ;
			pGrid->nodeInfo.strTextureNames[1]  = gridtexname;

			if ( terrainTexIndexsSet.find(gridtexname) == terrainTexIndexsSet.end() && gridtexname != "" )
			{
				terrainTexIndexsSet.insert(gridtexname);

				++dwTexCnt;
			}

			// 保证至少有第一层纹理，而且不会出现第一层纹理为空而存在第二层纹理的情况
			if ( pGrid->nodeInfo.strTextureNames[0] == "" )
			{
				if ( pGrid->nodeInfo.strTextureNames[1] == "" ) 
				{
					pGrid->nodeInfo.strTextureNames[0] = m_strDefaultTerrainTexName;
				}
				else
				{
					pGrid->nodeInfo.strTextureNames[0] = pGrid->nodeInfo.strTextureNames[1];
					pGrid->nodeInfo.strTextureNames[1];
				}
			}

			//3
			pGrid->nodeInfo.strTextureNames[2] = "" ;

			//4
			pGrid->nodeInfo.strTextureNames[3] = "" ;
			//////////////////////////////////////////////////////////////

			fread(&pGrid->bCliffLayerOne, sizeof(bool), 1, fp);
			//读取纹理坐标信息,第1层纹理坐标的信息
			for (int m = 0; m < 4; ++m)
			{
				fread(&pGrid->uv[0][m].x, sizeof(float), 1, fp);
				fread(&pGrid->uv[0][m].y, sizeof(float), 1, fp);
			}

			fread(&pGrid->vCenter.y,sizeof(float),1,fp);

			//////////////////////////////////////////////////////////////////////////

			SGrid & gridW = pWater->GetGrid(i);

			gridW.vCenter.y = pGrid->vCenter.y + WATER_LAYER_BASE;

			//////////////////////////////////////////////////////////////////////////
			//bool bEffectByVertexColor = true;
			fread(&pGrid->bEffectByVertexColor,sizeof(bool),1,fp);
			fread(&pGrid->bCliffLayerOne,sizeof(bool),1,fp);

			// any central Object?
			DWORD dw;
			fread(&dw,sizeof(DWORD),1,fp);

			vector<int> errorlist;
			for (size_t j = 0; j < dw; j++ )
			{
				// effect bound?

				bool bEffectBound;
				fread(&bEffectBound,sizeof(bool),1,fp);
				string strEffectName;
				if ( bEffectBound )
				{
					DWORD dwItemNameLen;
					fread(&dwItemNameLen,sizeof(DWORD),1,fp);
					strEffectName.resize(dwItemNameLen);
					fread((TCHAR*)strEffectName.data(),dwItemNameLen,1,fp);
				}
				//////////////////////////////////////////////////////////////////////////

				DWORD dwItemNameLen;
				fread(&dwItemNameLen,sizeof(DWORD),1,fp);
				string strItemName;
				strItemName.resize(dwItemNameLen);
				fread((TCHAR*)strItemName.data(),dwItemNameLen,1,fp);

				use_facet<ctype<TCHAR> >( loc ).tolower
					( &strItemName[0], &strItemName[strItemName.length()] );

				//////////////////////////////////////////////////////////////////////////

				EAUTO_GENERATION_TYPE eAutoGenerationType = EAGT_NONE;
				fread(&eAutoGenerationType,sizeof(BYTE),1,fp);
				int nMaxFrameCnt = 0xffffffff;
				if ( eAutoGenerationType == EAGT_GRASS )
				{
					fread(&nMaxFrameCnt,sizeof(int),1,fp);
				}
				//////////////////////////////////////////////////////////////////////////

				int nGridIndex;
				fread(&nGridIndex,sizeof(int),1,fp);

				float fRotation;
				fread(&fRotation,sizeof(float),1,fp);

				//////////////////////////////////////////////////////////////////////////

				int nRotationCnt;
				fread(&nRotationCnt,sizeof(int),1,fp);

				//////////////////////////////////////////////////////////////////////////
				//新版本新增的绕x，z轴渲染，物体缩放信息
				//z 轴
				float fPitch;
				fread(&fPitch,sizeof(float),1,fp);

				int nPitchCnt;
				fread(&nPitchCnt,sizeof(int),1,fp);

				//x 轴
				float fYaw;
				fread(&fYaw,sizeof(float),1,fp);

				int nYawCnt;
				fread(&nYawCnt,sizeof(int),1,fp);

				//缩放
				float xScale;
				fread(&xScale,sizeof(float),1,fp);
				float yScale;
				fread(&yScale,sizeof(float),1,fp);
				float zScale;
				fread(&zScale,sizeof(float),1,fp);
				//////////////////////////////////////////////////////////////////////////

				CVector3f vUserOffsetEx; // (0.0f,0.0f,0.0f);
				fread(&vUserOffsetEx,sizeof(CVector3f),1,fp);

				CVector3f vUserOffsetEx2(vUserOffsetEx.y,vUserOffsetEx.x,vUserOffsetEx.z);

				// 				if( strEffectName.find("场景光源\\Standar") != -1 && vUserOffsetEx.y != 0.0f )
				// 				{
				// 					DebugOut("%d, %f, %f, %f, %f, %f, %f\n", i, vUserOffsetEx.x, vUserOffsetEx.y, vUserOffsetEx.z,
				// 						fYaw, fRotation, fPitch);
				// 				}

				BYTE byRenderStyle = 0;
				fread(&byRenderStyle, sizeof(BYTE),1,fp);
				int nAtten = 0;
				fread(&nAtten,sizeof(int),1,fp);

				bool bShadow = true;
				fread(&bShadow,sizeof(bool),1,fp);

				///读取是否接收阴影信息
				bool bReceiveShadow = false;
				fread(&bReceiveShadow,sizeof(bool),1,fp);

				//是否消隐
				bool bTrans = true;
				fread(&bTrans,sizeof(bTrans),1,fp);

				///trans state
				BYTE bTransState = 0;
				fread(&bTransState, sizeof(bTransState), 1, fp);

				///mirror type
				int nMirrorType = 0, nRatio = 0;
				fread(&nMirrorType, sizeof(nMirrorType), 1, fp);

				///nAffectRadio
				fread( &nRatio, sizeof(int), 1, fp);

				bool bVisibleByConfigure = false;
				fread( &bVisibleByConfigure, sizeof(bVisibleByConfigure), 1, fp);
				bVisibleByConfigure = !bVisibleByConfigure;

				if( strItemName.find("pyramid") != -1 )
				{
					DebugOut("%s\n", strItemName.c_str());
					continue;
				}

				if ( nGridIndex < 0 || !pTerrain->IsValid(nGridIndex) )
				{
					ErrObject.push_back( strItemName );
				}
				else
				{
					if ( eAutoGenerationType == EAGT_GRASS ) // so all e  ffects are not auto generated
					{
						int ix = nGridIndex%pTerrain->GetWidth();
						if ( !pOperator->Rebirth(strItemName, nGridIndex, fRotation, nRotationCnt, fPitch, nPitchCnt, fYaw, nYawCnt, xScale, yScale, zScale, &vUserOffsetEx2, 0, NULL, 0, 0, 1.0f, EAGT_GRASS,
							byRenderStyle,nAtten,bVisibleByConfigure,bShadow,bTrans,bTransState,nMirrorType,nRatio) )
							errorlist.push_back(j);
					}
					else if ( !bEffectBound )
					{
						pOperator->SetReceiveShadow(bReceiveShadow);

						if ( !pOperator->Rebirth(strItemName, nGridIndex, fRotation, nRotationCnt,
							fPitch, nPitchCnt, fYaw, nYawCnt, xScale, yScale, zScale, &vUserOffsetEx2, 0, NULL, 0,0,1.0f,eAutoGenerationType,byRenderStyle,nAtten,bVisibleByConfigure,bShadow,bTrans,bTransState,nMirrorType,nRatio, "", "") )
							errorlist.push_back(j);
					}
					else // effect
					{
						CVector3f pos = CVector3f(0.0f, 0.0f, 0.0f);
						pOperator->RebirthIndividualEffect(strEffectName, nGridIndex, fRotation, nRotationCnt,
							fPitch, nPitchCnt, fYaw, nYawCnt, xScale, yScale, zScale, &vUserOffsetEx, pos, 0, NULL, 0);
					}

				}
			}

			if ( !errorlist.empty() && dw > 0 )
				LoadFaildModelMap[i] = errorlist;

			//progress->SetProgress( float(i+dwVertexCnt)/Total );
		}
#pragma endregion GRID

		///拼接纹理和创建地表纹理
		MakeTerrainTexture(terrainTexIndexsSet);

		for ( DWORD i = 0; i < dwVertexCnt; ++i )
		{
			SVertex &vertex = pTerrain->GetVertex(i);

			if ( vertex.fLogicHeight > 1.0f || vertex.fLogicHeight < -1.0f )
			{
				CVector3f vCenter = vertex.vPosition;
				vertex.GetWireBoxLogicHeight()->SetHeight(vertex.fLogicHeight);
				vertex.GetWireBoxLogicHeight()->SetRelativePos(vCenter);
				vertex.GetWireBoxLogicHeight()->UpdateAbsolute();
			}
		}

		//////////////////////////////////////////////////////////////////////
		///载入阻挡信息
		CToolSetTileBlock::GetInst()->Load(strFileName);

		///pTerrain
		size_t erasebloclsize = 0;
		fread( &erasebloclsize, sizeof(size_t), 1, fp );
		DWORD gridindex = 0;
		for ( size_t i = 0; i < erasebloclsize; ++i )
		{
			fread( &gridindex, sizeof(DWORD), 1, fp );
		}
		//progress->SetProgress( float(dwVertexCnt+dwVertexCnt+2)/Total );

		//////////////////////////////////////////////////////////////////////////
		// water
		LoadWaterInfo(fp);

		//progress->SetProgress( float(dwVertexCnt+dwVertexCnt+2+dwGridCnt/4)/Total );

		int ExtraData = 0;
		TCHAR ExtraName[255];

		//----load point light info
		LoadPointLightInfo(fp);

		///读取是否是bCave信息
		ExtraData = 0;
		fread(&ExtraData,sizeof(int),1,fp);
		if ( ExtraData == 6 )
		{
			fread( ExtraName, ExtraData, 1, fp);

			bool bCave = false;
			fread( &bCave, sizeof(bool), 1, fp);
		}
		else
		{
			::MessageBox( 0, "载入是否室内信息错误", "提示", MB_ICONINFORMATION);
			fclose(fp);
			return;
		}

		//progress->SetProgress( float(dwVertexCnt+dwVertexCnt+2+dwGridCnt/3)/Total );

		//////////////////////////////////////////////////////////////////////////
		///利用新的方式开始读取新增加的数据信息
		DWORD dwVersion;
		fread(&dwVersion,sizeof(DWORD),1,fp);

		///读取聚光灯信息
		switch (dwVersion)
		{
		case MODEL_ANIPLAYMARK:
			LoadOffAnimateModelIndex(fp);

		case MODEL_EXACTBLOCKTYPE:
			LoadExactBlockInfo(fp);

		case WAVE_SIDE_SAVE:
			LoadWaveSideInfo(fp);

		case SPOTLIGHT_SAVE:
			LoadSpotLightInfo(fp);
			break;

		default:
			break;
		}

		// ----------------------------------------------------------------------------
		// 读取摄像机路径信息

		CToolRecord::GetInst()->Load(fp);

		fclose(fp);

		//progress->SetProgress( 1.0f );

		if (ErrObject.size()>0)
		{
			TCHAR tt[255];
			sprintf( tt, "剔除错误场景物件 %d 个", ErrObject.size() );
			::MessageBox( 0, tt, "提示", MB_ICONINFORMATION);
			vector<string>::iterator eoitor = ErrObject.begin();
			while (eoitor!=ErrObject.end())
			{
				string pr;
				for (int i=0;eoitor!=ErrObject.end() && i<10;++i)
				{
					pr = pr + (*eoitor) + "\n";
					eoitor++;
				}
				::MessageBox( 0, pr.c_str(), "提示", MB_ICONINFORMATION);
			}
		}

		if( pLastTerrain )
			CMapEditApp::GetInst()->SetTerrainMesh(pLastTerrain);
		LogConsole("Map Name is: " << mapname.c_str() << "\n");
	}

	//------------------------------------------------------------------------------
	void
		CLocalMapLoader::LoadAsOldLocal(FILE * fp, DWORD& version, string strFileName)
	{
		//Ast(NULL != progress);
		if ( !fp )
			return;

		CRenderDevice * pRender = NULL;
		pRender = CRenderDevice::GetInst();
		if ( pRender == NULL )
			return;

		//////////////////////////////////////////////////////////////////////////

		///游戏客户端服务器端文件ID号
		DWORD dwGameClientID = 0, dwGameServerID = 0;
		fread(&dwGameClientID,sizeof(DWORD),1,fp);
		fread(&dwGameServerID,sizeof(DWORD),1,fp);

		if ( dwGameServerID != dwGameClientID )
			dwGameClientID = dwGameServerID;

		LogConsole("Old Version: " << version << "  GameClientID: " << dwGameClientID << "  GameServerID: " << dwGameServerID << "\n" );

		// background music

		DWORD dwFileNameLen;
		string str;
		fread(&dwFileNameLen,sizeof(DWORD),1,fp);
		str.resize(dwFileNameLen);
		fread((TCHAR*)str.data(),dwFileNameLen,1,fp);

		CSceneEnvMgr::GetInst()->SetBackGroundMusic(str, false);
		///大家希望一进去不听背景音乐，觉得不好听
		CSceneEnvMgr::GetInst()->StopBackMusic();

		// mainplayer
		CToolMainPlayer::GetInst()->Load(fp);

		//////////////////////////////////////////////////////////////////////////

		// camera
		CVector3f vView;
		fread(&vView,sizeof(CVector3f),1,fp);
		if ( vView.y < 0 )
		{
			vView.y = 447.676605f;
		}
		CMapEditCamera * pCamera = CCameraManager::GetInst()->GetMapEditCamera();
		pCamera->SetNewTarget(vView);

		//fov
		float fov = 0.0f;
		fread(&fov,sizeof(float),1,fp);

		//yaw
		float yaw = 0.0f;
		fread(&yaw,sizeof(float),1,fp);

		//pitch
		float pitch = 0.0f;
		fread(&pitch,sizeof(float),1,fp);

		pCamera->SetFovY(fov);
		pCamera->SetPitchAndYaw(pitch,yaw);

		//////////////////////////////////////////////////////////////////////////
		MapCommonDataLoader::GetInst()->SetOperator(pOperator);

		MapCommonDataLoader::GetInst()->LoadEnvInfo(fp);

		MapCommonDataLoader::GetInst()->LoadBackPictureInfo(fp);

		int fogMode;
		fread(&fogMode,sizeof(int),1,fp);
		CSceneEnvMgr::GetInst()->SetVertexFog((sqr::CSceneEnvMgr::FogModeType)fogMode);

		//全屏泛光
		CSceneEnvMgr::GetInst()->LoadGaussParamsInfo(fp);

		//////////////////////////////////////////////////////////////////////////
		MapCommonDataLoader::GetInst()->LoadWaveInfo(fp);

		// ambient fx info
		CMapEffect::GetInst()->ReadAmbientFXInfo(fp);
		//////////////////////////////////////////////////////////////////////////

		// width depth
		DWORD dwWidth;
		DWORD dwDepth;
		fread(&dwWidth,sizeof(DWORD),1,fp);
		fread(&dwDepth,sizeof(DWORD),1,fp);

		LogConsole("Map Size is: " << dwWidth << " " << dwDepth << "\n" );

		// create a new terrain
		CTerrainMesh *pLastTerrain = NULL;
		pLastTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		CMapEditApp::GetInst()->GetDataScene()->SetWidth( uint16(dwWidth) );
		CMapEditApp::GetInst()->GetDataScene()->SetHeight( uint16(dwDepth) );
		CMapEditApp::GetInst()->GetDataScene()->Create();
		CSyncSystem::BeginSyncFun();

		CMapEditApp::GetInst()->InitNewRenderScene();
		CMapEditApp::GetInst()->GetRenderScene()->InitSceneRes();

		CTerrainMesh * pTerrain = new CTerrainMesh; 
		pTerrain->Create(dwWidth,dwDepth,m_strDefaultTerrainTexName,false,false);
		if( pLastTerrain != NULL )
		{
			CMapEditApp::GetInst()->SetTransformTerrainMesh(pTerrain);
		}
		CMapEditApp::GetInst()->SetTerrainMesh(pTerrain);
		pTerrain->SetTerrainVersion(version);

		MapCommonDataLoader::GetInst()->SetTerrainMesh(pTerrain);

		size_t npos = strFileName.rfind("\\");
		string mapname = "";
		if ( npos != -1 )
		{
			mapname = strFileName.substr(npos+1, strFileName.length());

			npos = mapname.rfind(".");
			if ( npos != -1 )
				mapname = mapname.substr(0, npos);
		}

		pTerrain->m_strMapName = mapname;

		pOperator->InitProcessParams(false,version);

		CTerrainMesh * pWater = new CTerrainMesh; 
		pTerrain->Create(dwWidth,dwDepth,m_strDefaultWaterTexName,true,false);
		pTerrain->SetWater(pWater);

		pTerrain->SetGameGacMapVersion(dwGameClientID);
		pTerrain->SetGameGasMapVersion(dwGameServerID);

		//////////////////////////////////////////////////////////////////////////
		///read terrain road and sub texture name
		size_t size = 0;
		int iNameLen = 0, weight = 0;
		string texname = "";
		fread(&size,sizeof(size_t),1,fp);
		for ( size_t i = 0; i < size; ++i )
		{
			fread(&iNameLen,sizeof(int),1,fp);
			texname.resize(iNameLen);
			fread((TCHAR*)texname.data(),iNameLen,1,fp);
			fread(&weight,sizeof(int),1,fp);

			pTerrain->m_MainRoadWeight[texname] = weight;
		}
		//////////////////////////////////////////////////////////////////////////

		DWORD dwVertexCnt = pTerrain->GetVertexCount();
		DWORD dwGridCnt	  = pTerrain->GetGridCount();
		float Total = float(dwVertexCnt + dwGridCnt + dwGridCnt/2);
		//progress->SetProgress( 0.0f );

#pragma region VERTEX
		short s = 0;
		for ( DWORD i = 0; i< dwVertexCnt; ++i )
		{
			SVertex *pVertex = &pTerrain->GetVertex(i);
			MapCommonDataLoader::GetInst()->LoadEachVertexInfo(fp, pVertex);

			pWater->GetVertex(i).vPosition.y = pVertex->vPosition.y + WATER_LAYER_BASE;

			//progress->SetProgress( float(i)/Total );
		}
#pragma endregion VERTEX

		//////////////////////////////////////////////////////////////////////////
		///model property version
		fread(&uModelProVersion,sizeof(uModelProVersion),1,fp);
		//////////////////////////////////////////////////////////////////////////

		// grids
		int dwNameLen = 0;
		int dwTexCnt = 0;
		set<string> terrainTexIndexsSet;
		string gridtexname = "";
		size_t nPos = 0;

#pragma region TEXTURE
		///读取地图用到的所有贴图
		map<int16, string> terrainTextureNames;///用于记录地图中用到的所有纹理的名字
		this->LoadTerrainGridTexIndex(fp, terrainTextureNames);
#pragma endregion TEXTURE

#pragma region GRID

		CTObject * p = NULL;
		map<string, int16>::iterator texiter;
		int16  nTextureIndex = 0;

		for ( DWORD i = 0; i < dwGridCnt; ++i )
		{
			SGrid * pGrid = &pTerrain->GetGrid(i);
			//grid hide mask
			fread(&pGrid->bGridHide,sizeof(bool),1,fp);

			//////////////////////////////////////////////////////////////
			//读取纹理信息
			//1
			fread(&nTextureIndex,sizeof(nTextureIndex),1,fp);
			gridtexname= terrainTextureNames[nTextureIndex] ;

			// 			DebugOut("Grid %d\n", i);
			// 			if( gridtexname != "" )
			// 				DebugOut("tex0 %s, %d\n", gridtexname.c_str(), nTextureIndex);
			pGrid->nodeInfo.strTextureNames[0]  = gridtexname;

			if ( terrainTexIndexsSet.find(gridtexname) == terrainTexIndexsSet.end() && gridtexname != "" )
			{
				terrainTexIndexsSet.insert(gridtexname);
				++dwTexCnt;
			}

			//2
			fread(&nTextureIndex,sizeof(nTextureIndex),1,fp);
			gridtexname= terrainTextureNames[nTextureIndex] ;
			pGrid->nodeInfo.strTextureNames[1]  = gridtexname;
			// 			if( gridtexname != "" )
			// 				DebugOut("tex1 %s, %d\n", gridtexname.c_str(), nTextureIndex);

			if ( terrainTexIndexsSet.find(gridtexname) == terrainTexIndexsSet.end() && gridtexname != "" )
			{
				terrainTexIndexsSet.insert(gridtexname);

				++dwTexCnt;
			}

			// 保证至少有第一层纹理，而且不会出现第一层纹理为空而存在第二层纹理的情况
			if ( pGrid->nodeInfo.strTextureNames[0] == "" )
			{
				if ( pGrid->nodeInfo.strTextureNames[1] == "" ) 
				{
					pGrid->nodeInfo.strTextureNames[0] = m_strDefaultTerrainTexName;
				}
				else
				{
					pGrid->nodeInfo.strTextureNames[0] = pGrid->nodeInfo.strTextureNames[1];
					pGrid->nodeInfo.strTextureNames[1];
				}
			}

			//3
			pGrid->nodeInfo.strTextureNames[2] = "" ;

			//4
			pGrid->nodeInfo.strTextureNames[3] = "" ;
			//////////////////////////////////////////////////////////////

			this->LoadEachGridInfo(fp, pTerrain, pGrid);

			SGrid & gridW = pWater->GetGrid(i);
			gridW.vCenter.y = pGrid->vCenter.y + WATER_LAYER_BASE;

			//progress->SetProgress( float(i+dwVertexCnt)/Total );
		}

#pragma endregion GRID

		///拼接纹理和创建地表纹理
		MakeTerrainTexture(terrainTexIndexsSet);

		for ( DWORD i = 0; i < dwVertexCnt; ++i )
		{
			SVertex &vertex = pTerrain->GetVertex(i);

			if ( vertex.fLogicHeight > 1.0f || vertex.fLogicHeight < -1.0f )
			{
				CVector3f vCenter = vertex.vPosition;
				vertex.GetWireBoxLogicHeight()->SetHeight(vertex.fLogicHeight);
				vertex.GetWireBoxLogicHeight()->SetRelativePos(vCenter);
				vertex.GetWireBoxLogicHeight()->UpdateAbsolute();
			}
		}

		//////////////////////////////////////////////////////////////////////
		///载入阻挡信息
		CToolSetTileBlock::GetInst()->Load(strFileName);

		///pTerrain
		size_t erasebloclsize = 0;
		fread( &erasebloclsize, sizeof(size_t), 1, fp );
		DWORD gridindex = 0;
		for ( size_t i = 0; i < erasebloclsize; ++i )
		{
			fread( &gridindex, sizeof(DWORD), 1, fp );
		}
		//progress->SetProgress( float(dwVertexCnt+dwVertexCnt+2)/Total );

		//////////////////////////////////////////////////////////////////////////
		// water
		LoadWaterInfo(fp);

		//progress->SetProgress( float(dwVertexCnt+dwVertexCnt+2+dwGridCnt/4)/Total );

		int ExtraData = 0;
		TCHAR ExtraName[255];

		//----load point light info
		LoadPointLightInfo(fp);

		///读取是否是bCave信息
		ExtraData = 0;
		fread(&ExtraData,sizeof(int),1,fp);
		if ( ExtraData == 6 )
		{
			fread( ExtraName, ExtraData, 1, fp);

			bool bCave = false;
			fread( &bCave, sizeof(bool), 1, fp);
		}
		else
		{
			::MessageBox( 0, "载入是否室内信息错误", "提示", MB_ICONINFORMATION);
			fclose(fp);
			return;
		}

		//progress->SetProgress( float(dwVertexCnt+dwVertexCnt+2+dwGridCnt/3)/Total );

		//////////////////////////////////////////////////////////////////////////
		///利用新的方式开始读取新增加的数据信息
		DWORD dwVersion;
		fread(&dwVersion,sizeof(DWORD),1,fp);

		///读取聚光灯信息
		switch (dwVersion)
		{
		case VERTEX_BAKING_COLOR:
			{
				for ( DWORD i = 0; i< dwVertexCnt; ++i )
				{
					SVertex &vertex = pTerrain->GetVertex(i);
					fread(&vertex.bColor,sizeof(DWORD),1,fp);
					vertex.tColor = vertex.bColor;
				}
			}

		case SPECULAR_LIGHT_PARA:
			{
				//////////////////////////////////////////////////////////////////////////
				//read specular light parameter
				for (size_t i = 0; i < 4; ++i)
				{
					// light
					DWORD dwSceneSpecurlarColor = 0;
					fread(&dwSceneSpecurlarColor,sizeof(DWORD),1,fp);
					CSceneEnvMgr::GetInst()->SetSceneSpecurlarColor(dwSceneSpecurlarColor);

					DWORD dwPlayerSpecurlarColor = 0;
					fread(&dwPlayerSpecurlarColor,sizeof(DWORD),1,fp);
					CSceneEnvMgr::GetInst()->SetPlayerSpecurlarColor(dwPlayerSpecurlarColor);
				}

				float fSceneSPower = 0.0f;
				fread(&fSceneSPower,sizeof(fSceneSPower),1,fp);
				CSceneEnvMgr::GetInst()->SetSpecularReflectionPower(0, fSceneSPower);

				float fNpcSPower = 0.0f;
				fread(&fNpcSPower,sizeof(fNpcSPower),1,fp);
				CSceneEnvMgr::GetInst()->SetSpecularReflectionPower(1, fNpcSPower);
				//////////////////////////////////////////////////////////////////////////
			}


		case GRID_MATERIALINDEX:
			LoadTerGridMatIndex(fp);

		case MODEL_ANIPLAYMARK:
			LoadOffAnimateModelIndex(fp);

		case MODEL_EXACTBLOCKTYPE:
			LoadExactBlockInfo(fp);

		case WAVE_SIDE_SAVE:
			LoadWaveSideInfo(fp);

		case SPOTLIGHT_SAVE:
			LoadSpotLightInfo(fp);
			break;

		default:
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		///还原之前sColor保存的烘培顶点色
		int diff = dwVersion - SPECULAR_LIGHT_PARA;
		if( diff < 0 )
		{
			for ( DWORD i = 0; i< dwVertexCnt; ++i )
			{
				SVertex &vertex = pTerrain->GetVertex(i);

				vertex.sColor = VERTEX_SPECULAR_COLOR;
			}
		}
		//////////////////////////////////////////////////////////////////////////

		// ----------------------------------------------------------------------------
		// 读取摄像机路径信息

		CToolRecord::GetInst()->Load(fp);

		fclose(fp);

		//progress->SetProgress( 1.0f );

		if (ErrObject.size()>0)
		{
			TCHAR tt[255];
			sprintf( tt, "剔除错误场景物件 %d 个", ErrObject.size() );
			::MessageBox( 0, tt, "提示", MB_ICONINFORMATION);
			vector<string>::iterator eoitor = ErrObject.begin();
			while (eoitor!=ErrObject.end())
			{
				string pr;
				for (int i=0;eoitor!=ErrObject.end() && i<10;++i)
				{
					pr = pr + (*eoitor) + "\n";
					eoitor++;
				}
				::MessageBox( 0, pr.c_str(), "提示", MB_ICONINFORMATION);
			}
		}

		if( pLastTerrain )
			CMapEditApp::GetInst()->SetTerrainMesh(pLastTerrain);
		LogConsole("Map Name is: " << mapname.c_str() << "\n");
	}

	//------------------------------------------------------------------------------
	void
		CLocalMapLoader::LoadAsNewLocal(FILE * fp, DWORD& version, string strFileName)
	{
		//Ast(NULL != progress);
		if ( !fp )
			return;

		CRenderDevice * pRender = NULL;
		pRender = CRenderDevice::GetInst();
		if ( pRender == NULL )
			return;

		//////////////////////////////////////////////////////////////////////////

		///游戏客户端服务器端文件ID号
		DWORD dwGameClientID = 0, dwGameServerID = 0;
		fread(&dwGameClientID,sizeof(DWORD),1,fp);
		fread(&dwGameServerID,sizeof(DWORD),1,fp);

		if ( dwGameServerID != dwGameClientID )
			dwGameClientID = dwGameServerID;

		LogConsole("New Version: " << version << "  GameClientID: " << dwGameClientID << "  GameServerID: " << dwGameServerID << "\n" );

		// background music

		DWORD dwFileNameLen;
		string str;
		fread(&dwFileNameLen,sizeof(DWORD),1,fp);
		str.resize(dwFileNameLen);
		fread((TCHAR*)str.data(),dwFileNameLen,1,fp);

		CSceneEnvMgr::GetInst()->SetBackGroundMusic(str, false);
		///大家希望一进去不听背景音乐，觉得不好听
		CSceneEnvMgr::GetInst()->StopBackMusic();
		// mainplayer
		CToolMainPlayer::GetInst()->Load(fp);
		// camera
		CVector3f vView;
		fread(&vView,sizeof(CVector3f),1,fp);
		if ( vView.y < 0 )
		{
			vView.y = 447.676605f;
		}
		//CMapEditCamera * pCamera = CCameraManager::GetInst()->GetMapEditCamera();
		//pCamera->SetNewTarget(vView);

		//fov
		float fov = 0.0f;
		fread(&fov,sizeof(float),1,fp);
		//yaw
		float yaw = 0.0f;
		fread(&yaw,sizeof(float),1,fp);
		//pitch
		float pitch = 0.0f;
		fread(&pitch,sizeof(float),1,fp);

		//pCamera->SetFovY(fov);
		//pCamera->SetPitchAndYaw(pitch,yaw);

		//////////////////////////////////////////////////////////////////////////
		MapCommonDataLoader::GetInst()->SetOperator(pOperator);
		MapCommonDataLoader::GetInst()->LoadEnvInfo(fp);
		MapCommonDataLoader::GetInst()->LoadBackPictureInfo(fp);

		int fogMode;
		fread(&fogMode,sizeof(int),1,fp);
		CSceneEnvMgr::GetInst()->SetVertexFog((sqr::CSceneEnvMgr::FogModeType)fogMode);

		//全屏泛光
		CSceneEnvMgr::GetInst()->LoadGaussParamsInfo(fp);

		//////////////////////////////////////////////////////////////////////////
		MapCommonDataLoader::GetInst()->LoadWaveInfo(fp);

		// ambient fx info
		CMapEffect::GetInst()->ReadAmbientFXInfo(fp);  //这里很恶心人啊！
		//////////////////////////////////////////////////////////////////////////

		// width depth
		DWORD dwWidth;
		DWORD dwDepth;
		fread(&dwWidth,sizeof(DWORD),1,fp);
		fread(&dwDepth,sizeof(DWORD),1,fp);

		LogConsole("Map Size is: " << dwWidth << " " << dwDepth << "\n" );

		// create a new terrain
		CTerrainMesh *pLastTerrain = NULL;
		pLastTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		//CMapEditApp::GetInst()->GetDataScene()->SetWidth( uint16(dwWidth/4) );
		//CMapEditApp::GetInst()->GetDataScene()->SetHeight( uint16(dwDepth/4) );
		//CMapEditApp::GetInst()->GetDataScene()->Create();
		CSyncSystem::BeginSyncFun();

		//CMapEditApp::GetInst()->InitNewRenderScene();
		//CMapEditApp::GetInst()->GetRenderScene()->InitSceneRes();
		//CMapEditApp::GetInst()->GetDataScene()->SetHeight( uint16(dwDepth/4) );
		//CMapEditApp::GetInst()->GetDataScene()->Create();
		//CSyncSystem::BeginSyncFun();
		//CMapEditApp::GetInst()->InitNewRenderScene();
		//CMapEditApp::GetInst()->GetRenderScene()->InitSceneRes();

		CSyncSystem::BeginSyncFun();
		CToolMainPlayer::GetInst()->CreateModel();
		CTerrainMesh * pTerrain = new CTerrainMesh; 
		pTerrain->Create(dwWidth,dwDepth,m_strDefaultTerrainTexName,false,false);
		if( pLastTerrain != NULL )
		{
			CMapEditApp::GetInst()->SetTransformTerrainMesh(pTerrain);
		}
		CMapEditApp::GetInst()->SetTerrainMesh(pTerrain);
		pTerrain->SetTerrainVersion(version);

		CGraphic::GetInst()->SetAsynSwitch(false);
		CPkgFile::EnableAsyncRead(false);

		MapCommonDataLoader::GetInst()->SetTerrainMesh(pTerrain);

		pOperator->InitProcessParams(false,version);

		size_t npos = strFileName.rfind("\\");
		string mapname = "";
		if ( npos != -1 )
		{
			mapname = strFileName.substr(npos+1, strFileName.length());

			npos = mapname.rfind(".");
			if ( npos != -1 )
				mapname = mapname.substr(0, npos);
		}

		pTerrain->m_strMapName = mapname;

		CTerrainMesh * pWater = new CTerrainMesh; 
		pWater->Create(dwWidth,dwDepth,m_strDefaultWaterTexName,true,false);
		pTerrain->SetWater(pWater);

		pTerrain->SetGameGacMapVersion(dwGameClientID);
		pTerrain->SetGameGasMapVersion(dwGameServerID);

		//////////////////////////////////////////////////////////////////////////
		///read terrain road and sub texture name
		size_t size = 0;
		int iNameLen = 0, weight = 0;
		string texname = "";
		fread(&size,sizeof(size_t),1,fp);
		for ( size_t i = 0; i < size; ++i )
		{
			fread(&iNameLen,sizeof(int),1,fp);
			texname.resize(iNameLen);
			fread((TCHAR*)texname.data(),iNameLen,1,fp);
			fread(&weight,sizeof(int),1,fp);

			pTerrain->m_MainRoadWeight[texname] = weight;
		}
		//////////////////////////////////////////////////////////////////////////

		DWORD dwVertexCnt = pTerrain->GetVertexCount();
		DWORD dwGridCnt	  = pTerrain->GetGridCount();
		float Total = float(dwVertexCnt + dwGridCnt + dwGridCnt/2);
		//progress->SetProgress( 0.0f );

#pragma region VERTEX
		short s = 0;
		for ( DWORD i = 0; i< dwVertexCnt; ++i )
		{
			SVertex *pVertex = &pTerrain->GetVertex(i);
			MapCommonDataLoader::GetInst()->LoadEachVertexInfo(fp, pVertex);

			pWater->GetVertex(i).vPosition.y = pVertex->vPosition.y + WATER_LAYER_BASE;

			//progress->SetProgress( float(i)/Total );
		}
#pragma endregion VERTEX

		//////////////////////////////////////////////////////////////////////////
		///model property version
		fread(&uModelProVersion,sizeof(uModelProVersion),1,fp);

		///grid property version
		fread(&uGridProVersion,sizeof(uGridProVersion),1,fp);
		//////////////////////////////////////////////////////////////////////////

		// grids
		int dwNameLen = 0;
		int dwTexCnt = 0;
		set<string> terrainTexIndexsSet;
		vector<string> terrainTexIndexsVec;
		string gridtexname = "";
		size_t nPos = 0;

#pragma region TEXTURE
		///读取地图用到的所有贴图
		map<int16, string> terrainTextureNames;///用于记录地图中用到的所有纹理的名字
		this->LoadTerrainGridTexIndex(fp, terrainTextureNames);
#pragma endregion TEXTURE

#pragma region GRID

		CTObject * p = NULL;
		map<string, int16>::iterator texiter;
		int16  nTextureIndex = 0;

		for ( DWORD i = 0; i < dwGridCnt; ++i )
		{
			SGrid * pGrid = &pTerrain->GetGrid(i);
			//grid hide mask
			fread(&pGrid->bGridHide,sizeof(bool),1,fp);

			// 			if( pGrid->bGridHide )
			// 				DebugOut("gird %d, bhide %d\n", i, pGrid->bGridHide);
			//////////////////////////////////////////////////////////////
			//读取纹理信息
			//1
			fread(&nTextureIndex,sizeof(nTextureIndex),1,fp);
			gridtexname= terrainTextureNames[nTextureIndex] ;

			// 			DebugOut("Grid %d\n", i);
			// 			if( gridtexname != "" )
			// 				DebugOut("tex0 %s, %d\n", gridtexname.c_str(), nTextureIndex);
			pGrid->nodeInfo.strTextureNames[0]  = gridtexname;

			if ( terrainTexIndexsSet.find(gridtexname) == terrainTexIndexsSet.end() && gridtexname != "" )
			{
				terrainTexIndexsSet.insert(gridtexname);
				terrainTexIndexsVec.push_back(gridtexname);
				++dwTexCnt;
			}

			//2
			fread(&nTextureIndex,sizeof(nTextureIndex),1,fp);
			gridtexname= terrainTextureNames[nTextureIndex] ;
			pGrid->nodeInfo.strTextureNames[1]  = gridtexname;
			// 			if( gridtexname != "" )
			// 				DebugOut("tex1 %s, %d\n", gridtexname.c_str(), nTextureIndex);

			if ( terrainTexIndexsSet.find(gridtexname) == terrainTexIndexsSet.end() && gridtexname != "" )
			{
				terrainTexIndexsSet.insert(gridtexname);
				terrainTexIndexsVec.push_back(gridtexname);

				++dwTexCnt;
			}

			// 保证至少有第一层纹理，而且不会出现第一层纹理为空而存在第二层纹理的情况
			if ( pGrid->nodeInfo.strTextureNames[0] == "" )
			{
				if ( pGrid->nodeInfo.strTextureNames[1] == "" ) 
				{
					pGrid->nodeInfo.strTextureNames[0] = m_strDefaultTerrainTexName;
				}
				else
				{
					pGrid->nodeInfo.strTextureNames[0] = pGrid->nodeInfo.strTextureNames[1];
					pGrid->nodeInfo.strTextureNames[1];
				}
			}

			//3
			pGrid->nodeInfo.strTextureNames[2] = "" ;

			//4
			pGrid->nodeInfo.strTextureNames[3] = "" ;
			//////////////////////////////////////////////////////////////

			this->LoadEachGridInfo(fp, pTerrain, pGrid);

			SGrid & gridW = pWater->GetGrid(i);
			gridW.vCenter.y = pGrid->vCenter.y + WATER_LAYER_BASE;

			//progress->SetProgress( float(i+dwVertexCnt)/Total );
		}

#pragma endregion GRID

		///拼接纹理和创建地表纹理
		MakeTerrainTexture(terrainTexIndexsSet);

		for ( DWORD i = 0; i < dwVertexCnt; ++i )
		{
			SVertex &vertex = pTerrain->GetVertex(i);

			if ( vertex.fLogicHeight > 1.0f || vertex.fLogicHeight < -1.0f )
			{
				CVector3f vCenter = vertex.vPosition;
				vertex.GetWireBoxLogicHeight()->SetHeight(vertex.fLogicHeight);
				vertex.GetWireBoxLogicHeight()->SetRelativePos(vCenter);
				vertex.GetWireBoxLogicHeight()->UpdateAbsolute();
			}
		}

		//////////////////////////////////////////////////////////////////////
		///载入阻挡信息
		CToolSetTileBlock::GetInst()->Load(strFileName);

		///pTerrain
		size_t erasebloclsize = 0;
		fread( &erasebloclsize, sizeof(size_t), 1, fp );
		DWORD gridindex = 0;
		for ( size_t i = 0; i < erasebloclsize; ++i )
		{
			fread( &gridindex, sizeof(DWORD), 1, fp );
		}
		//progress->SetProgress( float(dwVertexCnt+dwVertexCnt+2)/Total );

		//////////////////////////////////////////////////////////////////////////
		// water
		LoadWaterInfo(fp);

		//progress->SetProgress( float(dwVertexCnt+dwVertexCnt+2+dwGridCnt/4)/Total );

		int ExtraData = 0;
		TCHAR ExtraName[255];

		//----load point light info
		LoadPointLightInfo(fp);

		///读取是否是bCave信息
		ExtraData = 0;
		fread(&ExtraData,sizeof(int),1,fp);
		if ( ExtraData == 6 )
		{
			fread( ExtraName, ExtraData, 1, fp);

			bool bCave = false;
			fread( &bCave, sizeof(bool), 1, fp);
		}
		else
		{
			::MessageBox( 0, "载入是否室内信息错误", "提示", MB_ICONINFORMATION);
			fclose(fp);
			return;
		}

		//progress->SetProgress( float(dwVertexCnt+dwVertexCnt+2+dwGridCnt/3)/Total );

		//////////////////////////////////////////////////////////////////////////
		///利用新的方式开始读取新增加的数据信息
		DWORD dwVersion;
		fread(&dwVersion,sizeof(DWORD),1,fp);

		///读取聚光灯信息
		switch (dwVersion)
		{
		case VERTEX_BAKING_COLOR:
			{
				for ( DWORD i = 0; i< dwVertexCnt; ++i )
				{
					SVertex &vertex = pTerrain->GetVertex(i);
					fread(&vertex.bColor,sizeof(DWORD),1,fp);
					vertex.tColor = vertex.bColor;
				}
			}

		case SPECULAR_LIGHT_PARA:
			{
				//////////////////////////////////////////////////////////////////////////
				//read specular light parameter
				for (size_t i = 0; i < 4; ++i)
				{
					// light
					DWORD dwSceneSpecurlarColor = 0;
					fread(&dwSceneSpecurlarColor,sizeof(DWORD),1,fp);
					CSceneEnvMgr::GetInst()->SetSceneSpecurlarColor(dwSceneSpecurlarColor);

					DWORD dwPlayerSpecurlarColor = 0;
					fread(&dwPlayerSpecurlarColor,sizeof(DWORD),1,fp);
					CSceneEnvMgr::GetInst()->SetPlayerSpecurlarColor(dwPlayerSpecurlarColor);
				}

				float fSceneSPower = 0.0f;
				fread(&fSceneSPower,sizeof(fSceneSPower),1,fp);
				CSceneEnvMgr::GetInst()->SetSpecularReflectionPower(0, fSceneSPower);

				float fNpcSPower = 0.0f;
				fread(&fNpcSPower,sizeof(fNpcSPower),1,fp);
				CSceneEnvMgr::GetInst()->SetSpecularReflectionPower(1, fNpcSPower);
				//////////////////////////////////////////////////////////////////////////
			}


		case GRID_MATERIALINDEX:
			LoadTerGridMatIndex(fp);

		case MODEL_ANIPLAYMARK:
			LoadOffAnimateModelIndex(fp);

		case MODEL_EXACTBLOCKTYPE:
			LoadExactBlockInfo(fp);

		case WAVE_SIDE_SAVE:
			LoadWaveSideInfo(fp);

		case SPOTLIGHT_SAVE:
			LoadSpotLightInfo(fp);
			break;

		default:
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		///还原之前sColor保存的烘培顶点色
		int diff = dwVersion - SPECULAR_LIGHT_PARA;
		if( diff < 0 )
		{
			for ( DWORD i = 0; i< dwVertexCnt; ++i )
			{
				SVertex &vertex = pTerrain->GetVertex(i);

				vertex.sColor = VERTEX_SPECULAR_COLOR;
			}
		}
		//////////////////////////////////////////////////////////////////////////

		// ----------------------------------------------------------------------------
		// 读取摄像机路径信息

		CToolRecord::GetInst()->Load(fp);

		fclose(fp);

		//progress->SetProgress( 1.0f );

		if (ErrObject.size()>0)
		{
			TCHAR tt[255];
			sprintf( tt, "剔除错误场景物件 %d 个", ErrObject.size() );
			::MessageBox( 0, tt, "提示", MB_ICONINFORMATION);
			vector<string>::iterator eoitor = ErrObject.begin();
			while (eoitor!=ErrObject.end())
			{
				string pr;
				for (int i=0;eoitor!=ErrObject.end() && i<10;++i)
				{
					pr = pr + (*eoitor) + "\n";
					eoitor++;
				}
				::MessageBox( 0, pr.c_str(), "提示", MB_ICONINFORMATION);
			}
		}

		LogConsole("Map Name is: " << mapname.c_str() << "\n");

		if( pLastTerrain )
			CMapEditApp::GetInst()->SetTerrainMesh(pLastTerrain);
	}

	//------------------------------------------------------------------------------
	void
	CLocalMapLoader::LoadPointLightInfo(FILE * fp)
	{
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetTerrain();

		int ExtraData = 0;
		TCHAR ExtraName[255];

		///读取场景所有点光源信息
		fread(&ExtraData,sizeof(int),1,fp);
		if ( ExtraData == 11 )
		{
			fread( ExtraName, ExtraData, 1, fp);

			stringstream errmsg;
			int SceneLightCount = 0;
			fread( &SceneLightCount, sizeof(int), 1, fp);
			SLB temp;
			for (int i=0;i<SceneLightCount;++i)
			{
				fread( &temp.GridIndex, sizeof(int), 1, fp);
				//DebugOut("grid %d\n", temp.GridIndex);

				//////////////////////////////////////////////////////////////////////////

				fread( &temp.iter, sizeof(int), 1, fp);
				fread( &temp.m_SL, sizeof(SceneLight), 1, fp);

#ifdef USE_OLD_LIGHT_MODEL
				temp.m_SL.m_bEffectTerrain = temp.m_SL.m_bEffectTerrain;
#else
				///强行将是否影响地表标志改成true，全部影响
				temp.m_SL.m_bEffectTerrain = true;
#endif

				int gridModelIndex = 0;
				gridModelIndex = temp.iter;
				bool OK = false;

				if ( pTerrain->IsValid(temp.GridIndex) )
				{
					SGrid *pGrid = NULL;
					pGrid = &pTerrain->GetGrid(temp.GridIndex);
					size_t size = pGrid->vecCenterObjects.size();

					if ( !pGrid->vecCenterObjects.empty() )
					{
						if ( gridModelIndex < (int)(size) )
						{
							CTObjectPtr p = pGrid->vecCenterObjects[temp.iter];

							p->GetModelGroup()->m_SL = temp.m_SL;
							p->GetModelGroup()->SetIsLightEffect(true);
							OfflineRender::GetInst()->UpdateGridLights(temp.GridIndex);

							//insert
							pTerrain->m_DynamicLightGrids.insert(temp.GridIndex);

							OK = true;
						}
						else
						{
							map<int, vector<int>>::iterator iter = LoadFaildModelMap.find(temp.GridIndex);
							CEditModelGroup *pModelGroup = NULL;
							if ( iter != LoadFaildModelMap.end() )
							{
								if ( gridModelIndex - 1 < int(size) )
								{
									pModelGroup = pGrid->vecCenterObjects[gridModelIndex - 1]->m_pModelGroup;

									if ( pModelGroup && pModelGroup->GetIsEffectBased())
									{
										pModelGroup->m_SL = temp.m_SL;
										pModelGroup->SetIsLightEffect(true);

										//insert
										pTerrain->m_DynamicLightGrids.insert(temp.GridIndex);
									}
								}
								::MessageBox( 0, "自动挪位一个", "提示", MB_ICONINFORMATION);
							}
							else //
							{
								CTObjectPtr p = pGrid->vecCenterObjects[size-1];
								CEditModelGroup *pModelGroup = p->GetModelGroup();
								if( pModelGroup )
								{
									string strEffectName = "";
									pModelGroup->GetEffectName(strEffectName);
									string strItemName = pModelGroup->GetItemName();

									if( strItemName == "effectdummy" && strEffectName.find("场景光源\\Standar") != -1 )
									{
										pModelGroup->m_SL = temp.m_SL;
										pModelGroup->SetIsLightEffect(true);
										OfflineRender::GetInst()->UpdateGridLights(temp.GridIndex);

										//insert
										pTerrain->m_DynamicLightGrids.insert(temp.GridIndex);

										OK = true;
									}
									else
										OK = false;
								}
								else
									OK = false;
							}
						}
					}
				}
				OfflineRender::GetInst()->UpdateGridLights(temp.GridIndex);

				if ( !OK )
				{
					TCHAR str[255];
					sprintf( str, "%d", temp.GridIndex );

					string log = str;
					log = log +  "载入场景点光源错误";

					::MessageBox( 0, log.c_str(), "提示", MB_ICONINFORMATION);
				}
			}
		}
		else
		{
			::MessageBox( 0, "载入场景灯信息错误", "提示", MB_ICONINFORMATION);
			fclose(fp);

			return;
		}
	}

	//------------------------------------------------------------------------------
	void
	CLocalMapLoader::LoadSpotLightInfo(FILE * fp)
	{
		int nSceneSpotLightCnt = 0;
		fread( &nSceneSpotLightCnt, sizeof(int), 1, fp);
		SpotLB temp;

		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		for ( int i=0; i < nSceneSpotLightCnt; ++i )
		{
			fread( &temp.GridIndex, sizeof(int), 1, fp);

			fread( &temp.iter, sizeof(size_t), 1, fp);
			fread( &temp.m_SpotLight, sizeof(SceneSpotLight), 1, fp);

#ifdef USE_OLD_LIGHT_MODEL
			temp.m_SpotLight.m_bEffectTerrain = temp.m_SpotLight.m_bEffectTerrain;
#else
			///强行将是否影响地表标志改成true，全部影响
			temp.m_SpotLight.m_bEffectTerrain = true;
#endif

			int gridModelIndex = 0;
			gridModelIndex = temp.iter;
			bool OK = false;

			if ( pTerrain->IsValid(temp.GridIndex) )
			{
				SGrid *pGrid = NULL;
				pGrid = &pTerrain->GetGrid(temp.GridIndex);
				size_t size = pGrid->vecCenterObjects.size();

				if ( !pGrid->vecCenterObjects.empty() )
				{
					if ( gridModelIndex < (int)(size) )
					{
						CTObjectPtr p = pGrid->vecCenterObjects[gridModelIndex];
						CEditModelGroup *pModelGroup = p->GetModelGroup();

						if ( pModelGroup )
						{
							string str = pModelGroup->m_strItemName;
							size_t npos = str.find("effectdummy");

							if ( npos != -1 )
							{
								float x = pModelGroup->GetShiftXEx();
								float z = pModelGroup->GetShiftZEx();
								float y = pModelGroup->GetLiftEx();
								CVector3f lightPos = p->GetModelGroup()->GetSelfPresentation(0)->GetGridCenter();

								lightPos.x += x;
								lightPos.y += y;
								lightPos.z += z;

								pModelGroup->m_SpotLightPos = lightPos;
								pModelGroup->m_SpotLight = temp.m_SpotLight;

								pTerrain->m_DynamicLightGrids.insert(temp.GridIndex);
								OfflineRender::GetInst()->UpdateGridLights(temp.GridIndex);
								pModelGroup->SetIsLightEffect(true);

								OK = true;
							}
						}
					}
				else //
				{
					CTObjectPtr p = pGrid->vecCenterObjects[size-1];
					CEditModelGroup *pModelGroup = p->GetModelGroup();
					if( pModelGroup )
					{
						string strEffectName = "";
						pModelGroup->GetEffectName(strEffectName);
						string strItemName = pModelGroup->GetItemName();

						if( strItemName == "effectdummy" && strEffectName.find("场景光源\\Standar") != -1 )
						{
							float x = pModelGroup->GetShiftXEx();
							float z = pModelGroup->GetShiftZEx();
							float y = pModelGroup->GetLiftEx();
							CVector3f lightPos = p->GetModelGroup()->GetSelfPresentation(0)->GetGridCenter();

							lightPos.x += x;
							lightPos.y += y;
							lightPos.z += z;

							pModelGroup->m_SpotLightPos = lightPos;
							pModelGroup->m_SpotLight = temp.m_SpotLight;

							pTerrain->m_DynamicLightGrids.insert(temp.GridIndex);
							OfflineRender::GetInst()->UpdateGridLights(temp.GridIndex);
							pModelGroup->SetIsLightEffect(true);

							OK = true;
						}
						else
							OK = false;
					}
					else
						OK = false;
				}
				}
			}

			if ( !OK )
			{
				TCHAR str[255];
				sprintf( str, "%d", temp.GridIndex );

				string log = str;
				log = log +  "载入场景聚光灯错误";

				::MessageBox( 0, log.c_str(), "提示", MB_ICONINFORMATION);
			}
		}
	}

	//------------------------------------------------------------------------------
	void
		CLocalMapLoader::LoadExactBlockInfo(FILE * fp)
	{
		int dwCnt = 0, nGridIndex = 0;
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		///设置了精确阻挡的个数
		fread(&dwCnt, sizeof(dwCnt), 1, fp);

		for ( int i = 0; i < dwCnt; ++i )
		{
			fread(&nGridIndex, sizeof(nGridIndex), 1, fp);
			SGrid & grid = pTerrain->GetGrid(nGridIndex);

			grid.m_bSetExactBlock = true;
			fread(&grid.subGridTypes, sizeof(grid.subGridTypes), 1, fp);
		}
	}

	//------------------------------------------------------------------------------
	void
		CLocalMapLoader::LoadWaveSideInfo(FILE * fp)
	{
		uint fourcc;
		fread(&fourcc, sizeof(uint), 1, fp);
		Ast(fourcc == 'WVSD');
		CTerrainMesh* water = CMapEditApp::GetInst()->GetWater();
		SGrid* grids = water->GetGrids();
		vector<byte> waveGrids;
		waveGrids.resize(water->GetGridCount());
		fread(&waveGrids.front(), sizeof(byte), waveGrids.size(), fp);
		for (index_t i = 0; i < waveGrids.size(); ++i)
		{
			grids[i].bSide = waveGrids[i] == 1;

			if (!grids[i].bSide && NULL != grids[i].pWave)
			{
				index_t regionId = water->GetRegionIndexByGridIndex(grids[i].dwGridIndex);
				grids[i].pRiver->RemoveWave(regionId, grids[i].pWave);

				grids[i].pWave = NULL;
			}
		}
	}

	//------------------------------------------------------------------------------
	void
		CLocalMapLoader::LoadOffAnimateModelIndex(FILE * fp)
	{
		int dwCnt = 0, nGridIndex = 0;
		size_t gridModelIndex = 0;
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		///设置个数
		fread(&dwCnt, sizeof(dwCnt), 1, fp);

		for ( int i = 0; i < dwCnt; ++i )
		{
			fread(&nGridIndex, sizeof(nGridIndex), 1, fp);
			fread( &gridModelIndex, sizeof(size_t), 1, fp);

 			if ( pTerrain->IsValid(nGridIndex) )
 			{
 				SGrid & grid = pTerrain->GetGrid(nGridIndex);
 
 				if ( !grid.vecCenterObjects.empty() )
 				{
 					if ( gridModelIndex < grid.vecCenterObjects.size() )
 					{
 						CTObjectPtr p = grid.vecCenterObjects[gridModelIndex];
 
 						p->GetModelGroup()->StopModelAnimation();
 					}
 				}
 			}
		}
	}

	//------------------------------------------------------------------------------
	void
		CLocalMapLoader:: LoadTerGridMatIndex(FILE* fp)
	{
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetTerrain();
		int nGridCount = pTerrain->GetGridCount();
		int8 nMatIndex  = -1;

		for ( int i = 0; i < nGridCount; ++i )
		{
			fread(&nMatIndex,sizeof(nMatIndex),1,fp);

			SGrid * pGrid = &pTerrain->GetGrid(i);
			pGrid->nMaterialIndex = nMatIndex;

			// 			if( nMatIndex != -1 )
			// 				DebugOut("grid %d, %d\n", i, nMatIndex);
		}

	}

	//------------------------------------------------------------------------------
	void
		CLocalMapLoader::LoadWaterInfo(FILE* fp)
	{
		CTerrainMesh * pTerrain			= CMapEditApp::GetInst()->GetTerrain();
		CTerrainMesh * pWater			= pTerrain->GetWater();
		CWaterOperator* pWaterOperator	= CMapEditApp::GetInst()->GetWaterOperator();
		vector<int> vecGridIndicesRiver;

		DWORD dwRiverCnt;
		fread(&dwRiverCnt,sizeof(DWORD),1,fp);

		for ( size_t nRiverIndex = 0; nRiverIndex < dwRiverCnt; ++nRiverIndex )
		{
			DWORD dwItemNameLen;
			fread(&dwItemNameLen,sizeof(DWORD),1,fp);
			string strItemName;
			strItemName.resize(dwItemNameLen);
			fread((TCHAR*)strItemName.data(),dwItemNameLen,1,fp);

			if( strItemName.find("test") == -1 )
			{
				if( strItemName.find("环境映射水") == -1 )
				{
					if( strItemName.find("岩浆") == -1 )
					{
						string log = "";
						log = "地图中存在水的纹理是用地表纹理刷的，刷的纹理图素包包名是：" + strItemName;
						log = log + "\n是否需要将其替换成环境映射水，替换点YES，不替换NO";
						if ( IDYES == MessageBox(NULL, log.c_str(), "提示", MB_YESNO) )
							strItemName = "环境映射水";
					}
				}
			}

			pWaterOperator->RebirthRiver(strItemName);
			CRiver * pRiver = pWaterOperator->GetRiver();

			DWORD dwWaterCnt;
			fread(&dwWaterCnt,sizeof(DWORD),1,fp);

			for ( size_t nWaterIndex = 0; nWaterIndex < dwWaterCnt; ++nWaterIndex )
			{
				int nGridIndex;
				fread(&nGridIndex,sizeof(DWORD),1,fp);

				vecGridIndicesRiver.push_back(nGridIndex);

				SGrid & gridW = pWater->GetGrid(nGridIndex);
				gridW.pRiver = pRiver;

				fread(&gridW.vCenter.y,sizeof(float),1,fp);

				for ( int j = 0; j < 4; ++j )
				{
					int nVertexIndex = gridW.dwVertexIndex[j];
	
					SVertex & vertex = pWater->GetVertex(nVertexIndex);

					fread(&vertex.vPosition.y,sizeof(float),1,fp);

					fread(&vertex.oColor,sizeof(DWORD),1,fp);
				}
			}
		}

		DWORD dwUnoccupiedWaterCnt;
		fread(&dwUnoccupiedWaterCnt,sizeof(DWORD),1,fp);

		for ( size_t nWaterIndex = 0; nWaterIndex < dwUnoccupiedWaterCnt; ++nWaterIndex )
		{
			int nGridIndex;
			fread(&nGridIndex,sizeof(DWORD),1,fp);

			vecGridIndicesRiver.push_back(nGridIndex);

			SGrid & gridW = pWater->GetGrid(nGridIndex);

			fread(&gridW.vCenter.y,sizeof(float),1,fp);

			for ( int j = 0; j < 4; ++j )
			{
				int nVertexIndex = gridW.dwVertexIndex[j];

				SVertex & vertex = pWater->GetVertex(nVertexIndex);

				fread(&vertex.vPosition.y,sizeof(float),1,fp);
				fread(&vertex.oColor,sizeof(DWORD),1,fp);
			}
		}

		for (size_t i = 0; i < vecGridIndicesRiver.size(); ++i )
		{
			pWater->AdjustWaterMeshVisibility(vecGridIndicesRiver[i],true);
		}
	}

	//------------------------------------------------------------------------------
	void
	CLocalMapLoader::MakeTerrainTexture(set<string> terrainTexIndexsSet)
	{
		string texname = "";
		CRenderDevice * pRender = NULL;
		pRender = CRenderDevice::GetInst();
		if ( pRender == NULL )
			return;

		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetTerrain();

		int texCnt = 0;
		size_t textureCnt = terrainTexIndexsSet.size();

		ITexture* pTexture;

		if ( pTerrain->m_pTerrainTexture == NULL )
			CGraphic::GetInst()->CreateEmptyTexture(EXPAND_MAX_SIZE,EXPAND_MAX_SIZE,1, TF_XRGB32,&(pTerrain->m_pTerrainTexture));
		/*	GetD3DDevice()->CreateTexture(
		EXPAND_MAX_SIZE, EXPAND_MAX_SIZE, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &pTerrain->m_pD3DTerrainTexture, NULL );*/

		set<string>::iterator iter = terrainTexIndexsSet.begin();
		set<string>::iterator eiter = terrainTexIndexsSet.end();
		set<string> erroTexList;
		for ( iter; iter != eiter; ++iter )
		{
			texname = *iter;
			pTexture = pRender->GetTexture(texname);
			if( texname.find("water.dds") != -1 )
				continue;

			if (NULL==pTexture)
			{
				string log = "拼接过程:"+texname+"纹理无法找到";
				MessageBox(NULL,log.c_str(),"错误",MB_OK);
				erroTexList.insert(texname);
				continue;
			}

			if (NULL==pTexture->GetBuffer())
			{
				string log = "拼接过程:"+texname+"纹理无法找到";
				MessageBox(NULL,log.c_str(),"错误",MB_OK);
				continue;
			}

			UINT H = (texCnt/ROW_SIZE)*EXPAND_TEXTURE_SIZE;
			UINT W = (texCnt%ROW_SIZE)*EXPAND_TEXTURE_SIZE;


			if (pTexture->GetWidth()!=TEXTURE_SIZE||pTexture->GetHeight()!=TEXTURE_SIZE)
			{
				string log = "拼接过程:"+texname+"尺寸不合法,所有贴图纹理应为128X128";
				erroTexList.insert(texname);
				MessageBox(NULL,log.c_str(),"错误",MB_OK);
				continue;
			}

			pRender->m_TerrainTexIndexs[texname] = texCnt;
			//DebugOut("%d, %s\n", texCnt, texname.c_str());
			++texCnt;

			EDITOR_TRY
			{
				CIRect rectT,rectO;
				_TEXLOCKED_RECT lockT,lockO;
				rectO.top = rectO.left = 0;
				rectO.bottom = rectO.right = TEXTURE_SIZE;
				rectT.top = H;
				rectT.bottom = H + EXPAND_TEXTURE_SIZE;
				rectT.left = W;
				rectT.right = W + EXPAND_TEXTURE_SIZE;
				pTerrain->m_pTerrainTexture->Lock(0,&lockT,&rectT,0,LOCK_DISCARD);
				pTexture->Lock(0,&lockO,&rectO,0,LOCK_READONLY);
				//pTerrain->pTar = (TCHAR*)lockT.pBits;
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
			}
			EDITOR_CATCH(exp)
			{
				erroTexList.insert(texname);
				string log = exp.ErrorMsg();
				string msg = texname + "地表纹理有可能是水纹理，导致无法拼接地图，因此该格子纹理是空黑色，需要重刷";
				::MessageBox(NULL, msg.c_str(), "提示", MB_ICONEXCLAMATION);
			}
			EDITOR_TRY_END;

			pTexture->Unlock(0);
			pTerrain->m_pTerrainTexture->Unlock(0);
		}

		///删掉载入不成功的纹理列表
		set<string>::iterator erroriter = erroTexList.begin();
		set<string>::iterator eeiter = erroTexList.end();
		map<string, DWORD>::iterator fiter;
		map<string, DWORD>::iterator feiter = pRender->m_TerrainTexIndexs.end();
		for (erroriter; erroriter != eeiter; ++erroriter)
		{
			texname = *erroriter;

			fiter = pRender->m_TerrainTexIndexs.find(texname);

			if ( fiter != feiter )
				pRender->m_TerrainTexIndexs.erase(fiter);
		}

		int nGridCnt = pTerrain->GetGridCount();
		string gridtexname = "";
		for ( int i = 0; i < nGridCnt; ++i )
		{
			SGrid &Grid = pTerrain->GetGrid(i);

			gridtexname = Grid.nodeInfo.strTextureNames[0];
			erroriter = erroTexList.find(gridtexname);
			if (erroriter != eeiter)
				Grid.nodeInfo.strTextureNames[0] = "";

			gridtexname = Grid.nodeInfo.strTextureNames[1];
			erroriter = erroTexList.find(gridtexname);
			if (erroriter != eeiter)
				Grid.nodeInfo.strTextureNames[1] = "";
		}
		texCnt = 0;
		map<string, DWORD>::iterator titer = pRender->m_TerrainTexIndexs.begin();
		map<string, DWORD>::iterator teiter = pRender->m_TerrainTexIndexs.end();

		for ( titer; titer != teiter; ++titer )
		{
			texname = titer->first;
			pRender->m_TerrainTexIndexs[texname] = texCnt;

			++texCnt;
		}

		IoSystem* ioSystem = IoSystem::GetInst();
		string homePath = IoSystem::GetInst()->GetFullPath("home:");
		string selfTexPath = homePath + pTerrain->m_strMapName + ".jpg";

		pTerrain->m_pTerrainTexture->SaveToFile(selfTexPath.c_str(),IFF_JPG);
	}

	void CLocalMapLoader::LoadTerrainGridTexIndex( FILE * fp, map<int16, string>& terrainTextureNames )
	{
		int16  nTextureIndex = 0;
		size_t nTerrainTexCnt = 0;
		string gridtexname = "";
		DWORD  dwNameLen = 0;

		fread(&nTerrainTexCnt,sizeof(nTerrainTexCnt),1,fp);
		for ( size_t i = 0; i < nTerrainTexCnt; ++i )
		{
			fread(&dwNameLen,sizeof(int),1,fp);
			gridtexname.resize(dwNameLen);
			fread((TCHAR*)gridtexname.data(),dwNameLen,1,fp);
			fread(&nTextureIndex,sizeof(nTextureIndex),1,fp);

			use_facet<ctype<TCHAR> >( loc ).tolower
				( &gridtexname[0], &gridtexname[gridtexname.length()] );

			if (gridtexname.find(".tga")!=string::npos)
			{
				gridtexname = gridtexname.substr(0, gridtexname.rfind(".tga")) + ".dds";
			}

			if (gridtexname.find("defaultlayerone") != string::npos ||
				gridtexname.find("defaultlayertwo") != string::npos)
			{
				gridtexname = m_strDefaultTerrainTexName;
			}

			if( gridtexname.find("default.bmp") == 0 )
				gridtexname = m_strDefaultTerrainTexName;

			if( gridtexname.find("water.dds") != -1 )
				gridtexname = m_strDefaultTerrainTexName;

			terrainTextureNames[nTextureIndex] = gridtexname;
		}
	}

	void CLocalMapLoader::LoadEachGridInfo( FILE * fp, CTerrainMesh*pTerrain, SGrid* pGrid )
	{
		//////////////////////////////////////////////////////////////////////////
		///读取地表标志等属性
		switch (uGridProVersion)
		{
		case GRIDPRO_ENFORCELOWBLOCK:
			fread(&pGrid->bCliffLayerOne, sizeof(bool), 1, fp);

		case GRIDPRO_ENFORCEMAT:
			fread(&pGrid->bMatNotEffectByTileSets, sizeof(bool), 1, fp);

		case DEFAULT_GRIDPRO:
			fread(&pGrid->bCliffLayerOne, sizeof(bool), 1, fp);

		default:
			break;
		}
		//////////////////////////////////////////////////////////////////////////

		//读取纹理坐标信息,第1层纹理坐标的信息
		for (int m = 0; m < 4; ++m)
		{
			fread(&pGrid->uv[0][m].x, sizeof(float), 1, fp);
			fread(&pGrid->uv[0][m].y, sizeof(float), 1, fp);
		}

		fread(&pGrid->vCenter.y,sizeof(float),1,fp);

		//bool bEffectByVertexColor = true;
		fread(&pGrid->bEffectByVertexColor,sizeof(bool),1,fp);
		fread(&pGrid->bCliffLayerOne,sizeof(bool),1,fp);

		// any central Object?
		map<string, MoldeSaveProperty> sGridModelItemNames, sGridEffectNames;
		map<string, MoldeSaveProperty>::iterator modelfind, effectfind;
		// 		map<string, int> sGridEffectNames;
		// 		map<string, int>::iterator effectfind;
		DWORD dw = 0;
		int   nGridIndex = pGrid->dwGridIndex;
		fread(&dw,sizeof(DWORD),1,fp);

		vector<int> errorlist;
		for (size_t j = 0; j < dw; j++ )
		{
			// effect bound?
			bool bEffectBound = false;
			fread(&bEffectBound,sizeof(bool),1,fp);
			string strEffectName = "";
			if ( bEffectBound )
			{
				DWORD dwItemNameLen;
				fread(&dwItemNameLen,sizeof(DWORD),1,fp);
				strEffectName.resize(dwItemNameLen);
				fread((TCHAR*)strEffectName.data(),dwItemNameLen,1,fp);
			}

			//////////////////////////////////////////////////////////////////////////

			DWORD dwItemNameLen;
			fread(&dwItemNameLen,sizeof(DWORD),1,fp);
			string strItemName;
			strItemName.resize(dwItemNameLen);
			fread((TCHAR*)strItemName.data(),dwItemNameLen,1,fp);

			use_facet<ctype<TCHAR> >( loc ).tolower
				( &strItemName[0], &strItemName[strItemName.length()] );

			// 			if( strItemName == "effectdummy" && strEffectName.find("场景光源") == -1 )
			// 			{
			// 				bEffectBound = false;
			// 			}

			//////////////////////////////////////////////////////////////////////////
			int nAtten = 0;
			fread(&nAtten,sizeof(int),1,fp);

			///nAffectRadio
			int nRatio = 0;
			fread( &nRatio, sizeof(int), 1, fp);

			MoldeSaveProperty sModelSavePro;

			switch (uModelProVersion)
			{
			case DEFAULT_MODELPRO:
				{
					MoldeSavePropertyOld sModelSaveProOld;
					fread( &sModelSaveProOld, sizeof(sModelSaveProOld), 1, fp);
					sModelSavePro = sModelSaveProOld;
				}
				break;

			case MODELPRO_EYECOLLISION:
				{
					MoldeSavePropertyNoPos sModelSaveProOld;
					fread( &sModelSaveProOld, sizeof(MoldeSavePropertyNoPos), 1, fp);
					sModelSavePro = sModelSaveProOld;
					break;
				}

			case MODELPRO_POSITION:
				{
					fread( &sModelSavePro, sizeof(MoldeSaveProperty), 1, fp);
					break;
				}

			default:
				break;
			}

			if( strItemName.find("pyramid") != -1 )
			{
				DebugOut("%s\n", strItemName.c_str());
				continue;
			}

			if ( sModelSavePro.nCenterGridIndex < 0|| !pTerrain->IsValid(sModelSavePro.nCenterGridIndex) )
			{
				ErrObject.push_back( strItemName );
			}
			else
			{
				if ( !bEffectBound )
				{
					modelfind = sGridModelItemNames.find(strItemName);
					if( modelfind != sGridModelItemNames.end() )
					{
						if( modelfind->second == sModelSavePro )
						{
							char c[10];
							sprintf(c, "%d", nGridIndex);
							string str = c;
							str = str + "格子上有相同的物体: ";
							str = str + strItemName + " YES删掉，NO不删掉";

							if ( IDYES == MessageBox(NULL, str.c_str(), "提示", MB_YESNO) )
								continue;
						}
					}
					sGridModelItemNames[strItemName] = sModelSavePro;

					//DebugOut("model grid %d, %d, %s\n", pGrid->dwGridIndex, sModelSavePro.nCenterGridIndex, strItemName.c_str());
					if ( !pOperator->Rebirth(&sModelSavePro, strItemName, "", nRatio, strItemName) )
					{
						errorlist.push_back(j);
					}
				}
				else // effect
				{
					effectfind = sGridEffectNames.find(strEffectName);
					if( effectfind != sGridEffectNames.end() )
					{
						if( effectfind->second == sModelSavePro )
						{
							char c[10];
							sprintf(c, "%d", nGridIndex);
							string str = c;
							str = str + "格子上有相同的特效: ";
							str = str + strEffectName + " YES删掉，NO不删掉";

							if ( IDYES == MessageBox(NULL, str.c_str(), "提示", MB_YESNO) )
								continue;
						}
					}
					if( strEffectName.find("场景光源\\Standar") == -1 )
						sGridEffectNames[strEffectName] = sModelSavePro;

					//DebugOut("effect grid %d, %d, %s\n", pGrid->dwGridIndex, sModelSavePro.nCenterGridIndex, strEffectName.c_str());
					pOperator->RebirthIndividualEffect(strEffectName, sModelSavePro.nCenterGridIndex, sModelSavePro.vRotate.y, sModelSavePro.nRotationCnt,
						sModelSavePro.vRotate.z, sModelSavePro.nPitchCnt, sModelSavePro.vRotate.x, sModelSavePro.nYawCnt, sModelSavePro.vScale.x, sModelSavePro.vScale.y, sModelSavePro.vScale.z, &sModelSavePro.vUserOffsetEx, sModelSavePro.vPosition, 0, NULL, 0);
				}

				// 				DebugOut("scale  %f, %f, %f\n", sModelSavePro.vScale.x, sModelSavePro.vScale.y, sModelSavePro.vScale.z);
				// 				DebugOut("rotate %f, %f, %f\n", sModelSavePro.vRotate.x, sModelSavePro.vRotate.y, sModelSavePro.vRotate.z);
				// 				DebugOut("offset %f, %f, %f\n", sModelSavePro.vUserOffsetEx.x, sModelSavePro.vUserOffsetEx.y, sModelSavePro.vUserOffsetEx.z);
			}
		}

		sGridModelItemNames.clear();
		sGridEffectNames.clear();

		if ( !errorlist.empty() && dw > 0 )
			LoadFaildModelMap[pGrid->dwGridIndex] = errorlist;
	}
}
