#include "stdafx.h"
#include "Line\CEditDashed.h"
#include "CGraphic.h"
#include "CWindowTarget.h"
#include "TraceHelper.h"
#include "TSqrAllocator.inl"

namespace sqr
{
	const float fWeigthSetp = 0.1f;

	CEditDashed::CEditDashed()
	{
		m_fDashedStep = 5.0f;
	}

	CEditDashed::~CEditDashed()
	{
	}

	void CEditDashed::SetDashedStep(const float fStep)
	{
		m_fDashedStep = fStep;
	}

	void CEditDashed::Render() const
	{
		if( m_vEnd.x == m_vStart.x )
			return;

		VerColor3D* pVB;
		RenderState* pRS;
		float fDisX = m_vEnd.x - m_vStart.x;
		int   nStep = (int)(fabs(fDisX) / m_fDashedStep);
		float tg    = (m_vEnd.z - m_vStart.z) / (m_vEnd.x - m_vStart.x);
		vector<CVector3f> points;

		for ( int m = 0;m < nStep; )
		{
			CVector3f temp = m_vStart;

			temp.x += m * m_fDashedStep;
			temp.z = m_vStart.z + m * m_fDashedStep * tg;
			points.push_back(temp);

			temp.x += m_fDashedStep;
			temp.z = m_vStart.z + (temp.x - m_vStart.x) * tg;
			points.push_back(temp);

			m += 2;
		}

		if( points.empty() )
			return;

		size_t PointNum = points.size();
		size_t noWeightPointNum = PointNum;;
		size_t startIndex = PointNum;
		PointNum = PointNum + m_nWeight * PointNum * 2;
		CMatrix* pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_LINELIST, 
			PointNum, PointNum/2, VerColor3D::Format, sizeof(VerColor3D), 
			(void**)&pVB, NULL, (void**)&pRS );
		pRS->m_LightEnable = FALSE;
		pMatrix->Identity();

		for ( size_t m = 0; m < noWeightPointNum; ++m )
		{
			pVB[m].p = points[m];
			pVB[m].diffuse = m_dwColor;
		}

		for ( int m = 1; m <= m_nWeight; ++m )
		{
			CVector3f ptemp = CVector3f(0.0f, 0.0f, m * fWeigthSetp);
			CVector3f ntemp = CVector3f(0.0f, 0.0f, -m * fWeigthSetp);

			for ( size_t m = 0; m < noWeightPointNum; )
			{
				CVector3f start = pVB[m].p;
				CVector3f end   = pVB[m+1].p;

	 			pVB[startIndex].p = start + ptemp;
	 			pVB[startIndex].diffuse = m_dwColor;
	 			points.push_back(pVB[startIndex].p);
	 			++startIndex;
	 
	 			pVB[startIndex].p = end + ptemp;
	 			pVB[startIndex].diffuse = m_dwColor;
	 			points.push_back(pVB[startIndex].p);
	 			++startIndex;
	 
	 			pVB[startIndex].p = start + ntemp;
	 			pVB[startIndex].diffuse = m_dwColor;
	 			points.push_back(pVB[startIndex].p);
	 			++startIndex;
	 
	 			pVB[startIndex].p = end + ntemp;
	 			pVB[startIndex].diffuse = m_dwColor;
	 			points.push_back(pVB[startIndex].p);
	 			++startIndex;

				m += 2;
			}
		}
		size_t size = points.size();
		Ast(size==PointNum);
	}
}