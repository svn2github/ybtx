#include "StdAfx.h"
#include "CFreeCameraCtrller.h"
#include "BaseHelper.h"
#include "CCamera.h"
#include "CMath.h"
#include "CVector2.h"

CFreeCameraCtrller::CFreeCameraCtrller(void)
:m_XAngle(90.0f)
,m_YAngle(135.0f)
,m_MaxYAngle(179.0f)
,m_MinYAngle(1.0)
,m_MoveSpeed(1000.0f)
,m_PullSpeed(200.0f)
,m_XRate(0.2f) //´ó¸ÅÃ¿ÏñËØ0.2¶È
,m_YRate(0.2f)
,m_XPixel(0)
,m_YPixel(0)
,m_State(FREECAM_STOP)
{
	m_Length=1000.0f;
	m_Dir.x=0;
	m_Dir.y=0;
	m_Dir.z=0;
}

CFreeCameraCtrller::~CFreeCameraCtrller(void)
{
}

CtrlState CFreeCameraCtrller::_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery )
{
	return CS_CTRL;
}
