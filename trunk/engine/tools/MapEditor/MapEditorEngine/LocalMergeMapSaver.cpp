//------------------------------------------------------------------------------
//  CLocalMergeMapSaver.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "LocalMergeMapSaver.h"
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
#include "TerrainMeshRegion.h"
#include "CMapEditCamera.inl"
#include "CCameraManager.h"
#include "LocalMapAudioSaver.h"
#include "LocalMapLoginSaver.h"
#include "TraceHelper.h"
#include "StringHelper.h"
#include "CMapEditModelObjPro.h"
#include "IoSystem.h"
#include "LocalVersion.h"
#include "SceneRegionMgr.h"
#include "CXmlConfig.inl"
#include "OffLineBaker/OfflineRender.h"
#include "SceneAudio.h"
#include "MapCommonDataLoader.h"
#include "Console.h"
#include "CWindowTarget.h"
#include "CGraphic.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "ToolMainPlayer.h"
#include "SceneAudioMgr.h"
#include "CEditDataScene.h"
#include "CEditDataScene.h"
#include "CEditContext.h"
#include "CMapEffect.h"

extern float WATER_LAYER_BASE;

namespace sqr
{

	//------------------------------------------------------------------------------
	CLocalMergeMapSaver::CLocalMergeMapSaver()
		: terrain(NULL)
		, progress(NULL)
	{
	}

	//------------------------------------------------------------------------------
	void
	CLocalMergeMapSaver::SaveAsMergeProject()
	{
		string strFileName	 = m_strMapPathName + ".project";
		size_t setTileSize   = setTitleResourceName.size();
		size_t setEffectSize = setEffecteResourceName.size();
		set<string>::iterator iter = setTitleResourceName.begin();

		vector<string> vecAttribValueName, vecAttribValueValue;
		TCHAR s[64];
		sprintf(s,"%d",setTileSize);
		vecAttribValueName.push_back("ResSetCount");
		vecAttribValueValue.push_back(s);

		for ( size_t i = 0; i < setTileSize ; ++i )
		{
			TCHAR s[64];
			string str = *iter;
			sprintf(s,"ResSetFileName%d",i);
			
			vecAttribValueName.push_back(s);
			vecAttribValueValue.push_back(str);

			++iter;
		}

		CXmlConfig *pProjectXmlCfg = new CXmlConfig("x");
		pProjectXmlCfg->AttribSet("x", "图素集", vecAttribValueName, vecAttribValueValue);
		pProjectXmlCfg->SaveConfig(strFileName.c_str());
		delete pProjectXmlCfg;
		pProjectXmlCfg = NULL;
	}

	//------------------------------------------------------------------------------
	void
	CLocalMergeMapSaver::LoadAsMapProject(const string& path)
	{
		string strFile = path;
		strFile += ".project";

		//////////////////////////////////////////////////////////////////////////
		///load map project
		LogConsole(strFile.c_str());

		SQR_TRY
		{
			locale::global(locale(""));
			ifstream in(strFile.c_str());
			
			CXmlConfig *pMapProjectFile = new CXmlConfig( "x", in );
			///x property
			TiXmlNode  *pResourceSetNode = pMapProjectFile->Get<TiXmlElement*>("图素集");
			if( pResourceSetNode )
			{
				TiXmlElement *pElement = pResourceSetNode->ToElement();
				if( pElement )
				{
					int nCnt = atoi(pElement->Attribute("ResSetCount"));
					for ( int i = 0; i < nCnt; ++i )
					{
						TCHAR s[64];
						sprintf(s,"ResSetFileName%d",i);
						string str = pElement->Attribute(s);

						setTitleResourceName.insert(str);
					}
				}
			}

			in.close();

			delete pMapProjectFile;
			pMapProjectFile = NULL;
		}
		SQR_CATCH(exp)
		{
			MessageBox(NULL, exp.ErrorMsg(), "错误", MB_OK);

			return;
		}
		SQR_TRY_END;

	}

	//------------------------------------------------------------------------------
	void
	CLocalMergeMapSaver::LoadAsRgnLightInfo(const string& path)
	{
		//////////////////////////////////////////////////////////////////////////
		setHasRegNames.clear();

		///存储已有的区域，加载新的区域后，好对新的地图的区域进行处理
		map<string, SceneRegion*>::iterator iter = CSceneRegionMgr::GetInst()->m_mapRegion.begin();
		map<string, SceneRegion*>::iterator eiter = CSceneRegionMgr::GetInst()->m_mapRegion.end();
		string strRgnName = "";
		int nSceneRgnIndex = 0;
		for ( iter; iter != eiter; ++iter )
		{
			strRgnName = iter->first;
			setHasRegNames.insert(strRgnName);
		}
		//////////////////////////////////////////////////////////////////////////

		if ( path == "" )
			return;

		string strFile = path;
		size_t pos = strFile.rfind(".");
		if ( pos != -1)
			strFile = strFile.substr(0, pos);

		strFile += ".operaterinfo";

		FILE * fp = NULL;
		fp = fopen(strFile.c_str(),"rb");
		if ( fp )
		{
			bool flag = false;

			//开始读文件
			//是否全屏泛光
			fread(&flag, 1, 1, fp);
			//是否开启主角光效
			fread(&flag, 1, 1, fp);
			//是否开启雾效
			fread(&flag, 1, 1, fp);
			//是否开启物件透明
			fread(&flag, 1, 1, fp);
			//是否开启全局视角阴影
			fread(&flag, 1, 1, fp);
			//是否开启标准视角阴影
			fread(&flag, 1, 1, fp);

			CSceneRegionMgr::GetInst()->LoadSceneRegionInfo(fp);

			fclose(fp);
		}
		else
		{
			string log = strFile;
			log += "该文件打开失败";

			MessageBox( NULL, log.c_str() , "提示", MB_OK );
		}
	}

	//------------------------------------------------------------------------------
	void
	CLocalMergeMapSaver::SaveAsMergeRgnLightInfo()
	{
		COperator *pOperator = NULL;
		pOperator = CMapEditApp::GetInst()->GetOperator();
		if ( pOperator == NULL )
			return;

		//////////////////////////////////////////////////////////////////////////
		//将已设置的信息进行保存

		string strRgnLightPath = m_strMapPathName + ".operaterinfo";
		FILE * fp = NULL;

		fp = fopen(strRgnLightPath.c_str(),"wb");
		if (fp)
		{
			CSceneRegionMgr::GetInst()->SaveSceneRegionInfo(fp);

			fclose(fp);
		}
		else
		{
			string log = strRgnLightPath;
			log += "该文件已被锁定，保存前先解锁，否则保存该文件失败";

			MessageBox( NULL, log.c_str() , "提示", MB_OK );
		}
	}

	//------------------------------------------------------------------------------
	void
	CLocalMergeMapSaver::SaveAsMergeLocal(const string& path, const int xSort, const int zSort)
	{
		if( terrain->IsWater() )
		{
			MessageBox(NULL,"将当前状态改成地形状态，而非水面层状态，否则不能保存地图信息","提示",MB_OK);
			return;
		}

		set<int>				 nLoginGridIndexs;
		Ast(NULL != this->terrain);
		Ast(NULL != this->progress);

		COperator *pOperator = NULL;
		pOperator = CMapEditApp::GetInst()->GetOperator();
		if ( pOperator == NULL )
			return;

		string strReadMapName = path;
		size_t npos = path.rfind("\\");
		if( npos != -1 )
			strReadMapName = strReadMapName.substr(npos+1, strReadMapName.length());

		// For ExtraData   兼容旧地编文件   在原数据末尾加的Data块
		string strFileName = m_strMapPathName + ".local";
		locale loc;
		use_facet<ctype<TCHAR> >( loc ).tolower
			( &strFileName[0], &strFileName[strFileName.length()] );

		//////////////////////////////////////////////////////////////////////////
		///防止灯的信息有误，所以先存了份备份文件，如果灯的信息没有问题，就将备份文件copy给local
		string backupname = "";
		backupname = strFileName + "_" + strReadMapName + "_backup";

		//////////////////////////////////////////////////////////////////////////
		int nSaveGridCnt   = terrain->GetGridCount();
		int nSaveVertexCnt = terrain->GetVertexCount();

		float Total = float(nSaveGridCnt*1.5 + nSaveVertexCnt + 1.0f);
		progress->SetProgress( 0.0f );

		string strReadMapFile = path;
		strReadMapFile += ".local";
		FILE *pReadFile = NULL;
		pReadFile = fopen(strReadMapFile.c_str(),"rb");
		if (!pReadFile)
		{
			string log = strReadMapFile + "无法保存";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
			return;
		}

		// header
		TCHAR szHeader[4] = "";
		fread(szHeader,4,1,pReadFile);
		if ( *((DWORD*)szHeader) != MAKEFOURCC('L','O','C','L') )
		{
			MessageBox(NULL,"文件类型不符","错误",MB_OK);
			fclose(pReadFile);
			return;
		}

		// version
		DWORD dwSaveVersion = LOCAL_VERSION, dwReadVersion = 0;
		fread(&dwReadVersion, sizeof(DWORD),1,pReadFile);
		if( dwReadVersion < LOCAL_VERSION )
		{
			string log = strReadMapFile + "不是最新的文件格式";
			MessageBox(NULL,log.c_str(),"错误",MB_OK);
			return;
		}

		///游戏客户端服务器端文件ID号
		DWORD dwReadGameClientID = 0, dwReadGameServerID = 0;

		DWORD dwSaveGameClientID = terrain->GetGameGacMapVersion();
		fread(&dwReadGameClientID,sizeof(DWORD), 1,pReadFile);

		DWORD dwSaveGameServerID = terrain->GetGameGasMapVersion();
		fread(&dwReadGameServerID,sizeof(DWORD), 1,pReadFile);

		//////////////////////////////////////////////////////////////////////////
		///read
		DWORD dwFileNameLen = 0;
		string str;
		fread(&dwFileNameLen,sizeof(DWORD),1,pReadFile);
		str.resize(dwFileNameLen);
		fread((TCHAR*)str.data(),dwFileNameLen,1,pReadFile);
		CSceneEnvMgr::GetInst()->SetBackGroundMusic(str, false);
		///大家希望一进去不听背景音乐，觉得不好听
		CSceneEnvMgr::GetInst()->StopBackMusic();

		//////////////////////////////////////////////////////////////////////////
		// mainplayer
		CToolMainPlayer::GetInst()->Load(pReadFile);

		//////////////////////////////////////////////////////////////////////////
		// read camera
		CMapEditCamera * pCamera = CCameraManager::GetInst()->GetMapEditCamera();
		CVector3f vView;
		fread(&vView,sizeof(vView),1,pReadFile);
		if ( vView.y < 0 )
		{
			vView.y = 447.676605f;
		}
		//pCamera->SetNewTarget(vView);

		//fov
		float fov = 0.0f, yaw = 0.0f, pitch = 0.0f;
		fread(&fov,sizeof(float),1,pReadFile);
		//yaw
		fread(&yaw,sizeof(float),1,pReadFile);
		//pitch
		fread(&pitch,sizeof(float),1,pReadFile);

		//pCamera->SetFovY(fov);
		//pCamera->SetPitchAndYaw(pitch,yaw);

		///save
		vView = pCamera->GetTarget();

		//////////////////////////////////////////////////////////////////////////
		MapCommonDataLoader::GetInst()->SetOperator(pOperator);

		MapCommonDataLoader::GetInst()->LoadEnvInfo(pReadFile);

		MapCommonDataLoader::GetInst()->LoadBackPictureInfo(pReadFile);

		// fog mode
		int fogMode = 0;
		fread(&fogMode,sizeof(int),1,pReadFile);
		CSceneEnvMgr::GetInst()->SetVertexFog((sqr::CSceneEnvMgr::FogModeType)fogMode);
	
		//全屏泛光
		CSceneEnvMgr::GetInst()->LoadGaussParamsInfo(pReadFile);

		//////////////////////////////////////////////////////////////////////////
		MapCommonDataLoader::GetInst()->LoadWaveInfo(pReadFile);

		// ambient fx info
		CMapEffect::GetInst()->ReadAmbientFXInfo(pReadFile);

		progress->SetProgress( 1.0f/Total );

		// width depth
		DWORD dwSaveWidth = terrain->GetWidthInRegion(), dwReadWidth = 0;
		DWORD dwSaveDepth = terrain->GetDepthInRegion(), dwReadDepth = 0;
		if ( dwSaveWidth % 4 ) dwSaveWidth+=4-dwSaveWidth % 4;
		if ( dwSaveDepth % 4 ) dwSaveDepth+=4-dwSaveDepth % 4;

		fread(&dwReadWidth,sizeof(DWORD),1,pReadFile);
		fread(&dwReadDepth,sizeof(DWORD),1,pReadFile);
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		///read terrain road and sub texture name
		size_t size = 0;
		int iNameLen = 0, weight = 0;
		string texname = "";
		fread(&size,sizeof(size_t),1,pReadFile);
		for ( size_t i = 0; i < size; ++i )
		{
			fread(&iNameLen,sizeof(int),1,pReadFile);
			texname.resize(iNameLen);
			fread((TCHAR*)texname.data(),iNameLen,1,pReadFile);
			fread(&weight,sizeof(int),1,pReadFile);

			terrain->m_MainRoadWeight[texname] = weight;
		}

		DWORD gridSaveW			= dwSaveWidth * REGION_SPACE;
		DWORD gridSaveH			= dwSaveDepth * REGION_SPACE;
		DWORD colSaveVertexCnt	= gridSaveW + 1; //横向
		DWORD rowSaveVertexCnt	= gridSaveH + 1; //纵向
		DWORD dwSaveVertexCnt	= colSaveVertexCnt * rowSaveVertexCnt;
		int   nSaveGridStartX   = xSort * REGION_SPACE;
		int   nSaveGridStartZ   = zSort * REGION_SPACE;
		int   nSaveVertexStartX = nSaveGridStartZ + 1;
		int   nSaveVertexStartZ = nSaveGridStartZ + 1;

		DWORD gridReadW		    = dwReadWidth * REGION_SPACE;
		DWORD gridReadH			= dwReadDepth * REGION_SPACE;
		DWORD colReadVertexCnt  = gridReadW + 1; //横向
		DWORD rowReadVertexCnt  = gridReadH + 1; //纵向
		DWORD dwReadVertexCnt   = colReadVertexCnt * rowReadVertexCnt;
		DWORD dwReadGridCnt     = gridReadW * gridReadH;

		this->mapOldMapGridIndex.clear();
		this->mapReadMapDivideRegionIndex.clear();
		this->mapReadMapGridIndex.clear();
		this->mapOldMapVertexIndex.clear();
		this->mapReadMapVertexIndex.clear();

		Ast(nSaveVertexCnt == dwSaveVertexCnt);

		CMapEditApp::GetInst()->GetDataScene()->SetWidth( uint16(dwReadWidth) );
		CMapEditApp::GetInst()->GetDataScene()->SetHeight( uint16(dwReadDepth) );
		CMapEditApp::GetInst()->GetDataScene()->Create();
		CSyncSystem::BeginSyncFun();

		CMapEditApp::GetInst()->InitNewRenderScene();
		CMapEditApp::GetInst()->GetRenderScene()->InitSceneRes();

		CTerrainMesh *pTerrainNew = new CTerrainMesh; 
		pTerrainNew->Create(dwReadWidth, dwReadDepth, m_strDefaultTerrainTexName, false, false);
		MapCommonDataLoader::GetInst()->SetTerrainMesh(terrain);
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//计算重叠部分
		DWORD dw = pTerrainNew->GetVertexCount();
		Ast(dwReadVertexCnt == dw);

		int nSaveDivideRegionIndex = 0, nReadDivideRegionIndex = 0;
		int nSaveVertexIndex = 0, nReadVertexIndex = 0;
		for ( DWORD z = 0 ; z < gridReadH; z++ )
		{
			for ( DWORD x = 0 ; x < gridReadW ; x++ )
			{
				DWORD dwReadGridIndex = x + z * gridReadW;
				DWORD dwSaveGridIndex = x + nSaveGridStartX + (nSaveGridStartZ + z) * gridSaveW;
// 				int col = x + nSaveGridStartX;
// 				int row = (nSaveGridStartZ + z) * gridSaveW;

//				if( terrain->IsValid(dwSaveGridIndex) )
				{
					SGrid * pGridRead = &pTerrainNew->GetGrid(dwReadGridIndex);
					SGrid * pGridSave = &terrain->GetGrid(dwSaveGridIndex);

					//DebugOut("Read Grid 列:%d, 行: %d, %d\n",   x, z, dwReadGridIndex);
				//	DebugOut("Save Grid 列:%d, 行: %d, %d\n\n", col, row, dwSaveGridIndex);

					nSaveDivideRegionIndex = pGridSave->m_nDivideRegionIndex;
					nReadDivideRegionIndex = pGridRead->m_nDivideRegionIndex;

					this->mapOldMapGridIndex[dwSaveGridIndex] = dwReadGridIndex;
					this->mapReadMapGridIndex[dwReadGridIndex]= dwSaveGridIndex;
					this->mapReadMapDivideRegionIndex[nReadDivideRegionIndex] = nSaveDivideRegionIndex;
					//DebugOut("Read reg index %d, save %d\n", nReadDivideRegionIndex, nSaveDivideRegionIndex);

					for( int m = 0; m < 4; ++m )
					{
						nSaveVertexIndex = pGridSave->dwVertexIndex[m];
						nReadVertexIndex = pGridRead->dwVertexIndex[m];

						this->mapOldMapVertexIndex[nSaveVertexIndex] = nReadVertexIndex;
						this->mapReadMapVertexIndex[nReadVertexIndex] = nSaveVertexIndex;

						//DebugOut("old vertex %d, new %d\n", nReadVertexIndex, nSaveVertexIndex);
					}
				}
			}
		}

		//DebugOut("size %d, %d\n", mapReadMapVertexIndex.size(), mapOldMapVertexIndex.size());
		//////////////////////////////////////////////////////////////////////////

		CTerrainMesh * pWater = terrain->GetWater();

#pragma region VERTEX
		//save 顶点信息
		{
			DWORD dwVertexIndexOld = 0, oColor = VERTEX_COLOR, sColor = VERTEX_COLOR;
			float fPositionY = 0.0f;
			CVector3f vNormal(0.0, 1.0f, 0.0f);
			short sLogicHeight = 0;
			for (int k = 0; k < nSaveVertexCnt; ++k )
			{
				SVertex *pVertex = &terrain->GetVertex(k);
				if( this->mapOldMapVertexIndex.find(k) != this->mapOldMapVertexIndex.end() )
				{
					MapCommonDataLoader::GetInst()->LoadEachVertexInfo(pReadFile, pVertex);
					pWater->GetVertex(k).vPosition.y = pVertex->vPosition.y + WATER_LAYER_BASE;
				}

				progress->SetProgress( (k+1.0f)/Total );
			}
		}
#pragma endregion VERTEX

#pragma region GRID
		//////////////////////////////////////////////////////////////////////////
		///model property version
		fread(&uModelVersion, sizeof(uModelVersion),1,pReadFile);
		///grid property version
		fread(&uGridProVersion,sizeof(uGridProVersion),1,pReadFile);
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
#pragma region TEXTURE
		///读取地图用到的所有贴图
		int dwNameLen = 0;
		int dwTexCnt = 0;
		string gridtexname = "";
		size_t nPos = 0;
		int16 nTextureIndex = 0;
		size_t nTerrainTexCnt = 0;
		set<string> terrainTexIndexsSet;
		map<string, int16> terrainTextureNames;///用于记录地图中用到的所有纹理的名字，保存各层纹理信息则保存index就行
		map<int16, string> terrainTextureNameIndexs;

		fread(&nTerrainTexCnt,sizeof(nTerrainTexCnt),1,pReadFile);
		for ( size_t i = 0; i < nTerrainTexCnt; ++i )
		{
			fread(&dwNameLen,sizeof(int),1,pReadFile);
			gridtexname.resize(dwNameLen);
			fread((TCHAR*)gridtexname.data(),dwNameLen,1,pReadFile);
			fread(&nTextureIndex,sizeof(nTextureIndex),1,pReadFile);

			//DebugOut("load tex %s, %d\n", gridtexname.c_str(), nTextureIndex);

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

			size_t npos0 = gridtexname.find("tile\\地表纹理");
			if( npos0 != -1 )
			{
				npos0 = gridtexname.find("\\");
				gridtexname = gridtexname.substr(npos0+1, gridtexname.length());
				gridtexname = "..\\tools\\" + gridtexname;
			}

			if( gridtexname.find("default.bmp") == 0 )
				gridtexname = m_strDefaultTerrainTexName;

			terrainTextureNames[gridtexname] = nTextureIndex;
			terrainTextureNameIndexs[nTextureIndex] = gridtexname;
		}

		//存纹理信息
		string strName = "";
		map<string, string>::iterator iter;
		iNameLen = 0;
		nTextureIndex = 0;

		size_t terrainTextureCnt = 0;

		DWORD dwSceneModelCnt = 0;
		for ( int i = 0; i < nSaveGridCnt; ++i )
		{
			SGrid *pGrid = &terrain->GetGrid(i);
			strName = pGrid->nodeInfo.strTextureNames[0];

			if ( terrainTextureNames.find(strName) == terrainTextureNames.end() )
			{
				terrainTextureNames[strName] = nTextureIndex;
				iNameLen = strName.length();

				++nTextureIndex;
			}
			strName = pGrid->nodeInfo.strTextureNames[1];
			if ( terrainTextureNames.find(strName) == terrainTextureNames.end() )
			{
				terrainTextureNames[strName] = nTextureIndex;
				iNameLen = strName.length();

				++nTextureIndex;
			}

		}

		terrainTextureCnt = terrainTextureNames.size();

#pragma endregion TEXTURE

		//////////////////////////////////////////////////////////////////////////

		bool bEffectBased = false, bEffecIsExit = false;
		string strEffectName = "", strCueName = "";
		vector<string> ErrObject;

		// grids
		//格子信息
		{
			map<int, int>::iterator iter;
			for ( int j = 0; j< nSaveGridCnt; ++j )
			{
				SGrid * pSaveGrid = &terrain->GetGrid(j);

				iter = this->mapOldMapGridIndex.find(j);
				// any central Object count
				DWORD dwObjectCnt = 0;
				if( iter != this->mapOldMapGridIndex.end() )
				{
					int nReadGridIndex = iter->second;
					SGrid * pReadGrid = &pTerrainNew->GetGrid(nReadGridIndex);

					//DebugOut("new gird %d, old grid %d\n", j, nReadGridIndex);

					//grid hide mask
					fread(&pReadGrid->bGridHide,sizeof(bool),1,pReadFile);
					//////////////////////////////////////////////////////////////

					//读取纹理信息
					//1
					fread(&nTextureIndex,sizeof(nTextureIndex),1,pReadFile);
					gridtexname= terrainTextureNameIndexs[nTextureIndex] ;
					//DebugOut("tex0 %s, %d\n", gridtexname.c_str(), nTextureIndex);

					pReadGrid->nodeInfo.strTextureNames[0]  = gridtexname;
					if ( terrainTexIndexsSet.find(gridtexname) == terrainTexIndexsSet.end() && gridtexname != "" )
					{
						terrainTexIndexsSet.insert(gridtexname);
					}
					pSaveGrid->nodeInfo.strTextureNames[0] = gridtexname;

					//2
					fread(&nTextureIndex,sizeof(nTextureIndex),1,pReadFile);
					gridtexname= terrainTextureNameIndexs[nTextureIndex] ;
					//DebugOut("tex1 %s, %d\n", gridtexname.c_str(), nTextureIndex);
					pReadGrid->nodeInfo.strTextureNames[1]  = gridtexname;
					if ( terrainTexIndexsSet.find(gridtexname) == terrainTexIndexsSet.end() && gridtexname != "" )
					{
						terrainTexIndexsSet.insert(gridtexname);
					}
					pSaveGrid->nodeInfo.strTextureNames[1] = gridtexname;

					pSaveGrid->nodeInfo.strTextureNames[2] = "";
					pSaveGrid->nodeInfo.strTextureNames[3] = "";

					//////////////////////////////////////////////////////////////////////////
					///读取地表标志等属性
					switch (uGridProVersion)
					{
					case GRIDPRO_ENFORCELOWBLOCK:
						fread(&pReadGrid->bCliffLayerOne, sizeof(bool), 1, pReadFile);

					case GRIDPRO_ENFORCEMAT:
						fread(&pReadGrid->bMatNotEffectByTileSets, sizeof(bool), 1, pReadFile);

					case DEFAULT_GRIDPRO:
						fread(&pReadGrid->bCliffLayerOne, sizeof(bool), 1, pReadFile);
					default:
						break;
					}
					//////////////////////////////////////////////////////////////////////////

					//读取纹理坐标信息,第1层纹理坐标的信息
					for (int m = 0; m < 4; ++m)
					{
						fread(&pReadGrid->uv[0][m].x, sizeof(float), 1, pReadFile);
						fread(&pReadGrid->uv[0][m].y, sizeof(float), 1, pReadFile);
					}

					fread(&pReadGrid->vCenter.y,sizeof(float),1,pReadFile);

					//////////////////////////////////////////////////////////////////////////

					SGrid & gridW = pWater->GetGrid(j);
					gridW.vCenter.y = pReadGrid->vCenter.y + WATER_LAYER_BASE;

					//////////////////////////////////////////////////////////////////////////

					fread(&pReadGrid->bEffectByVertexColor,sizeof(bool),1,pReadFile);
					fread(&pReadGrid->bCliffLayerOne,sizeof(bool),1,pReadFile);

					fread(&dwObjectCnt,sizeof(DWORD),1,pReadFile);

					pSaveGrid->bMatNotEffectByTileSets = pReadGrid->bMatNotEffectByTileSets;
					pSaveGrid->bGridHide = pReadGrid->bGridHide;
					pSaveGrid->nBlockType = pReadGrid->nBlockType;

					pSaveGrid->bCliffLayerOne = pReadGrid->bCliffLayerOne;

					for (int m = 0; m < 4; ++m)
					{
						pSaveGrid->uv[0][m] = pReadGrid->uv[0][m];
						pSaveGrid->uv[1][m] = pReadGrid->uv[1][m];
					}

					pSaveGrid->vCenter.y			= pReadGrid->vCenter.y;
					pSaveGrid->vecCenterObjects		= pReadGrid->vecCenterObjects;
					pSaveGrid->bEffectByVertexColor	= pReadGrid->bEffectByVertexColor;
				}
				else
				{
					gridtexname = pSaveGrid->nodeInfo.strTextureNames[0];
					if( gridtexname != "" )
						terrainTexIndexsSet.insert(gridtexname);
					gridtexname = pSaveGrid->nodeInfo.strTextureNames[1];
					if( gridtexname != "" )
						terrainTexIndexsSet.insert(gridtexname);
				}
				//hide mask
				//fwrite(&pSaveGrid->bGridHide,sizeof(bool),1,pSaveFile);
				///////////////////////////////////////////////////////////////
				//纹理信息

				//1
				strName = pSaveGrid->nodeInfo.strTextureNames[0];
				nTextureIndex = terrainTextureNames[strName];

				//2
				strName = pSaveGrid->nodeInfo.strTextureNames[1];
				nTextureIndex = terrainTextureNames[strName];

				//格子上的模型信息
				// central  object
				if ( dwObjectCnt != 0 )
				{
					DWORD dw = 0;
					//long lCntOffset = ftell(pSaveFile);
					//fwrite(&dw,sizeof(DWORD),1,pSaveFile);

					bool bEffectBound = false;
					string strEffectName, strItemName;
					DWORD dwItemNameLen = 0;
					for ( DWORD m = 0; m < dwObjectCnt; ++m )
					{
						fread(&bEffectBound,sizeof(bool),1,pReadFile);
						if ( bEffectBound )
						{
							DWORD dwItemNameLen;
							fread(&dwItemNameLen,sizeof(DWORD),1,pReadFile);
							strEffectName.resize(dwItemNameLen);
							fread((TCHAR*)strEffectName.data(),dwItemNameLen,1,pReadFile);
						}

						if ( bEffectBound )
						{
							bool b = true;
							//fwrite(&b,sizeof(bool),1,pSaveFile);
							DWORD dwLen = strEffectName.size();
							//fwrite(&dwLen,sizeof(int),1,pSaveFile);
							//fwrite(strEffectName.c_str(),dwLen,1,pSaveFile);
						}
						else
						{
							bool b = false;
							
							//fwrite(&b,sizeof(bool),1,pSaveFile);
						}

						dw++;

						fread(&dwItemNameLen,sizeof(DWORD),1,pReadFile);
						string strItemName;
						strItemName.resize(dwItemNameLen);
						fread((TCHAR*)strItemName.data(),dwItemNameLen,1,pReadFile);

						use_facet<ctype<TCHAR> >( loc ).tolower
							( &strItemName[0], &strItemName[strItemName.length()] );

						DWORD dwLen = strItemName.size();
						//fwrite(&dwLen,sizeof(int),1,pSaveFile);
						//fwrite(strItemName.c_str(),dwLen,1,pSaveFile);

						//////////////////////////////////////////////////////////////////////////
						///save as modelgroup
						//////////////////////////////////////////////////////////////////////////
						int nAtten = 0;
						fread(&nAtten,sizeof(int),1,pReadFile);

						///nAffectRadio
						int nRatio = 0;
						fread( &nRatio, sizeof(int), 1, pReadFile);

						MoldeSaveProperty sModelSavePro;

						switch (uModelVersion)
						{
						case DEFAULT_MODELPRO:
							{
								MoldeSavePropertyOld sModelSaveProOld;
								fread( &sModelSaveProOld, sizeof(sModelSaveProOld), 1, pReadFile);
								sModelSavePro = sModelSaveProOld;
							}
							break;

						case MODELPRO_EYECOLLISION:
							{
								MoldeSavePropertyNoPos sModelSaveProOld;
								fread( &sModelSaveProOld, sizeof(MoldeSavePropertyNoPos), 1, pReadFile);
								sModelSavePro = sModelSaveProOld;
								break;
							}

						case MODELPRO_POSITION:
							{
								fread( &sModelSavePro, sizeof(MoldeSaveProperty), 1, pReadFile);
								break;
							}


						default:
							break;
						}

						int nReadModelGridIndex = sModelSavePro.nCenterGridIndex;
						int nSaveModelGridIndex = this->mapReadMapGridIndex[nReadModelGridIndex];

						sModelSavePro.nCenterGridIndex = nSaveModelGridIndex;

						if ( sModelSavePro.nCenterGridIndex < 0|| !terrain->IsValid(sModelSavePro.nCenterGridIndex) )
						{
							ErrObject.push_back( strItemName );
						}
						else
						{
							if ( !bEffectBound )
							{
								//DebugOut("Grid %d, %d, %s\n", i, sModelSavePro.nCenterGridIndex, strItemName.c_str());
								if ( !pOperator->Rebirth(&sModelSavePro, strItemName, "", nRatio, strItemName) )
								{
									--dwSceneModelCnt;
								}
							}
							else // effect
							{
								pOperator->RebirthIndividualEffect(strEffectName, sModelSavePro.nCenterGridIndex, sModelSavePro.vRotate.y, sModelSavePro.nRotationCnt,
									sModelSavePro.vRotate.z, sModelSavePro.nPitchCnt, sModelSavePro.vRotate.x, sModelSavePro.nYawCnt, sModelSavePro.vScale.x, sModelSavePro.vScale.y, sModelSavePro.vScale.z, &sModelSavePro.vUserOffsetEx, sModelSavePro.vPosition, 0, NULL, 0);
							}

						}
					}
				}
				else
				{
					DWORD dw = 0;
				}

				progress->SetProgress( (j+1.0f+ + terrain->GetVertexCount())/Total );
			}
		}

		///拼接纹理和创建地表纹理
		MakeTerrainTexture(terrainTexIndexsSet);
#pragma endregion GRID
		//////////////////////////////////////////////////////////////////////////

		// info from other places

		// block lines 单独保存到一个文件pReadGrid
		this->SaveMergeMapBlockInfo(path, nSaveGridCnt, dwReadGridCnt);
		///pTerrain
		size_t erasebloclsize = 0;
		fread( &erasebloclsize, sizeof(size_t), 1, pReadFile );
		//fwrite( &erasebloclsize, sizeof(size_t), 1, pSaveFile );
		DWORD gridindex = 0;
		for ( size_t i = 0; i < erasebloclsize; ++i )
		{
			fread( &gridindex, sizeof(DWORD), 1, pReadFile );
			//fwrite( &gridindex, sizeof(size_t), 1, pSaveFile );
		}

		///save water 
		this->SaveWaterInfo(pReadFile, /*pSaveFile*/NULL, nSaveGridCnt, dwReadGridCnt);

		///save point light
		this->SavePointLightInfo(pReadFile, /*pSaveFile*/NULL);

		progress->SetProgress( (terrain->GetGridCount() * 1.125f +1.0f + terrain->GetVertexCount())/Total );

		///////////////////////--- 增加cave数据块，用来保存是否是“室内”
		int ExtraData = 0;
		TCHAR ExtraName[255];
		fread(&ExtraData,sizeof(int),1,pReadFile);
		if ( ExtraData == 6 )
		{
			fread( ExtraName, ExtraData, 1, pReadFile);

			bool bCave = false;
			fread( &bCave, sizeof(bool), 1, pReadFile);
		}

		int tLength = 6;

		progress->SetProgress( (terrain->GetGridCount() * 1.25f+1.0f+terrain->GetVertexCount())/Total );

		//////////////////////////////////////////////////////////////////////////
		/*针对以前的格式修改导致的一系列问题，从现在开始借用特效中的公告板保存格式，
		也即是利用版本号加上switch的方式，保存新增加的地图信息*/

		DWORD dwNewVersion = 0;
		fread(&dwNewVersion,sizeof(DWORD),1,pReadFile);

		///读取聚光灯信息
		switch (dwNewVersion)
		{
		case VERTEX_BAKING_COLOR:
			{
				DWORD dwVertexCnt = pTerrainNew->GetVertexCount();
				for ( DWORD i = 0; i< dwVertexCnt; ++i )
				{
					SVertex &vertex = terrain->GetVertex(i);
					fread(&vertex.bColor,sizeof(DWORD),1,pReadFile);
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
					fread(&dwSceneSpecurlarColor,sizeof(DWORD),1,pReadFile);
					CSceneEnvMgr::GetInst()->SetSceneSpecurlarColor(dwSceneSpecurlarColor);

					DWORD dwPlayerSpecurlarColor = 0;
					fread(&dwPlayerSpecurlarColor,sizeof(DWORD),1,pReadFile);
					CSceneEnvMgr::GetInst()->SetPlayerSpecurlarColor(dwPlayerSpecurlarColor);
				}

				float fSceneSPower = 0.0f;
				fread(&fSceneSPower,sizeof(fSceneSPower),1,pReadFile);
				CSceneEnvMgr::GetInst()->SetSpecularReflectionPower(0, fSceneSPower);
				fSceneSPower = CSceneEnvMgr::GetInst()->GetSpecularReflectionPower(0);

				float fNpcSPower = 0.0f;
				fread(&fNpcSPower,sizeof(fNpcSPower),1,pReadFile);
				CSceneEnvMgr::GetInst()->SetSpecularReflectionPower(1, fNpcSPower);
				fNpcSPower = CSceneEnvMgr::GetInst()->GetSpecularReflectionPower(1);
				//////////////////////////////////////////////////////////////////////////
			}


 		case GRID_MATERIALINDEX:
 			SaveTerGridMatIndexInfo(pReadFile, /*pSaveFile*/NULL, nSaveGridCnt, dwReadGridCnt);
 
 		case MODEL_ANIPLAYMARK:
 			SaveOffAnimateModelIndex(pReadFile, /*pSaveFile*/NULL);
 
 		case MODEL_EXACTBLOCKTYPE:
 			SaveExactBlockInfo(pReadFile, /*pSaveFile*/NULL);
 
 		case WAVE_SIDE_SAVE:
 			SaveWaveSideInfo(pReadFile, /*pSaveFile*/NULL, nSaveGridCnt, dwReadGridCnt);

		case SPOTLIGHT_SAVE:
			SaveSpotLightInfo(pReadFile, /*pSaveFile*/NULL);
			break;

		default:
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		///还原之前sColor保存的烘培顶点色
		int diff = dwNewVersion - SPECULAR_LIGHT_PARA;
		if( diff < 0 )
		{
			for ( DWORD i = 0; i< dwSaveVertexCnt; ++i )
			{
				SVertex &vertex = terrain->GetVertex(i);

				vertex.sColor = VERTEX_SPECULAR_COLOR;
			}
		}

		// ----------------------------------------------------------------------------
		// 保存摄像机路径信息

		SaveRecordInfo(pReadFile, /*pSaveFile*/NULL);
		
//		fclose(pSaveFile);
		fclose(pReadFile);

		//save .audio file 
		this->SaveAsAudio(path);

		LoadAsRgnLightInfo(path);
		CreateReadMapLightInfo(path);
//		SaveAsMergeProject();
//		SaveAsMergeRgnLightInfo();

		terrain->ReWriteTerrainBuffer(terrain->m_TerrainBufferIndexs, 0);
		terrain->ReWriteTerrainBuffer(terrain->m_TerrainBufferIndexs, 1);
		terrain->ReWriteTerrainBuffer(terrain->m_TerrainBufferIndexs, 2);
		terrain->ReWriteTerrainBuffer(terrain->m_TerrainBufferIndexs, 3);
		terrain->ReWriteTerrainBuffer(terrain->m_TerrainBufferIndexs, 4);
		terrain->ReWriteTerrainBuffer(terrain->m_TerrainBufferIndexs, 5);

		delete pTerrainNew;
		pTerrainNew = NULL;

		MessageBox(NULL,"合并OK","提示",MB_OK);
	}

	//------------------------------------------------------------------------------
	void
	CLocalMergeMapSaver::SaveSpotLightInfo(FILE *pRead, FILE *pSave)
	{
		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
		bool bReSizeMap = pOperator->GetIsResizeMap();
		int nSceneSpotLightCnt = 0;
		map<int, int>::iterator iter;
		SpotLB temp;

		fread( &nSceneSpotLightCnt, sizeof(int), 1, pRead);
		//fwrite( &nSceneSpotLightCnt, sizeof(int), 1, pSave);

		for ( int i = 0; i < nSceneSpotLightCnt; ++i )
		{
			fread( &temp.GridIndex, sizeof(int), 1, pRead);
			fread( &temp.iter, sizeof(size_t), 1, pRead);
			fread( &temp.m_SpotLight, sizeof(SceneSpotLight), 1, pRead);

			int nLightGridIndex = temp.GridIndex;
			iter = this->mapReadMapGridIndex.find(temp.GridIndex);
			if( iter != this->mapReadMapGridIndex.end() )
				nLightGridIndex = iter->second;

			if ( terrain->IsValid(nLightGridIndex) )
			{
				SGrid *pGrid = NULL;
				pGrid = &terrain->GetGrid(nLightGridIndex);

				if ( !pGrid->vecCenterObjects.empty() )
				{
					int gridModelIndex = 0;
					gridModelIndex = temp.iter;
					if ( gridModelIndex < (int)(pGrid->vecCenterObjects.size()) )
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

								terrain->m_DynamicLightGrids.insert(temp.GridIndex);
								OfflineRender::GetInst()->UpdateGridLights(temp.GridIndex);
								pModelGroup->SetIsLightEffect(true);
							}
						}
					}
				}
			}

			//fwrite( &nLightGridIndex,	 sizeof(nLightGridIndex), 1, pSave);
			//fwrite( &temp.iter,			 sizeof(int),			  1, pSave);
			//fwrite( &temp.m_SpotLight,	 sizeof(SceneSpotLight),  1, pSave);
		}
	}

	//------------------------------------------------------------------------------
	void
	CLocalMergeMapSaver::SaveExactBlockInfo(FILE *pRead, FILE *pSave)
	{
		int dwCnt = 0, nGridIndex = 0;
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		map<int, vector<BYTE>> mapReadMapGridExactBlock;

		///设置了精确阻挡的个数
		fread(&dwCnt, sizeof(dwCnt), 1, pRead);
		for ( int i = 0; i < dwCnt; ++i )
		{
			fread(&nGridIndex, sizeof(nGridIndex), 1, pRead);
			nGridIndex = this->mapReadMapGridIndex[nGridIndex];

			vector<BYTE> subGridTypes;
			BYTE type;
			subGridTypes.resize(4);

			for( int m = 0; m < 4; ++m )
			{
				fread(&type, sizeof(type), 1, pRead);
				subGridTypes[m] = type;
			}
			mapReadMapGridExactBlock[nGridIndex] = subGridTypes;
		}

		map<int, vector<BYTE>> ::iterator beg = mapReadMapGridExactBlock.begin();
		map<int, vector<BYTE>> ::iterator end = mapReadMapGridExactBlock.end();
		for (beg; beg != end; ++beg)
		{
			nGridIndex = beg->first;
			vector<BYTE> subGridTypes = beg->second;

			SGrid &grid = terrain->GetGrid(nGridIndex);

			//fwrite(&nGridIndex, sizeof(i), 1, pSave);
			for( int m = 0; m < 4; ++m )
			{
				BYTE type = subGridTypes[m];
				grid.subGridTypes[m] = type;
				//fwrite(&type, sizeof(type), 1, pSave);
			}
		}
	}

	//------------------------------------------------------------------------------
	void
	CLocalMergeMapSaver::SaveWaveSideInfo(FILE *pRead, FILE *pSave, const int nSaveMapGridCnt, const int nReadMapGridCnt)
	{
		uint fourcc;
		fread(&fourcc, sizeof(uint), 1, pRead);
		Ast(fourcc == 'WVSD');
	
		vector<byte> waveGrids;
		waveGrids.resize(nReadMapGridCnt);
		fread(&waveGrids.front(), sizeof(byte), waveGrids.size(), pRead);

		map<int, bool> mapGridWaves;
		int nGridIndex = 0;
		for (int i = 0; i < nReadMapGridCnt; ++i)
		{
			nGridIndex = i;
			nGridIndex = this->mapReadMapGridIndex[i];
			mapGridWaves[nGridIndex] = waveGrids[i] == 1;
		}

		//////////////////////////////////////////////////////////////////////////
		// 保存格子浪花标记
		{
			CTerrainMesh* water = terrain->GetWater();
			SGrid* grids = water->GetGrids();

			uint fourcc = 'WVSD';
			//fwrite(&fourcc, sizeof(uint), 1, pSave);
			waveGrids.clear();
			waveGrids.resize(nSaveMapGridCnt);
			for (index_t i = 0; i < waveGrids.size(); ++i)
			{
				waveGrids[i] = 0;
			}

			map<int, bool>::iterator iter;
			for ( int i = 0; i < nSaveMapGridCnt; ++i )
			{
				bool bSize = false;
				iter = mapGridWaves.find(i);
				if( iter != mapGridWaves.end() )
					bSize = iter->second;
				else
					bSize = false;
		
				waveGrids[nGridIndex] = bSize ? 1 : 0;
				grids[i].bSide = bSize;

				if (!grids[i].bSide && NULL != grids[i].pWave)
				{
					index_t regionId = water->GetRegionIndexByGridIndex(grids[i].dwGridIndex);
					grids[i].pRiver->RemoveWave(regionId, grids[i].pWave);

					grids[i].pWave = NULL;
				}
			}
			//fwrite(&waveGrids.front(), sizeof(byte), waveGrids.size(), pSave);
		}
	}

	//------------------------------------------------------------------------------
	void
	CLocalMergeMapSaver::SaveTerGridMatIndexInfo(FILE *pRead, FILE *pSave, const int nSaveMapGridCnt, const int nReadMapGridCnt)
	{
		int8 nMatIndex  = -1, nNoMatIndex = -1;
		int  nGridIndex = 0;
		map<int, int> mapReadMapGridMatIndex;
		
		for ( int i = 0; i < nReadMapGridCnt; ++i )
		{
			fread(&nMatIndex,sizeof(nMatIndex),1,pRead);
			nGridIndex = this->mapReadMapGridIndex[i];
			mapReadMapGridMatIndex[nGridIndex] = nMatIndex;
		}

		map<int, int>::iterator beg;
		for ( int i  = 0; i < nSaveMapGridCnt; ++i )
		{
			SGrid &grid = terrain->GetGrid(i);

			beg = mapReadMapGridMatIndex.find(i);
			if( beg != mapReadMapGridMatIndex.end() )
				grid.nMaterialIndex = beg->second;

			//fwrite(&nMatIndex, sizeof(int), 1, pSave);
		}
	}

	//------------------------------------------------------------------------------
	void
	CLocalMergeMapSaver::SaveOffAnimateModelIndex(FILE *pRead, FILE *pSave)
	{
		int dwCnt = 0, nGridIndex = 0;
		size_t gridModelIndex = 0;
		map<int, vector<size_t>> mapModelAniInfo;
		map<int, int>::iterator iter;

		///设置个数
		fread(&dwCnt, sizeof(dwCnt), 1, pRead);
		//fwrite(&dwCnt, sizeof(dwCnt), 1, pSave);

		for ( int i = 0; i < dwCnt; ++i )
		{
			fread(&nGridIndex, sizeof(nGridIndex), 1, pRead);
			fread( &gridModelIndex, sizeof(size_t), 1, pRead);

			///变换新的地形格子索引
			iter = this->mapReadMapGridIndex.find(nGridIndex);
			if( iter != this->mapReadMapGridIndex.end() )
				nGridIndex = iter->second;

			mapModelAniInfo[nGridIndex].push_back(nGridIndex);
		}

		map<int, vector<size_t>>::iterator beg = mapModelAniInfo.begin();
		map<int, vector<size_t>>::iterator end = mapModelAniInfo.end();
		for ( beg; beg != end; ++beg )
		{
			vector<size_t> modelIndexs = beg->second;
			size_t size = modelIndexs.size();
			nGridIndex = beg->first;

			for ( size_t i = 0; i < size; ++i )
			{
				gridModelIndex = modelIndexs[i];
				//fwrite( &nGridIndex,     sizeof(int),    1, pSave);///格子索引
				//fwrite( &gridModelIndex, sizeof(size_t), 1, pSave);///第几个模型的索引

				if ( terrain->IsValid(nGridIndex) )
				{
					SGrid & grid = terrain->GetGrid(nGridIndex);

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
	}

	 void CLocalMergeMapSaver::SaveAsAudio(const string& path)
	 {
		 string strSaveAudioFile = m_strMapPathName + ".audio";
		 string strReadAudioFile = path + ".audio";
		
// 		 FILE * pSaveFile = NULL;
// 		 pSaveFile = fopen(strSaveAudioFile.c_str(),"wb");
// 		 if (!pSaveFile)
// 		 {
// 			 string log = strSaveAudioFile + "无法保存";
// 			 MessageBox(NULL,log.c_str(),"提示",MB_OK);
// 			 return;
// 		 }

		 //save .audio file 
		 FILE * pReadFile = NULL;
		 pReadFile = fopen(strReadAudioFile.c_str(),"rb");
		 if (! pReadFile )
		 {
			 string log = strReadAudioFile + "无法打开";
			 MessageBox(NULL,log.c_str(),"请换其他地图",MB_OK);
			 return;
		 }
		 else
		 {
			 COperator *pOperator = CMapEditApp::GetInst()->GetOperator();

			 locale loc = locale::global(locale(""));

			 size_t size = 0;
			 int nNameLen = 0;
			 string strName = "";
			 DWORD dwVersion = 0;

			 fread(&dwVersion, sizeof(dwVersion), 1, pReadFile);
			 //fwrite(&dwVersion, sizeof(dwVersion), 1, pSaveFile);

			 int nGridCnt   = terrain->GetGridCount();
			 int nGridIndex = 0;
			 string strCueName = "", strItemName = "";
			 DWORD dwItemNameLen = 0;
			 bool bTrans = true;
			 ///trans state
			 BYTE bTransState = 0;
			 int nMirrorType = 0, nRatio = 0;
			 CVector3f vUserOffsetEx, vUserOffsetEx2; // (0.0f,0.0f,0.0f);
			 float fRotation, fPitch, fYaw;
			 float xScale, yScale, zScale;
			 int nRotationCnt = 0, nPitchCnt = 0, nYawCnt = 0;
			 EAUTO_GENERATION_TYPE eAutoGenerationType = EAGT_NONE;
			 BYTE byRenderStyle = 0;
			 bool bShadow = true;
			 bool bReceiveShadow = false;
			 int nAtten = 0;
			 DWORD dwPlayLenMin = 0, dwPlayLenMax = 0, dwPlayIntervalMin = 0, dwPlayIntervalMax = 0;

			 fread(&size, sizeof(size), 1, pReadFile);
			 //fwrite(&size,sizeof(size), 1, pSaveFile);

			 if( size > 0 )
				 MessageBox(NULL,"本地图有音效文件","提示",MB_OK);

			 for( size_t i = 0; i < size; ++i )
			 {
				 int nGrid = 0;
				 fread(&nGrid,sizeof(nGrid),1,pReadFile);
				 nGrid = this->mapReadMapGridIndex[nGrid];
				 //fwrite(&nGrid,sizeof(nGrid),1,pSaveFile);	

				 DWORD dw = 0;
				 fread(&dw,sizeof(DWORD),1,pReadFile);
				 //fwrite(&dw,sizeof(dw),1,pSaveFile);	

				 set<string> sGridCueNames;

				 for (size_t j = 0; j < dw; j++ )
				 {
					 switch(dwVersion)
					 {
					 case AUDIO_PLAYTIME_INTERVAL:
						 {
							 //////////////////////////////////////////////////////////////////////////
							 ///play time mechanism
							 fread(&dwPlayLenMin,	 sizeof(DWORD),1,pReadFile);
							 fread(&dwPlayLenMax,	 sizeof(DWORD),1,pReadFile);
							 fread(&dwPlayIntervalMin,sizeof(DWORD),1,pReadFile);
							 fread(&dwPlayIntervalMax,sizeof(DWORD),1,pReadFile);

							 //fwrite(&dwPlayLenMin,sizeof(DWORD),1,pSaveFile);
							 //fwrite(&dwPlayLenMax,sizeof(DWORD),1,pSaveFile);
							 //fwrite(&dwPlayIntervalMin,sizeof(DWORD),1,pSaveFile);
							 //fwrite(&dwPlayIntervalMax,sizeof(DWORD),1,pSaveFile);
							 //////////////////////////////////////////////////////////////////////////
						 }
					 case AUDIO_DEFAULT:
						 {
							 ///cue name
							 fread(&dwItemNameLen,sizeof(DWORD),1,pReadFile);
							 strCueName.resize(dwItemNameLen);
							 fread((TCHAR*)strCueName.data(),dwItemNameLen,1,pReadFile);

							 fread(&dwItemNameLen,sizeof(DWORD),1,pReadFile);

							 strItemName.resize(dwItemNameLen);
							 fread((TCHAR*)strItemName.data(),dwItemNameLen,1,pReadFile);

							 use_facet<ctype<TCHAR> >( loc ).tolower
								 ( &strItemName[0], &strItemName[strItemName.length()] );

							 //fwrite(&dwItemNameLen,sizeof(DWORD),1,pSaveFile);
							 //fwrite(strItemName.c_str(),dwItemNameLen,1,pSaveFile);
							 //////////////////////////////////////////////////////////////////////////

							 ///offset
							 fread(&vUserOffsetEx,sizeof(CVector3f),1,pReadFile);
							 vUserOffsetEx2 = CVector3f(vUserOffsetEx.y,vUserOffsetEx.x,vUserOffsetEx.z);
							 //fwrite(&vUserOffsetEx2,sizeof(CVector3f),1,pSaveFile);
							
							 //////////////////////////////////////////////////////////////////////////
							 fread(&nGridIndex,sizeof(int),1,pReadFile);
							 nGridIndex = this->mapReadMapGridIndex[nGridIndex]; 
							 //fwrite(&nGridIndex,sizeof(nGridIndex),1,pSaveFile);

							 //y 轴
							 fread(&fRotation,sizeof(float),1,pReadFile);
							 fread(&nRotationCnt,sizeof(int),1,pReadFile);

							 //fwrite(&fRotation,sizeof(float),1,pSaveFile);
							 //fwrite(&nRotationCnt,sizeof(int),1,pSaveFile);

							 //z 轴
							 fread(&fPitch,sizeof(float),1,pReadFile);
							 fread(&nPitchCnt,sizeof(int),1,pReadFile);

							 //fwrite(&fPitch,sizeof(float),1,pSaveFile);
							 //fwrite(&nPitchCnt,sizeof(int),1,pSaveFile);

							 //x 轴
							 fread(&fYaw,sizeof(float),1,pReadFile);
							 fread(&nYawCnt,sizeof(int),1,pReadFile);

							 //fwrite(&fYaw,sizeof(float),1,pSaveFile);
							 //fwrite(&nYawCnt,sizeof(int),1,pSaveFile);

							 //缩放
							 fread(&xScale,sizeof(float),1,pReadFile);
							 fread(&yScale,sizeof(float),1,pReadFile);
							 fread(&zScale,sizeof(float),1,pReadFile);

							 //fwrite(&xScale,sizeof(float),1,pSaveFile);
							 //fwrite(&yScale,sizeof(float),1,pSaveFile);
							 //fwrite(&zScale,sizeof(float),1,pSaveFile);
							 //////////////////////////////////////////////////////////////////////////

							 fread(&byRenderStyle, sizeof(BYTE),1,pReadFile);
							 fread(&nAtten,sizeof(int),1,pReadFile);				
							 fread(&bShadow,sizeof(bool),1,pReadFile);

							 //fwrite(&byRenderStyle, sizeof(BYTE),1,pSaveFile);
							 //fwrite(&nAtten,sizeof(int),1,pSaveFile);				
							 //fwrite(&bShadow,sizeof(bool),1,pSaveFile);

							 ///读取是否接收阴影信息
							 fread(&bReceiveShadow,sizeof(bool),1,pReadFile);
							 //fwrite(&bReceiveShadow,sizeof(bool),1,pSaveFile);

							 //是否消隐
							 fread(&bTrans,sizeof(bTrans),1,pReadFile);
							 //fwrite(&bTrans,sizeof(bTrans),1,pSaveFile);

							 ///mirror type
							 fread(&nMirrorType, sizeof(nMirrorType), 1, pReadFile);
							 //fwrite(&nMirrorType, sizeof(nMirrorType), 1, pSaveFile);

							 ///nAffectRadio
							 fread( &nRatio, sizeof(int), 1, pReadFile);
							 //fwrite( &nRatio, sizeof(int), 1, pSaveFile);
						 }

					 default:
						 break;
					 }

					 // 					if( strCueName.find("blacksmith01") != -1/* || strCueName.find("forest02") != -1*/ )
					 // 					{			
					 // 						DebugOut("cue name is %s, %d, %d, %d, %d\n", strCueName.c_str(), dwPlayLenMin, dwPlayLenMax, dwPlayIntervalMin, dwPlayIntervalMax);
					 // 					}

					 if( sGridCueNames.find(strCueName) != sGridCueNames.end() )
					 {
						 char c[10];
						 sprintf(c, "%d", nGridIndex);
						 string str = c;
						 str = str + "格子上有相同音效,YES删掉，NO不删掉";

						 if ( IDYES == MessageBox(NULL, str.c_str(), "提示", MB_YESNO) )
							 continue;
					 }

					 sGridCueNames.insert(strCueName);

 					 if(pOperator->Rebirth(strItemName, nGridIndex, fRotation, nRotationCnt,
 						 fPitch, nPitchCnt, fYaw, nYawCnt, xScale, yScale, zScale, &vUserOffsetEx2, 0, NULL, 0,0,1.0f,eAutoGenerationType,byRenderStyle,nAtten,false,bShadow,bTrans,bTransState,nMirrorType,nRatio, "", strItemName, strCueName))
 					 {
						 SGrid *pGrid = &terrain->GetGrid(nGridIndex);
 						 size_t size = pGrid->vecCenterObjects.size();
 						 CEditModelGroup *pModelGroup = NULL;
 						 CTObjectPtr p = pGrid->vecCenterObjects[size-1];
 						 pModelGroup = p->GetModelGroup();
 						 if( pModelGroup )
 						 {
 							 pModelGroup->m_pSceneAudio = new SceneAudio;
 
 							 pModelGroup->SetPlayTimeLenMin(dwPlayLenMin);
 							 pModelGroup->SetPlayTimeLenMax(dwPlayLenMax);
 							 pModelGroup->SetPlayIntervalMin(dwPlayIntervalMin);
 							 pModelGroup->SetPlayIntervalMax(dwPlayIntervalMax);
 
							 CSceneAudioMgr::GetInst()->AddIndividualSoundObject(p);
 						 }
 					 }
				 }

				 sGridCueNames.clear();
			 }

			 fclose(pReadFile);
//			 fclose(pSaveFile);
			 locale::global(loc);//恢复全局locale	

			 if( size > 0 )
				 MessageBox(NULL,"音效文件加载完毕","提示",MB_OK);
		}
	 }

	 void CLocalMergeMapSaver::SaveAsLogin(const string& path, set<int> nGridIndexs)
	 {
// 		 //save .login file 
// 		 CLocalMapLoginSaverPtr loginsaver = new CLocalMapLoginSaver();
// 		 loginsaver->SetTerrainMesh(CMapEditApp::GetInst()->GetTerrain());
// 		 loginsaver->SaveAsSceneLogin(path, nGridIndexs);
	 }

    void CLocalMergeMapSaver::SaveAsRgn( const string& path )
	{
		Ast(NULL != this->terrain);

		string szFullPath = path + ".rgn";

		// save .rgn file
		DWORD dwWidth = terrain->GetWidthInRegion();
		DWORD dwDepth = terrain->GetDepthInRegion();

		FILE * fp = NULL;
		fp = fopen(szFullPath.c_str(),"wb");
		if (!fp)
		{
			string log = szFullPath + "无法保存";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
			return;
		} 	
		//fwrite( &dwWidth, sizeof(DWORD), 1, fp );
		//fwrite( &dwDepth, sizeof(DWORD), 1, fp );
		fclose(fp);
	}

	void CLocalMergeMapSaver::SaveMergeMapBlockInfo( const string& path, const int nSaveMapGridCnt, const int nReadMapGridCnt )
	{
		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
		if( terrain->IsWater() )
		{
			MessageBox(NULL,"将当前状态改成地形状态，而非水面层状态，否则不能保存地图阻挡信息","提示",MB_OK);
			return ;
		}

		string strFileName = path;
		locale loc;
		use_facet<ctype<TCHAR> >( loc ).tolower
			( &strFileName[0], &strFileName[strFileName.length()] );

		DWORD dwVersion = BLOCK_VERSION;

		size_t npos = strFileName.rfind(".");
		if( npos != -1 )
			strFileName = strFileName.substr(0, npos);
		strFileName += ".block";

		//////////////////////////////////////////////////////////////////////////
		FILE * pReadFile = NULL;
		pReadFile = fopen(strFileName.c_str(),"rb");

		if (!pReadFile)
		{
			string log = strFileName + "无法打开";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
			return ;
		}

		///version
		fread(&dwVersion,sizeof(dwVersion),1,pReadFile);
		///grid
		int nBlockType = 0, nGridIndex = 0;
		map<int, int> mapReadMapGridBlockIndex;///first grid index second block type
		for ( int i  = 0; i < nReadMapGridCnt; ++i )
		{
			fread(&nBlockType,sizeof(nBlockType),1,pReadFile);
			nGridIndex = this->mapReadMapGridIndex[i];
			mapReadMapGridBlockIndex[nGridIndex] = nBlockType;
		}

		fclose(pReadFile);

		///扩展后旧地图格子索引在新地图中的新索引映射 first new grid   index second old grid index
		map<int, int>::iterator iter, beg;
		//////////////////////////////////////////////////////////////////////////

		strFileName = m_strMapPathName;
		use_facet<ctype<TCHAR> >( loc ).tolower
			( &strFileName[0], &strFileName[strFileName.length()] );

		npos = strFileName.rfind(".");
		if( npos != -1 )
			strFileName = strFileName.substr(0, npos);

		strFileName += ".block";

// 		FILE * pSaveFile = NULL;
// 		pSaveFile = fopen(strFileName.c_str(),"wb");
// 		if (!pSaveFile)
// 		{
// 			string log = strFileName + "无法保存";
// 			MessageBox(NULL,log.c_str(),"提示",MB_OK);
// 			return ;
// 		}

		///version
		int   nNoBlock  = 0;
		//fwrite(&dwVersion, sizeof(dwVersion), 1, pSaveFile);
		int nSaveGridIndex = 0, nReadGridIndex = 0;
		for ( int i  = 0; i < nSaveMapGridCnt; ++i )
		{
			SGrid &grid = terrain->GetGrid(i);

			beg = mapReadMapGridBlockIndex.find(i);
			if( beg != mapReadMapGridBlockIndex.end() )
				grid.nBlockType = beg->second;
		
			//fwrite(&nBlockType, sizeof(int), 1, pSaveFile);
		}
		//////////////////////////////////////////////////////////////////////////
//		fclose(pSaveFile);
	}

	void CLocalMergeMapSaver::SaveWaterInfo( FILE *pRead, FILE *pSave, const int nSaveMapGridCnt, const int nReadMapGridCnt )
	{
		COperator    *pOperator = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetTerrain();
		CTerrainMesh * pWater   = pTerrain->GetWater();
		vector<int> vecGridIndicesRiver;

		DWORD dwRiverCnt = 0;
		fread(&dwRiverCnt,sizeof(DWORD),1,pRead);
		//fwrite(&dwRiverCnt,sizeof(DWORD),1,pSave);
		map<int, int>::iterator iter, end = this->mapReadMapGridIndex.end();

		for ( size_t nRiverIndex = 0; nRiverIndex < dwRiverCnt; ++nRiverIndex )
		{
			///river name
			DWORD dwItemNameLen = 0;
			fread(&dwItemNameLen,sizeof(DWORD),1,pRead);
			string strItemName;
			strItemName.resize(dwItemNameLen);
			fread((TCHAR*)strItemName.data(),dwItemNameLen,1,pRead);

			//fwrite(&dwItemNameLen,sizeof(int),1,pSave);
			//fwrite(strItemName.c_str(),dwItemNameLen,1,pSave);

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

			CMapEditApp::GetInst()->GetWaterOperator()->RebirthRiver(strItemName);
			CRiver * pRiver = CMapEditApp::GetInst()->GetWaterOperator()->GetRiver();

			DWORD dwWaterCnt = 0;
			fread(&dwWaterCnt,sizeof(DWORD),1,pRead);
			//fwrite(&dwWaterCnt,sizeof(DWORD),1,pSave);

			int nGridIndex = 0;
			float fWaterHeight = 0.0f, fTerrainHeight = 0.0f;
			DWORD oColor = 0;
			for ( size_t nWaterIndex = 0; nWaterIndex < dwWaterCnt; ++nWaterIndex )
			{
				fread(&nGridIndex,sizeof(DWORD),1,pRead);
				///变换新的地形格子索引
				iter = this->mapReadMapGridIndex.find(nGridIndex);
				if( iter != end )
					nGridIndex = iter->second;

				fread(&fWaterHeight,sizeof(float),1,pRead);

				vecGridIndicesRiver.push_back(nGridIndex);
				SGrid & gridW = pWater->GetGrid(nGridIndex);
				gridW.pRiver = pRiver;
				gridW.vCenter.y = fWaterHeight;

				for ( int j = 0; j < 4; ++j )
				{
					fread(&fTerrainHeight,sizeof(float),1,pRead);
					fread(&oColor,sizeof(DWORD),1,pRead);

					int nVertexIndex = gridW.dwVertexIndex[j];
					SVertex & vertex = pWater->GetVertex(nVertexIndex);

					vertex.vPosition.y = fTerrainHeight;
					vertex.oColor = oColor;
				}
			}
		}

		DWORD dwUnoccupiedWaterCnt = 0;
		fread(&dwUnoccupiedWaterCnt,sizeof(DWORD),1,pRead);
		//fwrite(&dwUnoccupiedWaterCnt,sizeof(DWORD),1,pSave);

		float fWaterHeight = 0.0f, fTerrainHeight = 0.0f;
		DWORD oColor = 0;
		for ( size_t nWaterIndex = 0; nWaterIndex < dwUnoccupiedWaterCnt; ++nWaterIndex )
		{
			int nGridIndex = 0;
			fread(&nGridIndex,sizeof(DWORD),1,pRead);

			///变换新的地形格子索引
			nGridIndex = pOperator->m_OldMapGridIndexMap[nGridIndex];
			iter = this->mapReadMapGridIndex.find(nGridIndex);
			if( iter != end )
				nGridIndex = iter->second;
			
			SGrid & gridW = pWater->GetGrid(nGridIndex);
			fread(&fWaterHeight,sizeof(float),1,pRead);
			gridW.vCenter.y = fWaterHeight;

			for ( int j = 0; j < 4; ++j )
			{
				int nVertexIndex = gridW.dwVertexIndex[j];

				SVertex & vertex = pWater->GetVertex(nVertexIndex);

				fread(&fTerrainHeight,sizeof(float),1,pRead);
				fread(&oColor,sizeof(DWORD),1,pRead);

				vertex.vPosition.y = fTerrainHeight;
				vertex.oColor = oColor;
			}
		}

		for (size_t i = 0; i < vecGridIndicesRiver.size(); ++i )
		{
			pWater->AdjustWaterMeshVisibility(vecGridIndicesRiver[i],true);
		}
	}

	void CLocalMergeMapSaver::SavePointLightInfo( FILE *pRead, FILE *pSave )
	{
		int  tLength = 11;
		bool bSceneLightCntEqual = true;
		
		int ExtraData = 0;
		TCHAR ExtraName[255];
		map<int, int>::iterator iter;

		///读取场景所有点光源信息
		fread(&ExtraData,sizeof(int),1,pRead);
		//fwrite( &tLength, sizeof(int),1,pSave);
		if ( ExtraData == 11 )
		{
			fread( ExtraName, ExtraData, 1, pRead);
			//fwrite( "SceneLight", 11, 1, pSave);

			stringstream errmsg;
			int SceneLightCount = 0;
			fread( &SceneLightCount, sizeof(int), 1, pRead);
			//fwrite( &SceneLightCount, sizeof(int), 1, pSave);

			SLB temp;
			for ( int i = 0; i < SceneLightCount; ++i )
			{
				fread( &temp.GridIndex, sizeof(int), 1, pRead);
				//DebugOut("grid %d\n", temp.GridIndex);

				//////////////////////////////////////////////////////////////////////////

				fread( &temp.iter, sizeof(int), 1, pRead);
				fread( &temp.m_SL, sizeof(SceneLight), 1, pRead);

				int nLightGridIndex = temp.GridIndex;
				iter = this->mapReadMapGridIndex.find(temp.GridIndex);
				if( iter != this->mapReadMapGridIndex.end() )
					nLightGridIndex = iter->second;

				SGrid *pGrid = NULL;
				pGrid = &terrain->GetGrid(nLightGridIndex);
				size_t size = pGrid->vecCenterObjects.size();

				if ( !pGrid->vecCenterObjects.empty() )
				{
					int gridModelIndex = 0;
					gridModelIndex = temp.iter;
					if ( gridModelIndex < (int)(size) )
					{
						CTObjectPtr p = pGrid->vecCenterObjects[temp.iter];

						p->GetModelGroup()->m_SL = temp.m_SL;
						p->GetModelGroup()->SetIsLightEffect(true);
 						OfflineRender::GetInst()->UpdateGridLights(temp.GridIndex);
 
 						//insert
 						terrain->m_DynamicLightGrids.insert(temp.GridIndex);
					}
				}

				//fwrite( &nLightGridIndex,	 sizeof(nLightGridIndex), 1, pSave);
				//fwrite( &temp.iter,			 sizeof(int),			  1, pSave);
				//fwrite( &temp.m_SL,			 sizeof(SceneLight),      1, pSave);
			}
		}
		else
			return;
	}

	void CLocalMergeMapSaver::SaveRecordInfo( FILE *pRead, FILE *pSave )
	{
		uint32 uAnimCount = 0;
		fread( &uAnimCount, sizeof(uint32), 1, pRead );
		//fwrite( &uAnimCount, sizeof(uint32), 1, pSave );

		for ( uint32 i = 0; i < uAnimCount; ++i )
		{
			int		pc		= 0;
			fread(&pc,sizeof(int),1,pRead);
			//fwrite(&pc,sizeof(int),1,pSave);
		}
	}

	void CLocalMergeMapSaver::CreateReadMapLightInfo(const string& path)
	{
		///处理原有的
		set<int> regionUnits, regionUnitsHasSet;
 		map<string, SceneRegion*>::iterator iter = CSceneRegionMgr::GetInst()->m_mapRegion.begin();
 		map<string, SceneRegion*>::iterator eiter = CSceneRegionMgr::GetInst()->m_mapRegion.end();
 		string strRgnName = "";
 		int nSceneRgnIndex = 0;
 		for ( iter; iter != eiter; ++iter )
 		{
 			strRgnName = iter->first;

			if( setHasRegNames.find(strRgnName) == setHasRegNames.end() )
			{
				SceneRegion *pSceneReg = iter->second;
				regionUnits = pSceneReg->_RegionUnitList;

				set<int> regionNewUnits;
				int nDivideRegIndex = 0;
				for( set<int>::iterator beg = regionUnits.begin(); beg != regionUnits.end(); ++beg )
				{
					nDivideRegIndex = *beg;
					nDivideRegIndex = this->mapReadMapDivideRegionIndex[nDivideRegIndex];

					//DebugOut("load%d\n", nDivideRegIndex);
					regionUnitsHasSet.insert(nDivideRegIndex);
					regionNewUnits.insert(nDivideRegIndex);
				}

				pSceneReg->_RegionUnitList.clear();
				pSceneReg->_RegionUnitList = regionNewUnits;
			}
 		}

		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();

		DWORD ambient = CSceneEnvMgr::GetInst()->GetSceneAmbientColor();
		DWORD diffuse = CSceneEnvMgr::GetInst()->GetSceneDirColor();
		DWORD playerambient = CSceneEnvMgr::GetInst()->GetPlayerAmbientColor();
		DWORD playerdiffuse = CSceneEnvMgr::GetInst()->GetPlayerDirColor();
		DWORD scenespecular = CSceneEnvMgr::GetInst()->GetSceneSpecurlarColor();
		DWORD playespecular = CSceneEnvMgr::GetInst()->GetPlayerSpecurlarColor();
		DWORD shadow = CSceneEnvMgr::GetInst()->GetShadowColor();
		DWORD fog = CSceneEnvMgr::GetInst()->GetFogColor();

		float fFogStart   = CSceneEnvMgr::GetInst()->GetFogStart();
		float fFogEnd     = CSceneEnvMgr::GetInst()->GetFogEnd();
		float fFogDensity = CSceneEnvMgr::GetInst()->GetDensity();

		SceneRegion *pCurSceneRegion = new SceneRegion;
		if ( pCurSceneRegion )
		{
			pCurSceneRegion->lightParam.globalAmbient = ambient;
			pCurSceneRegion->lightParam.globalDiffuse = diffuse;
			pCurSceneRegion->lightParam.globalShadow = shadow;
			pCurSceneRegion->lightParam.playerAmbient = playerambient;
			pCurSceneRegion->lightParam.playerDiffuse = playerdiffuse;
			pCurSceneRegion->lightParam.globalSpecular = scenespecular;
			pCurSceneRegion->lightParam.playerSpecular = playespecular;
			pCurSceneRegion->lightParam.fogColor = fog;
			pCurSceneRegion->SetFogStart(fFogStart);
			pCurSceneRegion->SetFogEnd(fFogEnd);
			pCurSceneRegion->SetFogDensity(fFogDensity);
		}

		string strReginName = path;
		size_t npos = strReginName.rfind("\\");
		if( npos != -1 )
			strReginName = strReginName.substr(npos+1, strReginName.length());

		DebugOut("%s, %u, %u\n", strReginName.c_str(), ambient, diffuse);
		CVector3f sunDir = CSceneEnvMgr::GetInst()->GetDirDir();
		DebugOut("Merge Sun Direction is: %s, %f, %f, %f" ,strReginName.c_str(), sunDir.x, sunDir.y ,sunDir.z );

		regionUnits.clear();
		map<int, int>::iterator beg = mapReadMapDivideRegionIndex.begin();
		map<int, int>::iterator end = mapReadMapDivideRegionIndex.end();
		for ( beg; beg != end; ++beg )
		{
			nSceneRgnIndex = beg->first;

			if( regionUnitsHasSet.find(nSceneRgnIndex) == regionUnitsHasSet.end() )
			{
				nSceneRgnIndex = mapReadMapDivideRegionIndex[nSceneRgnIndex];
				regionUnits.insert(beg->second);
				//DebugOut("region unit is %d\n", beg->second);
			}
		}

		//DebugOut("region cnt %d\n", regionUnits.size());

		CSceneRegionMgr::GetInst()->AddSceneRegion(strReginName, pCurSceneRegion);
		if(!CSceneRegionMgr::GetInst()->AddSceneRegionUnits(strReginName, regionUnits))
			MessageBox( NULL, "加载区域光失败" , "提示", MB_OK );

		return;
	}

	//------------------------------------------------------------------------------
	void
	CLocalMergeMapSaver::MakeTerrainTexture(set<string> terrainTexIndexsSet)
	{
		// #ifdef _DEBUG
		// 		return;
		// #endif

		CRenderDevice * pRender = NULL;
		pRender = CRenderDevice::GetInst();
		if ( pRender == NULL )
			return;

		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetTerrain();

		int texCnt = 0;

		ITexture* pTexture;

		if ( pTerrain->m_pTerrainTexture == NULL )
			CGraphic::GetInst()->CreateEmptyTexture(EXPAND_MAX_SIZE,EXPAND_MAX_SIZE,1, TF_XRGB32,&(pTerrain->m_pTerrainTexture));
			//GetD3DDevice()->CreateTexture(
			//EXPAND_MAX_SIZE, EXPAND_MAX_SIZE, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &pTerrain->m_pD3DTerrainTexture, NULL );

		string texname = "";
		set<string>::iterator iter = terrainTexIndexsSet.begin();
		set<string>::iterator eiter = terrainTexIndexsSet.end();
		set<string> erroTexList;
		for ( iter; iter != eiter; ++iter )
		{
			texname = *iter;
			pTexture = pRender->GetTexture(texname);
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

			gridtexname = Grid.nodeInfo.strTextureNames[2];
			erroriter = erroTexList.find(gridtexname);
			if (erroriter != eeiter)
				Grid.nodeInfo.strTextureNames[2] = "";
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
		string selfTexPath = homePath + pTerrain->m_strMapName;
		format(selfTexPath,"%s.jpg",selfTexPath.c_str());
		pTerrain->m_pTerrainTexture->SaveToFile(selfTexPath.c_str(),IFF_JPG);
		//D3DXSaveTextureToFile(,D3DXIFF_JPG,,&pale);
	}

	void CLocalMergeMapSaver::SetTerrainMesh( CTerrainMesh* t )
	{
		Ast(NULL != t);
		this->terrain = t;
		m_strDefaultTerrainTexName = terrain->GetTerrainDefaultTextureName();
	}
}