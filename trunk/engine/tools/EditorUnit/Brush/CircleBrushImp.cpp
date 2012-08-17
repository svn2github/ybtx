#include "StdAfx.h"
#include "Brush/CircleBrushImp.h"
#include "CurvePatch/CEditCurveRender.h"
#include "CBezierSampler.h"
#include "CEditContext.h"
#include "CSceneCommon.h"

CCircleBrushImp::CCircleBrushImp(void)
{
	m_curveSampler.SetSmooth(1.0);
}

CCircleBrushImp::~CCircleBrushImp(void)
{
}
