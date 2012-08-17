#pragma once
#include "BaseBrushDraw.h"

namespace sqr_tools
{
	class CCircleBrushImp : public CBaseBrushDraw
	{
	public:
		CCircleBrushImp(void);
		~CCircleBrushImp(void);

		virtual BRUSH_TYPE	GetBrushType()
		{
			return BRUSH_CIRCLE;
		}
	};
}
