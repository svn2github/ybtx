#pragma once
#include "BaseBrushDraw.h"

namespace sqr_tools
{
	class CRegularGridBrushImp : public CBaseBrushDraw
	{
	public:
		CRegularGridBrushImp(void);
		~CRegularGridBrushImp(void);

		virtual BRUSH_TYPE	GetBrushType()
		{
			return BRUSH_REGULAR;
		}

		virtual void		UpdateControlPoints();
		virtual void		UpdatePointsHeight();
		virtual void		Draw();
		virtual void		DrawGrid();
	};
}
