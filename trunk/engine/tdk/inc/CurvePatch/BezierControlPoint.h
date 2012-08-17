#pragma once
#include "CVector3.h"

namespace sqr
{
	class CBezierControlPoint
	{
	private:
		CVector3f			m_centerPoint;
		vector<CVector3f>   m_SquarePoints;//以控制点为中心的小正方形，便于查看和操作
		bool				m_bSelect;
		CVector3f			m_SquareMin;
		CVector3f			m_SquareMax;
	public:
		CBezierControlPoint(void);
		~CBezierControlPoint(void);

		void CreatePointSquare(const CVector3f& point);
		bool IsSelect(const CVector3f& point);
		void SetIsSelect(const bool b);
		CVector3f GetControlPoint() const;
		
		void Render();
	};

}