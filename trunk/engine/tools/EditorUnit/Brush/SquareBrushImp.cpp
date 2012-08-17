#include "StdAfx.h"
#include "Brush/SquareBrushImp.h"
#include "CurvePatch/CEditCurveRender.h"
#include "CEditContext.h"
#include "CEditDataScene.h"
#include "CEditLogicDataMgr.h"
#include "CGraphic.h"
#include "TraceHelper.h"

CSquareBrushImp::CSquareBrushImp(void)
{
	m_curveSampler.SetSmooth(0.0);
}

CSquareBrushImp::~CSquareBrushImp(void)
{
}
