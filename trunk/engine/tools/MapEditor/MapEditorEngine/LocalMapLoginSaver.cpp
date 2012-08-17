//------------------------------------------------------------------------------
//  CLocalMapLoginSaver.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "LocalMapLoginSaver.h"
#include "GraphicCommon.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "Operator.h"
#include "Operator.inl"
#include "TerrainMeshObject.h"
#include "CMapEditLoginObj.h"
#include "ModelGroup.h"



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
	CLocalMapLoginSaver::CLocalMapLoginSaver()
		: terrain(NULL)
	{
	}

	//------------------------------------------------------------------------------
	void
	CLocalMapLoginSaver::SaveAsSceneLogin(const string& path, set<int> nGridIndexs)
	{
		//string strFileName = path;
		//locale loc;
		//use_facet<ctype<TCHAR> >( loc ).tolower
		//	( &strFileName[0], &strFileName[strFileName.length()] );

		//size_t npos = strFileName.rfind(".");
		//if( npos != -1 )
		//	strFileName = strFileName.substr(0, npos);

		//strFileName += ".login";

		//locale::global(locale(""));
		//FILE * fp = NULL;
		//fp = fopen(strFileName.c_str(),"wb");

		//if (!fp)
		//{
		//	string log = strFileName + "无法保存";
		//	MessageBox(NULL,log.c_str(),"提示",MB_OK);
		//	return;
		//}
		//else
		//{
		//	DWORD version = LOGIN_OFFSET;
		//	fwrite(&version, sizeof(version), 1, fp);

		//	///login图素包
		//	CMapEditorManage::GetInst()->GetMapEditorFrame()->GetLoginView()->SaveProject(fp);

		//	///login model
		//	string strEffectName = "";
		//	int nGridIndex = 0;
		//	set<int>::iterator beg = nGridIndexs.begin();
		//	set<int>::iterator end = nGridIndexs.end();
		//	size_t size = nGridIndexs.size();

		//	fwrite(&size,sizeof(size),1,fp);

		//	for ( beg; beg != end; ++beg )
		//	{
		//		nGridIndex = *beg;
		//		SGrid * pGrid = &terrain->GetGrid(nGridIndex);
		//		if ( !pGrid->vecCenterObjects.empty() )
		//		{
		//			int16 dw = 0;
		//			long lCntOffset = ftell(fp);
		//			fwrite(&dw,sizeof(dw),1,fp);

		//			for ( vector< CTObjectPtr >::iterator iter = pGrid->vecCenterObjects.begin(); iter != pGrid->vecCenterObjects.end(); ++iter)
		//			{
		//				CTObjectPtr p = (*iter);

		//				CEditModelGroup *pModelGroup = p->GetModelGroup();
		//				CMapEditLoginObj *pLoginObj = static_cast<CMapEditLoginObj*>(pModelGroup->m_vecSelfPresentation[0]);

		//				///login model 跳过
		//				if( pModelGroup->m_bLoginModel )
		//				{
		//					//////////////////////////////////////////////////////////////////////////
		//					///offset
		//					CVector3f vUserOffsetEx = pModelGroup->GetUserOffsetEx();
		//					fwrite(&vUserOffsetEx,sizeof(CVector3f),1,fp);
		//					//////////////////////////////////////////////////////////////////////////

		//					//////////////////////////////////////////////////////////////////////////
		//					///logic name
		//					string strLogicName = pModelGroup->GetLoginModelLogicName();

		//					DWORD dwLen = strLogicName.size();
		//					fwrite(&dwLen,sizeof(int),1,fp);
		//					fwrite(strLogicName.c_str(),dwLen,1,fp);
		//					//////////////////////////////////////////////////////////////////////////

		//					//////////////////////////////////////////////////////////////////////////
		//					//default
		//					// effect bound?
		//					bool bEffectBased = pModelGroup->GetIsEffectBased();
		//					bool bEffecIsExit = pModelGroup->GetEffectName(strEffectName);
		//					if ( bEffectBased && bEffecIsExit )
		//					{
		//						bool b = true;
		//						fwrite(&b,sizeof(bool),1,fp);
		//						DWORD dwLen = strEffectName.size();
		//						fwrite(&dwLen,sizeof(int),1,fp);
		//						fwrite(strEffectName.c_str(),dwLen,1,fp);
		//					}
		//					else
		//					{
		//						bool b = false;
		//						fwrite(&b,sizeof(bool),1,fp);
		//					}

		//					dw++;

		//					string strItemName = pModelGroup->GetItemName();

		//					dwLen = strItemName.size();
		//					fwrite(&dwLen,sizeof(int),1,fp);
		//					fwrite(strItemName.c_str(),dwLen,1,fp);

		//					//////////////////////////////////////////////////////////////////////////

		//					int n = p->GetCenterGridIndex();
		//					fwrite(&n,sizeof(int),1,fp);

		//					//y 轴
		//					float fRotation = pModelGroup->GetRotation();
		//					fwrite(&fRotation,sizeof(float),1,fp);

		//					int nRotationCnt = pModelGroup->GetRotationCount();
		//					fwrite(&nRotationCnt,sizeof(int),1,fp);

		//					//////////////////////////////////////////////////////////////////////////
		//					//新版本新增的绕x，z轴渲染，物体缩放信息
		//					//z 轴
		//					float fPitch = pModelGroup->GetPitch();
		//					fwrite(&fPitch,sizeof(float),1,fp);

		//					int nPitchCnt = pModelGroup->GetPitchCount();
		//					fwrite(&nPitchCnt,sizeof(int),1,fp);

		//					//x 轴
		//					float fYaw = pModelGroup->GetYaw();
		//					fwrite(&fYaw,sizeof(float),1,fp);

		//					int nYawCnt = pModelGroup->GetYawCount();
		//					fwrite(&nYawCnt,sizeof(int),1,fp);

		//					//缩放
		//					float xScale = pModelGroup->GetScaleX();
		//					fwrite(&xScale,sizeof(float),1,fp);
		//					float yScale = pModelGroup->GetScaleY();
		//					fwrite(&yScale,sizeof(float),1,fp);
		//					float zScale = pModelGroup->GetScaleZ();
		//					fwrite(&zScale,sizeof(float),1,fp);
		//					//////////////////////////////////////////////////////////////////////////

		//					BYTE byRenderStyle = pModelGroup->GetRenderStyle();
		//					fwrite(&byRenderStyle,sizeof(BYTE),1,fp);

		//					int nAtten = pModelGroup->GetAtten();
		//					fwrite(&nAtten,sizeof(int),1,fp);

		//					bool bShadow = p->IsCastShadow();
		//					fwrite(&bShadow,sizeof(bool),1,fp);

		//					///写是否接收阴影信息
		//					bool bReceiveShadow = pModelGroup->IsReceiveShadow();
		//					fwrite(&bReceiveShadow,sizeof(bool),1,fp);

		//					///是否消隐,登录模型应该不需要消隐的吧
		//					bool bTrans = false;
		//					fwrite(&bTrans,sizeof(bTrans),1,fp);

		//					///mirror type
		//					int nMirrorType = pModelGroup->GetMirrorType();
		//					fwrite(&nMirrorType, sizeof(nMirrorType), 1, fp);

		//					///nAffectRadio
		//					int nRatio = pModelGroup->GetAffectRatio();
		//					fwrite( &nRatio, sizeof(int), 1, fp);

		//					//////////////////////////////////////////////////////////////////////////
		//					///Frame
		//					CamPathAniMap allAniLoginFrameList = pLoginObj->m_AniLoginFrameList;
		//					CamPathAniMap::iterator beg = allAniLoginFrameList.begin();
		//					CamPathAniMap::iterator end = allAniLoginFrameList.end();
		//					AniFrameMap   aniFrameList;
		//					vector<int16> frameList;
		//					size_t aniSize    = 0;
		//					size_t allAniSize = allAniLoginFrameList.size();
		//					int16   frm = 0;

		//					fwrite(&allAniSize,sizeof(allAniSize),1,fp);
		//					for ( beg = allAniLoginFrameList.begin(); beg != end; ++beg )
		//					{
		//						int nCameraPathIndex = beg->first;
		//						fwrite(&nCameraPathIndex,sizeof(nCameraPathIndex),1,fp);

		//						aniFrameList = beg->second;
		//						aniSize = aniFrameList.size();
		//						
		//						fwrite(&aniSize,sizeof(aniSize),1,fp);
		//						for( AniFrameMap::iterator iter = aniFrameList.begin(); iter != aniFrameList.end(); ++iter )
		//						{
		//							string str = iter->first;
		//							frameList = iter->second;

		//							DWORD dwLen = str.size();
		//							fwrite(&dwLen,sizeof(dwLen),1,fp);
		//							fwrite(str.c_str(),dwLen,1,fp);

		//							size_t size = frameList.size();
		//							fwrite(&size,sizeof(size),1,fp);
		//							for ( size_t m = 0; m < size; ++m )
		//							{
		//								frm = frameList[m];
		//								fwrite(&frm,sizeof(frm),1,fp);
		//							}
		//						}
		//					}
		//					//////////////////////////////////////////////////////////////////////////
		//				}
		//			}

		//			long lNow = ftell(fp);
		//			fseek(fp,lCntOffset,SEEK_SET);
		//			fwrite(&dw,sizeof(dw),1,fp);
		//			fseek(fp,lNow,SEEK_SET);
		//		}
		//		else
		//		{
		//			int16 dw = 0;
		//			fwrite(&dw,sizeof(dw),1,fp);
		//		}
		//	}

		//	fclose(fp);
		//	locale::global(loc);//恢复全局locale	

		//	//MessageBox(NULL,"场景登录信息保存完毕","提示",MB_OK);
		//}
	}

//!namespace
}