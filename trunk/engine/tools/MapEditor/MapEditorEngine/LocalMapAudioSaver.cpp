//------------------------------------------------------------------------------
//  CLocalMapAudioSaver.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "LocalMapAudioSaver.h"
#include "GraphicCommon.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "Operator.h"
#include "Operator.inl"
#include "TerrainMeshObject.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "LocalVersion.h"

namespace sqr
{

	//------------------------------------------------------------------------------
	CLocalMapAudioSaver::CLocalMapAudioSaver()
		: terrain(NULL)
	{
	}

	//------------------------------------------------------------------------------
	void
	CLocalMapAudioSaver::SaveAsSceneAudio(const string& path, bool bResize)
	{
		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();

		if( terrain->IsWater() )
		{
			MessageBox(NULL,"当前为水面层操作，请转换成地表层操作","提示",MB_OK);
			return;
		}

		string strFileName = path;
		locale loc;
		use_facet<ctype<TCHAR> >( loc ).tolower
			( &strFileName[0], &strFileName[strFileName.length()] );

		size_t npos = strFileName.rfind(".");
		if( npos != -1 )
			strFileName = strFileName.substr(0, npos);

		strFileName += ".audio";

		FILE * fp = NULL;
		fp = fopen(strFileName.c_str(),"wb");

		if (!fp)
		{
			string log = strFileName + "无法保存";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
			return;
		}
		else
		{
			DWORD version = AUDIO_PLAYTIME_INTERVAL;
			fwrite(&version, sizeof(version), 1, fp);

			locale::global(locale(""));

			//////////////////////////////////////////////////////////////////////////
			///save cue name
			int nNameLen = 0;
			int nGridCnt   = terrain->GetGridCount();
			string strCueName = "";
			size_t dwLen = 0;
			set<int> nGrindIndexs;

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
							nGrindIndexs.insert(i);
					}
				}
			}

			size_t size = nGrindIndexs.size();
			fwrite(&size,sizeof(size),1,fp);

			set<int>::iterator end = nGrindIndexs.end();
			for( set<int>::iterator beg = nGrindIndexs.begin(); beg != end; ++beg )
			{
				int nGridIndex = *beg;
				int nSaveGridIndex = 0;

				if( bResize )
					nSaveGridIndex = pOperator->m_OldMapGridIndexMap[nGridIndex];
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

							if( bResize )
								nSaveModelIndex = pOperator->m_OldMapGridIndexMap[n];
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
}