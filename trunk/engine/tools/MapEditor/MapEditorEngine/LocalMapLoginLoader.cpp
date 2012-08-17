//------------------------------------------------------------------------------
//  CLocalMapLoginLoader.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "LocalMapLoginLoader.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "Operator.h"
#include "Operator.inl"
#include "TerrainMeshObject.h"
#include "CMapEditLoginObj.h"
#include "ModelGroup.h"
#include "ToolSetModel.h"
#include "ToolSetModel.h"
#include "UserMsg.h"



enum
{
	INVALID_VERSION,
	LOGIN_DEFAULT,
	LOGIN_LOGICNAME,
	LOGIN_OFFSET,
};

namespace sqr
{

	//------------------------------------------------------------------------------
	CLocalMapLoginLoader::CLocalMapLoginLoader()
		: terrain(NULL), m_dwVersion(0)
	{
	}

	//------------------------------------------------------------------------------
	void
	CLocalMapLoginLoader::LoadAsSceneLogin(const string& path)
	{
//		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
//		size_t npos = path.rfind(".");
//		string loginPath = path.substr(0, npos);
//		loginPath += ".login";
//
//		FILE * fp = NULL;
//		fp = fopen(loginPath.c_str(),"rb");
//
//		if (! fp )
// 		{
//// 			string log = loginPath + "无法打开";
//// 
//// 			MessageBox(NULL,log.c_str(),"请换其他地图",MB_OK);
//			return;
//		}
//		else
//		{
//			locale loc = locale::global(locale(""));
//
//			fread(&m_dwVersion, sizeof(m_dwVersion), 1, fp);
//
//			///login图素包
//			CMapEditorManage::GetInst()->GetMapEditorFrame()->GetLoginView()->LoadProject(fp);
//				
//			///login model info
//			this->LoadLoginModelInfo(fp);
//
//			fclose(fp);
//			locale::global(loc);//恢复全局locale	
//		}
	}

	void CLocalMapLoginLoader::LoadLoginModelInfo( FILE *fp )
	{
		locale loc;
		COperator *pOperator = NULL;
		pOperator = CMapEditApp::GetInst()->GetOperator();
		if ( pOperator == NULL )
			return;

		HWND hLoginSetView = CToolSetModel::GetInst()->GetLoginSetView();
		size_t modelsize = 0;
		fread(&modelsize, sizeof(modelsize), 1, fp);

		for ( size_t i = 0; i < modelsize; ++i )
		{
			CVector3f vUserOffsetEx; // (0.0f,0.0f,0.0f);
			string strLogicName = "";
			int16 dw = 0;
			fread(&dw,sizeof(dw),1,fp);
			for (int16 j = 0; j < dw; ++j)
			{
				switch (m_dwVersion)
				{
				case LOGIN_OFFSET:
					{
						fread(&vUserOffsetEx,sizeof(CVector3f),1,fp);
					}

				case LOGIN_LOGICNAME:
					{
						DWORD dwItemNameLen;
						fread(&dwItemNameLen,sizeof(DWORD),1,fp);
						strLogicName.resize(dwItemNameLen);
						fread((TCHAR*)strLogicName.data(),dwItemNameLen,1,fp);
					}

				case LOGIN_DEFAULT:
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

						int nGridIndex = 0;
						fread(&nGridIndex,sizeof(int),1,fp);

						pOperator->m_LoginModelGridIndexs[nGridIndex] = strItemName;
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

						///mirror type
						int nMirrorType = 0, nRatio = 0;
						fread(&nMirrorType, sizeof(nMirrorType), 1, fp);

						///nAffectRadio
						fread( &nRatio, sizeof(int), 1, fp);

						EAUTO_GENERATION_TYPE eAutoGenerationType = EAGT_NONE;
						BYTE bTransState = 0;
						bool bSecondShadow = false;

						//////////////////////////////////////////////////////////////////////////
						///frame list
						CamPathAniMap allAniLoginFrameList;
						size_t allAniSize = 0, aniSize = 0, frameSize = 0;
						int16   frm = 0;
						int		nCameraPathIndex = 0;
						DWORD  dwLen = 0;
						string strAniName;

						fread(&allAniSize,sizeof(allAniSize),1,fp);
						for( size_t n = 0; n < allAniSize; ++n )
						{
							AniFrameMap   aniFrameList;
							fread(&nCameraPathIndex,sizeof(nCameraPathIndex),1,fp);

							fread(&aniSize,sizeof(aniSize),1,fp);
							for ( size_t m = 0; m < aniSize;  ++m )
							{
								fread(&dwLen,sizeof(dwLen),1,fp);
								strAniName.resize(dwLen);
								fread((TCHAR*)strAniName.data(),dwLen,1,fp);

								fread(&frameSize,sizeof(frameSize),1,fp);
								for( size_t n = 0; n < frameSize; ++n )
								{
									fread(&frm,sizeof(frm),1,fp);
									aniFrameList[strAniName].push_back(frm);
								}
							}

							allAniLoginFrameList[nCameraPathIndex] = aniFrameList;
						}
						//////////////////////////////////////////////////////////////////////////

						if ( nGridIndex > 0 || !terrain->IsValid(nGridIndex) )	
						{
							size_t size = 0;
							map<string, int>::iterator iter = pOperator->m_LoginLoginItemNameMap.find(strItemName);
							if( iter != pOperator->m_LoginLoginItemNameMap.end() )
								size = iter->second;

							if( strLogicName == "" )
							{
								TCHAR temp[64];
								sprintf( temp, "%d", size );
								string str = temp;
								strLogicName = strItemName + "_" + str;
							}

							use_facet<ctype<TCHAR> >( loc ).tolower
								( &strLogicName[0], &strLogicName[strLogicName.length()] );

							pOperator->m_LoginLoginItemNameMap[strItemName] = ++size;

							::SendMessage(hLoginSetView,WM_LOGINVIEW_REBIRTH,(WPARAM)(strItemName).c_str(),0);

							CEditModelGroup *pModelGroup = NULL;
							CTObject *pTObject = NULL;
							pTObject = CToolSetModel::GetInst()->GetLastActiveObject();
							if( pTObject )
							{
								pModelGroup = pTObject->GetModelGroup();
								if( pModelGroup == NULL )
									continue;
								CMapEditLoginObj *pLoginObj = static_cast<CMapEditLoginObj*>(pModelGroup->m_vecSelfPresentation[0]);
								pLoginObj->SetAniFrames(allAniLoginFrameList);

								pLoginObj->SetLoginModelLogicName(strLogicName);
								pOperator->m_LoginLoginNameList.insert(strLogicName);

								pModelGroup->SetUserOffsetEx(vUserOffsetEx);
								if( !CToolSetModel::GetInst()->Rebirth(nGridIndex, bTransState,nMirrorType,nRatio, fRotation, nRotationCnt, fPitch, nPitchCnt, fYaw, nYawCnt, xScale, yScale, zScale,
									0, 1.0f, eAutoGenerationType, byRenderStyle,nAtten,bSecondShadow,bShadow,bTrans) )
								{
									string log = strItemName + "创建失败";
									MessageBox(NULL, log.c_str(),"错误",MB_OK);
								}
							}

						}
					}
				}
			}
		}
	}
}