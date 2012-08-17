//------------------------------------------------------------------------------
//  CLocalSplitMapSaver.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "LocalSplitMapSaver.h"
#include "GraphicCommon.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "Operator.h"
#include "Operator.inl"
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
#include "CodeCvs.h"
#include "CXmlConfig.inl"
#include "CMapEditModelObjPro.h"
#include "IoSystem.h"
#include "LocalVersion.h"
#include "SceneRegionMgr.h"
#include "SceneAudio.h"
#include "MapCommonDataSaver.h"
#include "Console.h"
#include "ToolRecord.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "ToolMainPlayer.h"
#include "CMapEffect.h"

namespace sqr
{

	//------------------------------------------------------------------------------
	CLocalSplitMapSaver::CLocalSplitMapSaver()
		: terrain(NULL)
		, progress(NULL)
	{
	}

	//------------------------------------------------------------------------------
	///save 分割后地图的.project file
	void
	CLocalSplitMapSaver::SaveAsSplitProject()
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
	///load 被分割地图的.project file
	void
	CLocalSplitMapSaver::LoadAsMapProject(const string& path)
	{
		if( path.empty() )
		{
			MessageBox(NULL, "被分割的地图名为空", "提示", MB_OK);
			return;
		}
		
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
	CLocalSplitMapSaver::SaveAsSplitRgnLightInfo()
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
			bool flag = false;

			//是否全局光效
			flag = CSceneEnvMgr::GetInst()->GetIsTurnOnGaussTarget();
			fwrite(&flag, 1, 1, fp);

			//是否开启主角光效
			flag = false;
			fwrite(&flag, 1, 1, fp);

			//是否开启雾效
			flag = CSceneEnvMgr::GetInst()->GetDoesEnableFog();
			fwrite(&flag, 1, 1, fp);

			//是否开启物件透明
			flag = false;
			fwrite(&flag, 1, 1, fp);

			//是否开启全局视角阴影
			flag = CSceneEnvMgr::GetInst()->GetIsLightMapShadow();
			fwrite(&flag, 1, 1, fp);

			//是否开启标准视角阴影
			flag = CSceneEnvMgr::GetInst()->GetIsRealtimeShadow();
			fwrite(&flag, 1, 1, fp);

			CSceneRegionMgr::GetInst()->SaveSceneRegionInfo(fp, true);

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
	CLocalSplitMapSaver::SaveAsSplitLocal()
	{
		COperator *pOperator = NULL;
		pOperator = CMapEditApp::GetInst()->GetOperator();
		if( pOperator == NULL )
			return;

		CMapCommonDataSaver::GetInst()->SetOperator(pOperator);
		CMapCommonDataSaver::GetInst()->SetTerrainMesh(terrain);

		CMapEditApp::GetInst()->SetEditingMesh(sqr::EEM_TERRAIN);

		locale loc;
		use_facet<ctype<TCHAR> >( loc ).tolower
			( &m_strMapPathName[0], &m_strMapPathName[m_strMapPathName.length()] );

		string strLocalPath = m_strMapPathName + ".local";
		FILE * fp = fopen(strLocalPath.c_str(),"wb");
		int SceneLightCount = 0;	//记录场景点光源个数
		int nSpotLightCount = 0;    //记录场景聚光灯个数

		if ( fp )
		{
			set<DWORD> eraseGrids;

			// header
			fwrite("LOCL",4,1,fp);

			// version
			DWORD dwVersion = LOCAL_VERSION;
			fwrite(&dwVersion,sizeof(DWORD),1,fp);

			///游戏客户端服务器端文件ID号
			DWORD dwGameClientID = 0;
			fwrite(&dwGameClientID,sizeof(DWORD),1,fp);

			DWORD dwGameServerID = 0;
			fwrite(&dwGameServerID,sizeof(DWORD),1,fp);

			// background music
			string str = CSceneEnvMgr::GetInst()->GetBackGroundMusic();

			DWORD dwFileNameLen = str.length();
			fwrite(&dwFileNameLen,sizeof(DWORD),1,fp);
			fwrite(str.c_str(),dwFileNameLen,1,fp);

			// main player
			CToolMainPlayer::GetInst()->Save(fp);

			// camera
			CMapEditCamera * pCamera = CCameraManager::GetInst()->GetMapEditCamera();
			CVector3f vView = pCamera->GetTarget();
			fwrite(&vView,sizeof(CVector3f),1,fp);

			//fov
			float fov = pCamera->GetFovY();
			fwrite(&fov,sizeof(float),1,fp);

			//yaw
			float yaw = pCamera->GetYaw();
			fwrite(&yaw,sizeof(float),1,fp);

			//pitch
			float pitch = pCamera->GetPitch();
			fwrite(&pitch,sizeof(float),1,fp);

			CMapCommonDataSaver::GetInst()->SaveEnvInfo(fp);

			///背景图移动相关信息
			CMapCommonDataSaver::GetInst()->SaveBackPictureInfo(fp);

			// fog mode
			int fogMode = CSceneEnvMgr::GetInst()->GetFogMode();
			fwrite(&fogMode,sizeof(int),1,fp);

			//全屏泛光
			CSceneEnvMgr::GetInst()->SaveGaussParamsInfo(fp);

			// wave info
			CMapCommonDataSaver::GetInst()->SaveWaveInfo(fp);

			// ambient fx info
			CMapEffect::GetInst()->SaveAmbientFXInfo(fp);

			// width depth
			fwrite(&m_dwSplitMapWidth,sizeof(DWORD),1,fp);
			fwrite(&m_dwSplitMapDepth,sizeof(DWORD),1,fp);

			//////////////////////////////////////////////////////////////////////////
			///write main road texture name and weight
			CMapCommonDataSaver::GetInst()->SaveMainRoadWeightInfo(fp);
			//////////////////////////////////////////////////////////////////////////

			// rgn mask
			int   newGridCnt = 0, SceneLightCount = 0;
			DWORD newVertexCnt = 0;

			DWORD dwMapWidth = m_dwSplitMapWidth * REGION_SPACE;
			DWORD dwMapDepth = m_dwSplitMapDepth * REGION_SPACE;
			newGridCnt		 = dwMapWidth * dwMapDepth;
			newVertexCnt     = (dwMapWidth + 1) * (dwMapDepth + 1);

			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			CTerrainMesh *pTerrainNew = NULL;
			pTerrainNew = new CTerrainMesh; 
			pTerrainNew->Create(m_dwSplitMapWidth, m_dwSplitMapDepth, m_strDefaultTerrainTexName, false, false);
			if( pTerrainNew == NULL )
				return;

			DWORD gridSplitedW		= pTerrainNew->GetWidth();
			DWORD gridSplitedH		= pTerrainNew->GetDepth();
			DWORD gridReadW			= terrain->GetWidth();

			int   nSplitGridStartX   = m_dwSplitStartX * REGION_SPACE;
			int   nSplitGridStartZ   = m_dwSplitStartZ * REGION_SPACE;

			int nNewVertexIndex = 0, nOldVertexIndex = 0;
			DWORD dwNewGridIndex = 0, dwOldGridIndex = 0;
			for ( DWORD z = 0 ; z < gridSplitedH; z++ )
			{
				for ( DWORD x = 0 ; x < gridSplitedW ; x++ )
				{
					dwNewGridIndex = x + z * gridSplitedW;
					dwOldGridIndex = x + nSplitGridStartX + (nSplitGridStartZ + z) * gridReadW;
					// 				int col = x + nSaveGridStartX;
					// 				int row = (nSaveGridStartZ + z) * gridSaveW;

					if( pTerrainNew->IsValid(dwNewGridIndex) )
					{
						SGrid * pGridNew = &pTerrainNew->GetGrid(dwNewGridIndex);
						SGrid * pGridOld = &terrain->GetGrid(dwOldGridIndex);

						this->mapSplitMapGridIndex[dwNewGridIndex]		     = dwOldGridIndex;
						this->mapRevertSplitMapGridIndex[dwOldGridIndex]     = dwNewGridIndex;
						pOperator->m_SplitOldMapGridIndexMap[dwOldGridIndex] = dwNewGridIndex;
	
						//DebugOut("Read reg index %d, save %d\n", nReadDivideRegionIndex, nSaveDivideRegionIndex);

						for( int m = 0; m < 4; ++m )
						{
							nOldVertexIndex = pGridOld->dwVertexIndex[m];
							nNewVertexIndex = pGridNew->dwVertexIndex[m];

							this->mapSplitMapVertexIndex[nNewVertexIndex] = nOldVertexIndex;

							//DebugOut("old vertex %d, new %d\n", nReadVertexIndex, nSaveVertexIndex);
						}
					}
					else
						MessageBox(NULL,"越界，请注意","提示",MB_OK);
				}
			}
			//////////////////////////////////////////////////////////////////////////

#pragma region VERTEX
			///顶点信息
			map<int, int>::iterator vertexiter;
			for ( DWORD n = 0; n < newVertexCnt; ++n )
			{
				DWORD dwVertexIndexOld = 0, oColor = VERTEX_COLOR, sColor = VERTEX_COLOR;
				float fPositionY = 0.0f;
				CVector3f vNormal(0.0, 1.0f, 0.0f);
				short sLogicHeight = 0;

				vertexiter = this->mapSplitMapVertexIndex.find(n);
				if ( vertexiter != this->mapSplitMapVertexIndex.end() )
				{
					SVertex &VertexOld = terrain->GetVertex(vertexiter->second);

					oColor		 = VertexOld.oColor;
					sColor		 = VertexOld.sColor;
					fPositionY   = VertexOld.vPosition.y;
					vNormal		 = VertexOld.vNormal;
					sLogicHeight = short(VertexOld.fLogicHeight);
				}
				else
				{
					char c[10];
					sprintf(c, "%d", n);
					string str = c;
					str = str + "顶点切割失败";

					MessageBox(NULL,str.c_str(),"提示",MB_OK);
				}

				CMapCommonDataSaver::GetInst()->SaveEachVertexInfo(fp, fPositionY, vNormal, oColor, sColor, sLogicHeight);
			}
#pragma endregion VERTEX

			//////////////////////////////////////////////////////////////////////////
			///model property version
			CMapCommonDataSaver::GetInst()->SaveGridModeProVersion(fp);
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			///记录格子纹理索引
			int nGridCnt = terrain->GetGridCount();
			map<string, int16> terrainTextureNames;///用于记录地图中用到的所有纹理的名字，保存各层纹理信息则保存index就行
			CMapCommonDataSaver::GetInst()->SaveTerrainGridTexIndex(fp, nGridCnt, terrainTextureNames);
			//////////////////////////////////////////////////////////////////////////

#pragma region GRID
			// grids
			string strName = "";
			SGrid * pGridOld = NULL;
			int16  nFirstTextureIndex = 0, nSecondTextureIndex = 0;
			DWORD dwGridIndexOld = 0;
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

			map<int, int>::iterator griditer;
			for (int i = 0; i < newGridCnt; ++i )
			{
				int x = i % dwMapWidth;
				int z = i / dwMapWidth;
				int nX = x % 2;
				int nZ = z % 2;

				SGrid * pGridNew = &pTerrainNew->GetGrid(i);
				pGridNew->dwGridIndex = i;

				griditer = this->mapSplitMapGridIndex.find(i);
				if ( griditer != this->mapSplitMapGridIndex.end() )
				{
					dwGridIndexOld = griditer->second;
					pGridOld = &terrain->GetGrid(dwGridIndexOld);

					pGridNew->bGridHide = pGridOld->bGridHide;
					pGridNew->nBlockType = pGridOld->nBlockType;

					pGridNew->nodeInfo.strTextureNames[0] = pGridOld->nodeInfo.strTextureNames[0];
					pGridNew->nodeInfo.strTextureNames[1] = pGridOld->nodeInfo.strTextureNames[1];

					pGridNew->bCliffLayerOne = pGridOld->bCliffLayerOne;

					for (int m = 0; m < 4; ++m)
					{
						pGridNew->uv[0][m] = pGridOld->uv[0][m];
						pGridNew->uv[1][m] = pGridOld->uv[1][m];
					}

					pGridNew->vCenter.y				= pGridOld->vCenter.y;
					pGridNew->vecCenterObjects		= pGridOld->vecCenterObjects;
					pGridNew->bEffectByVertexColor	= pGridOld->bEffectByVertexColor;
				}
				else
				{
					char c[10];
					sprintf(c, "%d", i);
					string str = c;
					str = str + "格子切割失败";

					MessageBox(NULL,str.c_str(),"提示",MB_OK);

					pGridNew->nodeInfo.strTextureNames[0] = m_strDefaultTerrainTexName;
					pGridNew->nodeInfo.strTextureNames[1] = "";

					for (int m = 0; m < 4; ++m)
					{
						pGridNew->uv[0][m].x = U[nX][m];
						pGridNew->uv[0][m].y = V[nZ][m];
					}
				}

				//grid hide mask
				fwrite(&pGridNew->bGridHide,sizeof(bool),1,fp);

				//1
				strName = pGridNew->nodeInfo.strTextureNames[0];
				nFirstTextureIndex = terrainTextureNames[strName];
				fwrite(&nFirstTextureIndex,sizeof(nFirstTextureIndex),1,fp);

				//2
				strName = pGridNew->nodeInfo.strTextureNames[1];
				nSecondTextureIndex = terrainTextureNames[strName];
				fwrite(&nSecondTextureIndex,sizeof(nSecondTextureIndex),1,fp);

				/////////////////////////////////////////////////////////////////
				CMapCommonDataSaver::GetInst()->SaveEachGridInfo(fp, pGridNew, SceneLightCount, nSpotLightCount, false, true);
			}
#pragma endregion GRID
			//////////////////////////////////////////////////////////////////////////

			// block lines
			this->SaveSplitMapBlockInfo(newGridCnt);
			
			///pTerrain
			size_t erasebloclsize = 0;
			fwrite( &erasebloclsize, sizeof(size_t), 1, fp );

			//////////////////////////////////////////////////////////////////////////
			// water

			CTerrainMesh * pWater = terrain->GetWater();
			set<int> setGridIndicesVisibleWater = pWater->GetGridIndicesVisibleWater();
			map< CRiver*, vector<int> > mapRivers;
			map<int, int>::iterator splitRevertGriditer;

			for ( set<int>::iterator iter = setGridIndicesVisibleWater.begin(); iter != setGridIndicesVisibleWater.end(); )
			{
				int nGridIndex = *iter;
				SGrid & gridW = pWater->GetGrid(nGridIndex);

				++iter;

				Ast(gridW.bVisible);
				if ( gridW.pRiver )
				{
					setGridIndicesVisibleWater.erase(nGridIndex);

					splitRevertGriditer = this->mapRevertSplitMapGridIndex.find(nGridIndex);
					if( splitRevertGriditer != this->mapRevertSplitMapGridIndex.end() )
						nGridIndex = splitRevertGriditer->second;
					else
					{
						//DebugOut("Grid %d\n", nGridIndex);
						continue;
					}

					mapRivers[gridW.pRiver].push_back(nGridIndex);
				}
			}

			DWORD dwCnt = mapRivers.size();
			fwrite(&dwCnt,sizeof(DWORD),1,fp);

			// visible and occupied
			for ( map< CRiver*, vector<int> >::iterator iterMapRivers = mapRivers.begin(); iterMapRivers != mapRivers.end(); ++iterMapRivers )
			{
				CRiver * pRiver = iterMapRivers->first;

				string strItemName = pRiver->GetItemName();

				DWORD dw = strItemName.size();
				fwrite(&dw,sizeof(int),1,fp);
				fwrite(strItemName.c_str(),dw,1,fp);

				dwCnt = iterMapRivers->second.size();
				fwrite(&dwCnt,sizeof(DWORD),1,fp);

				for (size_t i = 0; i < iterMapRivers->second.size(); ++i  )
				{
					int nGridIndex = iterMapRivers->second[i];
					fwrite(&nGridIndex,sizeof(DWORD),1,fp);///切割后格子索引

					///得到被切割地图对应索引，将该格子得到，取得水的高度值
					splitRevertGriditer = this->mapSplitMapGridIndex.find(nGridIndex);
					if( splitRevertGriditer != this->mapSplitMapGridIndex.end() )
						nGridIndex = splitRevertGriditer->second;
					else
					{
						nGridIndex = 0;
						MessageBox(NULL,"地图切割水的地方有问题，请注意","提示",MB_OK);
					}

					SGrid & gridW = pWater->GetGrid(nGridIndex);
					fwrite(&gridW.vCenter.y,sizeof(float),1,fp);

					for ( int j = 0; j < 4; ++j )
					{
						int nVertexIndex = gridW.dwVertexIndex[j];

						SVertex & vertex = pWater->GetVertex(nVertexIndex);
						fwrite(&vertex.vPosition.y, sizeof(float),1,fp);
						fwrite(&vertex.oColor,sizeof(DWORD),1,fp);
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
			dwCnt = setGridIndicesVisibleWater.size();
			fwrite(&dwCnt,sizeof(DWORD),1,fp);

			for ( set<int>::iterator iter = setGridIndicesVisibleWater.begin(); iter != setGridIndicesVisibleWater.end(); ++iter )
			{
				int nGridIndex = *iter;
				fwrite(&nGridIndex,sizeof(DWORD),1,fp);///切割后格子索引

				///得到被切割地图对应索引，将该格子得到，取得水的高度值
				splitRevertGriditer = this->mapSplitMapGridIndex.find(nGridIndex);
				if( splitRevertGriditer != this->mapSplitMapGridIndex.end() )
					nGridIndex = splitRevertGriditer->second;
				else
				{
					nGridIndex = 0;
					MessageBox(NULL,"地图切割水的地方有问题，请注意","提示",MB_OK);
				}

				SGrid & gridW = pWater->GetGrid(nGridIndex);
				fwrite(&gridW.vCenter.y,sizeof(float),1,fp);

				for ( int j = 0; j < 4; ++j )
				{
					int nVertexIndex = gridW.dwVertexIndex[j];

					SVertex & vertex = pWater->GetVertex(nVertexIndex);
					fwrite(&vertex.vPosition.y,sizeof(float),1,fp);
					fwrite(&vertex.oColor,sizeof(DWORD),1,fp);
				}
			}

			/////////////////// //--- 增加 SceneLight 数据块
			///save point light
			bool bSceneLightCntEqual = true;
			this->SavePointLightInfo(fp, SceneLightCount);


			///////////////////////--- 增加cave数据块，用来保存是否是“室内”
			int tLength = 6;
			fwrite( &tLength, sizeof(int), 1, fp);
			fwrite( "bCave", 6, 1, fp);
			// cave？ no shadow
			bool bCave = false;
			fwrite(&bCave,sizeof(bool),1,fp);

			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			DWORD dwNewVersion = SPECULAR_LIGHT_PARA;
			fwrite(&dwNewVersion,sizeof(DWORD),1,fp);

			//////////////////////////////////////////////////////////////////////////
			//save specular light parameter
			for (size_t i = 0; i < 4; ++i)
			{
				// light
				DWORD dwSceneSpecurlarColor = CSceneEnvMgr::GetInst()->GetSceneSpecurlarColor();
				fwrite(&dwSceneSpecurlarColor,sizeof(DWORD),1,fp);

				DWORD dwPlayerSpecurlarColor = CSceneEnvMgr::GetInst()->GetPlayerSpecurlarColor();
				fwrite(&dwSceneSpecurlarColor, sizeof(DWORD), 1, fp);
			}

			float fSceneSPower = CSceneEnvMgr::GetInst()->GetSpecularReflectionPower(0);
			fwrite(&fSceneSPower, sizeof(fSceneSPower), 1, fp);

			float fNpcSPower = CSceneEnvMgr::GetInst()->GetSpecularReflectionPower(1);
			fwrite(&fNpcSPower, sizeof(fNpcSPower), 1, fp);
			//////////////////////////////////////////////////////////////////////////

			/// 保存格子材质索引值
			this->SaveSplitMapGridMatIndexInfo(fp, newGridCnt);

			//保存不播放动画的信息
			this->SaveOffAnimateModelIndex(fp);

			//保存精确阻挡信息
			this->SaveSplitMapExactBlockInfo(fp, newGridCnt);

			// 保存格子浪花标记
			this->SaveWaveSideInfo(fp, newGridCnt);

			///保存聚光灯的信息
			this->SaveSpotLightInfo(fp, nSpotLightCount);

			//////////////////////////////////////////////////////////////////////////

			// ----------------------------------------------------------------------------
			// 保存摄像机路径信息
			CToolRecord::GetInst()->Save(fp);

			fclose(fp);

			this->SaveAsAudio();

			MessageBox(NULL,"地图已分割完成，如果要看分割后的地图，请重新打开该地图","提示",MB_OK);
			MessageBox(NULL,"打开分割地图后，请重新生成lightmap，否则调出主角走到会爆掉","提示",MB_OK);

			delete pTerrainNew;
			pTerrainNew = NULL;
		}
		else
		{
			MessageBox(NULL,"写入文件错误,有可能文件被锁住了","错误",MB_OK);
		}
	}

	//------------------------------------------------------------------------------
	void
	CLocalSplitMapSaver::SaveSpotLightInfo(FILE * fp, const int nSpotLightCnt)
	{
		int nSceneSpotLightCnt = 0;
		int nGridCnt = terrain->GetGridCount();
		map<int, int>::iterator iter;

		fwrite( &nSpotLightCnt, sizeof(int), 1, fp);

		map<int, int>::iterator revertgridIter = this->mapRevertSplitMapGridIndex.begin();
		map<int, int>::iterator revertgridend  = this->mapRevertSplitMapGridIndex.end();
		int nGridIndex = 0;
		for (revertgridIter; revertgridIter != revertgridend; ++revertgridIter )
		{
			nGridIndex = revertgridIter->first;
			SGrid * pGrid = &terrain->GetGrid(nGridIndex);
			if ( !pGrid->vecCenterObjects.empty())
			{
				size_t objectCnt = pGrid->vecCenterObjects.size();
				for ( size_t j = 0; j < objectCnt; ++j)
				{
					CTObjectPtr p = pGrid->vecCenterObjects[j];

					string strEffectName;

					// effect bound?
					if ( p->GetModelGroup()->GetIsEffectBased() && p->GetModelGroup()->GetEffectName(strEffectName) )
					{
						//---
						if (p->GetModelGroup()->m_SpotLight.m_bIAmbSceneSpotLight)
						{
							if ( nSceneSpotLightCnt < nSpotLightCnt )
							{
								int nLightGridIndex = 0;

								///变换新的地形格子索引
								iter = this->mapRevertSplitMapGridIndex.find(nGridIndex);
								if( iter != this->mapRevertSplitMapGridIndex.end() )
									nLightGridIndex = iter->second;
								else
								{
									MessageBox(NULL,"地图切割点光源，请注意","提示",MB_OK);
									nLightGridIndex = 0;
								}

								fwrite( &nLightGridIndex,			sizeof(nLightGridIndex), 1, fp);   ///格子索引
								fwrite( &j,							sizeof(size_t),			 1, fp);///第几个模型的索引
								fwrite( &p->GetModelGroup()->m_SpotLight, sizeof(SceneSpotLight),  1, fp);

								++nSceneSpotLightCnt;
							}
						}
					}
				}
			}
		}
	}

	//------------------------------------------------------------------------------
	void
	CLocalSplitMapSaver::SaveWaveSideInfo(FILE *pRead, FILE *pSave, const int nSaveMapGridCnt, const int nReadMapGridCnt)
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

	void CLocalSplitMapSaver::SaveWaveSideInfo( FILE* file, DWORD newGridCnt )
	{
		int nGridCount = terrain->GetGridCount();
		SGrid* grids = CMapEditApp::GetInst()->GetWater()->GetGrids();
		uint fourcc = 'WVSD';

		fwrite(&fourcc, sizeof(uint), 1, file);
		vector<byte> waveGrids;
		waveGrids.resize(newGridCnt);

		map<int, int>::iterator iter;
		int nOldGridIndex = 0;

		index_t size = waveGrids.size();
		for (index_t i = 0; i < size; ++i)
		{
			iter = this->mapSplitMapGridIndex.find(i);
			if( iter != this->mapSplitMapGridIndex.end() )
			{
				nOldGridIndex = iter->second;
				waveGrids[i]  = grids[nOldGridIndex].bSide ? 1 : 0;
			}
			else
			{
				char c[10];
				sprintf(c, "%d", i);
				string str = c;
				str = str + "格子切割保存浪花失败";

				MessageBox(NULL,str.c_str(),"提示",MB_OK);
			}
		}
		fwrite(&waveGrids.front(), sizeof(byte), waveGrids.size(), file);
	}
	//------------------------------------------------------------------------------
	void
	CLocalSplitMapSaver::SaveSplitMapGridMatIndexInfo(FILE * fp, DWORD newGridCnt)
	{
		map<int, int>::iterator beg, end = this->mapSplitMapGridIndex.end();

		int nGridIndex = 0;
		int8 nMatIndex = -1;
		for ( DWORD i = 0; i < newGridCnt; ++i )
		{
			beg = this->mapSplitMapGridIndex.find(i);
			if( beg != end )
			{
				nGridIndex = beg->second;
				SGrid &grid = terrain->GetGrid(nGridIndex);
				nMatIndex = grid.nMaterialIndex;
			}
			else
				nMatIndex = -1;

			fwrite( &nMatIndex, sizeof(nMatIndex), 1, fp);  
		}
	}

	//------------------------------------------------------------------------------
	void
	CLocalSplitMapSaver::SaveSplitMapExactBlockInfo( FILE * fp, DWORD newGridCnt )
	{
		int dwCnt = 0;
		///设置了精确阻挡的个数
		long lExactBlokGridCntPt = ftell(fp);
		fwrite(&dwCnt,sizeof(int),1,fp);

		///扩展后旧地图格子索引在新地图中的新索引映射 first new grid   index second old grid index
		map<int, int>::iterator beg, end = this->mapSplitMapGridIndex.end();

		int nGridIndex = 0;
		for ( DWORD i = 0; i < newGridCnt; ++i )
		{
			beg = this->mapSplitMapGridIndex.find(i);
			if( beg != end )
			{
				nGridIndex = beg->second;
				SGrid & grid = terrain->GetGrid(nGridIndex);
				if ( grid.m_bSetExactBlock )
				{
					fwrite(&i, sizeof(beg->first), 1, fp);
					fwrite(&grid.subGridTypes, sizeof(grid.subGridTypes), 1, fp); // size

					++dwCnt;
				}
			}
		}

		long l = ftell(fp);
		fseek(fp, lExactBlokGridCntPt, SEEK_SET);
		fwrite(&dwCnt, sizeof(dwCnt), 1, fp); // size
		fseek(fp, l, SEEK_SET);
	}

	//------------------------------------------------------------------------------
	void
	CLocalSplitMapSaver::SaveOffAnimateModelIndex(FILE * fp)
	{
		int dwCnt = 0;
		///设置了要保存信息的总个数的个数
		long lOffAniModelIndexCntPt = ftell(fp);
		fwrite(&dwCnt,sizeof(int),1,fp);

		int nGridCount = terrain->GetGridCount();
		for ( int i = 0; i< nGridCount; ++i )
		{
			SGrid * pGrid = &terrain->GetGrid(i);
			if ( !pGrid->vecCenterObjects.empty())
			{
				size_t objectCnt = pGrid->vecCenterObjects.size();
				for ( size_t j = 0; j < objectCnt; ++j)
				{
					CTObjectPtr p = pGrid->vecCenterObjects[j];

					string strAgpName = p->GetModelGroup()->GetAnimationGroupName();
					bool bPlay = p->GetModelGroup()->GetIsPlayAnimation();

					if ( strAgpName != "" && !bPlay )
					{
						fwrite( &i, sizeof(int), 1, fp);   ///格子索引
						fwrite( &j, sizeof(size_t), 1, fp);///第几个模型的索引

						++dwCnt;
					}
				}
			}
		}

		long l = ftell(fp);
		fseek(fp, lOffAniModelIndexCntPt, SEEK_SET);
		fwrite(&dwCnt, sizeof(dwCnt), 1, fp); // size
		fseek(fp, l, SEEK_SET);
	}

	 void CLocalSplitMapSaver::SaveAsAudio()
	 {
		 string strSaveAudioFile = m_strMapPathName + ".audio";

		 //save .audio file 
		 COperator *pOperator = CMapEditApp::GetInst()->GetOperator();

		 locale loc = locale::global(locale(""));
		 FILE * fp = NULL;
		 fp = fopen(strSaveAudioFile.c_str(),"wb");

		 if (!fp)
		 {
			 string log = strSaveAudioFile + "无法保存";
			 MessageBox(NULL,log.c_str(),"提示",MB_OK);
			 return;
		 }
		 else
		 {
			 DWORD version = AUDIO_PLAYTIME_INTERVAL;
			 fwrite(&version, sizeof(version), 1, fp);

			 //////////////////////////////////////////////////////////////////////////
			 ///save cue name
			 int nNameLen = 0;
			 int nGridCnt   = terrain->GetGridCount(), nSaveGridIndex = 0;
			 string strCueName = "";
			 size_t dwLen = 0;
			 set<int> nGrindIndexs;
			 map<int, int>::iterator splitRevertGriditer, splitGriditer;

			 for( int i = 0; i < nGridCnt; ++i )
			 {
				 SGrid * pGrid = &terrain->GetGrid(i);
				 if ( !pGrid->vecCenterObjects.empty() )
				 {
					 for ( vector< CTObjectPtr >::iterator iter = pGrid->vecCenterObjects.begin(); iter != pGrid->vecCenterObjects.end(); ++iter)
					 {
						 CTObjectPtr p = (*iter);

						 CEditModelGroup *pModelGroup = p->GetModelGroup();

						 if( pModelGroup->m_pSceneAudio )
						 {
							 splitRevertGriditer = this->mapRevertSplitMapGridIndex.find(i);
							 if( splitRevertGriditer != this->mapRevertSplitMapGridIndex.end() )
							 {
								 nSaveGridIndex = splitRevertGriditer->second;
								 nGrindIndexs.insert(i);
							 }
						 }
					 }
				 }
			 }

			 size_t size = nGrindIndexs.size();
			 fwrite(&size,sizeof(size),1,fp);

			 set<int>::iterator end = nGrindIndexs.end();
			 map<int, int>::iterator begsplit, endsplit = this->mapRevertSplitMapGridIndex.end();

			 for( set<int>::iterator beg = nGrindIndexs.begin(); beg != end; ++beg )
			 {
				 int nGridIndex = *beg;
				 int nSaveGridIndex = 0;

				 begsplit = this->mapRevertSplitMapGridIndex.find(nGridIndex);
				 if( begsplit != endsplit )
					 nSaveGridIndex = begsplit->second;
				 else
					 nSaveGridIndex = nGridIndex;

				 fwrite(&nSaveGridIndex,sizeof(nSaveGridIndex),1,fp);	

				 SGrid * pGrid = &terrain->GetGrid(nGridIndex);
				 if ( !pGrid->vecCenterObjects.empty() )
				 {
					 int8 nAudioObjectIndex = 0; 
					 DWORD dw = 0;
					 long lCntOffset = ftell(fp);
					 fwrite(&dw,sizeof(DWORD),1,fp);

					 for ( vector< CTObjectPtr >::iterator iter = pGrid->vecCenterObjects.begin(); iter != pGrid->vecCenterObjects.end(); ++iter)
					 {
						 CTObjectPtr p = (*iter);

						 CEditModelGroup *pModelGroup = p->GetModelGroup();

						 strCueName = pModelGroup->GetSceneCueName();
						 if( pModelGroup->m_pSceneAudio && !strCueName.empty() )
						 {
							 //////////////////////////////////////////////////////////////////////////
							 ///play time mechanism

							 ///一段时间的播放长度最小最大值
							 DWORD time = pModelGroup->GetPlayTimeLenMin();
							 fwrite(&time,sizeof(time),1,fp);

							 time = pModelGroup->GetPlayTimeLenMax();
							 fwrite(&time,sizeof(time),1,fp);

							 ///二段播放间隔长度最小最大值
							 time = pModelGroup->GetPlayIntervalMin();
							 fwrite(&time,sizeof(time),1,fp);

							 time = pModelGroup->GetPlayIntervalMax();
							 fwrite(&time,sizeof(time),1,fp);

							 //////////////////////////////////////////////////////////////////////////

							 ///cue name
							 dwLen = strCueName.size();
							 fwrite(&dwLen,sizeof(int),1,fp);
							 fwrite(strCueName.c_str(),dwLen,1,fp);

							 dw++;

							 string strItemName = pModelGroup->GetItemName();

							 dwLen = strItemName.size();
							 fwrite(&dwLen,sizeof(int),1,fp);
							 fwrite(strItemName.c_str(),dwLen,1,fp);

							 //////////////////////////////////////////////////////////////////////////
							 ///offset
							 CVector3f vUserOffsetEx = pModelGroup->GetUserOffset();
							 fwrite(&vUserOffsetEx,sizeof(CVector3f),1,fp);
							 //////////////////////////////////////////////////////////////////////////

							 int n = p->GetCenterGridIndex();
							 int nSaveModelIndex = 0;

							 begsplit = this->mapRevertSplitMapGridIndex.find(nGridIndex);
							 if( begsplit != endsplit )
								 nSaveModelIndex = begsplit->second;
							 else
								 nSaveModelIndex = n;

							 fwrite(&nSaveModelIndex,sizeof(nSaveModelIndex),1,fp);

							 //y 轴
							 float fRotation = pModelGroup->GetRotation();
							 fwrite(&fRotation,sizeof(float),1,fp);

							 int nRotationCnt = pModelGroup->GetRotationCount();
							 fwrite(&nRotationCnt,sizeof(int),1,fp);

							 //////////////////////////////////////////////////////////////////////////
							 //新版本新增的绕x，z轴渲染，物体缩放信息
							 //z 轴
							 float fPitch = pModelGroup->GetPitch();
							 fwrite(&fPitch,sizeof(float),1,fp);

							 int nPitchCnt = pModelGroup->GetPitchCount();
							 fwrite(&nPitchCnt,sizeof(int),1,fp);

							 //x 轴
							 float fYaw = pModelGroup->GetYaw();
							 fwrite(&fYaw,sizeof(float),1,fp);

							 int nYawCnt = pModelGroup->GetYawCount();
							 fwrite(&nYawCnt,sizeof(int),1,fp);

							 //缩放
							 float xScale = pModelGroup->GetScaleX();
							 fwrite(&xScale,sizeof(float),1,fp);
							 float yScale = pModelGroup->GetScaleY();
							 fwrite(&yScale,sizeof(float),1,fp);
							 float zScale = pModelGroup->GetScaleZ();
							 fwrite(&zScale,sizeof(float),1,fp);
							 //////////////////////////////////////////////////////////////////////////

							 BYTE byRenderStyle = pModelGroup->GetRenderStyle();
							 fwrite(&byRenderStyle,sizeof(BYTE),1,fp);

							 int nAtten = pModelGroup->GetAtten();
							 fwrite(&nAtten,sizeof(int),1,fp);

							 bool bShadow = p->IsCastShadow();
							 fwrite(&bShadow,sizeof(bool),1,fp);

							 ///写是否接收阴影信息
							 bool bReceiveShadow = pModelGroup->IsReceiveShadow();
							 fwrite(&bReceiveShadow,sizeof(bool),1,fp);

							 ///是否消隐,登录模型应该不需要消隐的吧
							 bool bTrans = false;
							 fwrite(&bTrans,sizeof(bTrans),1,fp);

							 ///mirror type
							 int nMirrorType = pModelGroup->GetMirrorType();
							 fwrite(&nMirrorType, sizeof(nMirrorType), 1, fp);

							 ///nAffectRadio
							 int nRatio = pModelGroup->GetAffectRatio();
							 fwrite( &nRatio, sizeof(int), 1, fp);
						 }

					 }

					 long lNow = ftell(fp);
					 fseek(fp,lCntOffset,SEEK_SET);
					 fwrite(&dw,sizeof(dw),1,fp);
					 fseek(fp,lNow,SEEK_SET);
				 }
				 else
				 {
					 int16 dw = 0;
					 fwrite(&dw,sizeof(dw),1,fp);
				 }
			 }

			 fclose(fp);
			 locale::global(loc);//恢复全局locale	

			 MessageBox(NULL,"场景音源保存完毕","提示",MB_OK);
		}
	 }

	void CLocalSplitMapSaver::SaveSplitMapBlockInfo( const int nSaveMapGridCnt )
	{
		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
		if( terrain->IsWater() )
		{
			MessageBox(NULL,"将当前状态改成地形状态，而非水面层状态，否则不能保存地图阻挡信息","提示",MB_OK);
			return ;
		}

		CTerrainMesh* pTerrain = NULL;
		pTerrain =CMapEditApp::GetInst()->GetTerrain();
		if( pTerrain == NULL )
			MessageBox(NULL,"地形为空","提示",MB_OK);

		string strFileName = m_strMapPathName;
		locale loc;
		use_facet<ctype<TCHAR> >( loc ).tolower
			( &strFileName[0], &strFileName[strFileName.length()] );

		size_t npos = strFileName.rfind(".");
		if( npos != -1 )
			strFileName = strFileName.substr(0, npos);

		strFileName += ".block";

		FILE * fp = NULL;
		fp = fopen(strFileName.c_str(),"wb");

		if (!fp)
		{
			string log = strFileName + "无法保存";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}

		//////////////////////////////////////////////////////////////////////////
		///version
		DWORD dwVersion = BLOCK_VERSION;
		fwrite(&dwVersion, sizeof(dwVersion), 1, fp);

		int	nBlockType = 0, nGridIndex = 0;
		map<int, int>::iterator iter;
		for ( int i  = 0; i < nSaveMapGridCnt; ++i )
		{
			iter = this->mapSplitMapGridIndex.find(i);
			if ( iter != this->mapSplitMapGridIndex.end() )
			{
				nGridIndex = iter->second;
				SGrid &grid = terrain->GetGrid(i);
				nBlockType = grid.nBlockType;
			}

			fwrite(&nBlockType, sizeof(nBlockType), 1, fp);
		}
		//////////////////////////////////////////////////////////////////////////
		fclose(fp);
	}

	void CLocalSplitMapSaver::SavePointLightInfo( FILE * fp, const int SceneLightCount )
	{
		int     tLength = 11;
		int		nSceneLightCnt = 0;///用于判断保存的场景点光源数量是否和SceneLightCount一致的计数
		int     nGridCnt = terrain->GetGridCount();

		fwrite( &tLength, sizeof(int), 1, fp);
		fwrite( "SceneLight", 11, 1, fp);
		fwrite( &SceneLightCount, sizeof(int), 1, fp);
		map<int, int>::iterator griditer;

		map<int, int>::iterator revertgridIter, end = this->mapRevertSplitMapGridIndex.end();
		int nGridIndex = 0;
		for (revertgridIter = this->mapRevertSplitMapGridIndex.begin(); revertgridIter != end; ++revertgridIter )
		{
			nGridIndex = revertgridIter->first;
			SGrid * pGrid = &terrain->GetGrid(nGridIndex);
			if ( !pGrid->vecCenterObjects.empty())
			{
				int size = 0;
				size = (int)(pGrid->vecCenterObjects.size());
				for ( int iter = 0; iter < size; ++iter)
				{
					CTObjectPtr p = pGrid->vecCenterObjects[iter];

					string strEffectName;

					// effect bound?
					if ( p->GetModelGroup()->GetIsEffectBased() && p->GetModelGroup()->GetEffectName(strEffectName) )
					{
						//---
						if (p->GetModelGroup()->m_SL.m_bIAmbScenePointLight)
						{
							if ( nSceneLightCnt < SceneLightCount )
							{
								int nLightGridIndex = 0;
								
								///变换新的地形格子索引
								griditer = this->mapRevertSplitMapGridIndex.find(nGridIndex);
								if( griditer != this->mapRevertSplitMapGridIndex.end() )
									nLightGridIndex = griditer->second;
								else
									MessageBox(NULL,"地图切割点光源，请注意","提示",MB_OK);

								fwrite( &nLightGridIndex, sizeof(nLightGridIndex), 1, fp);
								fwrite( &iter, sizeof(int), 1, fp);
								fwrite( &p->GetModelGroup()->m_SL, sizeof(SceneLight), 1, fp);
								++nSceneLightCnt;
							}
						}
					}
				}
			}
		}
	}

	void CLocalSplitMapSaver::SetTerrainMesh( CTerrainMesh* t )
	{
		Ast(NULL != t);
		this->terrain = t;
		m_strDefaultTerrainTexName = terrain->GetTerrainDefaultTextureName();
	}
}