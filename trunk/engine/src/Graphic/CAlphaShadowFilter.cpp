#include "stdafx.h"
#include "CAlphaShadowFilter.h"
//#include "CLightSystem.h"
#include "CRenderGroup.h"
//#include "CRenderPipeline.h"
//
//#include "CVertexAniMesh.h"
//#include "CShadowFilter.h"
//#include "BaseHelper.h"

//-----------CDefaultFilter------------------------------------------------
namespace sqr
{
FilterNode* CAlphaShadowFilter::Filter(FilterNode* fn)
{
	FilterNode FN = *(FilterNode*)fn;
	FN.m_eType = ROS_SHADOW;
	m_pRenderGroup->InsertRender(&FN);
	return CDefaultFilter::Filter(fn);
}

}