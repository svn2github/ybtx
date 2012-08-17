#pragma once

#include "CBaseSampler.h"

namespace sqr
{
	class CEditCurveRender
	{
	public:
		CEditCurveRender();
		~CEditCurveRender();

		///operation
		void SetSampler(CBaseSampler<CVector3f>* sampler);
		void SetColor(DWORD color);
		void SetSolid(bool value);
		void SetPointSize(float value);
		void SetPointColor(DWORD color);

		///render
		void DrawControlPoint(void);
		void DrawCurve();

	private:
		DWORD m_color;
		bool  m_bSolid;
		float m_fPointSize;
		DWORD m_PointColor;
		CBaseSampler<CVector3f>* m_pSampler;
	};
}