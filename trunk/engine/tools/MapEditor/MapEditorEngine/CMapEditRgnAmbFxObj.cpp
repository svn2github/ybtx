#include "stdafx.h"
#include "CMapEditRgnAmbFxObj.h"
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
#include "CEditModel.h"
#include "MEMath.h"
#include "EditStateOpertiaonMgr.h"
#include "ToolMainPlayer.h"
#include "CWindowTarget.h"
#include "CRenderGroup.h"

const DWORD Minute  = 60000;// 60*1000
const float rMinute = 1.0f / Minute;//

namespace sqr
{
	CMapEditRgnAmbFxObj::CMapEditRgnAmbFxObj()
	{
		m_dwPlayTimeLenMin		= 0;
		m_dwPlayTimeLenMax		= 0;
		m_dwPlayIntervalMin		= 0;
		m_dwPlayIntervalMax		= 0;
		m_dwPlayTime			= 0;
		m_dwIntervalTime		= 0;
		m_dwCurRandomPlayTime	= 0;
		m_dwCurRandomInterval	= 0;
		m_bNeedPlayBegin		= true;
		m_bNeedIntervalBegin	= true;
	}

	CMapEditRgnAmbFxObj::~CMapEditRgnAmbFxObj()
	{
		this->Release();
	}

	void CMapEditRgnAmbFxObj::Render()
	{
		CEditStateOpertiaonMgr::EEDIT_STATE eState = CEditStateOpertiaonMgr::GetInst()->GetEditState();

		DWORD dwCurrentTime = (uint32)GetProcessTime();

		static DWORD dwCurrentTimeForBestMinimap = dwCurrentTime;

		dwCurrentTimeForBestMinimap = dwCurrentTime;

		CTerrainMesh *pTerrain  = CMapEditApp::GetInst()->GetEditingMesh();
		COperator	 *pOperator = CMapEditApp::GetInst()->GetOperator();

		int nGridIndexBlockCenter = 0;
		if( eState == CEditStateOpertiaonMgr::EEDIT_STATE::EES_RUN )
		{
			CVector3f playerPos = CToolMainPlayer::GetInst()->GetCurPlayerPosition();
			sModelCommonPro.matWorldOff.SetTranslate(playerPos.x, playerPos.y/*0.0f*/, playerPos.z);
		}
		else if ( EES_MOVING == sModelCommonPro.eEditState )
		{
			nGridIndexBlockCenter = pOperator->GetGridIndexHot();
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
		}

		sModelCommonPro.mat = sModelCommonPro.matSelfOff*sModelCommonPro.matUserOffset*sModelCommonPro.matWorldOff;

		//this->SetAbsolute(sModelCommonPro.WireBoxMat);

		CEditModel::SetVisible(true);

		CColor color;
		color.A = 255;
		color.R = 255;
		color.G = 0;
		color.B = 0;

		CEditModel::SetAmbient(color);
		CEditModel::SetDiffuse(color);

		CEditModel::SetRenderStyle(ROS_LIGHTMAT1);

		SetPosition(sModelCommonPro.mat.GetLoc());
		CEditModel::OnPreRender(dwCurrentTime,CMainWindowTarget::GetInst()->GetGroup()->GetCamera());
		
		if( this->IsNeedRender(dwCurrentTime) )
			CEditModel::Render();
	}

	/*
	间隔为0，表示该环境特效一直播，否则根据
	*/
	bool CMapEditRgnAmbFxObj::IsNeedRender( DWORD dwTime )
	{
		if( m_dwPlayTimeLenMin == 0 || m_dwPlayIntervalMin == 0 )
			return true;

		if( m_dwPlayTimeLenMin < m_dwPlayTimeLenMax && m_dwPlayIntervalMin < m_dwPlayIntervalMax )
		{
			if( m_bNeedPlayBegin )
			{
				DWORD nRandom = sqr::GetRandomInt(m_dwPlayTimeLenMin,  m_dwPlayTimeLenMax);
				m_dwCurRandomPlayTime = nRandom * Minute;
				m_dwPlayTime = dwTime;
				m_bNeedPlayBegin = false;
			}

			DWORD diff = dwTime - m_dwPlayTime;
			if( diff < m_dwCurRandomPlayTime )
			{
				return true;
			}
			else
			{
				if( m_bNeedIntervalBegin )
				{
					DWORD nRandom = sqr::GetRandomInt(m_dwPlayIntervalMin, m_dwPlayIntervalMax);
					m_dwCurRandomInterval = nRandom * Minute;
					m_dwIntervalTime = dwTime;
					m_bNeedIntervalBegin = false;
				}

				diff = dwTime - m_dwIntervalTime;
				if( diff > m_dwCurRandomInterval )
				{
// 					m_dwCurRandomPlayTime = sqr::GetRandomInt(m_dwPlayTimeLenMin,  m_dwPlayTimeLenMax)  * Minute;
// 					m_dwCurRandomInterval = sqr::GetRandomInt(m_dwPlayIntervalMin, m_dwPlayIntervalMax) * Minute;
					m_bNeedPlayBegin	  = m_bNeedIntervalBegin = true;
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
			return true;

		return false;
	}

	void CMapEditRgnAmbFxObj::SetPlayTimeLenMin( const DWORD dwTimeMin )
	{
		m_dwPlayTimeLenMin = dwTimeMin;
// 		DWORD nRandom = sqr::GetRandomInt(m_dwPlayTimeLenMin,  m_dwPlayTimeLenMax);
// 		m_dwCurRandomPlayTime = nRandom * Minute;
	}

	void CMapEditRgnAmbFxObj::SetPlayTimeLenMax( const DWORD dwTimeMax )
	{
		m_dwPlayTimeLenMax = dwTimeMax;
// 		DWORD nRandom = sqr::GetRandomInt(m_dwPlayTimeLenMin,  m_dwPlayTimeLenMax);
// 		m_dwCurRandomPlayTime = nRandom * Minute;
	}

	void CMapEditRgnAmbFxObj::SetPlayIntervalMin( const DWORD dwTimeMin )
	{
		m_dwPlayIntervalMin = dwTimeMin;
// 		DWORD nRandom = sqr::GetRandomInt(m_dwPlayIntervalMin, m_dwPlayIntervalMax);
// 		m_dwCurRandomInterval = nRandom * Minute;
	}

	void CMapEditRgnAmbFxObj::SetPlayIntervalMax( const DWORD dwTimeMax )
	{
		m_dwPlayIntervalMax = dwTimeMax;
// 		DWORD nRandom = sqr::GetRandomInt(m_dwPlayIntervalMin, m_dwPlayIntervalMax);
// 		m_dwCurRandomInterval = nRandom * Minute;
	}

	DWORD CMapEditRgnAmbFxObj::GetPlayTimeLenMin()
	{
		return DWORD(m_dwPlayTimeLenMin);
	}

	DWORD CMapEditRgnAmbFxObj::GetPlayTimeLenMax()
	{
		return DWORD(m_dwPlayTimeLenMax);
	}

	DWORD CMapEditRgnAmbFxObj::GetPlayIntervalMin()
	{
		return DWORD(m_dwPlayIntervalMin);
	}

	DWORD CMapEditRgnAmbFxObj::GetPlayIntervalMax()
	{
		return DWORD(m_dwPlayIntervalMax);
	}

	void CMapEditRgnAmbFxObj::SetNeedPlayBeginMark()
	{
		m_bNeedIntervalBegin = m_bNeedPlayBegin = true;
	}
}