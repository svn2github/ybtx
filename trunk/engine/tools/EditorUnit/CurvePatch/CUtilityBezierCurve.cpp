#include "stdafx.h"
#include "CurvePatch\CUtilityBezierCurve.h"
#include "CGraphic.h"
#include "CWindowTarget.h"

namespace sqr
{
	CUtilityBezierCurve::CUtilityBezierCurve()
	{
		m_matRelative.Identity();
	}

	CUtilityBezierCurve::~CUtilityBezierCurve()
	{
	}

	void CUtilityBezierCurve::Clear()
	{
		m_BeizerCurve.Clear();
	}

	void CUtilityBezierCurve::AddPoint(const CVector3f& point)
	{
		m_BeizerCurve.Add(point);
	}

	void CUtilityBezierCurve::DeletePoint(const CVector3f& point)
	{
		m_BeizerCurve.Delete(point);
	}

	void CUtilityBezierCurve::SetTranslate(const CVector3f& trans)
	{
		m_matRelative.SetTranslate( trans.x, trans.y, trans.z );
	}

	void CUtilityBezierCurve::DrawControlPoints()
	{
		vector<CVector3f> ControlPoints = m_BeizerCurve.GetAllControllerPoints();
		if(ControlPoints.empty())
			return;

		size_t PointNum = ControlPoints.size();
		
		VerColor3D* pVB;
		RenderState* pRS;
		CMatrix* pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_POINTLIST, 
			PointNum, PointNum, VerColor3D::Format, sizeof(VerColor3D), 
			(void**)&pVB, NULL, (void**)&pRS );
		pRS->m_LightEnable = FALSE;
		pMatrix->Identity();

		for(size_t i = 0; i < PointNum; ++i)
		{
			pVB[i].p = ControlPoints[i];
			pVB[i].diffuse = 0xff00ffff;
		}

		*pMatrix = m_matRelative; 

	}

	void CUtilityBezierCurve::DrawBezierCurve()
	{

	}
}