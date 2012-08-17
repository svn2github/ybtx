#pragma once
#include "BaseBrushDraw.h"

namespace sqr_tools
{
	class CSquareBrushImp : public CBaseBrushDraw
	{
	public:
		CSquareBrushImp(void);
		~CSquareBrushImp(void);

		virtual BRUSH_TYPE	GetBrushType()
		{
			return BRUSH_SQUARE;
		}
	};
}
