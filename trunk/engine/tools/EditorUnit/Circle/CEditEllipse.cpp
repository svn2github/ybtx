#include "stdafx.h"
#include "Sphere\CEditEllipse.h"
#include "CGraphic.h"
#include "CWindowTarget.h"
#include "CTrace.h"
#include "TNedAllocator.inl"

namespace sqr
{
	float fRadiaValue = CMath::pi / 180;

	CEditEllipse::CEditEllipse()
	{
		m_vCenter = CVector3f(0.0f, 0.0f, 0.0f);
		m_fRadiusX = m_fRadiusY = 1.0f;
		m_fSlices = 20;
		m_dwColor = 0xffff0000;

		m_fRadiusX = 40;
		m_fRadiusY = 20;
	}

	CEditEllipse::~CEditEllipse()
	{
	}

	void CEditEllipse::SetEllipseCenter(const CVector3f& center)
	{
		if( center == m_vCenter)
			return;

		m_vCenter = center;
		this->CreateEllipsePoints();
	}

	void CEditEllipse::SetEllipseRadiusX(const float radius)
	{
		if( radius == m_fRadiusX )
			return;

		m_fRadiusX = radius;
		
		this->CreateEllipsePoints();
	}

	void CEditEllipse::SetEllipseRadiusY(const float  radius)
	{
		if( radius == m_fRadiusY )
			return;

		m_fRadiusY = radius;

		this->CreateEllipsePoints();
	}

	void CEditEllipse::SetEllipseSlices(const size_t slices)
	{
		if( slices == m_fSlices )
			return;

		m_fSlices = slices;
		
		this->CreateEllipsePoints();
	}

	void CEditEllipse::SetEllipseColor(const DWORD color)
	{
		m_dwColor = color;
	}

	/*
	x = x0 + radiusX * cos(a)
	y = y0 + radiusY * sin(a)
	*/
	void CEditEllipse::CreateEllipsePoints()
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
			fRadian = fRadiaValue * nDegree;

			point = m_vCenter + CVector3f(m_fRadiusX *  CMath::Cos(fRadian), 0.0f, m_fRadiusY * CMath::Sin(fRadian));
			m_spherePoints[m] = point;
		}
		m_spherePoints[m] = m_spherePoints[0];
	}

	void CEditEllipse::Render() const
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