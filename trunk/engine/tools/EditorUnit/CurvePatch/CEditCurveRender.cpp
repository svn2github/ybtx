#include "stdafx.h"
#include "CurvePatch\CEditCurveRender.h"
#include "CBaseSampler.h"
#include "CGraphic.h"
#include "CWindowTarget.h"
#include "TraceHelper.h"
#include "TSqrAllocator.inl"

const int INCREMENT = 100;

namespace sqr
{
	CEditCurveRender::CEditCurveRender()
	: m_pSampler(NULL)
	, m_color(0xffffff00)
	, m_bSolid(true)
	, m_fPointSize(5.0f)
	, m_PointColor(0xff00ffff)
	{
	}

	CEditCurveRender::~CEditCurveRender()
	{
	}

	void CEditCurveRender::SetSampler( CBaseSampler<CVector3f>* sampler )
	{
		m_pSampler = sampler;
	}

	void CEditCurveRender::SetColor( DWORD color )
	{
		m_color = color;
	}

	void CEditCurveRender::SetSolid( bool value )
	{
		m_bSolid = value;
	}

	void CEditCurveRender::SetPointSize( float value )
	{
		m_fPointSize = value;
	}

	void CEditCurveRender::SetPointColor( DWORD color )
	{

	}

	void CEditCurveRender::DrawControlPoint( void )
	{
		if (m_pSampler)
		{
			MathVector<CVector3f> point_vector = m_pSampler->GetControlPoint();
			for (unsigned int i = 0; i < point_vector.size(); i++)
			{
				VerColor3D* pVB;
				RenderState* pRS;
				CMatrix* pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_LINESTRIP, 
					5, 4, VerColor3D::Format, sizeof(VerColor3D), 
					(void**)&pVB, NULL, (void**)&pRS );
				pRS->m_LightEnable = FALSE;
				pMatrix->Identity();

				pVB[0].p = point_vector[i] + CVector3f(m_fPointSize, 0.0f, m_fPointSize);
				pVB[1].p = point_vector[i] + CVector3f(-m_fPointSize, 0.0f, m_fPointSize);
				pVB[2].p = point_vector[i] + CVector3f(-m_fPointSize, 0.0f, -m_fPointSize);
				pVB[3].p = point_vector[i] + CVector3f(m_fPointSize, 0.0f, -m_fPointSize);
				pVB[4].p = point_vector[i] + CVector3f(m_fPointSize, 0.0f, m_fPointSize);

				pVB[0].diffuse = m_PointColor;
				pVB[1].diffuse = m_PointColor;
				pVB[2].diffuse = m_PointColor;
				pVB[3].diffuse = m_PointColor;
				pVB[4].diffuse = m_PointColor;

				*pMatrix = CMatrix::UNIT_MATRIX; 
			}
		}
	}

	void CEditCurveRender::DrawCurve()
	{
		if (m_pSampler)
		{
			MathVector<CVector3f> point_vector =  m_pSampler->GetAllPoints();

			EPrimitiveType type;
			if (m_bSolid)
				type = PT_LINESTRIP;
			else
				type = PT_LINELIST;

			if( point_vector.size() > 1 )
			{
				VerColor3D* pVB;
				RenderState* pRS;
				CMatrix* pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, ROS_LIGHTMAT1, NULL, NULL, type, 
					point_vector.size(), point_vector.size()-1, VerColor3D::Format, sizeof(VerColor3D), 
					(void**)&pVB, NULL, (void**)&pRS );
				pRS->m_LightEnable = FALSE;
				pRS->m_ZTestEnable = true;
				pRS->m_ZWrite = true;
				pMatrix->Identity();

				for(size_t i = 0; i < point_vector.size(); i++)
				{
					pVB[i].p = point_vector[i];
					pVB[i].diffuse = m_color;
				}

				*pMatrix = CMatrix::UNIT_MATRIX; 
			}
		}
	}
}