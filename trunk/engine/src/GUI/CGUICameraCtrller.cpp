#include "stdafx.h"
#include "CGUICameraCtrller.h"
#include "BaseHelper.h"
#include "CCamera.h"
#include "CMath.h"
#include "CVector2.h"
#include "TimeHelper.h"


CGUICameraCtrller::CGUICameraCtrller()
: m_fWearTime(2.0f)
, m_NeedReQuery(false)
, m_bNormal(true)
, m_fWearAlready(.0f)
, m_bInMove(false)
{
}

void CGUICameraCtrller::SwitchCamState()
{
	if ( m_bInMove )	// 当前摄像机还在移动状态
	{
		//立即完成当前移动
		CCamera* pCamera = (CCamera*)m_pCtrlNode;
		if (!pCamera)
			return;

		float temp_xdis, temp_ydis, temp_zdis;
		if (m_bNormal)
		{
			// 从近景到远景的调整
			temp_xdis = (m_fCurXDis - m_LookAt.x) * GUI_CAM_XSCALE_MAX + m_LookAt.x;
			temp_ydis = (m_fCurYDis - m_LookAt.y) * GUI_CAM_YSCALE_MAX + m_LookAt.y;
			temp_zdis = (m_fCurZDis - m_LookAt.z) * GUI_CAM_ZSCALE_MAX + m_LookAt.z;

			pCamera->SetPosition( CVector3f( temp_xdis, temp_ydis, temp_zdis ) );
		}
		else
		{
			// 从远景到近景
			temp_xdis = (m_fCurXDis - m_LookAt.x) / GUI_CAM_XSCALE_MAX + m_LookAt.x;
			temp_ydis = (m_fCurYDis - m_LookAt.y) / GUI_CAM_YSCALE_MAX + m_LookAt.y;
			temp_zdis = (m_fCurZDis - m_LookAt.z) / GUI_CAM_ZSCALE_MAX + m_LookAt.z;

			pCamera->SetPosition( CVector3f( temp_xdis, temp_ydis, temp_zdis ) );
		}

		m_fCurXDis = temp_xdis;
		m_fCurYDis = temp_ydis;
		m_fCurZDis = temp_zdis;

		m_bNormal = !m_bNormal;	
		m_NeedReQuery = false;
		m_fWearAlready = 0.0f;
		m_bInMove = false;
	}

	// switch
	m_NeedReQuery = true;
	m_fWearAlready = 0.0f;
	m_uCurTime = (float)((uint32)GetProcessTime());
}

CtrlState CGUICameraCtrller::_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery )
{
	if ( m_NeedReQuery )
	{
		if ( m_fWearAlready < m_fWearTime )
		{
			m_bInMove = true;
			float fDeltaTime = float(uCurTime - m_uCurTime) * 0.001f;
			m_fWearAlready += fDeltaTime;
			m_fWearAlready = m_fWearAlready < m_fWearTime ? m_fWearAlready : m_fWearTime; 
			float temp_scale, temp_xdis, temp_ydis, temp_zdis;
			if (m_bNormal)
			{
				// 从近景到远景的调整
				// 调整X
				temp_scale = 1.0f + (GUI_CAM_XSCALE_MAX - 1.0f) * m_fWearAlready / m_fWearTime;
				temp_xdis = (m_fCurXDis - m_LookAt.x) * temp_scale + m_LookAt.x;
				// 调整Y
				temp_scale = 1.0f + (GUI_CAM_YSCALE_MAX - 1.0f) * m_fWearAlready / m_fWearTime;
				temp_ydis = (m_fCurYDis - m_LookAt.y) * temp_scale + m_LookAt.y;
				// 调整Z
				temp_scale = 1.0f + (GUI_CAM_ZSCALE_MAX - 1.0f) *  m_fWearAlready / m_fWearTime;
				temp_zdis = (m_fCurZDis - m_LookAt.z) * temp_scale + m_LookAt.z;
				pCamera->SetPosition( CVector3f( temp_xdis, temp_ydis, temp_zdis ) );
			}
			else
			{
				// 从远景到近景
				// 调整X
				temp_scale = 1.0f + (GUI_CAM_XSCALE_MAX - 1.0f) * m_fWearAlready / m_fWearTime;
				temp_xdis = (m_fCurXDis - m_LookAt.x) / temp_scale + m_LookAt.x;
				// 调整Y
				temp_scale = 1.0f + (GUI_CAM_YSCALE_MAX - 1.0f) * m_fWearAlready / m_fWearTime;
				temp_ydis = (m_fCurYDis - m_LookAt.y) / temp_scale + m_LookAt.y;
				// 调整Z
				temp_scale = 1.0f + (GUI_CAM_ZSCALE_MAX - 1.0f) *  m_fWearAlready / m_fWearTime;
				temp_zdis = (m_fCurZDis - m_LookAt.z) / temp_scale + m_LookAt.z;
				pCamera->SetPosition( CVector3f( temp_xdis, temp_ydis, temp_zdis ) );
			}

			if ( !(m_fWearAlready < m_fWearTime) )
			{
				// 完成远近景切换
				m_fCurXDis = temp_xdis;
				m_fCurYDis = temp_ydis;
				m_fCurZDis = temp_zdis;
				m_bNormal = !m_bNormal;	
				m_NeedReQuery = false;
				m_fWearAlready = 0.0f;
				m_bInMove = false;
			}
		}
	}

	return CS_CTRL;
}

void sqr::CGUICameraCtrller::_InitBind( void )
{
}

void CGUICameraCtrller::SetWearTime( float t )
{
	m_fWearTime = t;
}

// 设置GUI相机在NORMAL状态下的位置
void CGUICameraCtrller::SetCamNormalPos( float x, float y, float z )
{
	CCamera* pCamera = (CCamera*)m_pCtrlNode;
	if (!pCamera)
		return;

	m_fCurXDis = x;
	m_fCurYDis = y;
	m_fCurZDis = z;
	m_bNormal = true;	
	m_NeedReQuery = false;
	m_fWearAlready = 0.0f;
	m_bInMove = false;
	m_LookAt.Init(0.0f, 0.0f, 1.0f);

	pCamera->SetPosition( CVector3f( x, y, z ) );
	return;
}

void CGUICameraCtrller::InitCamCtrller()
{
	CCamera* pCamera = (CCamera*)m_pCtrlNode;
	if (!pCamera)
		return;

	SetCamNormalPos( 0.0f, GUI_CAM_YDIS_DFT, GUI_CAM_ZDIS_DFT );
	pCamera->lookAt( CVector3f( 0, 0, 0 ));
	pCamera->setFOVy(30.0f*CMath::pi/180.0f);

	pCamera->setNearDist( Z_GUI_NEAR );
	pCamera->setFarDist( Z_GUI_FAR );
	return;
}

void CGUICameraCtrller::InitCamCtrller( const CVector3f& pos, const CVector3f& look )
{
	CCamera* pCamera = (CCamera*)m_pCtrlNode;
	if (!pCamera)
		return;

	SetCamNormalPos( pos.x, pos.y, pos.z );
	pCamera->lookAt( look );
	pCamera->setFOVy( 30.0f*CMath::pi/180.0f );
	m_LookAt = look;

	pCamera->setNearDist( Z_GUI_NEAR );
	pCamera->setFarDist( Z_GUI_FAR );
	return;
}
