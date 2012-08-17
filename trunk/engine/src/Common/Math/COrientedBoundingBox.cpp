#include "stdafx.h"
#include "COrientedBoundingBox.h"
#include "CSphere.h"
#include "CPlane.h"
#include "TSqrAllocator.inl"

COrientedBoundingBox::COrientedBoundingBox()
		:m_bNull(true)
{
}

COrientedBoundingBox::COrientedBoundingBox(CVector3f fCenter,CVector3f ExVector)
		:m_vCenter(fCenter),m_vExtent(ExVector),m_bNull(false)
{}

COrientedBoundingBox::~COrientedBoundingBox()
{}

void COrientedBoundingBox::setNull(void)
{
	m_bNull = true;
}

bool COrientedBoundingBox::isNull(void) const
{
	return m_bNull;
}

const CVector3f& COrientedBoundingBox::getCenter(void)
{
	return m_vCenter;
}

const CVector3f& COrientedBoundingBox::getExtent(void)
{
	return m_vExtent;
	m_bNull = false;
}

const CQuaternion COrientedBoundingBox::getRotation(void)
{
	return m_qRotation;
}

void COrientedBoundingBox::SetCenter(const CVector3f& fCenter)
{
	m_vCenter = fCenter;
}

void COrientedBoundingBox::SetExtent(const CVector3f& fExtent)
{
	m_vExtent = fExtent;
}

void COrientedBoundingBox::SetRotation(const CQuaternion& fRot)
{
	m_qRotation = fRot;
}


void COrientedBoundingBox::SetCenter(float x,float y,float z)
{
	m_vCenter.x = x;
	m_vCenter.y = y;
	m_vCenter.z = z;
}

void COrientedBoundingBox::SetExtent(float x,float y,float z)
{
	m_vExtent.x = x;
	m_vExtent.y = y;
	m_vExtent.z = z;
}

void COrientedBoundingBox::mergeBox( const COrientedBoundingBox& rhs )
{
	//未完成
}

void COrientedBoundingBox::mergeBox( const CAxisAlignedBox& rhs )
{
	//未完成

}

void COrientedBoundingBox::mergeBox( const CVector3f& point )
{
	//未完成
}

void COrientedBoundingBox::transformBox( const CMatrix& matrix)
{
	CQuaternion temp;
	temp.FromRotationMatrix(matrix);
	m_vCenter.FastMultiply(matrix);
	//m_vExtent.FastMultiply(matrix);
	m_qRotation =  m_qRotation * temp;
}

void COrientedBoundingBox::scaleBox(const CVector3f& s)
{
	m_vExtent *= s;
}

bool COrientedBoundingBox::intersects(const COrientedBoundingBox& b2) const
{
	return false;//未完成
}

bool COrientedBoundingBox::intersects(const CAxisAlignedBox& b2) const
{
	return false;//未完成
}

bool COrientedBoundingBox::intersects(const CSphere& s) const
{
	return false; //未完成
}

bool COrientedBoundingBox::intersects(const CPlane& p) const
{
	return false; //未完成
}

bool COrientedBoundingBox::intersects(const CVector3f& v) const
{
	CVector3f RelPoint = v - m_vCenter;
	RelPoint = m_qRotation * RelPoint;
	if (RelPoint.x >= m_vExtent.x || RelPoint.x  <= -m_vExtent.x) return false;
	if (RelPoint.y >= m_vExtent.y || RelPoint.y  <= -m_vExtent.y) return false;
	if (RelPoint.z >= m_vExtent.z || RelPoint.z  <= -m_vExtent.z) return false;
	return true;
}

void COrientedBoundingBox::move( float x, float y, float z )
{
	m_vCenter.x += x;
	m_vCenter.y += y;
	m_vCenter.z += z;
}

void COrientedBoundingBox::move( const CVector3f& v3 )
{
	m_vCenter += v3;
}

void COrientedBoundingBox::rotate(CQuaternion quat)
{ //ie:u1*u2*p*u2'*u1'
	m_qRotation = m_qRotation * quat;
}

void COrientedBoundingBox::rotate(const CVector3f& axis,float angle)
{
	CQuaternion tmpQ;
	tmpQ.FromAngleAxis(angle,axis);
	return rotate(tmpQ);
}

void COrientedBoundingBox::pitch( float alpha )
{
	rotate(m_qRotation*CVector3f(1.0f,0.0f,0.0f),alpha);
}

void COrientedBoundingBox::yaw( float beta )
{
	rotate(m_qRotation*CVector3f(0.0f,0.1f,0.0f),beta);
}

void COrientedBoundingBox::roll( float gamma )
{
	rotate(m_qRotation*CVector3f(0.0f,0.0f,1.0f),gamma);
}
