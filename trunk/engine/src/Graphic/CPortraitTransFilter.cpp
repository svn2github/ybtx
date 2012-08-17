#include "stdafx.h"
#include "CPortraitTransFilter.h"
#include "CFilterNode.h"
#include "CRenderPipeline.h"

//------------------------------------------------------------------------------
CPortraitTransFilter::CPortraitTransFilter(ERenderObjStyle eRos,CRenderGroup* pRenderGroup)
		:CDefaultFilter(eRos,pRenderGroup)
{
}

//------------------------------------------------------------------------------
CPortraitTransFilter::~CPortraitTransFilter()
{
}
//------------------------------------------------------------------------------
FilterNode* CPortraitTransFilter::Filter(FilterNode*  fn)
{
	if(fn->m_RP.m_RS.m_Cull!=CULLT_CCW)
		fn->sortSeed[0]						|=	0x80000000; 
	fn->m_RP.m_RS.m_ShaderType |= MakeSysVS_ID(SSI_BASE_VS_SYSTEM_ID);
	return CDefaultFilter::Filter(fn);
}

//------------------------------------------------------------------------------
CNormalTransFilter::CNormalTransFilter(ERenderObjStyle eRos,CRenderGroup* pRenderGroup)
:CDefaultFilter(eRos,pRenderGroup)
{
}

//------------------------------------------------------------------------------
CNormalTransFilter::~CNormalTransFilter()
{
}
//------------------------------------------------------------------------------
FilterNode* CNormalTransFilter::Filter(FilterNode*  fn)
{
	if(fn->m_RP.m_RS.m_Cull!=CULLT_CCW)
		fn->sortSeed[0]						|=	0x80000000; 
	return CDefaultFilter::Filter(fn);
}

void CNormalTransFilter::Render(void)
{
	//CRenderPipeline::GetInst()->_SetRenderStyle(RS_DIFFUSEMATERIALSOURCE,MCS_MATERIAL);
	//CDefaultFilter::Render();
	//CRenderPipeline::GetInst()->_SetRenderStyle(RS_DIFFUSEMATERIALSOURCE,MCS_COLOR1);
}
