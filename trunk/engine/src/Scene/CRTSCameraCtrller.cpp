#include "stdafx.h"
#include "CRTSCameraCtrller.h"
#include "BaseHelper.h"
#include "CCamera.h"
#include "CMath.h"
#include "CVector2.h"


CRTSCameraCtrller::CRTSCameraCtrller()
: m_fDist(0.0f)
, m_fFov(0.0f)
, m_fNearPlan( 0.1f )
, m_fRadian(0.0f)
, m_fYOffset(0.0f)
, m_MaxSpeed(1000.0f)
, m_MaxXZDist(10.0f)
, m_MaxDist(100.0f)
, m_MinDist(0.0f)
, m_MinFov(0.0f)
, m_MaxFov(1.0f)
, m_MinRadian(0.0f)
, m_MaxRadian(1.0f)
, m_MaxYOffset(10.0f)
, m_MinYOffset(0.0f)
, m_fYRadian(0.0)
, m_fDistSpeed(200)
, m_uResetCtrlTime(2000) //
, m_CtrlTime(0)
, m_MaxYDegree(1.0f)
, m_fCurYDegree(0.0f)
, m_NeedReQuery(true)
, m_bFollow(true)
, m_fMoveDist(0.0f)
, m_MinNearPlan(1.0f)
, m_MaxNearPlan(1000.0f)
, m_fYDegree(0.0f)
{}

void	CRTSCameraCtrller::SetMaxCameraSpeed( float MaxSpeed )
{
	m_bFollow = gIsZero(MaxSpeed);
	if(!m_bFollow)
		m_MaxSpeed = MaxSpeed;
}


void	CRTSCameraCtrller::SetMaxFov( float fov )
{
	m_MaxFov = fov;
	m_PosNow = CVector3f(0.0f,0.0f,0.0f);
}

void	CRTSCameraCtrller::SetMinFov( float fov )
{
	m_MinFov = fov;
	m_PosNow = CVector3f(0.0f,0.0f,0.0f);
}


CtrlState CRTSCameraCtrller::_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery )
{
	CVector3f	CamPos	= pCamera->GetPosition();
	CVector3f	CamVec	= pCamera->getDIRECTION();
	float		Fov		= pCamera->getFOVy();
	float		NearPlan = pCamera->getNearDist();

	float		fDeltaTime = float(uCurTime - m_uCurTime) * 0.001f;

	if( m_PosNow==CamPos && m_DirNow==CamVec && m_fFov==Fov && m_fNearPlan==NearPlan && m_CtrlTime == 0.0f )
	{
		//In Control
		if( m_TargetNow != m_TargetPos )
		{
			CVector2f	fMove;
			float		dist;
			fMove.x = float(m_TargetPos.x - m_TargetNow.x);
			fMove.y = float(m_TargetPos.z - m_TargetNow.z);
			dist = float(fMove.normalize());
		
			if(m_bFollow && m_MaxXZDist > 0.0f)
			{
				dist -= (dist/m_MaxXZDist) * m_MaxSpeed * fDeltaTime;
				fMove *= limit2( dist, 0.0f, m_MaxXZDist);
			}
			else
			{
				if( dist <= m_MaxXZDist )
				{
					fMove.x = m_TargetPos.x - m_TargetNow.x;
					fMove.y = m_TargetPos.z - m_TargetNow.z;
				}
				else
				{
					fMove *= m_MaxXZDist;
				}
			}	

			m_TargetNow.x = m_TargetPos.x - fMove.x;
			m_TargetNow.z = m_TargetPos.z - fMove.y;
			m_TargetNow.y = m_TargetPos.y;

			m_PosNow = m_TargetNow - m_DirNow * m_fDistNow;
			m_PosNow.y += m_fYOffset;
			pCamera->SetPosition(m_PosNow);
			m_NeedReQuery = true;
		}
 
		float TarDist = m_fDist;
		float DistSpeed = m_fDistSpeed;
		if(NULL!=pQuery && m_NeedReQuery)
		{
			float Speed = 16.0f;
			CVector3f TestPos = m_TargetNow - m_TarDir;
			while(pQuery->SceneIntersects(TestPos) &&  (TarDist>m_MinDist || Speed > 0.0f) )
			{
				TarDist += Speed;
				if(TarDist>m_MaxDist)
				{
					TarDist = m_fDist;
					Speed = -16.0f;
					continue;
				}
				TarDist = max(TarDist,m_MinDist);
			
				float DeltaDist = (TarDist - m_MinDist)/(m_MaxDist - m_MinDist);
				float Angle = m_MinRadian + DeltaDist*(m_MaxRadian - m_MinRadian);
				m_DirNow.y	=  -CMath::SinTable(Angle);
				float templ = CMath::CosTable(Angle);
				m_DirNow.x	= templ*CMath::SinTable(m_fYRadian + m_fYDegree);
				m_DirNow.z	= templ*CMath::CosTable(m_fYRadian + m_fYDegree);
				TestPos			=	m_TargetNow - m_DirNow * TarDist;
				TestPos.y		+=  m_MinYOffset + DeltaDist * (m_MaxYOffset - m_MinYOffset);
				DistSpeed	= m_fDistSpeed * 4.0f;
				//TarDist = m_MaxDist;
			}
			m_fDist = TarDist;
			m_NeedReQuery = false;
		}

		if( m_fDistNow != TarDist || m_fYDegree != m_fCurYDegree )
		{
			if ( m_fDistNow > TarDist )
			{
				m_fDistNow -= DistSpeed * fDeltaTime;
				m_fDistNow = max(m_fDistNow,TarDist);
			}
			else if( m_fDistNow < TarDist )
			{
				m_fDistNow += DistSpeed * fDeltaTime;
				m_fDistNow = min(m_fDistNow,TarDist);
			}

			float DeltaDist = (m_fDistNow - m_MinDist)/(m_MaxDist - m_MinDist);
			float Angle = m_MinRadian + DeltaDist*(m_MaxRadian - m_MinRadian);
			m_DirNow.y =  -CMath::SinTable(Angle);
			float templ = CMath::CosTable(Angle);
			m_DirNow.x = templ*CMath::SinTable(m_fYRadian + m_fYDegree);
			m_DirNow.z = templ*CMath::CosTable(m_fYRadian + m_fYDegree);

			m_fYOffset	= m_MinYOffset	+ DeltaDist * (m_MaxYOffset - m_MinYOffset);
			m_fFov		= m_MinFov		+ DeltaDist * (m_MaxFov - m_MinFov);
			m_fNearPlan	= m_MinNearPlan		+ DeltaDist * (m_MaxNearPlan - m_MinNearPlan);
			m_PosNow		= m_TargetNow - m_DirNow * m_fDistNow;
			m_PosNow.y		+= m_fYOffset;
			pCamera->setFOVy(m_fFov);
			pCamera->setNearDist( m_fNearPlan );
			pCamera->SetPosition(m_PosNow);
			pCamera->lookTo(m_DirNow);
		}
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
		
		//Out of Control
		//代码可以优化
		m_fDistNow = m_fDist;
		float DeltaDist = (m_fDist - m_MinDist)/(m_MaxDist - m_MinDist);
		float Angle = m_MinRadian + DeltaDist*(m_MaxRadian - m_MinRadian);
		m_DirNow.y =  -CMath::SinTable(Angle);
		float templ = CMath::CosTable(Angle);
		m_DirNow.x = templ*CMath::SinTable(m_fYRadian + m_fYDegree);
		m_DirNow.z = templ*CMath::CosTable(m_fYRadian + m_fYDegree);

		m_fYOffset	= m_MinYOffset	+ DeltaDist * (m_MaxYOffset - m_MinYOffset);
		m_fFov		= m_MinFov		+ DeltaDist * (m_MaxFov - m_MinFov);
		m_fNearPlan	= m_MinNearPlan		+ DeltaDist * (m_MaxNearPlan - m_MinNearPlan);
		m_TargetNow	= m_TargetPos;
		m_PosNow	= m_TargetNow - m_DirNow * m_fDistNow;
		m_PosNow.y += m_fYOffset;

		float delta = m_CtrlTime / ( m_uResetCtrlTime *  0.001f );

		m_fFov	= (1 - delta) * m_fFov + delta * pCamera->getFOVy();

		m_fNearPlan = (1 - delta) * m_fNearPlan + delta * pCamera->getNearDist();
		m_PosNow = (1 - delta) * m_PosNow + delta * pCamera->GetPosition();

		m_DirNow = (1 - delta) * m_DirNow + delta * pCamera->getDIRECTION();

		pCamera->setFOVy(m_fFov);

		pCamera->setNearDist(m_fNearPlan);
		pCamera->SetPosition(m_PosNow);

		pCamera->lookTo(m_DirNow);
	}
	m_DirNow = pCamera->getDIRECTION();
	m_fCurYDegree = m_fYDegree;
	return CS_CTRL;
}

void	CRTSCameraCtrller::SetDist( float dist )
{
	m_fDist = limit2(dist,m_MinDist,m_MaxDist);
	_UpdateTarDir();
}

void sqr::CRTSCameraCtrller::_InitBind( void )
{
}

void CRTSCameraCtrller::SetMaxYDegree( float dgree )
{
	if( gIsEqual(dgree,m_MaxYDegree) )
		return;

	m_MaxYDegree = dgree;
	if(dgree<CMath::two_pi)
		m_fYDegree = limit2(m_fCurYDegree,-m_MaxYDegree,m_MaxYDegree);
	_UpdateTarDir();
	m_PosNow = CVector3f(0.0f,0.0f,0.0f);
	m_CtrlTime = 0.0f;
}

void CRTSCameraCtrller::ReSetPosNow()
{
	m_PosNow = CVector3f(0.0f,0.0f,0.0f);
}

void CRTSCameraCtrller::SetYRotateDegree( float dgree )
{
	if( m_MaxYDegree< CMath::two_pi )
		m_fYDegree = limit2(dgree,-m_MaxYDegree,m_MaxYDegree);
	else
		m_fYDegree = dgree;

	if(!gIsZero(m_MaxYDegree))
	{
		//_UpdateMaxDir();
		//_UpdateMinDir();
		_UpdateTarDir();
	}
}

//void CRTSCameraCtrller::_UpdateMaxDir(void)
//{
//	m_MaxDir.y =  -CMath::SinTable(m_MaxRadian);
//	float templ = CMath::CosTable(m_MaxRadian);
//	m_MaxDir.x = templ*CMath::SinTable(m_fYRadian + m_fYDegree);
//	m_MaxDir.z = templ*CMath::CosTable(m_fYRadian + m_fYDegree);
//	m_MaxDir	*= m_MaxDist;
//	m_MaxDir.y	-= m_MaxYOffset;
//}
//
//void CRTSCameraCtrller::_UpdateMinDir(void)
//{
//	m_MinDir.y =  -CMath::SinTable(m_MinRadian);
//	float templ = CMath::CosTable(m_MinRadian);
//	m_MinDir.x = templ*CMath::SinTable(m_fYRadian + m_fYDegree);
//	m_MinDir.z = templ*CMath::CosTable(m_fYRadian + m_fYDegree);	
//	m_MinDir	*=  m_MinDist;
//	m_MinDir.y	-= m_MinYOffset;
//}

void CRTSCameraCtrller::_UpdateTarDir(void)
{
	float DeltaDist = (m_fDist - m_MinDist)/(m_MaxDist - m_MinDist);
	float Angle = m_MinRadian + DeltaDist*(m_MaxRadian - m_MinRadian);
	m_TarDir.y =  -CMath::SinTable(Angle);
	float templ = CMath::CosTable(Angle);
	m_TarDir.x = templ*CMath::SinTable(m_fYRadian + m_fYDegree);
	m_TarDir.z = templ*CMath::CosTable(m_fYRadian + m_fYDegree);
	m_TarDir	*= m_fDist;
	m_TarDir.y	-= (m_MinYOffset + DeltaDist * (m_MaxYOffset - m_MinYOffset));
	m_NeedReQuery = true;
}
