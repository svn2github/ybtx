#include "stdafx.h"
#include "CMapEditAudioObj.h"
#include "Engine.h"
#include "CMapEditCamera.inl"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "Picker.h"
#include "Operator.h"
#include "Operator.inl"
#include "ModelGroup.h"
#include "WireBox.h"
#include "TerrainAdhereRenderer.h"
#include "IAudioModule.h"
#include "CCameraManager.h"
#include "MEMath.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "CEditModel.h"
#include "CRenderDialog.h"
#include "SQRRootWnd.h"
#include "CMapAudioDialog.h"
#include "CRenderDebugFun.h"
#include "CWindowTarget.h"
#include "CRenderGroup.h"

const float MatRGBWeight = 128.0f / 255.0f;

namespace sqr
{
	CMapEditAudioObj::CMapEditAudioObj() 
	{
		m_pAEmitter				= GetAudioModule()->CreateAudioEmitter();
		m_pAEmitter->SetAutoStop(true);
		m_dwPlayTimeLenMin		= 0;
		m_dwPlayTimeLenMax		= 0;
		m_dwPlayIntervalMin		= 0;
		m_dwPlayIntervalMax		= 0;
		m_dwPlayTime			= 0;
		m_dwIntervalTime		= 0;
		m_dwCurRandomPlayTime	= 0;
		m_dwCurRandomInterval	= 0;
		m_bNeedPlayBegin		= false;
		m_bNeedIntervalBegin	= false;
	}

	CMapEditAudioObj::~CMapEditAudioObj()
	{
		m_pAEmitter = NULL;
		this->Release();
		delete m_pObjectDialog;
	}

	void CMapEditAudioObj::Render()
	{
		if( !CSceneEnvMgr::GetInst()->GetIsPlaySceneCues() )
		{
			if( m_pAEmitter->IsPlaying() )
				m_pAEmitter->Stop();

			return;
		}

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

			if( m_pWireBox )
				m_pWireBox->SetAbsolute(sModelCommonPro.matPieceOff*sModelCommonPro.matUserOffset*sModelCommonPro.matRotY*sModelCommonPro.matScale*sModelCommonPro.matWorldOff);
		}
			
		bool bStop = this->PlaySceneCue(dwCurrentTime);

		CColor color;
		color.A = 255;
		///如果是强势stop，呈绿色
		if( sModelCommonPro.bStopCue )
		{
			color.R = 0;
			color.G = 255;
			color.B = 0;
		}
		else
		{
			///暂停呈天蓝色
			if( bStop )
			{
				color.R = 0;
				color.G = 255;
				color.B = 255;
			}
			else///播放呈蓝色
			{
				color.R = 0;
				color.G = 0;
				color.B = 255;
			}
		}

		RenderObjStyle rs;
		rs.m_eType = ROS_LIGHTMAT1;

		rs.m_bRenderLink = true;
		rs.m_bRenderMesh = true;
		rs.m_nParam1 = DELTARS_DIFFUSE|DELTARS_AMBIENT;
		//rs.m_nParam2 = (DWORD)&temp;
		
		CEditModel::SetAmbient(color);
		CEditModel::SetDiffuse(color);

		CEditModel::OnPreRender(dwCurrentTime,CMainWindowTarget::GetInst()->GetGroup()->GetCamera());
		//CEditModel::SetFixMatrix(sModelCommonPro.mat);
		SetPosition(sModelCommonPro.mat.GetLoc());
		//CEditModel::SetVisible(CMainWindowTarget::GetInst()->GetGroup()->GetCamera() != 0);
		CEditModel::Render();

#ifdef USE_GUI_DIALOG
		return;
#else
		if ( !CCameraManager::GetInst()->GetMapEditCamera()->GetIsOrthogonal() &&
			this->GetLocked() || this->GetSelected() && (!sModelCommonPro.bEffectBase) )
		{
		if ( m_pWireBox )
		{
			m_pWireBox->SetAbsolute(sModelCommonPro.WireBoxMat);
			CTerrainAdhereRenderer::GetInst()->AddRenderWireBox(m_pWireBox);
		}
		//	RenderBigBoxDebug(this);
		}
#endif

	}

	bool CMapEditAudioObj::IsNeedPlay()
	{
		bool b = m_pAEmitter->IsPlaying();
		return !b;
	}

	///返回值表示该音效是否为间隔暂停状态
	bool CMapEditAudioObj::PlaySceneCue(DWORD dwTime)
	{
		if( m_strSceneCueName.empty() )
			return false;

		CVector3f offset(sModelCommonPro.mat._41, sModelCommonPro.mat._42, sModelCommonPro.mat._43);
		m_pAEmitter->SetPosition(offset);

		if( m_dwPlayIntervalMax != 0 && m_dwPlayIntervalMin != 0 && m_dwPlayTimeLenMin != 0 && m_dwPlayTimeLenMax != 0 )
		{
			if( !sModelCommonPro.bStopCue )
			{
				if( m_bNeedPlayBegin )
				{
					m_dwPlayTime = dwTime;
					m_bNeedPlayBegin = false;
				}

				DWORD diff = dwTime - m_dwPlayTime;
				if( diff < m_dwCurRandomPlayTime )
				{
					if (IsNeedPlay())
						m_pAEmitter->Play(m_strSceneCueName.c_str());
				}
				else
				{
					bool bIsPlaying = m_pAEmitter->IsPlaying();
					if( bIsPlaying )
						m_pAEmitter->Stop();

					if( m_bNeedIntervalBegin )
					{
						m_dwIntervalTime = dwTime;
						m_bNeedIntervalBegin = false;
					}

					diff = dwTime - m_dwIntervalTime;
					if( diff > m_dwCurRandomInterval )
					{
						m_dwCurRandomPlayTime = sqr::GetRandomInt(m_dwPlayTimeLenMin,  m_dwPlayTimeLenMax);
						m_dwCurRandomInterval = sqr::GetRandomInt(m_dwPlayIntervalMin, m_dwPlayIntervalMax);
						
						m_bNeedPlayBegin	  = m_bNeedIntervalBegin = true;
						return false;
					}
					else
						return true;
				}
			}
		}
		else
		{
			if( !sModelCommonPro.bStopCue )
			{
				if (IsNeedPlay())
					m_pAEmitter->Play(m_strSceneCueName.c_str());
			}
		}
		
		return false;
	}

	void CMapEditAudioObj::SetSceneCueName( const string& name )
	{
		m_strSceneCueName = name;

		//UpdateBoundingBox(true);
	}

	void CMapEditAudioObj::StopSceneCue( const bool bStop, const bool bAuto )
	{
		bool bIsPlaying = m_pAEmitter->IsPlaying();
		if( bStop && bIsPlaying )
			m_pAEmitter->Stop();

		if( !bAuto )
			sModelCommonPro.bStopCue = bStop;

 		m_bNeedPlayBegin = m_bNeedIntervalBegin = bStop;
 		if( m_bNeedPlayBegin )
 		{
 			m_dwCurRandomPlayTime = sqr::GetRandomInt(m_dwPlayTimeLenMin,  m_dwPlayTimeLenMax);
 			m_dwCurRandomInterval = sqr::GetRandomInt(m_dwPlayIntervalMin, m_dwPlayIntervalMax);
 		}
	}

	void CMapEditAudioObj::SetPlayTimeLenMin( const DWORD dwTimeMin )
	{
		m_dwPlayTimeLenMin = dwTimeMin;

		if( m_dwPlayTimeLenMin != 0 && m_dwPlayTimeLenMax != 0 )
			m_dwCurRandomPlayTime = sqr::GetRandomInt(m_dwPlayTimeLenMin,  m_dwPlayTimeLenMax);
	}

	void CMapEditAudioObj::SetPlayTimeLenMax( const DWORD dwTimeMax )
	{
		m_dwPlayTimeLenMax = dwTimeMax;

		if( m_dwPlayTimeLenMin != 0 && m_dwPlayTimeLenMax != 0 )
			m_dwCurRandomPlayTime = sqr::GetRandomInt(m_dwPlayTimeLenMin,  m_dwPlayTimeLenMax);
	}

	void CMapEditAudioObj::SetPlayIntervalMin( const DWORD dwTimeMin )
	{
		m_dwPlayIntervalMin = dwTimeMin;

		if( m_dwPlayIntervalMax != 0 && m_dwPlayIntervalMin != 0 )
			m_dwCurRandomInterval = sqr::GetRandomInt(m_dwPlayIntervalMin, m_dwPlayIntervalMax);
	}

	void CMapEditAudioObj::SetPlayIntervalMax( const DWORD dwTimeMax )
	{
		m_dwPlayIntervalMax = dwTimeMax;
		if( m_dwPlayIntervalMax != 0 && m_dwPlayIntervalMin != 0 )
			m_dwCurRandomInterval = sqr::GetRandomInt(m_dwPlayIntervalMin, m_dwPlayIntervalMax);
	}

	DWORD CMapEditAudioObj::GetPlayTimeLenMin()
	{
		return m_dwPlayTimeLenMin;
	}

	DWORD CMapEditAudioObj::GetPlayTimeLenMax()
	{
		return m_dwPlayTimeLenMax;
	}

	DWORD CMapEditAudioObj::GetPlayIntervalMin()
	{
		return m_dwPlayIntervalMin;
	}

	DWORD CMapEditAudioObj::GetPlayIntervalMax()
	{
		return m_dwPlayIntervalMax;
	}

	void CMapEditAudioObj::SetNeedPlayBeginMark()
	{
		m_bNeedIntervalBegin = m_bNeedPlayBegin = true;
	}

	bool CMapEditAudioObj::GetIsStopSound()
	{
		return sModelCommonPro.bStopCue;
	}

	void CMapEditAudioObj::AddObjectRenderDialog(CTObject *pObject)
	{
		m_pObjectDialog = new CMapAudioDialog(pObject);
		CEditModel::AddChild(m_pObjectDialog, LT_CENTER, "");
	}

	void CMapEditAudioObj::SetVisible( bool b )
	{
		//m_bIsVisible = b;
		CEditModel::SetVisible(b);

		if( !b && m_strSceneCueName != "" )
			this->StopSceneCue(!b);

		if( m_pObjectDialog )
		{
			if( b )
				m_pObjectDialog->ShowWnd(true);
			else
				m_pObjectDialog->ShowWnd(false);
		}
	}

	void CMapEditAudioObj::HideSelf()
	{
		CMapEditObject::HideSelf();
		return;

		if( m_pObjectDialog )
			m_pObjectDialog->ShowWnd(false);
	}

	void CMapEditAudioObj::ShowSelf()
	{
		CMapEditObject::ShowSelf();
		return;

		if( m_pObjectDialog )
			m_pObjectDialog->ShowWnd(true);
	}

	void CMapEditAudioObj::SetLocked( bool b )
	{
		if (m_pObjectDialog)
		{
			sModelCommonPro.bLocked = b;

			if (b)
			{
				m_pObjectDialog->SetDialogWndText("WndNum", m_strSceneCueName);
			}
			else
			{
				m_pObjectDialog->SetDialogWndText("WndNum", "");
			}
		}
	}
}