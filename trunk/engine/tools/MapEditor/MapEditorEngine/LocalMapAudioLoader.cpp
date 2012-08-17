//------------------------------------------------------------------------------
//  CLocalMapAudioLoader.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "LocalMapAudioLoader.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "Operator.h"
#include "Operator.inl"
#include "TerrainMeshObject.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "SceneAudio.h"
#include "LocalVersion.h"
#include "SceneAudioMgr.h"

namespace sqr
{

	//------------------------------------------------------------------------------
	CLocalMapAudioLoader::CLocalMapAudioLoader()
		: terrain(NULL)
	{
	}

	//------------------------------------------------------------------------------
	void
	CLocalMapAudioLoader::LoadAsSceneAudio(const string& path)
	{
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		size_t npos = path.rfind(".");
		string audioPath = path.substr(0, npos);
		audioPath += ".audio";

		FILE * fp = NULL;
		fp = fopen(audioPath.c_str(),"rb");

		if (! fp )
		{
			string log = audioPath + "无法打开";

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

			fread(&dwVersion, sizeof(dwVersion), 1, fp);

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

			fread(&size, sizeof(size), 1, fp);

			if( size > 0 )
				MessageBox(NULL,"本地图有音效文件","提示",MB_OK);

			for( size_t i = 0; i < size; ++i )
			{
				int nGrid = 0;
				fread(&nGrid,sizeof(nGrid),1,fp);
				SGrid * pGrid = &pTerrain->GetGrid(nGrid);
				
				DWORD dw = 0;
				fread(&dw,sizeof(DWORD),1,fp);

				set<string> sGridCueNames;

				for (size_t j = 0; j < dw; j++ )
				{
					switch(dwVersion)
					{
					case AUDIO_PLAYTIME_INTERVAL:
						{
							//////////////////////////////////////////////////////////////////////////
							///play time mechanism
							fread(&dwPlayLenMin,	 sizeof(DWORD),1,fp);
							fread(&dwPlayLenMax,	 sizeof(DWORD),1,fp);
							fread(&dwPlayIntervalMin,sizeof(DWORD),1,fp);
							fread(&dwPlayIntervalMax,sizeof(DWORD),1,fp);
							//////////////////////////////////////////////////////////////////////////
						}
					case AUDIO_DEFAULT:
						{
							///cue name
							fread(&dwItemNameLen,sizeof(DWORD),1,fp);
							strCueName.resize(dwItemNameLen);
							fread((TCHAR*)strCueName.data(),dwItemNameLen,1,fp);

							fread(&dwItemNameLen,sizeof(DWORD),1,fp);
							strItemName.resize(dwItemNameLen);
							fread((TCHAR*)strItemName.data(),dwItemNameLen,1,fp);

							use_facet<ctype<TCHAR> >( loc ).tolower
								( &strItemName[0], &strItemName[strItemName.length()] );

							//////////////////////////////////////////////////////////////////////////

							///offset
							fread(&vUserOffsetEx,sizeof(CVector3f),1,fp);
							vUserOffsetEx2 = CVector3f(vUserOffsetEx.y,vUserOffsetEx.x,vUserOffsetEx.z);

							//////////////////////////////////////////////////////////////////////////
							fread(&nGridIndex,sizeof(int),1,fp);

							//y 轴
							fread(&fRotation,sizeof(float),1,fp);
							fread(&nRotationCnt,sizeof(int),1,fp);

							//z 轴
							fread(&fPitch,sizeof(float),1,fp);
							fread(&nPitchCnt,sizeof(int),1,fp);

							//x 轴
							fread(&fYaw,sizeof(float),1,fp);
							fread(&nYawCnt,sizeof(int),1,fp);

							//缩放
							fread(&xScale,sizeof(float),1,fp);
							fread(&yScale,sizeof(float),1,fp);
							fread(&zScale,sizeof(float),1,fp);
							//////////////////////////////////////////////////////////////////////////

							fread(&byRenderStyle, sizeof(BYTE),1,fp);
							fread(&nAtten,sizeof(int),1,fp);				
							fread(&bShadow,sizeof(bool),1,fp);

							///读取是否接收阴影信息
							fread(&bReceiveShadow,sizeof(bool),1,fp);

							//是否消隐
							fread(&bTrans,sizeof(bTrans),1,fp);

							///mirror type
							fread(&nMirrorType, sizeof(nMirrorType), 1, fp);

							///nAffectRadio
							fread( &nRatio, sizeof(int), 1, fp);
						}

					default:
						break;
					}

// 					if( strCueName.find("blacksmith01") != -1/* || strCueName.find("forest02") != -1*/ )
// 					{			
// 						DebugOut("cue name is %s, %d, %d, %d, %d\n", strCueName.c_str(), dwPlayLenMin, dwPlayLenMax, dwPlayIntervalMin, dwPlayIntervalMax);
// 					}

					if( strCueName == "" )
						continue;
					
					if( sGridCueNames.find(strCueName) != sGridCueNames.end() )
					{
						char c[10];
						sprintf(c, "%d", nGridIndex);
						string str = c;
						str = str + "格子上有相同音效";
						str = str + strCueName + " YES删掉，NO不删掉";

						if ( IDYES == MessageBox(NULL, str.c_str(), "提示", MB_YESNO) )
							continue;
					}

					sGridCueNames.insert(strCueName);

					//DebugOut("save audio grid %d, %s, %f, %f, %f\n", nGridIndex, strCueName.c_str(), vUserOffsetEx.x, vUserOffsetEx.y, vUserOffsetEx.z);
					if(pOperator->Rebirth(strItemName, nGridIndex, fRotation, nRotationCnt,
						fPitch, nPitchCnt, fYaw, nYawCnt, xScale, yScale, zScale, &vUserOffsetEx2, 0, NULL, 0,0,1.0f,eAutoGenerationType,byRenderStyle,nAtten,false,bShadow,bTrans,bTransState,nMirrorType,nRatio, "", strItemName, strCueName))
					{
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

			fclose(fp);
			locale::global(loc);//恢复全局locale	

			if( size > 0 )
				MessageBox(NULL,"音效文件加载完毕","提示",MB_OK);
		}
	}

}