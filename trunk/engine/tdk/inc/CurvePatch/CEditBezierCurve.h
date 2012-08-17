#pragma once
#include "CVector3.h"
#include "CMatrix.h"
#include "CBezierCurve.h"

namespace sqr
{
	class CBezierControlPoint;
	class CEditBezierCurve
	{
	private:
		vector<CBezierControlPoint*>	m_BeizerControlPoints;
		CBaseBeizerCurve<CVector3f>		m_BeizerCurve;
		CMatrix							m_matRelative;

	public:
		CEditBezierCurve();
		~CEditBezierCurve();

		///operation
		void Clear();
		void AddPoint(const CVector3f& point);
		void Rectify(const CVector3f& point, const int controlPointIndex);
		void DeletePoint(const CVector3f& point, const int controlPointIndex);
		int  ControlPointIsSelect(const CVector3f& point, CVector3f& selectpoint);

		void SetTranslate(const CVector3f& trans);

		///render
		void DrawControlPoints();
		void DrawBezierPointsLine();
		void DrawBezierCurve();
	};
}