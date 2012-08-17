#include "stdafx.h"
#include "CurvePatch\CEditBezierCurve.h"
#include "CurvePatch\BezierControlPoint.h"
#include "CGraphic.h"
#include "CWindowTarget.h"
#include "TraceHelper.h"
#include "TSqrAllocator.inl"
const float STEP = 0.1f;
const int   POINTNUM = int(1.0f / 0.1f) + 1;

namespace sqr
{
	CEditBezierCurve::CEditBezierCurve()
	{
		m_matRelative.Identity();
	}

	CEditBezierCurve::~CEditBezierCurve()
	{
		this->Clear();
	}

	void CEditBezierCurve::Clear()
	{
		vector<CBezierControlPoint*>::iterator beg ;
		for( beg = m_BeizerControlPoints.begin(); beg != m_BeizerControlPoints.end(); )
		{
			CBezierControlPoint *temp = *beg;
			delete temp;

			m_BeizerControlPoints.erase(beg);
		}
		m_BeizerControlPoints.clear();
	}

	void CEditBezierCurve::AddPoint(const CVector3f& point)
	{
		if( m_BeizerCurve.IsExit(point) )
			return;

		CBezierControlPoint *pNewControlPoint = new CBezierControlPoint;
		pNewControlPoint->CreatePointSquare(point);
		m_BeizerControlPoints.push_back(pNewControlPoint);

		m_BeizerCurve.Add(point);
	}

	void CEditBezierCurve::Rectify(const CVector3f& point, const int controlPointIndex)
	{
		if( (size_t)controlPointIndex < m_BeizerControlPoints.size() )
		{
			CBezierControlPoint *pControlPoint = m_BeizerControlPoints[controlPointIndex];
			pControlPoint->CreatePointSquare(point);

			m_BeizerCurve.Rectify(point,controlPointIndex);
		}

	}

	void CEditBezierCurve::DeletePoint(const CVector3f& point, const int controlPointIndex)
	{
		if( (size_t)controlPointIndex < m_BeizerControlPoints.size() )
		{
			vector<CBezierControlPoint*>::iterator iter = m_BeizerControlPoints.begin();
			int size = m_BeizerControlPoints.size();
			for ( int i = 0; i < controlPointIndex; ++i )
				++iter;

			CBezierControlPoint *temp = m_BeizerControlPoints[controlPointIndex];
			if( temp->GetControlPoint() == point )
			{
				delete temp;
				m_BeizerControlPoints.erase(iter);
				m_BeizerCurve.Delete(point);
			}
		}
	}

	int CEditBezierCurve::ControlPointIsSelect(const CVector3f& point, CVector3f& selectpoint)
	{
		size_t size = m_BeizerControlPoints.size();
		for ( size_t i = 0; i < size; ++i )
		{
			CBezierControlPoint *temp = m_BeizerControlPoints[i];
			temp->SetIsSelect(false);
		}

		for ( size_t i = 0; i < size; ++i )
		{
			CBezierControlPoint *temp = m_BeizerControlPoints[i];
			if(temp->IsSelect(point))
			{
				selectpoint = temp->GetControlPoint();
				return i;
			}
			else
				continue;
		}

		return -1;
	}

	void CEditBezierCurve::SetTranslate(const CVector3f& trans)
	{
		m_matRelative.SetTranslate( trans.x, trans.y, trans.z );
	}

	void CEditBezierCurve::DrawControlPoints()
	{
		MathVector<CVector3f> ControlPoints = m_BeizerCurve.GetAllControllerPoints();
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
			pVB[i].diffuse = 0xffffff00;
		}

		*pMatrix = m_matRelative; 

		vector<CBezierControlPoint*>::iterator beg ;
		for( beg = m_BeizerControlPoints.begin(); beg != m_BeizerControlPoints.end(); ++beg)
		{
			(*beg)->Render();
		}

	}

	void CEditBezierCurve::DrawBezierPointsLine()
	{
		MathVector<CVector3f> ControlPoints = m_BeizerCurve.GetAllControllerPoints();
		
		size_t PointNum = ControlPoints.size();
		if( PointNum > 1 )
		{
			VerColor3D* pVB;
			RenderState* pRS;
			CMatrix* pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_LINESTRIP, 
				PointNum, PointNum-1, VerColor3D::Format, sizeof(VerColor3D), 
				(void**)&pVB, NULL, (void**)&pRS );
			pRS->m_LightEnable = FALSE;
			pMatrix->Identity();

			for(size_t i = 0; i < PointNum; ++i)
			{
				pVB[i].p = ControlPoints[i];
				pVB[i].diffuse = 0xffffffff;
			}

			*pMatrix = m_matRelative; 
		}
	}

	void CEditBezierCurve::DrawBezierCurve()
	{
		MathVector<CVector3f> ControlPoints = m_BeizerCurve.GetAllControllerPoints();
		if( ControlPoints.empty() )
			return;

		size_t PointNum = ControlPoints.size();
		if( PointNum == 1 )
			return;

		this->DrawBezierPointsLine();

		if( PointNum > 2 )
		{
			VerColor3D* pVB;
			RenderState* pRS;

			CMatrix* pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_LINESTRIP, 
				POINTNUM, POINTNUM-1, VerColor3D::Format, sizeof(VerColor3D), 
				(void**)&pVB, NULL, (void**)&pRS );

			m_BeizerCurve.BinomialCoffs();

			int pointNum = 0;
			CVector3f point;

			point = m_BeizerCurve.Get(0.5f);
			point = m_BeizerCurve.Get(1.0f);

			for (float t = 0.0f; t < 1.01f; t+=0.1f )
			{
				point = m_BeizerCurve.Get(t);
				//DebugOut("pointt = %f, %f, %f, %f\n", t, point.x, point.y, point.z);
				pVB[pointNum].p = point;
				pVB[pointNum].diffuse = 0xffff0000;
				++pointNum;
			}
		}
	}
}