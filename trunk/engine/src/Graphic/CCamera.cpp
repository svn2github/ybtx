#include "stdafx.h"
#include "CCamera.h"
#include "CMath.h"
#include "CVector3.h"
#include "CVector2.h"

const DWORD CCamera::st_CameraType = 'CAME';

CCamera::CCamera(void)
{
	m_bRecalcFrustum = true;
	m_bNeedUpdate = true;
	m_bYawFixed = false;

	//m_v3TargetPos =  CVector3f::ZERO;
	m_vYawFixed = CVector3f::UNIT_Y;
	m_bYawFixed = true;

	m_matDeltaView.Identity();
}

CCamera::~CCamera(void)
{
}

void CCamera::_UpdateEx(void)  const
{
	//------------------------
	//更新view matrix和 视锥体
	//------------------------
	// View matrix is:
	//  [ Rx  Ry  Rz  0  ]
	//  [ Ux  Uy  Uz  0  ]
	//  [ Dx  Dy  Dz  0  ]
	//  [ Tx  Ty  Tz  1  ]

	CVector3f vDIRECTIONz = m_matCached.getColumn(2);
	CVector2f DirYZr(vDIRECTIONz.x, vDIRECTIONz.z);
	DirYZr.Normalize();
	m_matBillboardNoneX.Identity();
	m_matBillboardNoneX._11 =  DirYZr.y;
	m_matBillboardNoneX._13 = -DirYZr.x;
	m_matBillboardNoneX._31 =  DirYZr.x;
	m_matBillboardNoneX._33 =  DirYZr.y;
	CMatrix RotateX;
	RotateX.SetRotateX(asinf( -vDIRECTIONz.y ));
	m_matBillboardAll = RotateX*m_matBillboardNoneX;

	m_matView = m_matCached;
	m_matView.Transpose3x3();
	CVector3f  trans  = m_v3Position;
	trans.Rotate( m_matView );
	m_matView.m[3][0] = -trans.x;
	m_matView.m[3][1] = -trans.y;
	m_matView.m[3][2] = -trans.z;
	m_matView = m_matDeltaView * m_matView;
	//View matrix 更新完毕
	//下面更新 视锥体
	updateFrustum();

	m_matViewProject = m_matView * m_matProj;
	//向量向视锥体里
	m_Frustum.Update(m_matViewProject);
}

void CCamera::updateFrustum(void)  const
{
	if (!m_bRecalcFrustum)
		return;
	if (m_Frustum.isOrtho)
	{
		m_matProj.SetOrthoLH(m_Frustum.orthoWidth, m_Frustum.orthoHeight, m_Frustum.zNear, m_Frustum.zFar);
	}
	else
	{
		m_matProj.SetProjection(m_Frustum.fovY,m_Frustum.aspect,m_Frustum.zNear,m_Frustum.zFar);
	}
	m_bRecalcFrustum = false;
}

void CCamera::setFixedYawAxis( bool useFixed,const CVector3f& fixedYaw )
{
	if (CVector3f::ZERO == fixedYaw)
		return;
	m_bYawFixed = useFixed;
	m_vYawFixed = fixedYaw;
}

void CCamera::lookAt(const CVector3f& fPoint)
{
	//m_v3TargetPos = fPoint;
	lookTo(fPoint - m_v3Position);
}

void CCamera::lookTo( const CVector3f& fVector )
{
	if (CVector3f::ZERO == fVector)
		return;
	
	if (m_bYawFixed)
	{
		CVector3f dirFixed = fVector;
		dirFixed.Normalize();

		CVector3f xVec,yVec;
		CMatrix kRot;

		xVec = m_vYawFixed.Cross(dirFixed);
		xVec.Normalize();
		yVec = dirFixed.Cross(xVec);
		yVec.Normalize();
		m_Quaternion.FromAxes( xVec,yVec,dirFixed );
		m_bNeedUpdate = true;
	}
	else
	{
		setDIRECTION(fVector);
	}
}
