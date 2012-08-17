#include "stdafx.h"
#include "Line\CEditDirctionLine.h"
#include "CGraphic.h"
#include "CWindowTarget.h"
#include "TraceHelper.h"
#include "TSqrAllocator.inl"

namespace sqr
{
	const float fStep = 0.1f;

	CEditDirctionLine::CEditDirctionLine()
	{
	}

	CEditDirctionLine::~CEditDirctionLine()
	{
	}

	void CEditDirctionLine::Render() const
	{
		if( m_vEnd.x == m_vStart.x )
			return;

		size_t PointNum = 2 + m_nWeight * 4 + 4;

		VerColor3D* pVB;
		RenderState* pRS;
		CMatrix* pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_LINELIST, 
			PointNum, PointNum/2, VerColor3D::Format, sizeof(VerColor3D), 
			(void**)&pVB, NULL, (void**)&pRS );
		pRS->m_LightEnable = FALSE;
		pMatrix->Identity();

		pVB[0].p = m_vStart;
		pVB[1].p = m_vEnd;
		pVB[0].diffuse = pVB[1].diffuse = m_dwColor;

		int startIndex = 0;
		for ( int m = 1; m <= m_nWeight; ++m )
		{
			CVector3f temp = CVector3f(0.0f, 0.0f, m * fStep);

			pVB[startIndex].p = m_vStart + temp;
			pVB[startIndex].diffuse = m_dwColor;
			++startIndex;

			pVB[startIndex].p = m_vEnd + temp;
			pVB[startIndex].diffuse = m_dwColor;
			++startIndex;

			temp = CVector3f(0.0f, 0.0f, -m * fStep);

			pVB[startIndex].p = m_vStart + temp;
			pVB[startIndex].diffuse = m_dwColor;
			++startIndex;

			pVB[startIndex].p = m_vEnd + temp;
			pVB[startIndex].diffuse = m_dwColor;
			++startIndex;
		}

		pVB[startIndex].p = m_vStart;
		pVB[startIndex+1].p = m_vEnd;
		pVB[startIndex].diffuse = pVB[startIndex+1].diffuse = m_dwColor;


		pVB[startIndex+2].p			= m_vEnd;
		pVB[startIndex+2].diffuse	= m_dwColor;
		pVB[startIndex+3].p			= m_vEnd + CVector3f(-8.0f, 5.0f, 2.0f);
		pVB[startIndex+3].diffuse	= m_dwColor;

		pVB[startIndex+4].p			= m_vEnd;
		pVB[startIndex+4].diffuse   = m_dwColor;
		pVB[startIndex+5].p			= m_vEnd + CVector3f(-8.0f, -5.0f, 2.0f);
		pVB[startIndex+5].diffuse   = m_dwColor;

		pMatrix->Identity(); 
	}
}