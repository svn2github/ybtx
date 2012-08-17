#pragma once
#include "GraphicBase.h"
#include "CVector3.h"
#include "CMatrix.h"

namespace sqr
{
	class CBezierControlPoint;
	class CEditBezierPatch
	{
	private:
		vector<CBezierControlPoint*>	m_BeizerControlPoints;
		CMatrix							m_matRelative;
		vector<VerColorTex>				m_PatchControlPoints;
		int								m_nPatchWidth;
		int								m_nPatchHeight;

		CVector3f Bernstein(float u, const vector<CVector3f> vecPoints);
		void GenBezierPatch();

	public:
		CEditBezierPatch();
		~CEditBezierPatch();

		///operation
		void Clear();
		void AddPoint(const CVector3f& point);
		void Rectify(const CVector3f& point, const int controlPointIndex);
		void DeletePoint(const CVector3f& point, const int controlPointIndex);
		int  ControlPointIsSelect(const CVector3f& point, CVector3f& selectpoint);

		void SetTranslate(const CVector3f& trans);

		///render
		void DrawBezierPointsLine();
		void DrawBezierCurve();
	};
}