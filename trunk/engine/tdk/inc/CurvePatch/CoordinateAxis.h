#pragma once
#include "GraphicBase.h"

namespace sqr
{
	const int eachAxisPointNum = 3*2;
	const int AxisPointNum = 3*eachAxisPointNum;
	const int XYZLetterNum = 3*eachAxisPointNum;
	const int PointNum = AxisPointNum + XYZLetterNum;

	//y = ax + b;
	class CLine2D
	{
		float		m_fSlope;
		float		m_fFixValuel;
		CVector2f   m_LineStart;
		CVector2f   m_LineEnd;

	public:
		CLine2D()
		{
			;
		}
		~CLine2D()
		{

		}
		void CreateLine(const CVector2f& start, const CVector2f& end);
		bool IsIntersects(const CVector2f& point);
	};

	class CCoordinateAxis
	{
		vector<VerColor3D> m_AxisPoint;

		CMatrix			   m_matRelative;
		float			   m_fEndValue;
		int				   m_nSelectAxisIndex;

		void			   Update();
		bool			   PointIntersects2D(const CVector2f& pt, const CVector2f& start, const CVector2f& end);
	public:
		CCoordinateAxis();
		~CCoordinateAxis() { }
		void Create();
		void CalSelectAxisIndex(const CVector3f& pos);
		int  CalSelectAxisIndex(const CVector2f& pt, const DWORD dwWndWidth, const DWORD dwWndHeight);
		int  GetSelectAxisIndex() const;
		void SetTranslate(const CVector3f& trans);
		void Render();
	};

	inline int	CCoordinateAxis::GetSelectAxisIndex() const
	{
		return m_nSelectAxisIndex;
	}
}