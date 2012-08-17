#include "stdafx.h"
#include "CMapEditLoginObj.h"
#include "Engine.h"
#include "CMapEditCamera.inl"
#include "CCameraManager.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "Picker.h"
#include "Operator.h"
#include "Operator.inl"
#include "ModelGroup.h"
#include "TerrainAdhereRenderer.h"
#include "Render.h"
#include "CGraphic.h"
#include "ToolRecord.h"
#include "CEditModel.h"
#include "CSkeletalsMgr.h"
#include "CRenderDebugFun.h"
#include "WireBox.h"
#include "CWindowTarget.h"
#include "CRenderGroup.h"

namespace sqr
{
	CMapEditLoginObj::CMapEditLoginObj() 
	{
		m_AniLoginFrameList.clear();
		m_AniNameList.clear();
		m_bPlayAgainFromStart = false;
	}

	CMapEditLoginObj::~CMapEditLoginObj()
	{
		m_AniLoginFrameList.clear();
		m_AniNameList.clear();
		this->Release();
	}

	void CMapEditLoginObj::Render()
	{
		DWORD dwCurrentTime = (uint32)GetProcessTime();

		CTerrainMesh *pTerrain  = CMapEditApp::GetInst()->GetEditingMesh();
		COperator	 *pOperator = CMapEditApp::GetInst()->GetOperator();

		if ( EES_MOVING == sModelCommonPro.eEditState )
		{
			int nGridIndexBlockCenter;

			if ( sModelCommonPro.bCameraOriented )
			{
				CVector3f v;

				if ( !CPicker::GetInst()->GetIntersected(v,&nGridIndexBlockCenter) )
				{
					nGridIndexBlockCenter = pOperator->GetGridIndexHot();
				}
			}
			else
			{
				nGridIndexBlockCenter = pOperator->GetGridIndexHot();
			}

			nGridIndexBlockCenter += ( sModelCommonPro.nXGroupOffset + sModelCommonPro.nZGroupOffset*pTerrain->GetWidth() );

			if ( !pTerrain->IsValid(nGridIndexBlockCenter) )
			{
				return;
			}

			SGrid &gridBlockCenter = pTerrain->GetGrid(nGridIndexBlockCenter);

			sModelCommonPro.vBlockCenter = gridBlockCenter.vCenter;

			int nImageCenterGridIndex = this->GetUserOffsetAdjustedGridIndex(gridBlockCenter.dwGridIndex);
			SGrid &gridImageCenter = pTerrain->GetGrid(nImageCenterGridIndex);
			sModelCommonPro.vImageCenter = gridImageCenter.vCenter;

			sModelCommonPro.matWorldOff.SetTranslate(sModelCommonPro.vBlockCenter.x, sModelCommonPro.vImageCenter.y, sModelCommonPro.vBlockCenter.z);

			sModelCommonPro.mat = sModelCommonPro.matSelfOff*sModelCommonPro.matUserOffset*sModelCommonPro.matWorldOff;
		}

		//this->SetAbsolute(sModelCommonPro.WireBoxMat);

		CEditModel::SetVisible(true);

		//sMaterial temp;

		//temp.Diffuse.a = 1.0f;
		//temp.Ambient.a = 1.0f;

		//temp.Diffuse.r = sModelCommonPro.fRedOverallAmbientRatio;
		//temp.Diffuse.g = sModelCommonPro.fGreenOverallAmbientRatio;
		//temp.Diffuse.b = sModelCommonPro.fBlueOverallAmbientRatio;

		//temp.Ambient.r = sModelCommonPro.fRedOverallAmbientRatio;
		//temp.Ambient.g = sModelCommonPro.fGreenOverallAmbientRatio;
		//temp.Ambient.b = sModelCommonPro.fBlueOverallAmbientRatio;

		//RenderObjStyle rs;

		//if ( sModelSavePro.bShadow )
		//{
		//	if ( sModelSavePro.bReceiveShadow )
		//		rs.m_eType = ROS_SHADOWRECEIVE_LIGHT1_SHADOW;
		//	else
		//		rs.m_eType = ROS_SHADOW_LIGHT1;
		//}
		//else
		//{
		//	if ( sModelSavePro.bReceiveShadow )
		//		rs.m_eType = ROS_SHADOWRECEIVE_LIGHT1;
		//	else
		//		rs.m_eType = ROS_LIGHTMAT1;
		//}

		//rs.m_bRenderLink = true;
		//rs.m_bRenderMesh = true;
		//rs.m_nParam1 = DELTARS_DIFFUSE|DELTARS_AMBIENT;
		//rs.m_nParam2 = (DWORD)&temp;

		this->ChangeAnimation();
		
		SetPosition(sModelCommonPro.mat.GetLoc());
		CEditModel::OnPreRender(dwCurrentTime,CMainWindowTarget::GetInst()->GetGroup()->GetCamera());
		CEditModel::Render();

		if ( !CCameraManager::GetInst()->GetMapEditCamera()->GetIsOrthogonal() &&
			this->GetLocked() || this->GetSelected() && (!sModelCommonPro.bEffectBase) )
		{
			if( m_pWireBox )
			{
				m_pWireBox->SetAbsolute(sModelCommonPro.WireBoxMat);
				CTerrainAdhereRenderer::GetInst()->AddRenderWireBox(m_pWireBox);
			}

			//RenderBigBoxDebug(this);
		}
	}

	void CMapEditLoginObj::ChangeAnimation()
	{
 		COperator	*pOperator    = CMapEditApp::GetInst()->GetOperator();
		int			nPlayState    = CToolRecord::GetInst()->GetState();
		int			nCurKey	      = CToolRecord::GetInst()->GetCurrentKey();
		int			nCurCamerPath = CToolRecord::GetInst()->GetCurrentAnim();
		//float		fCurTime	  = pOperator->ToolRecord_GetAnimKeyTime(nCurKey);
		//int			nCurFrame     = (int)(fCurTime * 30.0f + 0.5f);
 		static	int nLastCurKey = -1;
		static  string strLastAni = "stand01";
		static  bool bStartDefaultAni = true;
		//DebugOut("curkey is %d, nCurCamerPath = %d\n", nCurKey, nCurCamerPath);

 		if ( nPlayState == CToolRecord::PLAY_STATE_PLAYING )
 		{
			map<int, map<int16, string>>::iterator find = m_FrameAniLoginList.find(nCurCamerPath);
			if( find != m_FrameAniLoginList.end() )
			{
				map<int16, string> aniFrameMap = find->second;
				map<int16, string>::iterator iter = aniFrameMap.find(nCurKey);
				if( iter != aniFrameMap.end() )
				{
					string strAniName = iter->second;

					if( m_bPlayAgainFromStart )
					{
						//DebugOut("replay %s\n", strAniName.c_str());
						this->SetNextAnimation(strAniName.c_str(), true, 200, -1.0f);
						m_bPlayAgainFromStart = false;
					}
					else
					{
						if( strLastAni != strAniName )
						{
							if( nLastCurKey != nCurKey )
							{
								//DebugOut("curkey is %d, strAniName = %s, strLastAni = %s\n", nCurKey, strAniName.c_str(), strLastAni.c_str());
								this->SetNextAnimation(strAniName.c_str(), true, 200, -1.0f);
							}
						}

					}

					//DebugOut("pre curkey is %d, strAniName = %s, strLastAni = %s\n", nCurKey, strAniName.c_str(), strLastAni.c_str());
					strLastAni = strAniName;
					nLastCurKey = nCurKey;
					bStartDefaultAni = true;
					//DebugOut("last curkey is %d, strAniName = %s, strLastAni = %s\n", nCurKey, strAniName.c_str(), strLastAni.c_str());
					return;

				}
// 				else
// 				{
// 					if( nLastCurKey != nCurKey )
// 						this->SetNextAnimation("stand01", true, 200, -1.0f);
// 					//nLastCurKey = -1;
// 				}
			}
 		}
   		else
  		{
  			if( bStartDefaultAni )
 			{
   				this->SetNextAnimation(strLastAni.c_str(), true, 200, -1.0f);
 				bStartDefaultAni = false;
 			}
  			//nLastCurKey = -1;
  		}

		nLastCurKey = nCurKey;
	}

	void CMapEditLoginObj::SetNextAnimation( const TCHAR* szAniFileName, bool bLoop, int32 DelayTime, float AniSpeed )
	{
		CEditModel::SetNextAnimation(szAniFileName, FramePair(0,-1), bLoop, DelayTime, AniSpeed);
	}

	void CMapEditLoginObj::SetLoginAniByAgpFileName( string strAgpFileName )
	{
		if( strAgpFileName == "" )
			return;

		//CAnimationsRes * pAnimationGroup;
		CSkeletalFrame* pSke;
		CAnimationGroup* pAnimationGroup;
		string sAgpFile = strAgpFileName.substr(0,strAgpFileName.size()-4)+".agp";

		SQR_TRY
		{
			if (CGraphic::GetInst()->CreateAnimationGroup(sAgpFile.c_str(),&pAnimationGroup)==INVALID||CSkeletalsMgr::GetInst()->GetSkeInPool(pAnimationGroup->GetSkeletalFrameName(),&pSke) == INVALID  )
			{
				string log = strAgpFileName;
				log = log + "骨架或者动画列表文件没有找到";

				MessageBox(NULL, log.c_str(), "错误", MB_OK);
			}
			else
			{
				sModelCommonPro.strModelAgpName = strAgpFileName;

				SQR_TRY
				{
					CEditModel::SetSkeletalFrame(pSke);
					CEditModel::SetAnimateGroup(pAnimationGroup);
					if( pAnimationGroup )
					{
						string strAni = "";
						int nAniNum = pAnimationGroup->GetAnimationNum();
						for ( int i = 0; i < nAniNum; ++i )
						{
							strAni = pAnimationGroup->GetAnimationName(i);
							m_AniNameList.push_back(strAni);
						}
					}

					sModelSavePro.bPlayAnimate = true;
					sModelCommonPro.strModelSkeName = pAnimationGroup->GetSkeletalFrameName();
					this->SetNextAnimation("stand01", true, 200, -1.0f);
				}
				SQR_CATCH(exp)
				{
					CEditModel::ClearAnimateGroup();
					CEditModel::SetSkeletalFrame(NULL);
					CEditModel::SetAnimateGroup(NULL);
					MessageBox(NULL, exp.ErrorMsg(), "错误", MB_OK);
				}
				SQR_TRY_END;
					pAnimationGroup->Release();
			}
		}
		SQR_CATCH(exp)
		{
			MessageBox(NULL, exp.ErrorMsg(), "错误", MB_OK);
		}
		SQR_TRY_END;
	}

	void CMapEditLoginObj::SetAnimationFrame( const string& str, const int8 nFrame )
	{
		int	nCurCamerPath = CToolRecord::GetInst()->GetCurrentAnim();
		m_AniLoginFrameList[nCurCamerPath][str].push_back(nFrame);
		m_FrameAniLoginList[nCurCamerPath][nFrame] = str;
	}

	int16 CMapEditLoginObj::GetAnimationFrame( const string& str )
	{
		int	nCurCamerPath = CToolRecord::GetInst()->GetCurrentAnim();
		CamPathAniMap::iterator iter = m_AniLoginFrameList.find(nCurCamerPath);
		if( iter != m_AniLoginFrameList.end() )
		{
			AniFrameMap aniFrameMap = iter->second;
			if( aniFrameMap.find(str) != aniFrameMap.end() )
				return aniFrameMap[str][0];
		}

		return 10000;
	}

	vector<int16> CMapEditLoginObj::GetAnimationFrames(const string& str)
	{
		int	nCurCamerPath = CToolRecord::GetInst()->GetCurrentAnim();
		CamPathAniMap::iterator iter = m_AniLoginFrameList.find(nCurCamerPath);
		if( iter != m_AniLoginFrameList.end() )
		{
			AniFrameMap aniFrameMap = iter->second;
			if( aniFrameMap.find(str) != aniFrameMap.end() )
				return aniFrameMap[str];
		}

		vector<int16> frameList; 
		return frameList;
	}

	void CMapEditLoginObj::CleanAniFrames(const string& str)
	{
		int	nCurCamerPath = CToolRecord::GetInst()->GetCurrentAnim();
		CamPathAniMap::iterator iter = m_AniLoginFrameList.find(nCurCamerPath);
		if( iter != m_AniLoginFrameList.end() )
		{
			AniFrameMap &aniFrameMap = iter->second;
			AniFrameMap::iterator find = aniFrameMap.find(str);
			if( find != aniFrameMap.end() )
				aniFrameMap.erase(find);
		}

		m_FrameAniLoginList.clear();
		string strAniName = "";
		int nCamaerPathIndex = 0;
		AniFrameMap   aniFrameList;
		CamPathAniMap::iterator end = m_AniLoginFrameList.end();
		for ( CamPathAniMap::iterator beg = m_AniLoginFrameList.begin(); beg != end; ++beg )
		{
			nCamaerPathIndex = beg->first;
			aniFrameList = beg->second;
			for( AniFrameMap::iterator iter = aniFrameList.begin(); iter != aniFrameList.end(); ++iter )
			{
				strAniName = iter->first;
				vector<int16> aniList = iter->second;

				size_t size = aniList.size();
				for ( size_t i = 0; i< size; ++i )
					m_FrameAniLoginList[nCamaerPathIndex][aniList[i]] = strAniName;
			}
		}
	}

	void CMapEditLoginObj::DeleteAniFrame(const string& str, const int nCurrKey)
	{
		int	nCurCamerPath = CToolRecord::GetInst()->GetCurrentAnim();
		CamPathAniMap::iterator iter = m_AniLoginFrameList.find(nCurCamerPath);
		if( iter != m_AniLoginFrameList.end() )
		{
			AniFrameMap &aniFrameMap = iter->second;
			AniFrameMap::iterator find = aniFrameMap.find(str);
			if( find != aniFrameMap.end() )
			{
				vector<int16> &frameList = find->second;
				vector<int16>::iterator frameIter = frameList.begin();
				size_t size = frameList.size();
				for( size_t i = 0; i < size; ++i)
				{
					if( nCurrKey == frameList[i] )
					{
						frameList.erase(frameIter);
						if( size == 1 )
							aniFrameMap.erase(find);
						return;
					}
					++frameIter;
				}
			}
		}

		m_FrameAniLoginList.clear();
		string strAniName = "";
		int nCamaerPathIndex = 0;
		AniFrameMap   aniFrameList;
		CamPathAniMap::iterator end = m_AniLoginFrameList.end();
		for ( CamPathAniMap::iterator beg = m_AniLoginFrameList.begin(); beg != end; ++beg )
		{
			nCamaerPathIndex = beg->first;
			aniFrameList = beg->second;
			for( AniFrameMap::iterator iter = aniFrameList.begin(); iter != aniFrameList.end(); ++iter )
			{
				strAniName = iter->first;
				vector<int16> aniList = iter->second;

				size_t size = aniList.size();
				for ( size_t i = 0; i< size; ++i )
					m_FrameAniLoginList[nCamaerPathIndex][aniList[i]] = strAniName;
			}
		}
	}

	void CMapEditLoginObj::SetAniFrames( CamPathAniMap aniFrames )
	{
		m_AniLoginFrameList = aniFrames;

		string strAniName = "";
		int nCamaerPathIndex = 0;
		AniFrameMap   aniFrameList;
		CamPathAniMap::iterator end = aniFrames.end();
		for ( CamPathAniMap::iterator beg = aniFrames.begin(); beg != end; ++beg )
		{
			nCamaerPathIndex = beg->first;
			aniFrameList = beg->second;
			for( AniFrameMap::iterator iter = aniFrameList.begin(); iter != aniFrameList.end(); ++iter )
			{
				strAniName = iter->first;
				vector<int16> aniList = iter->second;

				size_t size = aniList.size();
				for ( size_t i = 0; i< size; ++i )
					m_FrameAniLoginList[nCamaerPathIndex][aniList[i]] = strAniName;
			}
		}
	}
}