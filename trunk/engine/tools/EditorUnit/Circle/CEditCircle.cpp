#include "stdafx.h"
#include "Circle\CEditCircle.h"
#include "CGraphic.h"
#include "CWindowTarget.h"
#include "TraceHelper.h"
#include "TSqrAllocator.inl"

namespace sqr
{
	float fRadia = CMath::pi / 180;

	CEditCircle::CEditCircle()
	{
		m_vCenter = CVector3f(0.0f, 0.0f, 0.0f);
		m_fRadius = 1.0f;
		m_fSlices = 20;
		m_dwColor = 0xffff0000;
	}

	CEditCircle::~CEditCircle()
	{
	}

	void CEditCircle::SetCircleCenter(const CVector3f& center)
	{
		if( center == m_vCenter)
			return;

		m_vCenter = center;
		this->CreateSpherePoints();
	}

	void CEditCircle::SetCircleRadius(const float radius)
	{
		if( radius == m_fRadius )
			return;

		m_fRadius = radius;

		this->CreateSpherePoints();
	}

	void CEditCircle::SetCircleSlices(const size_t slices)
	{
		if( slices == m_fSlices )
			return;

		m_fSlices = slices;

		this->CreateSpherePoints();
	}

	void CEditCircle::SetSphereColor(const DWORD color)
	{
		m_dwColor = color;
	}

	/*
	x = x0 + radius * cos(a)
	y = y0 + radius * sin(a)
	*/
	void CEditCircle::CreateSpherePoints()
	{
		m_spherePoints.clear();
		m_spherePoints.resize(m_fSlices+1);

		float nDegree = 0.0f, fRadian = 0.0f;
		float nAverage = 360.0f / m_fSlices;
		CVector3f point = CVector3f(0.0f, 0.0f, 0.0f);
		size_t m = 0;
		for ( m = 0; m < m_fSlices; ++m )
		{
			nDegree = m * nAverage;
			fRadian = fRadia * nDegree;

			point = m_vCenter + CVector3f(m_fRadius *  CMath::Cos(fRadian), 0.0f, m_fRadius * CMath::Sin(fRadian));
			m_spherePoints[m] = point;
		}
		m_spherePoints[m] = m_spherePoints[0];
	}

	void CEditCircle::Render() const
	{
		size_t PointNum = m_spherePoints.size();
		if( PointNum == 0 )
			return;

		VerColor3D* pVB;
		RenderState* pRS;
		CMatrix* pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_LINESTRIP, 
			PointNum, PointNum/2, VerColor3D::Format, sizeof(VerColor3D), 
			(void**)&pVB, NULL, (void**)&pRS );
		pRS->m_LightEnable = FALSE;
		pMatrix->Identity();

		for ( size_t m = 0; m < PointNum; ++m )
		{
			pVB[m].p = m_spherePoints[m];
			pVB[m].diffuse = m_dwColor;
		}

		pMatrix->Identity(); 
	}
}