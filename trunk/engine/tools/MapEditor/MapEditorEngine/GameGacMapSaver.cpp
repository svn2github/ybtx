//------------------------------------------------------------------------------
//  CGameGacMapSaver.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "GameGacMapSaver.h"
#include "GraphicCommon.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "CMapEffect.h"
#include "WaterOperator.h"
#include "DepathGroupModelInfo.h"
#include "TerrainMeshObject.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "Engine.h"
#include "Render.h"
#include "River.h"
#include "SceneRegionMgr.h"
#include "TerrainMeshRegion.h"
#include "CMapEditLoginObj.h"
#include "CEditTerrainMgr.h"
#include <bitset>
#include "Console.h"
#include "CWindowTarget.h"
#include "CGraphic.h"
#include "ToolRecord.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "SceneAudioMgr.h"
#include "LocalVersion.h"
#include "CRegionAmbientEffectMgr.h"
#include "ToolHideTileRegionInView.h"
#include "TraceHelper.h"

#define CLIENT_CHUNK_COUNT (18) //--- 17

enum
{
	AUDIO_ORIGIN,
	AUDIO_PLAY_LEN,
};

enum
{
	LOGIN_ORIGIN,
};

struct CRegionMI
{
	float   cX;
	float   cY;
	float   cZ;
	float   fxRotation;//x
	float   fyRotation;//y
	float   fzRotation;//z
	int16	fScaleX;
	int16	fScaleY;
	int16	fScaleZ;
	uint16  SetIndex;
	uint8   GridIndex;
	uint8	nFlag;
	uint8	r;
	uint8	g;
	uint8	b;
	bool	bEffectByConfigur;
	bool	bCameraCollision;
};

namespace sqr
{

///change "\\" to "/"
void ChangeRightSlashToLeft(string& str)
{
	size_t npos = str.find("\\");
	while (npos != -1)
	{
		str = str.replace(npos, 1, "/");

		npos = str.find("\\");
	}
}

//------------------------------------------------------------------------------
CGameGacMapSaver::CGameGacMapSaver()
		: terrain(NULL)
		, progress(NULL)
{
}

//------------------------------------------------------------------------------
bool
CGameGacMapSaver::SaveAsClient(const string& path)
{
// 	///如果当前状态是烘培，则提示关掉，然后再生成客户端
// 	bool bLightMap = CSceneEnvMgr::GetInst()->GetIsLightMapShadow();
//  	if( bLightMap )
//  	{
//  		MessageBox(NULL,"请将渲染状态切换到实时状态，而非烘培状态","提示",MB_OK);
//  		return false;
//  	}

	MessageBox(NULL,"导客户端前，务必生成最新的烘培文件","提示",MB_OK);

	CSceneAudioMgr::GetInst()->CalTerrainMaterialIndex();

	Ast(NULL != this->terrain);
	Ast(NULL != this->progress);

	progress->SetProgress( 0.0f );

	locale loc = locale::global(locale(""));
	locale::global(locale(""));

	SaveResSetFile(path);
	SaveAmpSetFile(path);
	SaveRegionFile(path);

	locale::global(loc);//恢复全局locale	

	return true;
}

//------------------------------------------------------------------------------
void 
CGameGacMapSaver::SaveResSetFile(const std::string &path)
{
	CRenderDevice * pRender = CRenderDevice::GetInst();

	// create the res set file
	string strResSetFileName = "map\\" + path + "_c.ars";
	int nRegionCnt = terrain->GetRegionCount();
	float Total = float(nRegionCnt + 2);

	FILE * fpResSet = fopen(strResSetFileName.c_str(),"wb");
	if ( !fpResSet )
	{
		string log = strResSetFileName + "无法保存";
		MessageBox(NULL,log.c_str(),"错误",MB_OK);
		return;
	}
	DWORD dwMapTextureCnt = pRender->m_TerrainTexIndexs.size();

	map< pair<string,int>, int> mapModels;
	map<string ,int> mapAudios;
	vector< vector<string> > vvModelNames;
	vector<string> veCEditModelAnimationGroupNames;///apg list
	vector<string> veCEditModelAnimationSkeNames;///ske list
	vector<string> veCEditModelEffectNames;
	vector<string> veCEditModelEffectFileNames;
	vector<string> vecCueNames;
	vector<BYTE> veCEditModelShadow;
	vector<BYTE> veCEditModelTranslucent;
	vector<BYTE> veCEditModelFirstShadow;
	vector<BYTE> veCEditModelShadowZTest;
	vector<BYTE> veCEditModelSelfZTest;
	vector<BYTE> veCEditModelRenderStyle;
	// version
	int nVersion = CLIENT_RES_VERSION;
	fwrite(&nVersion,sizeof(int),1,fpResSet);

	static const basic_string <TCHAR>::size_type npos = -1;

	// texture streams

	long lTextureCntPt = ftell(fpResSet);
	DWORD dwPad = 0;
	fwrite(&dwPad,sizeof(DWORD),1,fpResSet); // texture cnt

	this->MakeAtlasTexture(path);
	//////////////////////////////////////////////////////////////////////////

	DWORD dwWaterTextureCnt = 0;

	CTerrainMesh * pWater = terrain->GetWater();

	size_t nRiverCnt = pWater->GetRiverCount();
	for (size_t i = 0; i < nRiverCnt; ++i )
	{
		CRiver * pRiver = pWater->GetRiver(i);
		dwWaterTextureCnt += pRiver->SaveTexture(fpResSet, dwWaterTextureCnt);
	}

	dwMapTextureCnt += dwWaterTextureCnt;
	//////////////////////////////////////////////////////////////////////////

	long lTextureEnd = ftell(fpResSet);
	fseek(fpResSet,lTextureCntPt,SEEK_SET);
	fwrite(&dwWaterTextureCnt, sizeof(DWORD), 1, fpResSet);
	fseek(fpResSet,lTextureEnd,SEEK_SET);

	//////////////////////////////////////////////////////////////////////////

	// model streams
	///找出复合图素
	terrain->DepathMapGroupModel();
	map<string, int> depathModelIndexs;
	map<string, int>::iterator depathModelIndexIter;
	// generate the model map

	int j = 0;
	int nAudioIndex = 0;
	string str = "";
	int nGridCnt = terrain->GetGridCount();
	for (int i = 0; i < nGridCnt; ++i )
	{
		SGrid * pGrid = &terrain->GetGrid(i);

		DepathModelInfoMap::iterator iter = terrain->m_DepathModelMap.find(i);
		if ( iter != terrain->m_DepathModelMap.end() )
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

				if( pModelGroup->m_bReginEffect )
					continue;

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
					veCEditModelTranslucent.push_back(false ? 1 : 0);
					// first shadow
					veCEditModelFirstShadow.push_back(p->GetModelGroup()->GetShadowSecondPass() ? 0 : 1);
					// shadow z test
					veCEditModelShadowZTest.push_back(0);
					// self z test
					veCEditModelSelfZTest.push_back(0);

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
	fwrite(&j,sizeof(int),1,fpResSet);

	for (size_t i = 0; i < vvModelNames.size(); ++i )
	{
		const vector<string>& vec = vvModelNames[i];

		BYTE byIndividualCnt = 0;

		for ( size_t k = 0; k < vec.size(); ++k )
		{
			// arp
			if ( !vec[k].empty() )
				++byIndividualCnt;
		}

		//////////////////////////////////////////////////////////////////////////
		// agp
		string strAgpName = veCEditModelAnimationGroupNames[i];
		string strSkeName = veCEditModelAnimationSkeNames[i];
		if ( !strAgpName.empty() && !strSkeName.empty() ) 
		{
			//agp
			++byIndividualCnt;
			//ske
			++byIndividualCnt;
		}
		//////////////////////////////////////////////////////////////////////////

		// are
		if ( !veCEditModelEffectFileNames[i].empty() ) 
			++byIndividualCnt;

		BYTE by = veCEditModelRenderStyle[i];

		fwrite(&by,sizeof(BYTE),1,fpResSet);

		// write cnt 模型+动作+特效个数总和，根据标志进行判断是模型，还是动作或者是特效
		fwrite(&byIndividualCnt,sizeof(BYTE),1,fpResSet);

		// write arp
		for ( size_t j2 = 0; j2 < vec.size(); ++j2 )
		{
			if ( !vec[j2].empty() )
			{
				fwrite("ARP",4,1,fpResSet);

				string str = vec[j2];

				int n = str.length();
				fwrite(&n,sizeof(int),1,fpResSet);
				fwrite(str.c_str(),n,1,fpResSet);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// write ara(agp)
		if ( !strAgpName.empty() && !strSkeName.empty() ) 
		{
			fwrite("AGP",4,1,fpResSet);
			int n = strAgpName.length();
			fwrite(&n,sizeof(int),1,fpResSet);
			fwrite(strAgpName.c_str(),n,1,fpResSet);

			fwrite("SKE",4,1,fpResSet);
			n = strSkeName.length();
			fwrite(&n,sizeof(int),1,fpResSet);
			fwrite(strSkeName.c_str(),n,1,fpResSet);
		}
		//////////////////////////////////////////////////////////////////////////

		// write are
		if ( !veCEditModelEffectFileNames[i].empty() )
		{
			fwrite("ARE",4,1,fpResSet);
			string str = veCEditModelEffectFileNames[i];

			int n = str.length();
			fwrite(&n,sizeof(int),1,fpResSet);
			fwrite(str.c_str(),n,1,fpResSet);

			str = veCEditModelEffectNames[i];
			n = str.length();
			fwrite(&n,sizeof(int),1,fpResSet);
			fwrite(str.c_str(),n,1,fpResSet);
		}
	}

	///audio info
	if( nAudioIndex != mapAudios.size() )
	{
		MessageBox(NULL,"客户端音源信息有问题","提示",MB_OK);
	}

	string strAudio = "";
	int nLen = 0;
	fwrite(&nAudioIndex,sizeof(nAudioIndex),1,fpResSet);

	size_t size = vecCueNames.size();
	for ( size_t i = 0; i < size; ++i )
	{
		strAudio = vecCueNames[i];

		nLen = strAudio.length();
		fwrite(&nLen, sizeof(nLen),  1, fpResSet);
		fwrite(strAudio.c_str(),nLen,1,fpResSet);
	}

	fclose(fpResSet);

	progress->SetProgress( 1.0f/Total );
}

//------------------------------------------------------------------------------
void 
CGameGacMapSaver::SaveAmpSetFile(const std::string &path)
{
	string strResSetFileName = "map\\" + path + "_c.ars";
	int nRegionCnt = terrain->GetRegionCount();
	float Total = float(nRegionCnt + 2);

	// map file
	string strClientMapFileName = "common\\map\\" + path + ".amp";

	FILE * fp = NULL;
	fp = fopen(strClientMapFileName.c_str(),"wb");

	if ( fp )
	{
		// header
		fwrite("AMAP",4,1,fp);

		// version
		int nVersion = CLIENT_AMP_VERSION;
		fwrite(&nVersion,sizeof(int),1,fp);

		// desc
		TCHAR s[64] = "MAP";
		fwrite(s,64,1,fp);

		// width depth in region
		WORD wWidth = terrain->GetWidthInRegion();
		WORD wDepth = terrain->GetDepthInRegion();
		fwrite(&wWidth,sizeof(WORD),1,fp);
		fwrite(&wDepth,sizeof(WORD),1,fp);

		// REGION_SPACE
		BYTE bRegionSpace = (BYTE)REGION_SPACE;
		fwrite(&bRegionSpace,sizeof(BYTE),1,fp);

		// GRID_SPACE
		BYTE bGridSpace = (BYTE)GRID_SPACE;
		fwrite(&bGridSpace,sizeof(BYTE),1,fp);

		// Tileset
		strcpy(s,strResSetFileName.c_str());
		//strcpy(s,GetAlphabetic(s));
		fwrite(s,64,1,fp);

		// cave？ no shadow
		bool bCave = false;

		//////////////////////////////////////////////////////////////////////////
		static const basic_string <TCHAR>::size_type npos = -1;

		for (size_t i = 0; i < 4; ++i)
		{
			// light
			DWORD dwAmbientColor = CSceneEnvMgr::GetInst()->GetSceneAmbientColor();
			fwrite(&dwAmbientColor,sizeof(DWORD),1,fp);

			DWORD dwShadowColor = CSceneEnvMgr::GetInst()->GetShadowColor();
			fwrite(&dwShadowColor,sizeof(DWORD),1,fp);

			DWORD dwSunColor = CSceneEnvMgr::GetInst()->GetSceneAmbientColor();
			fwrite(&dwSunColor,sizeof(DWORD),1,fp);

			CVector3f vSunDir = CSceneEnvMgr::GetInst()->GetDirDir();
			fwrite(&vSunDir,sizeof(CVector3f),1,fp);

			DWORD dwPlayerSunColor = CSceneEnvMgr::GetInst()->GetPlayerDirColor();
			fwrite(&dwPlayerSunColor,sizeof(DWORD),1,fp);

			DWORD dwPlayerColor = CSceneEnvMgr::GetInst()->GetPlayerAmbientColor();
			fwrite(&dwPlayerColor,sizeof(DWORD),1,fp);

			DWORD dwPlayerLightColor = 0;
			fwrite(&dwPlayerLightColor,sizeof(DWORD),1,fp);

			float fPlayerLightRatio = 0.0f;
			fwrite(&fPlayerLightRatio,sizeof(float),1,fp);

			float fPlayerLightRange = 0.0f;
			fwrite(&fPlayerLightRange,sizeof(float),1,fp);

			// fog
			DWORD dwFogColor = CSceneEnvMgr::GetInst()->GetFogColor();
			fwrite(&dwFogColor,sizeof(DWORD),1,fp);

			float fFogStart = CSceneEnvMgr::GetInst()->GetFogStart();
			fwrite(&fFogStart,sizeof(float),1,fp);

			float fFogEnd = CSceneEnvMgr::GetInst()->GetFogEnd();
			fwrite(&fFogEnd,sizeof(float),1,fp);

			string strTextureSky = CSceneEnvMgr::GetInst()->GetSkyTextureName();

			if ( strTextureSky.find(".tga") != string::npos || strTextureSky.find(".TGA") != string::npos )
			{
				strTextureSky = strTextureSky.substr(0, strTextureSky.rfind(".tga")) + ".dds";
			}

			if ( strTextureSky.find("HEI.TGA") != npos || strTextureSky.find("hei.tga") != npos || strTextureSky.find("MapEditor") != npos || strTextureSky.find("mapeditor") != npos || strTextureSky.find("MAPEDITOR") != npos )
				strTextureSky = "";

			strcpy(s,strTextureSky.c_str());

			//strcpy(s,GetAlphabetic(s));

			fwrite(s,64,1,fp);
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

		strcpy(s,strTextureSkyBack.c_str());
		//strcpy(s,GetAlphabetic(s));
		fwrite(s,64,1,fp);

		//////////////////////////////////////////////////////////////////////////

		// wave info
		CWaterOperator *pWaterOpertor = CMapEditApp::GetInst()->GetWaterOperator();
		int nWavePerGrid = pWaterOpertor->GetWaveDensity();
		fwrite(&nWavePerGrid,sizeof(int),1,fp);

		int nWaveSize = pWaterOpertor->GetWaveSize();
		fwrite(&nWaveSize,sizeof(int),1,fp);

		int nWaveOff = pWaterOpertor->GetWaveOff();
		fwrite(&nWaveOff,sizeof(int),1,fp);

		float fWaveSpeed = pWaterOpertor->GetWaveSpeedForClient();
		fwrite(&fWaveSpeed,sizeof(float),1,fp);

		int nWaveLife = pWaterOpertor->GetWaveLifeForClient();
		fwrite(&nWaveLife,sizeof(int),1,fp);

		TCHAR ss[256];

		long ttt = ftell(fp);

		strcpy(ss,"tile/水/wave.dds");
		//strcpy(ss,GetAlphabetic(ss));
		int n = strlen(ss);
		fwrite(&n,sizeof(int),1,fp);
		fwrite(ss,n,1,fp);

		strcpy(ss,"fx/setting/dimple.efx");
		//strcpy(ss,GetAlphabetic(ss));
		n = strlen(ss);
		fwrite(&n,sizeof(int),1,fp);
		fwrite(ss,n,1,fp);

		strcpy(ss,"wave/move");
		//strcpy(ss,GetAlphabetic(ss));
		n = strlen(ss);
		fwrite(&n,sizeof(int),1,fp);
		fwrite(ss,n,1,fp);

		strcpy(ss,"fx/setting/dimple.efx");
		//strcpy(ss,GetAlphabetic(ss));
		n = strlen(ss);
		fwrite(&n,sizeof(int),1,fp);
		fwrite(ss,n,1,fp);

		strcpy(ss,"wave/stand");
		/*		strcpy(ss,GetAlphabetic(ss));*/
		n = strlen(ss);
		fwrite(&n,sizeof(int),1,fp);
		fwrite(ss,n,1,fp);

		//////////////////////////////////////////////////////////////////////////

		// background music
		string str = CSceneEnvMgr::GetInst()->GetBackGroundMusic();
		/*		str = GetAlphabetic(str.c_str());*/
		DWORD dwFileNameLen = str.length();
		fwrite(&dwFileNameLen,sizeof(DWORD),1,fp);
		fwrite(str.c_str(),dwFileNameLen,1,fp);

		int nMusicInterval = 0;
		fwrite(&nMusicInterval,sizeof(int),1,fp);

		//////////////////////////////////////////////////////////////////////////

		// ambient fx info
		CMapEffect::GetInst()->SaveAmbientFXInfoAsClient(fp);

		//////////////////////////////////////////////////////////////////////////

		float fPitch = STDPITCH;
		fwrite(&fPitch,sizeof(float),1,fp);

		float fYaw = STDYAW;
		fwrite(&fYaw,sizeof(float),1,fp);

		float fFov = STDFOV;
		fwrite(&fFov,sizeof(float),1,fp);

		float fDistance = STDDISTANCE;
		fwrite(&fDistance,sizeof(float),1,fp);

		///背景图移动速度信息
		bool bsetbackpic = CSceneEnvMgr::GetInst()->GetISBackPicMove();
		fwrite(&bsetbackpic,sizeof(bool),1,fp);
		if (bsetbackpic)
		{
			float move = 0.0f;

			move = CSceneEnvMgr::GetInst()->GetBackPicLeftMoveOffset() * 0.00001f;
			fwrite(&move,sizeof(float),1,fp);

			move = CSceneEnvMgr::GetInst()->GetBackPicRightMoveOffset() * 0.00001f;
			fwrite(&move,sizeof(float),1,fp);

			move = CSceneEnvMgr::GetInst()->GetBackPicTopMoveOffset() * 0.00001f;
			fwrite(&move,sizeof(float),1,fp);

			move = CSceneEnvMgr::GetInst()->GetBackPicBottonMoveOffset() * 0.00001f;
			fwrite(&move,sizeof(float),1,fp);

		}

		//全屏泛光
		CSceneEnvMgr::GetInst()->SaveGaussParamsInfo(fp);

		//////////////////////////////////////////////////////////////////////////
		//read specular parameter
		DWORD dwColor = 0; 
		float fPower = 0.0f;

		dwColor = CSceneEnvMgr::GetInst()->GetSceneSpecurlarColor();
		fwrite(&dwColor,sizeof(dwColor),1,fp);

		dwColor = CSceneEnvMgr::GetInst()->GetPlayerSpecurlarColor();
		fwrite(&dwColor,sizeof(dwColor),1,fp);

		fPower = CSceneEnvMgr::GetInst()->GetSpecularReflectionPower(0);
		fwrite(&fPower,sizeof(fPower),1,fp);

		fPower = CSceneEnvMgr::GetInst()->GetSpecularReflectionPower(1);
		fwrite(&fPower,sizeof(fPower),1,fp);
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		map<string, string> rgnEffectFileNameList;
		map<string, int> rgnEffectNameIndexList;
		CMapRegionAmbientEffectMgr::GetInst()->SaveRgnAmbientFXInfoAsClient(fp, rgnEffectFileNameList, rgnEffectNameIndexList);

		///四分之一区域光保存
		size_t sceneSize = CSceneRegionMgr::GetInst()->m_mapRegion.size();
		fwrite(&sceneSize,sizeof(sceneSize),1,fp);

		string strMusicName = "";
		uint32 uLen			= 0;
		map<string, SceneRegion*>::iterator iter = CSceneRegionMgr::GetInst()->m_mapRegion.begin();
		map<string, SceneRegion*>::iterator eiter = CSceneRegionMgr::GetInst()->m_mapRegion.end();
		for ( iter; iter != eiter; ++iter )
		{
			SceneRegion *sceneRegion = NULL;
			sceneRegion = iter->second;
			if ( sceneRegion )
			{
				///scene
				fwrite(&sceneRegion->lightParam.globalAmbient.R,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.globalAmbient.G,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.globalAmbient.B,sizeof(BYTE),1,fp);

				fwrite(&sceneRegion->lightParam.globalDiffuse.R,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.globalDiffuse.G,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.globalDiffuse.B,sizeof(BYTE),1,fp);

				fwrite(&sceneRegion->lightParam.globalSpecular.R,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.globalSpecular.G,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.globalSpecular.B,sizeof(BYTE),1,fp);


				fwrite(&sceneRegion->lightParam.globalShadow.R,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.globalShadow.G,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.globalShadow.B,sizeof(BYTE),1,fp);

				fwrite(&sceneRegion->lightParam.fogColor.R,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.fogColor.G,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.fogColor.B,sizeof(BYTE),1,fp);

				///player
				fwrite(&sceneRegion->lightParam.playerAmbient.R,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.playerAmbient.G,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.playerAmbient.B,sizeof(BYTE),1,fp);

				fwrite(&sceneRegion->lightParam.playerDiffuse.R,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.playerDiffuse.G,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.playerDiffuse.B,sizeof(BYTE),1,fp);

				fwrite(&sceneRegion->lightParam.playerSpecular.R,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.playerSpecular.G,sizeof(BYTE),1,fp);
				fwrite(&sceneRegion->lightParam.playerSpecular.B,sizeof(BYTE),1,fp);

				///fog param
				uint8 uFogMode	  = sceneRegion->GetSceneRgnFogType();
				float fFogStart   = sceneRegion->GetSceneRgnFogStart();
				float fFogEnd	  = sceneRegion->GetSceneRgnFogEnd();
				float fFogDensity = sceneRegion->GetSceneRgnFogDensity();

				fwrite(&uFogMode,	sizeof(uFogMode),1,fp);
				fwrite(&fFogStart,  sizeof(fFogStart),1,fp);
				fwrite(&fFogEnd,	sizeof(fFogEnd),1,fp);
				fwrite(&fFogDensity,sizeof(fFogDensity),1,fp);

				///music name
				strMusicName = sceneRegion->GetSceneRgnMusicName();
				uLen		 = strMusicName.length();

				strcpy(ss,strMusicName.c_str());
				uLen = strlen(ss)+1;

				fwrite( &uLen, sizeof(uLen),	  1, fp );
				fwrite( ss,	   sizeof(char)*uLen, 1, fp );

				//////////////////////////////////////////////////////////////////////////
				///rgn effect
				RegionAmbientFxMap regionEffectUnitMap = sceneRegion->GetRgnAmbinetFxList();
				size_t ambientFxSize = regionEffectUnitMap.size();
				fwrite(&ambientFxSize, sizeof(ambientFxSize), 1, fp);
				
				RegionAmbientFxMap::iterator beg = regionEffectUnitMap.begin();
				RegionAmbientFxMap::iterator end = regionEffectUnitMap.end();
				for ( beg; beg != end; ++beg )
				{
					sRegAmbientFxUnit *pTemp = beg->second;
		
					string strFxName = pTemp->strEffectName;
					int nIndex = rgnEffectNameIndexList[strFxName];
					fwrite(&nIndex, sizeof(nIndex), 1, fp);

					DWORD dwPlayMin = pTemp->dwPlayTimeMin;
					fwrite(&dwPlayMin, sizeof(dwPlayMin), 1, fp);

					DWORD dwPlayMax = pTemp->dwPlayTimeMax;
					fwrite(&dwPlayMax, sizeof(dwPlayMax), 1, fp);

					DWORD dwIntervalMin = pTemp->dwPlayIntervalMin;
					fwrite(&dwIntervalMin, sizeof(dwIntervalMin), 1, fp);

					DWORD dwIntervalMax = pTemp->dwPlayIntervalMax;
					fwrite(&dwIntervalMax, sizeof(dwIntervalMax), 1, fp);
				}
				//////////////////////////////////////////////////////////////////////////
			}
		}

		///重新再算一次区域是否要实时计算
		CSceneRegionMgr::GetInst()->SubRegionIsNeedRealCal();

		//////////////////////////////////////////////////////////////////////////

		CToolRecord::GetInst()->Save(fp);

		//////////////////////////////////////////////////////////////////////////
		///IsTileVisible
		CToolHideTileRegionInView::GetInst()->SaveClientTileRegionInfo(fp);
		//////////////////////////////////////////////////////////////////////////

		fclose(fp);

		this->progress->SetProgress( 2.0f/Total );
	}
	else
	{
		string log = strClientMapFileName + "无法保存";

		MessageBox(NULL,log.c_str(),"错误",MB_OK);
	}
}

//------------------------------------------------------------------------------
void 
CGameGacMapSaver::SaveRegionFile(const string& path)
{
	int nRegionCnt = terrain->GetRegionCount();
	float Total = float(nRegionCnt + 2);
	CRenderDevice* pRender = CRenderDevice::GetInst();
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
	terrain->CalTerrainRoadWeight();
	map<string, sqr::uint16> sceneRgnIndexs;
	map<string, SceneRegion*>::iterator iter = CSceneRegionMgr::GetInst()->m_mapRegion.begin();
	map<string, SceneRegion*>::iterator eiter = CSceneRegionMgr::GetInst()->m_mapRegion.end();
	sqr::uint16 nSceneRgnIndex = 0;
	for ( iter; iter != eiter; ++iter )
	{
		sceneRgnIndexs[iter->first] = nSceneRgnIndex;
		++nSceneRgnIndex;
	}
	// region file
	string strRgnFileName = "map\\" + path + "_c.arg";

	FILE* fp = fopen(strRgnFileName.c_str(),"wb");

	if ( fp )
	{
		// header
		fwrite("REGN",4,1,fp);

		// version
		DWORD dwVersion = CLIENT_VERSION;
		fwrite(&dwVersion,sizeof(DWORD),1,fp);

		///model version
		uint32 m_uModelVersion = DEFAULT_NEWMODELPRO;
		fwrite(&m_uModelVersion,sizeof(m_uModelVersion),1,fp);

		// chunk count
		DWORD dwDataChunkCnt = CLIENT_CHUNK_COUNT;
		fwrite(&dwDataChunkCnt,sizeof(DWORD),1,fp);

		///客户端文件ID
		DWORD dwGameClientID = terrain->GetGameGacMapVersion();
		++dwGameClientID;
		fwrite(&dwGameClientID,sizeof(DWORD),1,fp);

		LogConsole("GameClientID: " << dwGameClientID << "\n" );

		locale loc;

		// rgn offset table
		long lRgnOffsetTablePtr = ftell(fp);
		long lPad = 0;

		for (int i = 0; i < nRegionCnt; ++i )
		{
			fwrite(&lPad,sizeof(long),1,fp); // offset only
		}

		//////////////////////////////////////////////////////////////////////////
		for (int nRegIndex = 0; nRegIndex < nRegionCnt; ++nRegIndex )
		{
			// patch the rgn offset table

			//////////////////////////////////////////////////////////////////////////
			int nReg = nRegIndex;
			SRegion & reg = terrain->GetRegion(nReg);

			//////////////////////////////////////////////////////////////////////////

			long lThisRgnOffsetPtr = ftell(fp);
			long lThisRgnOffsetTablePtr = lRgnOffsetTablePtr + nRegIndex*sizeof(long);

			fseek(fp,lThisRgnOffsetTablePtr,SEEK_SET);

			fwrite(&lThisRgnOffsetPtr,sizeof(long),1,fp); // offset
			fseek(fp,lThisRgnOffsetPtr,SEEK_SET);

			//////////////////////////////////////////////////////////////////////////
#pragma region Offsets
			// 0 chunk name 四分之一区域
			fwrite("SRGN",4,1,fp);

			// chunk offset and size
			long lSubRegionPt = ftell(fp);
			long lPad = 0;
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size

			//////////////////////////////////////////////////////////////////////////

			// 1 chunk name vertex height
			fwrite("VHGT",4,1,fp);

			// chunk offset and size
			long lVertexHeightPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size

			//////////////////////////////////////////////////////////////////////////

			// 2 chunk name vertex diffuse
			fwrite("VDIF",4,1,fp);

			// chunk offset and size
			long lVertexDiffusePt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size
			//////////////////////////////////////////////////////////////////////////

			// 3 chunk name vertex specular color
			fwrite("VSPE",4,1,fp);

			// chunk offset and size
			long lVertexSpecularPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size
			//////////////////////////////////////////////////////////////////////////

			// 4 chunk name grid attributes block
			fwrite("GATR",4,1,fp);

			// chunk offset and size
			long lGAtrPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size

			// 5 chunk name exact block
			fwrite("EBLK",4,1,fp);

			// chunk exact block
			long lExactBlockPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size
			//////////////////////////////////////////////////////////////////////////

			// 6 chunk name texture index
			fwrite("TEXT",4,1,fp);

			// chunk offset and size
			long lTextureIndexPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size

			//////////////////////////////////////////////////////////////////////////

			// 7 chunk name model info
			fwrite("MODL",4,1,fp);

			// chunk offset and size
			long lModelPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size

			//////////////////////////////////////////////////////////////////////////

			// 8 chunk name trap index
			fwrite("TRAP",4,1,fp);

			// chunk offset and size
			long lTrapPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size

			//////////////////////////////////////////////////////////////////////////

			// 9 chunk name logic height
			fwrite("LHGT",4,1,fp);

			// chunk offset and size
			long lLogicHeightPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size

			//////////////////////////////////////////////////////////////////////////

			// 10 chunk name subvertex info
			fwrite("SBVT",4,1,fp);

			// chunk offset and size
			long lSubvertexPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size

			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////

			// 11 chunk name object info
			fwrite("OBJT",4,1,fp);

			// chunk offset and size
			long lObjectPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size

			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////

			// 12 chunk name water
			fwrite("WATR",4,1,fp);

			// chunk offset and size
			long lWaterPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////

			// 13 chunk name scene point light
			fwrite("SCLT",4,1,fp);

			// chunk offset and size
			long lSceneLightPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size

			//////////////////////////////////////////////////////////////////////////

			// 14 chunk name scene spot light
			fwrite("SPLT",4,1,fp);

			// chunk offset and size
			long lSceneSpotLightPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////

			// 15 chunk name scene audio
			fwrite("AUID",4,1,fp);

			// chunk offset and size
			long lSceneAudioPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////

			// 16 chunk name login model
			fwrite("LOGI",4,1,fp);

			// chunk offset and size
			long lSceneLoginPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////

			// 17 chunk name login model "TGMI" = terrain grid material index
			fwrite("TGMI",4,1,fp);

			// chunk offset and size
			long lTerMatIndexPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size
			//////////////////////////////////////////////////////////////////////////


			// vertices cnt
			DWORD dwVertexCntPerRegion = (REGION_SPACE+1)*(REGION_SPACE+1);
			DWORD dwGridCntPerRegion = (REGION_SPACE)*(REGION_SPACE);
			//////////////////////////////////////////////////////////////////////////
#pragma endregion Offsets

#pragma region Region
			// Offset backfill
			long lSubRegionOffset = ftell(fp);
			fseek(fp,lSubRegionPt,SEEK_SET);
			fwrite(&lSubRegionOffset,sizeof(long),1,fp); // size
			fseek(fp,lSubRegionOffset,SEEK_SET);

			// chunk raw data
			int nSubRgnIndex = 0;

			sqr::uint16 nSceneRgnIndex = 0;

			for ( int m = 0; m < REGION_DIVIDE; ++m )
			{
				nSubRgnIndex = reg.divideRgnIndices[m];

				SDivideRegion *pDivideRegion = NULL;
				pDivideRegion = terrain->GetDivideRegion(nSubRgnIndex);
				if ( pDivideRegion )
				{
					nSceneRgnIndex = sceneRgnIndexs[pDivideRegion->m_strRegionName];
					CVector3f center = pDivideRegion->m_vCenter;
					bool		bNeedCal = pDivideRegion->bNeedCalRegion;

					fwrite(&nSceneRgnIndex, sizeof(nSceneRgnIndex), 1, fp);
					fwrite(&center.x, sizeof(float), 1, fp);
					fwrite(&center.z, sizeof(float), 1, fp);
					fwrite(&bNeedCal, sizeof(bNeedCal), 1, fp);
				}
			}

			// size backfill
			long lSubRegionSize = ftell(fp) - lSubRegionOffset;
			fseek(fp,lSubRegionPt+sizeof(long),SEEK_SET);
			fwrite(&lSubRegionSize,sizeof(long),1,fp); // size
			fseek(fp,lSubRegionSize+lSubRegionOffset,SEEK_SET);
#pragma endregion Region

#pragma region VertexHeight

			// Offset backfill
			long lVertexHeightOffset = ftell(fp);
			fseek(fp,lVertexHeightPt,SEEK_SET);
			fwrite(&lVertexHeightOffset,sizeof(long),1,fp); // size
			fseek(fp,lVertexHeightOffset,SEEK_SET);

			// chunk raw data
			for ( size_t i = REGION_SPACE+1; i < dwVertexCntPerRegion; i++ )
			{
				if ( !(i % (REGION_SPACE+1)) ) 
					continue;
				SVertex & vertex = terrain->GetVertex(reg.vVertexIndices[i]);
				short s = (short)vertex.vPosition.y;
				fwrite(&s,sizeof(s),1,fp);
			}

			// size backfill
			long lVertexHeightSize = ftell(fp) - lVertexHeightOffset;
			fseek(fp,lVertexHeightPt+sizeof(long),SEEK_SET);
			fwrite(&lVertexHeightSize,sizeof(long),1,fp); // size
			fseek(fp,lVertexHeightSize+lVertexHeightOffset,SEEK_SET);
#pragma endregion VertexHeight

#pragma region VertexDiffuse

			// Offset backfill
			long lVertexDiffuseOffset = ftell(fp);
			fseek(fp,lVertexDiffusePt,SEEK_SET);
			fwrite(&lVertexDiffuseOffset,sizeof(long),1,fp); // size
			fseek(fp,lVertexDiffuseOffset,SEEK_SET);

			// chunk raw data
			for ( size_t i = REGION_SPACE+1; i < dwVertexCntPerRegion; i++ )
			{
				if ( !(i % (REGION_SPACE+1)) )
					continue;
				SVertex & vertex = terrain->GetVertex(reg.vVertexIndices[i]);
				fwrite(&vertex.oColor,sizeof(DWORD),1,fp); //vertex diffuse  color rgb
				fwrite(&vertex.tColor,sizeof(DWORD),1,fp); //vertex specular color rgb 暂时保存烘培文件，以后第一个保存烘培文件
			}

			// size backfill
			long lVertexDiffuseSize = ftell(fp) - lVertexDiffuseOffset;
			fseek(fp,lVertexDiffusePt+sizeof(long),SEEK_SET);
			fwrite(&lVertexDiffuseSize,sizeof(long),1,fp); // size
			fseek(fp,lVertexDiffuseSize+lVertexDiffuseOffset,SEEK_SET);
#pragma endregion VertexDiffuse

#pragma region VertexSpecular

			// Offset backfill
			long lVertexSpecularOffset = ftell(fp);
			fseek(fp,lVertexSpecularPt,SEEK_SET);
			fwrite(&lVertexSpecularOffset,sizeof(long),1,fp); // size
			fseek(fp,lVertexSpecularOffset,SEEK_SET);

			// chunk raw data
			for ( size_t i = REGION_SPACE+1; i < dwVertexCntPerRegion; i++ )
			{
				if ( !(i % (REGION_SPACE+1)) )
					continue;
				SVertex & vertex = terrain->GetVertex(reg.vVertexIndices[i]);
				fwrite(&vertex.sColor,sizeof(DWORD),1,fp); //vertex specular  color rgb
			}

			// size backfill
			long lVertexSpecularSize = ftell(fp) - lVertexSpecularOffset;
			fseek(fp,lVertexSpecularPt+sizeof(long),SEEK_SET);
			fwrite(&lVertexSpecularSize,sizeof(long),1,fp); // size
			fseek(fp,lVertexSpecularSize+lVertexSpecularOffset,SEEK_SET);
#pragma endregion VertexSpecular

#pragma region BlockInfo
			long lGAtrOffset = ftell(fp);
			fseek(fp,lGAtrPt,SEEK_SET);
			fwrite(&lGAtrOffset,sizeof(long),1,fp); // size
			fseek(fp,lGAtrOffset,SEEK_SET);

			// chunk raw data
			for (size_t i = 0; i < dwGridCntPerRegion; i++ )
			{
				SGrid * pGrid = &terrain->GetGrid(reg.vGridIndices[i]);;
				BYTE byBlockType;

				bitset<8> bsGridAttributes;
				bsGridAttributes.reset();

				// block 0 1
				byBlockType = pGrid->nBlockType;

				if ( byBlockType & 0x01 )
					bsGridAttributes.set(0,1);

				if ( byBlockType & 0x02 )
					bsGridAttributes.set(1,1);

				// influence on ground objects 2，先将顶点色改成材质色（也及是顶点和物件的材质色），肯定影响npc
				bsGridAttributes.set(2,/*pGrid->bNpcIsInfluence*/true?1:0);

				BYTE by = (BYTE)bsGridAttributes.to_ulong();

				fwrite(&by,sizeof(BYTE),1,fp);

				///road weight
				uint8 weight = (uint8)pGrid->nGridSeekRoadWeight;
				fwrite(&weight, sizeof(uint8),1,fp);
			}

			// size backfill
			long lGAtrSize = ftell(fp) - lGAtrOffset;
			fseek(fp,lGAtrPt+sizeof(long),SEEK_SET);
			fwrite(&lGAtrSize,sizeof(long),1,fp); // size
			fseek(fp,lGAtrSize+lGAtrOffset,SEEK_SET);

			//////////////////////////////////////////////////////////////////////////

			// 4 grid attributes chunk block info lExactBlockPt
			// Offset backfill
			long lExactBlockOffset = ftell(fp);
			fseek(fp,lExactBlockPt,SEEK_SET);
			fwrite(&lExactBlockOffset,sizeof(long),1,fp); // size
			fseek(fp,lExactBlockOffset,SEEK_SET);

			// chunk raw data
			for (size_t i = 0; i < dwGridCntPerRegion; i++ )
			{
				SGrid * pGrid = &terrain->GetGrid(reg.vGridIndices[i]);

				fwrite(&pGrid->subGridTypes, sizeof(pGrid->subGridTypes),1,fp);
			}

			// size backfill
			long lExactBlockSize = ftell(fp) - lExactBlockOffset;
			fseek(fp,lExactBlockPt+sizeof(long),SEEK_SET);
			fwrite(&lExactBlockSize,sizeof(long),1,fp); // size
			fseek(fp,lExactBlockSize+lExactBlockOffset,SEEK_SET);
#pragma endregion BlockInfo

#pragma region Texture
			// Offset backfill
			long lTextureIndexOffset = ftell(fp);
			fseek(fp,lTextureIndexPt,SEEK_SET);
			fwrite(&lTextureIndexOffset,sizeof(long),1,fp); // size
			fseek(fp,lTextureIndexOffset,SEEK_SET);

			// chunk raw data
			for (size_t i = 0; i < dwGridCntPerRegion; i++ )
			{
				int gridIndex = reg.vGridIndices[i];
				SGrid &grid = terrain->GetGrid(gridIndex);

				string strFirst = grid.nodeInfo.strTextureNames[0];
				string strSecond = grid.nodeInfo.strTextureNames[1];
				string strThird = grid.nodeInfo.strTextureNames[2];
				string strFourth = grid.nodeInfo.strTextureNames[3];

				BYTE byFirst = 0, bySecond = 0, byThird = 0, byFourth = 0;

				if ( strFirst == "")
					byFirst = 0xff;
				else
					byFirst = (BYTE)pRender->m_TerrainTexIndexs[strFirst];

				if ( strSecond == "")
					bySecond = 0xff;
				else
					bySecond = (BYTE)pRender->m_TerrainTexIndexs[strSecond];

				if ( strThird == "")
					byThird = 0xff;
				else
					byThird = (BYTE)pRender->m_TerrainTexIndexs[strThird];

				if ( strFourth == "")
					byFourth = 0xff;
				else
					byFourth = (BYTE)pRender->m_TerrainTexIndexs[strFourth];

				fwrite(&byFirst,sizeof(BYTE),1,fp);
				fwrite(&bySecond,sizeof(BYTE),1,fp);
				fwrite(&byThird,sizeof(BYTE),1,fp);
				fwrite(&byFourth,sizeof(BYTE),1,fp);

				SGrid * pGrid = &terrain->GetGrid(gridIndex);

				fwrite(&(pGrid->uv[0][0].x),sizeof(float),1,fp);
				fwrite(&(pGrid->uv[0][0].y),sizeof(float),1,fp);
				fwrite(&(pGrid->uv[0][1].x),sizeof(float),1,fp);
				fwrite(&(pGrid->uv[0][1].y),sizeof(float),1,fp);
				fwrite(&(pGrid->uv[0][2].x),sizeof(float),1,fp);
				fwrite(&(pGrid->uv[0][2].y),sizeof(float),1,fp);
				fwrite(&(pGrid->uv[0][3].x),sizeof(float),1,fp);
				fwrite(&(pGrid->uv[0][3].y),sizeof(float),1,fp);
				//grid hide mask
				fwrite(&(pGrid->bGridHide),sizeof(bool),1,fp);
			}

			// size backfill
			long lTextureIndexSize = ftell(fp) - lTextureIndexOffset;
			fseek(fp,lTextureIndexPt+sizeof(long),SEEK_SET);
			fwrite(&lTextureIndexSize,sizeof(long),1,fp); // size
			fseek(fp,lTextureIndexSize+lTextureIndexOffset,SEEK_SET);
#pragma endregion Texture

#pragma region Model
			// Offset backfill
			long lModelOffset = ftell(fp);
			fseek(fp,lModelPt,SEEK_SET);
			fwrite(&lModelOffset,sizeof(long),1,fp); // size
			fseek(fp,lModelOffset,SEEK_SET);

			WORD wCnt = 0;
			fwrite(&wCnt,sizeof(WORD),1,fp); // list cnt

			// chunk raw data
			for (size_t nRegGridIndex = 0; nRegGridIndex < dwGridCntPerRegion; ++nRegGridIndex )
			{
				SGrid * pGrid = &terrain->GetGrid(reg.vGridIndices[nRegGridIndex]);

				DepathModelInfoMap::iterator iter = terrain->m_DepathModelMap.find(pGrid->dwGridIndex);
				if ( iter != terrain->m_DepathModelMap.end() )
				{
					const DepathModelInfoArray& depathModelInfoList = iter->second;
					size_t depathModelSize = depathModelInfoList.size();
					for ( size_t mm = 0; mm < depathModelSize; ++mm )
					{
						const DepathModelInfoPtr& pSDepathModelInfo = depathModelInfoList[mm];

						wCnt++;

						WORD w = pSDepathModelInfo->nModelIndex;

						float fX = pSDepathModelInfo->fX;
						float fZ = pSDepathModelInfo->fZ;
						float fY = pSDepathModelInfo->fY;

						float fscalex = 0.0f,fscaley = 0.0f,fscalez = 0.0f;
						fscalex = pSDepathModelInfo->fScaleX;
						fscaley = pSDepathModelInfo->fScaleY;
						fscalez = pSDepathModelInfo->fScaleZ;

						short nScaleX = (short)(fscalex * 5.0f + 0.5f);
						short nScaleY = (short)(fscaley * 5.0f + 0.5f);
						short nScaleZ = (short)(fscalez * 5.0f + 0.5f);

						// info
						bitset<8> bsObjectAttributes;
						bsObjectAttributes.reset();
						// shadow
						bsObjectAttributes.set(0,pSDepathModelInfo->bShadow?1:0); 
						// translucency
						bsObjectAttributes.set(1,pSDepathModelInfo->bTranslucent?1:0);
						// receive shadow
						bsObjectAttributes.set(2,pSDepathModelInfo->bRecieveShadow?1:0);
						// on/off play ani
						bsObjectAttributes.set(3,pSDepathModelInfo->bPlayAnimate?1:0);

						BYTE by = (BYTE)bsObjectAttributes.to_ulong();


						CRegionMI regionInfo;

						// position info
						regionInfo.cX			= pSDepathModelInfo->fX;
						regionInfo.cY			= pSDepathModelInfo->fY;
						regionInfo.cZ			= pSDepathModelInfo->fZ;

						// rotate info
						regionInfo.fxRotation	= pSDepathModelInfo->fRotationX;
						regionInfo.fyRotation	= pSDepathModelInfo->fRotationY;
						regionInfo.fzRotation	= pSDepathModelInfo->fRotationZ;

						// scale info
						regionInfo.fScaleX		= nScaleX;
						regionInfo.fScaleY		= nScaleY;
						regionInfo.fScaleZ		= nScaleZ;

						// model index 和 local grid index 调转了位置
						regionInfo.SetIndex		= pSDepathModelInfo->nModelIndex;
						regionInfo.GridIndex	= nRegGridIndex;

						// flag
						regionInfo.nFlag		= by;

						// ambient ratio
						regionInfo.r			= uint8(pSDepathModelInfo->r * 0xff);
						regionInfo.g			= uint8(pSDepathModelInfo->g * 0xff);
						regionInfo.b			= uint8(pSDepathModelInfo->b * 0xff);

						// 物体隐藏是否受机器配置影响标志
						regionInfo.bEffectByConfigur = pSDepathModelInfo->bVisibleByConfigure;
						regionInfo.bCameraCollision = pSDepathModelInfo->bUseCameraCollision;

						fwrite(&regionInfo, sizeof(CRegionMI), 1, fp);
	
// 						string strname = pSDepathModelInfo->modelName;
// 						if( strname.find("swjb_wl_1") != -1 )
// 						{
// 							DebugOut("compose grid %d, %s, pos: %f, %f, %f, rotate: %f, %f, %f, scale: %d, %d, %d\n", pGrid->dwGridIndex, strname.c_str(),
// 								regionInfo.cX,         regionInfo.cY,         regionInfo.cZ, 
// 								regionInfo.fxRotation, regionInfo.fyRotation, regionInfo.fzRotation, 
// 								regionInfo.fScaleX,    regionInfo.fScaleY,    regionInfo.fScaleZ );
// 						}
					}
				}

				if ( !pGrid->vecCenterObjects.empty() )
				{
					for ( vector< CTObjectPtr >::iterator iter = pGrid->vecCenterObjects.begin(); iter != pGrid->vecCenterObjects.end(); ++iter )
					{
						CTObject *p = (*iter);
						CEditModelGroup *pModelGroup = p->GetModelGroup();

						if ( pModelGroup->m_SL.m_bIAmbScenePointLight )  
							continue;
						if ( pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight )  
							continue;
						if ( pModelGroup->GetIsSubUnit() )            
							continue;
						if( pModelGroup->m_pSceneAudio )
							continue;
						if( pModelGroup->m_bLoginModel )
							continue;
						if( pModelGroup->m_bReginEffect )
							continue;

						wCnt++;

						WORD w = p->GetResSetIndex();

						float sX = 0.0f,sY = 0.0f,sZ = 0.0f;
						float x = 0.0f, y = 0.0f, z = 0.0f;
						float fRotationX = 0.0f,fRotationY = 0.0f,fRotationZ = 0.0f,fscalex = 1.0f,fscaley = 1.0f,fscalez = 1.0f;

						p->GetModelGroup()->GetImageOffsetToAdjustedCenterGrid(sX,sY,sZ,fRotationX,fRotationY,fRotationZ,fscalex,fscaley,fscalez);
						
						CRegionMI regionInfo;

						// position info
						regionInfo.cX			= sX;
						regionInfo.cY			= sY;
						regionInfo.cZ			= sZ;

						// rotate info
						regionInfo.fxRotation	= fRotationX;
						regionInfo.fyRotation	= fRotationY;
						regionInfo.fzRotation	= fRotationZ;

						// scale info
						short nScaleX = (short)(fscalex * 5.0f + 0.5f);
						short nScaleY = (short)(fscaley * 5.0f + 0.5f);
						short nScaleZ = (short)(fscalez * 5.0f + 0.5f);

						regionInfo.fScaleX		= nScaleX;
						regionInfo.fScaleY		= nScaleY;
						regionInfo.fScaleZ		= nScaleZ;

						// model index 和 local grid index 调转了位置
						regionInfo.SetIndex		= p->GetResSetIndex();
						regionInfo.GridIndex	= nRegGridIndex;

						// flag
						// info
						bitset<8> bsObjectAttributes;
						bsObjectAttributes.reset();
						// shadow
						bsObjectAttributes.set(0,p->IsCastShadow()?1:0);
						// translucency
						bsObjectAttributes.set(1,p->GetModelGroup()->GetIsTranslucent()?1:0);
						// receive shadow
						bsObjectAttributes.set(2,p->GetModelGroup()->IsReceiveShadow()?1:0);
						// on/off play ani
						bsObjectAttributes.set(3,p->GetModelGroup()->GetIsPlayAnimation()?1:0);

						BYTE by = (BYTE)bsObjectAttributes.to_ulong();
						regionInfo.nFlag = by;

						// ambient ratio
						byte r,g,b;
						p->GetModelGroup()->GetOverallAmbientRatio(r,g,b);
						regionInfo.r			= r;
						regionInfo.g			= g;
						regionInfo.b			= b;

						// 物体隐藏是否受机器配置影响标志
						regionInfo.bEffectByConfigur = p->GetModelGroup()->GetIsVisibleByConfigure();
						regionInfo.bCameraCollision = p->GetModelGroup()->GetIsUseCameraCollison();

						fwrite(&regionInfo, sizeof(CRegionMI), 1, fp);

//  						vector<string> strnames = p->GetModelGroup()->GetModelFileNames();
//  						string strname = strnames[0];
//  						if( strname.find("swjb_wl_1") != -1 )
//  						{
//  							DebugOut("singal grid %d, %s, pos: %f, %f, %f, rotate: %f, %f, %f, scale: %d, %d, %d\n", pGrid->dwGridIndex, strname.c_str(),
//  								regionInfo.cX,         regionInfo.cY,         regionInfo.cZ, 
//  								regionInfo.fxRotation, regionInfo.fyRotation, regionInfo.fzRotation, 
//  								regionInfo.fScaleX,    regionInfo.fScaleY,    regionInfo.fScaleZ );
// 						}
					}
				}
			}

			// chunk size and cnt backfill
			long lModelSize = ftell(fp) - lModelOffset;
			fseek(fp,lModelPt+sizeof(long),SEEK_SET);
			fwrite(&lModelSize,sizeof(long),1,fp); // size

			fseek(fp,lModelOffset,SEEK_SET);
			fwrite(&wCnt,sizeof(WORD),1,fp); // cnt

			fseek(fp,lModelSize+lModelOffset,SEEK_SET);
#pragma endregion Model

#pragma region Trap
			// Offset backfill
			long lTrapOffset = ftell(fp);
			fseek(fp,lTrapPt,SEEK_SET);
			fwrite(&lTrapOffset,sizeof(long),1,fp); // size
			fseek(fp,lTrapOffset,SEEK_SET);

			wCnt = 0;
			fwrite(&wCnt,sizeof(WORD),1,fp); // list cnt

			//// chunk raw data
			//for (size_t i = 0; i < dwGridCntPerRegion; ++i )
			//{
			//	SGrid * pGrid = &terrain->GetGrid(reg.vGridIndices[i]);

			//	if ( pGrid->dwLineHook) // if ( byTrapID != 0xff )
			//	{
			//		wCnt++;
			//		BYTE byTrapID = 0;

			//		// local grid index
			//		fwrite(&i,sizeof(BYTE),1,fp);
			//		// trap id
			//		fwrite(&byTrapID,sizeof(BYTE),1,fp);
			//	}
			//}

			// size backfill
			long lTrapSize = ftell(fp) - lTrapOffset;
			fseek(fp,lTrapPt+sizeof(long),SEEK_SET);
			fwrite(&lTrapSize,sizeof(long),1,fp); // size

			fseek(fp,lTrapOffset,SEEK_SET);
			fwrite(&wCnt,sizeof(WORD),1,fp); // cnt

			fseek(fp,lTrapSize+lTrapOffset,SEEK_SET);
#pragma endregion Trap

#pragma region LogicalHeight
			// Offset backfill
			long lLogicHeightOffset = ftell(fp);
			fseek(fp,lLogicHeightPt,SEEK_SET);
			fwrite(&lLogicHeightOffset,sizeof(long),1,fp);
			fseek(fp,lLogicHeightOffset,SEEK_SET);

			wCnt = 0;
			fwrite(&wCnt,sizeof(WORD),1,fp); // list cnt

			// chunk raw data
			for ( WORD w = REGION_SPACE+1; w < dwVertexCntPerRegion; w++ )
			{
				if ( !(w % (REGION_SPACE+1)) ) continue;

				SVertex & vertex = terrain->GetVertex(reg.vVertexIndices[w]);
				short sLogicHeight = short(vertex.fLogicHeight);

				// local grid index
				if ( sLogicHeight != 0 )
				{
					wCnt++;

					WORD NewWX = w % (REGION_SPACE+1) - 1;
					WORD NewWY = w / (REGION_SPACE+1) - 1;
					WORD NewW  = NewWY * REGION_SPACE + NewWX;
					fwrite(&NewW,sizeof(WORD),1,fp);
					fwrite(&sLogicHeight,sizeof(short),1,fp);
				}
			}

			// size backfill
			long lLogicHeightSize = ftell(fp) - lLogicHeightOffset;
			fseek(fp,lLogicHeightPt+sizeof(long),SEEK_SET);
			fwrite(&lLogicHeightSize,sizeof(long),1,fp); // size

			fseek(fp,lLogicHeightOffset,SEEK_SET);
			fwrite(&wCnt,sizeof(WORD),1,fp); // cnt

			fseek(fp,lLogicHeightSize+lLogicHeightOffset,SEEK_SET);
#pragma endregion LogicalHeight

#pragma region SubVertex
			// Offset backfill
			long lSubvertexOffset = ftell(fp);
			fseek(fp,lSubvertexPt,SEEK_SET);
			fwrite(&lSubvertexOffset,sizeof(long),1,fp);
			fseek(fp,lSubvertexOffset,SEEK_SET);

			wCnt = 0;
			fwrite(&wCnt,sizeof(WORD),1,fp); // list cnt

			// size backfill
			long lSubvertexSize = ftell(fp) - lSubvertexOffset;
			fseek(fp,lSubvertexPt+sizeof(long),SEEK_SET);
			fwrite(&lSubvertexSize,sizeof(long),1,fp); // size

			fseek(fp,lSubvertexOffset,SEEK_SET);
			fwrite(&wCnt,sizeof(WORD),1,fp); // cnt

			fseek(fp,lSubvertexSize+lSubvertexOffset,SEEK_SET);
#pragma endregion SubVertex

#pragma region Object
			// offset backfill
			long lObjectOffset = ftell(fp);
			fseek(fp,lObjectPt,SEEK_SET);
			fwrite(&lObjectOffset,sizeof(long),1,fp); // offset
			fseek(fp,lObjectOffset,SEEK_SET);

			// size backfill
			long lObjectSize = ftell(fp) - lObjectOffset;
			fseek(fp,lObjectPt+sizeof(long),SEEK_SET);
			fwrite(&lObjectSize,sizeof(long),1,fp); // size
			fseek(fp,lObjectSize+lObjectOffset,SEEK_SET);
#pragma endregion Object

#pragma region Water
			// offset backfill
			long lWaterOffset = ftell(fp);
			fseek(fp,lWaterPt,SEEK_SET);
			fwrite(&lWaterOffset,sizeof(long),1,fp); // offset
			fseek(fp,lWaterOffset,SEEK_SET);

			CTerrainMesh * pWater = terrain->GetWater();
			set<int> setGridIndicesVisibleWater = pWater->GetGridIndicesVisibleWater();
			map< CRiver *, pair< vector<int>, vector<int> > > mapRivers; // river and side

			for ( set<int>::iterator iter = setGridIndicesVisibleWater.begin(); iter != setGridIndicesVisibleWater.end(); ++iter )
			{
				int nGridIndex = *iter;
				SGrid & gridW = pWater->GetGrid(nGridIndex);

				Ast(gridW.bVisible);

				if ( gridW.pRiver )
				{
					bool bIn = false;

					for (size_t i = 0; i < dwGridCntPerRegion; i++ )
					{
						if ( gridW.dwGridIndex == reg.vGridIndices[i] )
						{
							bIn = true;
							break;
						}
					}

					if ( bIn )
					{
						if ( gridW.bSide )
						{
							mapRivers[gridW.pRiver].second.push_back(nGridIndex);
						}
						else
						{
							mapRivers[gridW.pRiver].first.push_back(nGridIndex);
						}
					}
				}
			}

			BYTE byCnt = mapRivers.size();
			fwrite(&byCnt,sizeof(BYTE),1,fp); // river count of this region

			set<int> setVertexIndicesWaterOn;

			// for each river
			for ( map< CRiver *, pair< vector<int>, vector<int> > >::iterator iterMapRivers = mapRivers.begin(); iterMapRivers != mapRivers.end(); ++iterMapRivers )
			{
				CRiver * pRiver = iterMapRivers->first;

				DWORD dwTextureBaseIndex = pRiver->GetTextureBaseIndex();
				DWORD dwTextureCnt = pRiver->GetTextureCount();

				fwrite(&dwTextureBaseIndex ,sizeof(DWORD),1,fp);
				fwrite(&dwTextureCnt ,sizeof(DWORD),1,fp);

				DWORD dwGridCnt = iterMapRivers->second.first.size();
				fwrite(&dwGridCnt,sizeof(DWORD),1,fp);

				for (size_t i = 0; i < iterMapRivers->second.first.size(); ++i  )
				{
					int nGridIndex = iterMapRivers->second.first[i];

					SGrid & gridW = pWater->GetGrid(nGridIndex);

					////water hide mask
					//fwrite(&(gridW.bGridHide),sizeof(bool),1,fp);

					// convert to local
					BYTE byLocalGridIndex = find(reg.vGridIndices.begin(),reg.vGridIndices.end(),nGridIndex) - reg.vGridIndices.begin();
					fwrite(&byLocalGridIndex,sizeof(BYTE),1,fp);

					for ( int j = 0; j < 4; ++j )
					{
						int nVertexIndex = gridW.dwVertexIndex[j];
						setVertexIndicesWaterOn.insert(nVertexIndex);
					}
				}

				//////////////////////////////////////////////////////////////////////////

				// river side
				dwGridCnt = iterMapRivers->second.second.size();
				fwrite(&dwGridCnt,sizeof(DWORD),1,fp);

				for (size_t i = 0; i < iterMapRivers->second.second.size(); ++i  )
				{
					int nGridIndex = iterMapRivers->second.second[i];

					SGrid & gridW = pWater->GetGrid(nGridIndex);

					// convert to local
					BYTE byLocalGridIndex = find(reg.vGridIndices.begin(),reg.vGridIndices.end(),nGridIndex) - reg.vGridIndices.begin();
					fwrite(&byLocalGridIndex,sizeof(BYTE),1,fp);

					if ( gridW.pWave)
					{
						for ( int j = 0; j < 4; ++j )
						{
							int nVertexIndex = gridW.dwVertexIndex[j];
							setVertexIndicesWaterOn.insert(nVertexIndex);
						}
					}
				}
			}

			//////////////////////////////////////////////////////////////////////////

			DWORD dwVertexCnt = setVertexIndicesWaterOn.size();
			fwrite(&dwVertexCnt,sizeof(DWORD),1,fp);

			for ( set<int>::iterator iter = setVertexIndicesWaterOn.begin(); iter != setVertexIndicesWaterOn.end(); ++iter )
			{
				DWORD dwVertexIndex = *iter;

				// convert to local
				WORD wLocalVertexIndex = find(reg.vVertexIndices.begin(),reg.vVertexIndices.end(),dwVertexIndex) - reg.vVertexIndices.begin();
				fwrite(&wLocalVertexIndex,sizeof(WORD),1,fp);

				SVertex & vertex = pWater->GetVertex(dwVertexIndex);
				short sVertexHeight = (short)vertex.vPosition.y;
				fwrite(&sVertexHeight,sizeof(short),1,fp);
				fwrite(&vertex.oColor,sizeof(DWORD),1,fp);
			}

			// size backfill
			long lWaterSize = ftell(fp) - lWaterOffset;
			fseek(fp,lWaterPt+sizeof(long),SEEK_SET);
			fwrite(&lWaterSize,sizeof(long),1,fp); // size
			fseek(fp,lWaterSize+lWaterOffset,SEEK_SET);
#pragma endregion Water

#pragma region SceneLight
			// Offset backfill
			long lSceneLightOffset = ftell(fp);
			fseek (  fp, lSceneLightPt,     SEEK_SET);
			fwrite( &lSceneLightOffset, sizeof(long), 1, fp); // size
			fseek (  fp, lSceneLightOffset, SEEK_SET);

			uint8 wSceneLightCnt = 0;
			fwrite( &wSceneLightCnt, sizeof(uint8), 1, fp); // list cnt

			// chunk raw data
			for ( uint16 i = 0; i < dwGridCntPerRegion; ++i )
			{
				SGrid * pGrid = &terrain->GetGrid(reg.vGridIndices[i]);

				if ( !pGrid->vecCenterObjects.empty() )
				{
					for (size_t j = 0; j < pGrid->vecCenterObjects.size(); ++j)
					{
						if ( (CTObject*)(pGrid->vecCenterObjects[j])->GetModelGroup()->m_SL.m_bIAmbScenePointLight )
						{
							wSceneLightCnt++;
							// Grid Index within current region
							fwrite(&i, sizeof(uint8), 1, fp);

							// Y offset
							short sY;

							sY = short(((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->GetSelfPresentation(0)->GetLiftEx()
								+ pGrid->vCenter.y);

							fwrite(& sY, sizeof(short), 1, fp);

							unsigned __int16 Rang =     short(( (CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SL.m_Rang);
							byte B  = GetBValue(((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SL.m_Color);
							byte G  = GetGValue(((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SL.m_Color);
							byte R  = GetRValue(((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SL.m_Color);
							byte dB = GetBValue(((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SL.m_Dyn_Color);
							byte dG = GetGValue(((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SL.m_Dyn_Color);
							byte dR = GetRValue(((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SL.m_Dyn_Color);

							ScenePointLight_Gac temp =
							{
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SL.m_Attenuation0,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SL.m_Attenuation1,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SL.m_Attenuation2,
								Rang,
								R,
								G,
								B,
								dR,
								dG,
								dB,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SL.m_Dyn_ChangeProbability,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SL.m_Dyn_BrightTime,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SL.m_Dyn_DarkTime,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SL.m_Dyn_ChangeTime
							};

							fwrite(& temp, sizeof(ScenePointLight_Gac), 1, fp);
						}
					}
				}
			}

			long lSceneLightSize = ftell(fp) - lSceneLightOffset;
			fseek (  fp, lSceneLightPt + sizeof(long), SEEK_SET );
			fwrite( &lSceneLightSize, sizeof(long), 1 ,fp ); // size

			fseek (  fp, lSceneLightOffset, SEEK_SET);
			fwrite( &wSceneLightCnt, sizeof(uint8) ,1, fp); // cnt

			fseek( fp, lSceneLightSize + lSceneLightOffset, SEEK_SET);
#pragma endregion SceneLight

#pragma region SpotLight
			// Offset backfill
			long lSceneSpotLightOffset = ftell(fp);
			fseek (  fp, lSceneSpotLightPt,     SEEK_SET);
			fwrite( &lSceneSpotLightOffset, sizeof(long), 1, fp); // size
			fseek (  fp, lSceneSpotLightOffset, SEEK_SET);

			uint8 wSceneSpotLightCnt = 0;
			fwrite( &wSceneSpotLightCnt, sizeof(uint8), 1, fp); // list cnt

			// chunk raw data
			for ( uint16 i = 0; i < dwGridCntPerRegion; ++i )
			{
				SGrid * pGrid = &terrain->GetGrid(reg.vGridIndices[i]);

				if ( !pGrid->vecCenterObjects.empty() )
				{
					for (size_t j = 0; j < pGrid->vecCenterObjects.size(); ++j)
					{
						if ( (CTObject*)(pGrid->vecCenterObjects[j])->GetModelGroup()->m_SpotLight.m_bIAmbSceneSpotLight )
						{
							wSceneSpotLightCnt++;
							// Grid Index within current region
							fwrite(&i, sizeof(uint8), 1, fp);

							// Y offset
							short sY;

							sY = short(((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->GetSelfPresentation(0)->GetLiftEx()
								+ pGrid->vCenter.y);

							fwrite(& sY, sizeof(short), 1, fp);

							unsigned __int16 Rang	 =     short(( (CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_fRang);
							unsigned __int16 FallOff =     short(( (CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_fFallOff);
							byte B  = GetBValue(((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_Color);
							byte G  = GetGValue(((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_Color);
							byte R  = GetRValue(((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_Color);

							SceneSpotLight_Gac temp =
							{
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_fAttenuation0,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_fAttenuation1,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_fAttenuation2,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_Direction.x,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_Direction.y,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_Direction.z,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_TargetPos.x,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_TargetPos.y,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_TargetPos.z,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_fTheta,
								((CTObject*)(pGrid->vecCenterObjects[j]))->GetModelGroup()->m_SpotLight.m_fPhi,
								Rang,
								FallOff,
								R,
								G,
								B,
							};

							fwrite(& temp, sizeof(SceneSpotLight_Gac), 1, fp);
						}
					}
				}
			}

			long lSceneSpotLightSize = ftell(fp) - lSceneSpotLightOffset;
			fseek (  fp, lSceneSpotLightPt + sizeof(long), SEEK_SET );
			fwrite( &lSceneSpotLightSize, sizeof(long), 1 ,fp ); // size

			fseek (  fp, lSceneSpotLightOffset, SEEK_SET);
			fwrite( &wSceneSpotLightCnt, sizeof(uint8) ,1, fp); // cnt

			fseek( fp, lSceneSpotLightSize + lSceneSpotLightOffset, SEEK_SET);
#pragma endregion SpotLight

#pragma region SceneAudio
			// Offset backfill
			long lSceneAudioOffset = ftell(fp);
			fseek (  fp, lSceneAudioPt,     SEEK_SET);
			fwrite( &lSceneAudioOffset, sizeof(long), 1, fp); // size
			fseek (  fp, lSceneAudioOffset, SEEK_SET);

			uint8 wSceneAudioCnt = 0;
			fwrite( &wSceneAudioCnt, sizeof(uint8), 1, fp); // list cnt

			// chunk raw data
			for ( uint16 i = 0; i < dwGridCntPerRegion; ++i )
			{
				SGrid * pGrid = &terrain->GetGrid(reg.vGridIndices[i]);

				if ( !pGrid->vecCenterObjects.empty() )
				{
					int ngridIndex = pGrid->dwGridIndex;
					int ngridsetIndex = 0;

					for (vector< CTObjectPtr >::iterator iter = pGrid->vecCenterObjects.begin(); iter != pGrid->vecCenterObjects.end(); ++iter)
					{
						++ngridsetIndex;
						CTObject * p = (*iter);
						CEditModelGroup *pModelGroup = p->GetModelGroup();
						if ( pModelGroup->m_pSceneAudio )
						{
							wSceneAudioCnt++;

							WORD w = p->GetResCueIndex();

							CMatrix mat;
							pModelGroup->GetFinalMatrix(mat);
							
							uint8 Version = AUDIO_PLAY_LEN;
							fwrite( &Version, sizeof(Version), 1, fp); // list cnt

							///play len ane mark
							DWORD dwPlayTimeLenMin = pModelGroup->GetPlayTimeLenMin();
							DWORD dwPlayTimeLenMax = pModelGroup->GetPlayTimeLenMax();

							///二段播放间隔长度最小最大值
							DWORD dwPlayIntervalMin = pModelGroup->GetPlayIntervalMin();
							DWORD dwPlayIntervalMax = pModelGroup->GetPlayIntervalMax();

							bool bPlayNotRepeat = dwPlayTimeLenMin != 0 && dwPlayTimeLenMax != 0 && dwPlayIntervalMin != 0 && dwPlayIntervalMax != 0 ;
							fwrite(&bPlayNotRepeat,sizeof(bPlayNotRepeat),1,fp);
							if( bPlayNotRepeat )///有播放机制才保存播放长度和间隔
							{
								fwrite(&dwPlayTimeLenMin, sizeof(dwPlayTimeLenMin), 1,fp);
								fwrite(&dwPlayTimeLenMax, sizeof(dwPlayTimeLenMax), 1,fp);
								fwrite(&dwPlayIntervalMin,sizeof(dwPlayIntervalMin),1,fp);
								fwrite(&dwPlayIntervalMax,sizeof(dwPlayIntervalMax),1,fp);
							}

							///default audio properpty
							fwrite(&mat._41,sizeof(float),1,fp);
							fwrite(&mat._42,sizeof(float),1,fp);
							fwrite(&mat._43,sizeof(float),1,fp);

							// model index 和 local grid index 调转了位置
							fwrite(&w,sizeof(WORD),1,fp);       // 这样读取的时候就能字节对齐

							// local grid index
							fwrite(&i,sizeof(BYTE),1,fp);
						}
					}
				}
			}

			long lSceneAudioSize = ftell(fp) - lSceneAudioOffset;
			fseek (  fp, lSceneAudioPt + sizeof(long), SEEK_SET );
			fwrite( &lSceneAudioSize, sizeof(long), 1 ,fp ); // size

			fseek (  fp, lSceneAudioOffset, SEEK_SET);
			fwrite( &wSceneAudioCnt, sizeof(uint8) ,1, fp); // cnt

			fseek( fp, lSceneAudioSize + lSceneAudioOffset, SEEK_SET);
#pragma endregion SceneAudio

#pragma region SceneLogin
			// Offset backfill
			long lSceneLoginOffset = ftell(fp);
			fseek (  fp, lSceneLoginPt,     SEEK_SET);
			fwrite( &lSceneLoginOffset, sizeof(long), 1, fp); // size
			fseek (  fp, lSceneLoginOffset, SEEK_SET);

			uint16 wSceneLoginCnt = 0;
			fwrite( &wSceneLoginCnt, sizeof(wSceneLoginCnt), 1, fp); // list cnt

			// chunk raw data
			for ( size_t i = 0; i < dwGridCntPerRegion; ++i )
			{
				SGrid * pGrid = &terrain->GetGrid(reg.vGridIndices[i]);

				if ( !pGrid->vecCenterObjects.empty() )
				{
					for (vector< CTObjectPtr >::iterator iter = pGrid->vecCenterObjects.begin(); iter != pGrid->vecCenterObjects.end(); ++iter)
					{
						CTObject * p = (*iter);
						if ( p->GetModelGroup()->m_bLoginModel )
						{
							wSceneLoginCnt++;

							WORD w = p->GetResSetIndex();

							float sX = 0.0f,sY = 0.0f,sZ = 0.0f;
							float x = 0.0f, y = 0.0f, z = 0.0f;
							float fRotation = 0.0f,fYaw = 0.0f,fPitch = 0.0f,fscalex = 1.0f,fscaley = 1.0f,fscalez = 1.0f;

							//////////////////////////////////////////////////////////////////////////
							///model info
							p->GetModelGroup()->GetImageOffsetToAdjustedCenterGrid(sX,sY,sZ,fRotation,fYaw,fPitch,fscalex,fscaley,fscalez);
	
							CRegionMI regionInfo;

							// position info
							regionInfo.cX			= sX;
							regionInfo.cY			= sY;
							regionInfo.cZ			= sZ;

							// rotate info
							regionInfo.fxRotation	= fYaw;
							regionInfo.fyRotation	= fRotation;
							regionInfo.fzRotation	= fPitch;

							// scale info
							short nScaleX = (short)(fscalex * 5.0f + 0.5f);
							short nScaleY = (short)(fscaley * 5.0f + 0.5f);
							short nScaleZ = (short)(fscalez * 5.0f + 0.5f);

							regionInfo.fScaleX		= nScaleX;
							regionInfo.fScaleY		= nScaleY;
							regionInfo.fScaleZ		= nScaleZ;

							// model index 和 local grid index 调转了位置
							regionInfo.SetIndex		= p->GetResSetIndex();
							regionInfo.GridIndex	= i;

							// flag
							// info
							bitset<8> bsObjectAttributes;
							bsObjectAttributes.reset();
							// shadow
							bsObjectAttributes.set(0,p->IsCastShadow()?1:0);
							// translucency
							bsObjectAttributes.set(1,p->GetModelGroup()->GetIsTranslucent()?1:0);
							// receive shadow
							bsObjectAttributes.set(2,p->GetModelGroup()->IsReceiveShadow()?1:0);
							// on/off play ani
							bsObjectAttributes.set(3,p->GetModelGroup()->GetIsPlayAnimation()?1:0);

							BYTE by = (BYTE)bsObjectAttributes.to_ulong();
							regionInfo.nFlag = by;

							// ambient ratio
							byte r,g,b;
							p->GetModelGroup()->GetOverallAmbientRatio(r,g,b);
							regionInfo.r			= r;
							regionInfo.g			= g;
							regionInfo.b			= b;

							// 物体隐藏是否受机器配置影响标志
							regionInfo.bEffectByConfigur = p->GetModelGroup()->GetIsVisibleByConfigure();
							regionInfo.bCameraCollision = p->GetModelGroup()->GetIsUseCameraCollison();

							fwrite(&regionInfo, sizeof(CRegionMI), 1, fp);
							//////////////////////////////////////////////////////////////////////////

							//////////////////////////////////////////////////////////////////////////
							///logic name
							string strLogicName = p->GetModelGroup()->GetLoginModelLogicName();
							
							use_facet<ctype<TCHAR> >( loc ).tolower
								( &strLogicName[0], &strLogicName[strLogicName.length()] );

							DWORD dwLen = strLogicName.size();
							fwrite(&dwLen,sizeof(dwLen),1,fp);
							fwrite(strLogicName.c_str(),dwLen,1,fp);
							//////////////////////////////////////////////////////////////////////////

							//////////////////////////////////////////////////////////////////////////
							///frame ani 
							CMapEditLoginObj *pLoginObj = static_cast<CMapEditLoginObj*>(p->GetModelGroup()->m_vecSelfPresentation[0]);

							CamPathAniMap allAniLoginFrameList = pLoginObj->m_AniLoginFrameList;
							CamPathAniMap::iterator beg = allAniLoginFrameList.begin();
							CamPathAniMap::iterator end = allAniLoginFrameList.end();
							AniFrameMap   aniFrameList;
							vector<int16> frameList;
							size_t aniSize    = 0;
							size_t allAniSize = allAniLoginFrameList.size();
							int16   frm = 0;

							fwrite(&allAniSize,sizeof(allAniSize),1,fp);
							for ( beg = allAniLoginFrameList.begin(); beg != end; ++beg )
							{
								int nCameraPathIndex = beg->first;
								fwrite(&nCameraPathIndex,sizeof(nCameraPathIndex),1,fp);

								aniFrameList = beg->second;
								aniSize = aniFrameList.size();

								fwrite(&aniSize,sizeof(aniSize),1,fp);
								for( AniFrameMap::iterator iter = aniFrameList.begin(); iter != aniFrameList.end(); ++iter )
								{
									string str = iter->first;
									frameList = iter->second;

									DWORD dwLen = str.size();
									fwrite(&dwLen,sizeof(dwLen),1,fp);
									fwrite(str.c_str(),dwLen,1,fp);

									size_t size = frameList.size();
									fwrite(&size,sizeof(size),1,fp);
									for ( size_t m = 0; m < size; ++m )
									{
										frm = frameList[m];
										fwrite(&frm,sizeof(frm),1,fp);
									}
								}
							}
						}
					}
				}
			}

			long lSceneLoginSize = ftell(fp) - lSceneLoginOffset;
			fseek (  fp, lSceneLoginPt + sizeof(long), SEEK_SET );
			fwrite( &lSceneLoginSize, sizeof(long), 1 ,fp ); // size

			fseek (  fp, lSceneLoginOffset, SEEK_SET);
			fwrite( &wSceneLoginCnt, sizeof(wSceneLoginCnt) ,1, fp); // cnt

			fseek( fp, lSceneLoginSize + lSceneLoginOffset, SEEK_SET);
#pragma endregion SceneLogin

#pragma region TerMaterialIndex
			// Offset backfill
			long lTerMatIndexOffset = ftell(fp);
			fseek (  fp, lTerMatIndexPt,     SEEK_SET);
			fwrite( &lTerMatIndexOffset, sizeof(long), 1, fp); // size
			fseek (  fp, lTerMatIndexOffset, SEEK_SET);

			int8 nMaterialIndex = -1;
			// chunk raw data
			for ( uint16 i = 0; i < dwGridCntPerRegion; ++i )
			{
				SGrid * pGrid = &terrain->GetGrid(reg.vGridIndices[i]);
				nMaterialIndex = pGrid->nMaterialIndex;
				fwrite(&nMaterialIndex,sizeof(nMaterialIndex),1,fp);
			}

			long lTerMatIndexSize = ftell(fp) - lTerMatIndexOffset;
			fseek (  fp, lTerMatIndexPt + sizeof(long), SEEK_SET );
			fwrite( &lTerMatIndexSize, sizeof(long), 1 ,fp ); // size
			fseek( fp, lTerMatIndexSize + lTerMatIndexOffset, SEEK_SET);

#pragma endregion TerMaterialIndex

			progress->SetProgress( (2.0f + nReg)/Total );
		}

		fclose(fp);

		terrain->SetGameGacMapVersion(dwGameClientID);
		CEngine::GetInst()->RenderGUIMap();

		progress->SetProgress( 1.0f );

		LogConsole("导出客户端文件完毕..., 请耐心等待服务端文件导出");
	}
	else
	{
		string log = strRgnFileName + "无法保存";

		MessageBox(NULL,log.c_str(),"错误",MB_OK);
	}
}

//------------------------------------------------------------------------------
void 
CGameGacMapSaver::MakeAtlasTexture(const string& path)
{
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
	terrain->m_pTerrain->m_Texture->SaveToFile(FileName.c_str(),IFF_JPG);
	//D3DXSaveTextureToFile(FileName.c_str(),D3DXIFF_JPG,terrain->m_pTerrain->m_Texture->GetBuffer(),&pale);
}

}// namespace sqr
