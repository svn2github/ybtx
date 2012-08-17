#include "stdafx.h"
#include "CTPSCameraCtrller.h"
#include "BaseHelper.h"
#include "CCamera.h"
#include "CMath.h"
#include "CVector2.h"
#include "CVector3.h"
#include "CQuaternion.h"

CTPSCameraCtrller::CTPSCameraCtrller()
: m_fMaxXDegree(CMath::half_pi - 0.001f)
, m_fMinXDegree(0.0f)
, m_fMaxYDegree(CMath::two_pi)
, m_fMaxTargetDist(2000.0f)
, m_fMinTargetDist(200.0f)
, m_fCurDist(0.0f)
, m_fCurXDegree(0.0f)
, m_fCurYDegree(0.0f)
, m_fTargetDist(200.0f)
, m_CtrlTime(0.0f)
, m_uResetCtrlTime(2000) //
, m_MinFov(CMath::qtr_pi)
, m_MaxFov(CMath::qtr_pi)
, m_fFov(CMath::qtr_pi)
{
}

CtrlState CTPSCameraCtrller::_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery )
{
	CVector3f	CamPos	= pCamera->GetPosition();
	CVector3f	CamVec	= pCamera->getDIRECTION();
	float		Fov		= pCamera->getFOVy();

	float		fDeltaTime = float(uCurTime - m_uCurTime) * 0.001f;

	if( m_PosNow==CamPos && m_DirNow==CamVec && m_fFov==Fov && m_CtrlTime == 0.0f )
	{

		m_DirNow = CVector3f::UNIT_Z;
		CQuaternion qX,qY;
		float DeltaDist = (m_fCurDist - m_fMinTargetDist)/(m_fMaxTargetDist - m_fMinTargetDist);
		qX.FromAngleAxis( m_fCurXDegree, CVector3f::UNIT_X );
		qY.FromAngleAxis( m_fCurYDegree, CVector3f::UNIT_Y );
		m_CurQuat = qX * qY;
		m_DirNow = m_CurQuat * m_DirNow;
		m_PosNow = m_DirNow * m_fTargetDist;
		m_PosNow = m_TargetPos - m_PosNow;

		m_fFov = m_MinFov + DeltaDist * (m_MaxFov - m_MinFov);
		pCamera->setFOVy(m_fFov);
		pCamera->SetPosition(m_PosNow);
		pCamera->lookAt(m_TargetPos);
	}
	else
	{
		if(0.0f==m_CtrlTime)
		{
			if(0!=m_uCurTime)
				m_CtrlTime = m_uResetCtrlTime *  0.001f;
			else
				m_CtrlTime = 0.0f;
		}
		else
		{
			if( m_uCurTime != 0)
			{
				m_CtrlTime -= fDeltaTime;
				if(m_CtrlTime<0.0f)
					m_CtrlTime = 0.0f;
			}
			else
				m_CtrlTime = 0.0f;
		}

		float DeltaDist = (m_fCurDist - m_fMinTargetDist)/(m_fMaxTargetDist - m_fMinTargetDist);
		m_fFov		= m_MinFov		+ DeltaDist * (m_MaxFov - m_MinFov);
		float delta = m_uResetCtrlTime == 0?0.0f:m_CtrlTime / ( m_uResetCtrlTime *  0.001f );
		m_DirNow = CVector3f::UNIT_Z;
		CQuaternion qX,qY;

		qX.FromAngleAxis( m_fCurXDegree, CVector3f::UNIT_X );
		qY.FromAngleAxis( m_fCurYDegree, CVector3f::UNIT_Y );
		m_CurQuat = qX * qY;
		m_DirNow = m_CurQuat * m_DirNow;
		m_PosNow = m_DirNow * m_fTargetDist;
		m_PosNow = m_TargetPos - m_PosNow;

		m_fFov	= (1 - delta) * m_fFov + delta * pCamera->getFOVy();
		m_PosNow = (1 - delta) * m_PosNow + delta * pCamera->GetPosition();
		m_DirNow = (1 - delta) * m_DirNow + delta * pCamera->getDIRECTION();

		pCamera->setFOVy(m_fFov);
		pCamera->SetPosition(m_PosNow);
		pCamera->lookTo(m_DirNow);
	}
	return CS_CTRL;
}

void  CTPSCameraCtrller::MoveTargetPos(CVector3f vec)
{
	vec = m_CurQuat * vec;
	m_TargetPos -= vec;
}

void CTPSCameraCtrller::SetMaxXDegree( float dgree )
{
	m_fMaxXDegree = dgree;
}

void CTPSCameraCtrller::SetMinXDegree( float dgree )
{
	m_fMinXDegree = dgree;
}

void CTPSCameraCtrller::SetMaxYDegree( float dgree )
{
	m_fMaxYDegree = dgree;
}

void CTPSCameraCtrller::SetMinTargetDist( float dist )
{
	m_fMinTargetDist = dist;
}

void CTPSCameraCtrller::SetMaxTargetDist( float dist )
{
	m_fMaxTargetDist = dist;
}

//void CTPSCameraCtrller::SetRotateSpeed( float speed )
//{
//	m_fRotateSpeed = speed;
//}

//void CTPSCameraCtrller::SetMaxMoveDist( float dist )
//{
//	m_fMaxMoveDist = dist;
//}

//void CTPSCameraCtrller::SetMoveDistSpeed( float speed )
//{
//	m_fMoveDistSpeed = speed;
//}

void CTPSCameraCtrller::SetXRotateDegree( float dgree )
{
	m_fCurXDegree = limit2(dgree,m_fMinXDegree,m_fMaxXDegree);
}

void CTPSCameraCtrller::SetYRotateDegree( float dgree )
{
	if( m_fMaxYDegree< CMath::two_pi )
		m_fCurYDegree = limit2(dgree,-m_fMaxYDegree,m_fMaxYDegree);
	else
		m_fCurYDegree = dgree;
}

void CTPSCameraCtrller::SetTargetDist( float dist )
{
	m_fTargetDist = limit2(dist,m_fMinTargetDist, m_fMaxTargetDist);
}

void CTPSCameraCtrller::SetMaxFov( float fov )
{
	m_MaxFov = fov;
}

void CTPSCameraCtrller::SetMinFov( float fov )
{
	m_MinFov = fov;
}

void CTPSCameraCtrller::SetResetCtrlTime( uint32 time )
{
	m_uResetCtrlTime = time;
}
