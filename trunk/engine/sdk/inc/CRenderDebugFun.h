#pragma once
#include "CGraphic.h"
#include "CRenderObject.h"
namespace sqr
{
	extern void RenderSmallBoxDebug(CRenderObject* pRenderObj,CBaseAABB* pBox = NULL);
	extern void RenderBigBoxDebug(CRenderObject* pRenderObj);
}