#include "stdafx.h"
#include "ToolMainPlayer.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "Picker.h"
#include "Engine.h"
#include "CEditModel.h"
#include "Render.h"
#include "CMapEditCamera.inl"
#include "SceneRegionMgr.h"
#include "Operator.h"
#include "Operator.inl"
#include "CGraphic.h"
#include "CCameraManager.h"
#include "CGameGacCamera.h"
#include "IAudioModule.h"
#include "IDName.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "EditStateOpertiaonMgr.h"
#include "SceneAudioMgr.h"
#include "CSkeletalsMgr.h"
#include "CRenderPipeline.h"
#include "CEditContext.h"

const float colorFloat = 1.0f / 255.0f;
namespace sqr
{
	CToolMainPlayer::CToolMainPlayer()
	{
		m_mat.Identity();
		m_vPos.x = m_vPos.y = m_vPos.z = 0.0f;
		m_vDir.x = m_vDir.y = m_vDir.z = 0.0f;
		m_vStart = m_vPos;
		m_fSpeed = 300.0f;
		m_fRoad = 0.0f;
		m_bStop = true;
		m_bBlock = true;
		m_fRed = m_fGreen = m_fBlue = 0.5f;

		m_bLoadMainPlayer = false;
		m_bHideMainPlayerShadow = false;
		m_lastKeyFrame = 0xffff;
		m_strCurMatCueName = "";
		m_bHumanPlayer = false;

		m_pMainPlayer = NULL;
	}

	CToolMainPlayer::~CToolMainPlayer()
	{
	}

	void CToolMainPlayer::UpdateFPSLimited()
	{
		COperator *pOperator = NULL;
		pOperator = CMapEditApp::GetInst()->GetOperator();
		if( pOperator == NULL )
			return;

		CTerrainMesh *pTerrainMesh = NULL;
		pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrainMesh == NULL )
			return;

		if ( !m_bStop && (m_vPos-m_vStart).Mag() < m_fRoad )
		{
			// update position
			m_vPos += m_vDir*m_fSpeed*0.05f;

			int nX = int(m_vPos.x / GRID_SPACE);
			int nZ = int(m_vPos.z / GRID_SPACE);

			DWORD gridIndex = nZ*pTerrainMesh->GetWidth() + nX;
			if( !pTerrainMesh->IsValid(gridIndex) )
				return;

			SGrid &grid = pTerrainMesh->GetGrid(gridIndex);
			int8  nMaterialIndex = grid.nMaterialIndex;
			m_strCurMatCueName = CSceneAudioMgr::GetInst()->GetPhyleMaterialName(nMaterialIndex);

			// logical block
			if ( grid.nBlockType > 1 && m_bBlock )
			{
				m_bStop = true;

				m_vPos -= m_vDir*m_fSpeed*0.05f;

				if ( m_ani != "stand01" )
				{
					m_pMainPlayer->SetNextAnimation("stand01",FramePair(0,-1),true,200,1.0f);
					m_ani = "stand01";
				}
			}

			// stand height
			m_vPos.y = CPicker::GetInst()->GetHeight(m_vPos.x, m_vPos.z);

			m_mat._41 = m_vPos.x;
			m_mat._42 = m_vPos.y;
			m_mat._43 = m_vPos.z;

 			if( CEditStateOpertiaonMgr::GetInst()->GetIsGameView() )
 				CCameraManager::GetInst()->GetGameGacCamera()->SetCameraTarget(m_vPos);
 			else
 				CCameraManager::GetInst()->GetMapEditCamera()->MoveToRTSByView(m_vPos);

			this->UpdatePlayerMaterial(gridIndex);
		}
		else
		{
			if ( !m_bHumanPlayer && m_ani != "stand01" )
			{
				if( m_pMainPlayer->SetNextAnimation("stand01",FramePair(0,-1),true,200,1.0f) )
					m_ani = "stand01";
			}
			else
			{
				if ( m_bHumanPlayer && m_ani != "stand01_y" )
				{
					if( m_pMainPlayer->SetNextAnimation("stand01_y",FramePair(0,-1),true,200,1.0f) )
						m_ani = "stand01_y";
				}
			}
		}
	}

#pragma region UpdateMat
	void CToolMainPlayer::UpdatePlayerMaterial(const int gridIndex)
	{
		//////////////////////////////////////////////////////////////////////////
		if( CSceneEnvMgr::GetInst()->ShadowedColorIsEmpty() )
		{
			CTerrainMesh *pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

			// grid color
			int width = pTerrainMesh->GetWidth();

			vector<DWORD> vecGridIndices;

			DWORD index = gridIndex - width;

			vecGridIndices.push_back(index - 1);
			vecGridIndices.push_back(index);
			vecGridIndices.push_back(index + 1);

			vecGridIndices.push_back(gridIndex - 1);
			vecGridIndices.push_back(gridIndex);
			vecGridIndices.push_back(gridIndex + 1);

			index = gridIndex + width;

			vecGridIndices.push_back(index - 1);
			vecGridIndices.push_back(index);
			vecGridIndices.push_back(index + 1);

			DWORD Red = 0;
			DWORD Green = 0;
			DWORD Blue = 0;

			size_t vecSize = vecGridIndices.size();

			for ( size_t i = 0; i < vecSize; ++i )
			{
				if( !pTerrainMesh->IsValid(vecGridIndices[i]) )
					continue;

				SGrid &Grid = pTerrainMesh->GetGrid(vecGridIndices[i]);
				for ( int k = 0; k < 4; ++k )
				{
					SVertex & vertex = pTerrainMesh->GetVertex(Grid.dwVertexIndex[k]);
					Red += VERTEX_COLOR;
					Green += VERTEX_COLOR;
					Blue += VERTEX_COLOR;
				}
			}

			m_fRed   = Red/(4*vecSize*255.0f);
			m_fGreen = Green/(4*vecSize*255.0f);
			m_fBlue  = Blue/(4*vecSize*255.0f);
		}
		else
		{
			int nX = int(m_vPos.x / GRID_SPACE);
			int nZ = int(m_vPos.z / GRID_SPACE);
			CColor shadow   = CSceneEnvMgr::GetInst()->GetShadowedColor(nX, nZ);
			CColor diffuse  = shadow.Modulate(0xff808080);///应该是Modulate格子顶点材质色
			m_fRed			= diffuse.R * colorFloat;
			m_fGreen		= diffuse.G * colorFloat;
			m_fBlue			= diffuse.B * colorFloat;
		}
		//////////////////////////////////////////////////////////////////////////
	}
#pragma endregion UpdateMat

	void CToolMainPlayer::OnEvent(const CMsg& msg)
	{
		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		switch ( msg.msgSource )
		{
		case MGS_MOUSEMSG:
			{
				if( msg.msgType.key_type == MGT_MOUSE_LBNDOWN )
				{
					if ( m_ani != "" && m_ani != "run01" )
					{
						if( !m_pMainPlayer->SetNextAnimation("run01",FramePair(0,-1),true,200,1.0f) )
						{
							if( m_pMainPlayer->SetNextAnimation("run01_y",FramePair(0,-1),true,200,1.0f) )
								m_ani = "run01_y";
						}
						else
							m_ani = "run01";
					}
					else
					{
						m_pMainPlayer->SetNextAnimation("stand01",FramePair(0,-1),true,200,1.0f) ;
					}

					if (  pTerrainMesh->IsValid(pOperator->GetGridIndexHot()) )
					{
						m_bStop = false;

						m_vEnd = pOperator->GetHitPoint();

						//实时更新区域值
						CSceneRegionMgr::GetInst()->CalcRegionLightByRegionDistance(m_vEnd);

						m_vStart = m_vPos;

						m_fRoad = (m_vEnd-m_vStart).Mag();

						m_vDir = m_vEnd - m_vPos;
						m_vDir.y = 0.0f;

						m_vDir.normalize();

						m_mat._11 = m_vDir.z;
						m_mat._13 = -m_vDir.x;

						m_mat._31 = m_vDir.x;
						m_mat._33 = m_vDir.z;
					}
				}
			}
			break;
		}
	}

	void CToolMainPlayer::Restart()
	{
		CPicker::GetInst()->GetIntersected(m_vPos);
		m_mat._41 = m_vPos.x;
		m_mat._42 = m_vPos.y;
		m_mat._43 = m_vPos.z;
	}

	void CToolMainPlayer::Render()
	{
		m_pMainPlayer->SetVisible(true);
		DWORD dwCurrentTime = (uint32)GetProcessTime();

		CGraphic * pCoreGraphic = CGraphic::GetInst();

		CColor color;

		color.A = 255;
		color.R = uint8(m_fRed * 255.0f);
		color.G = uint8(m_fGreen * 255.0f);
		color.B = uint8(m_fBlue * 255.0f);

		m_pMainPlayer->SetAmbient(color);
		m_pMainPlayer->SetDiffuse(color);

		m_pMainPlayer->OnPreRender(dwCurrentTime,NULL);
		m_pMainPlayer->SetFixMatrix(m_mat);
		m_pMainPlayer->SetVisible(true);
		
		m_pMainPlayer->SetRenderStyle(m_bHideMainPlayerShadow ? ROS_LIGHTMAT2 : ROS_SHADOW_LIGHT2);
		m_pMainPlayer->Render();
		
		// key frame handle
		WORD curFrame = (WORD)m_pMainPlayer->GetCurFrameNum();
		if (curFrame != m_lastKeyFrame)
		{
			this->OnKeyFrame(&m_pMainPlayer->GetCurAnimateName());
			m_lastKeyFrame = curFrame;
		}
	}

	void CToolMainPlayer::UnRender()
	{
		if( m_pMainPlayer )
			m_pMainPlayer->SetVisible(false);
	}

	void CToolMainPlayer::CreateModel()
	{
		m_pMainPlayer = CMapEditApp::GetInst()->GetRenderScene()->CreateEditModel(NULL);
	}

	void CToolMainPlayer::SetMainPlayerModel(string & str)
	{
		if ( str == "" || m_pMainPlayer == NULL )
			return;

		if( m_strMainPlayerModel !=  str )
			m_pMainPlayer->DelFramework();

		m_strMainPlayerModel = str;

		m_pMainPlayer->ClearMeshPiece();

		CPieceGroup  *pPieceGroup = NULL;

		CGraphic * pCoreGraphic = CGraphic::GetInst();

		SQR_TRY
		{
			if ( pCoreGraphic->CreatePieceGroup(str.c_str(), &pPieceGroup) == INVALID )
			{
				string log = str;
				log = log + "主角模型文件没有找到";

				MessageBox(NULL, log.c_str(), "错误", MB_OK);

				return;
			}
		}
		SQR_CATCH(exp)
		{
			MessageBox(NULL, exp.ErrorMsg(), "错误", MB_OK);

			return;
		}
		SQR_TRY_END;

		for ( size_t i = 0; i < pPieceGroup->GetPieceClassNum(); i++)
		{
			m_pMainPlayer->AddMeshPiece(pPieceGroup,pPieceGroup->GetPieceClassName(i),0);
		}

		m_bLoadMainPlayer = true;

		pPieceGroup->Release();
	}

	void CToolMainPlayer::SetMainPlayerAni(string & str)
	{
		if ( !m_bLoadMainPlayer )
			return;

		if ( str == "" )
			return;

		m_strMainPlayerAni = str;
		string sAgpFile = str.substr(0,str.size()-4)+".agp";

		SQR_TRY
		{
			CSkeletalFrame* pSke = NULL;
			CAnimationGroup* pAniGroup = NULL;
			if ( CGraphic::GetInst()->CreateAnimationGroup(sAgpFile.c_str(),&pAniGroup) == INVALID )
			{
				string log = str;
				log = log + "主角动画列表文件没有找到";

				MessageBox(NULL, log.c_str(), "错误", MB_OK);

				return;
			}

			string sSkeFile = pAniGroup->GetSkeletalFrameName();
			if ( CSkeletalsMgr::GetInst()->GetSkeInPool(sSkeFile.c_str(),&pSke) == INVALID )
			{
				string log = str;
				log = log + "主角骨架文件没有找到";

				MessageBox(NULL, log.c_str(), "错误", MB_OK);

				return;
			}

			m_pMainPlayer->ClearAnimateGroup();
			m_pMainPlayer->SetSkeletalFrame(pSke);
			m_pMainPlayer->SetAnimateGroup(pAniGroup);

			m_ani = "stand01";
			if( m_pMainPlayer->SetNextAnimation(m_ani.c_str(),FramePair(0,-1),true,200,1.0f) )
				m_bHumanPlayer = false;
			else
			{
				m_ani = "stand01_y";
				if( m_pMainPlayer->SetNextAnimation(m_ani.c_str(),FramePair(0,-1),true,200,1.0f) )
					m_bHumanPlayer = true;
			}

			pAniGroup->Release();
		}
		SQR_CATCH(exp)
		{
			MessageBox(NULL, exp.ErrorMsg(), "错误", MB_OK);
		}
		SQR_TRY_END;
	}

	void CToolMainPlayer::Save(FILE * fp)
	{
		string str = m_strMainPlayerModel;

		DWORD dwFileNameLen = str.length();

		fwrite(&dwFileNameLen,sizeof(DWORD),1,fp);
		fwrite(str.c_str(),dwFileNameLen,1,fp);

		str = m_strMainPlayerAni;
		dwFileNameLen = str.length();

		fwrite(&dwFileNameLen,sizeof(DWORD),1,fp);
		fwrite(str.c_str(),dwFileNameLen,1,fp);
	}

	void CToolMainPlayer::Load(FILE * fp)
	{
		DWORD dwFileNameLen;

		fread(&dwFileNameLen,sizeof(DWORD),1,fp);
		m_strMainPlayerModel.resize(dwFileNameLen);
		fread((TCHAR*)m_strMainPlayerModel.data(),dwFileNameLen,1,fp);

		fread(&dwFileNameLen,sizeof(DWORD),1,fp);
		m_strMainPlayerAni.resize(dwFileNameLen);
		fread((TCHAR*)m_strMainPlayerAni.data(),dwFileNameLen,1,fp);

		
	}

	void CToolMainPlayer::Init(void)
	{
		this->SetMainPlayerModel(m_strMainPlayerModel);
		this->SetMainPlayerAni(m_strMainPlayerAni);
	}


	CVector3f CToolMainPlayer::GetCurPlayerPosition()
	{
		CVector3f pos = CVector3f(m_mat._41, m_mat._42, m_mat._43);
		return pos;
	}

	//------------------------------------------------------------------------------
	void
	CToolMainPlayer::OnKeyFrame( const IDNAME* name )
	{
		if( m_strCurMatCueName.empty() )
			return;

		if (NULL == name)
			return;

		const TCHAR* strName = *name;
		static const IDNAME FootStep("f");///f == footstep
		if (FootStep == *name)
		{
			PlayCue(m_strCurMatCueName.c_str());
		}
	}
}
