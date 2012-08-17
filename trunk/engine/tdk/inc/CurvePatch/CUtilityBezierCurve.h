#pragma once
#include "CBezierCurve.h"
#include "CVector3.h"
#include "CMatrix.h"

namespace sqr
{

	class CUtilityBezierCurve
	{
	private:
		CBaseBeizerCurve<CVector3f> m_BeizerCurve;
		CMatrix						m_matRelative;

	public:
		CUtilityBezierCurve();
		~CUtilityBezierCurve();

		///operation
		void Clear();
		void AddPoint(const CVector3f& point);
		void DeletePoint(const CVector3f& point);

		void SetTranslate(const CVector3f& trans);

		///render
		void DrawControlPoints();
		void DrawBezierCurve();
	};
}