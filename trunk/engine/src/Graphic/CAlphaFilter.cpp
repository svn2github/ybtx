#include "stdafx.h"
#include "CAlphaFilter.h"
#include "CFilterNode.h"

namespace sqr
{
	//------------------------------------------------------------------------------
	CAlphaFilter::CAlphaFilter(ERenderObjStyle eRos,CRenderGroup* pRenderGroup)
		:CDefaultFilter(eRos,pRenderGroup)
	{
	}

	//------------------------------------------------------------------------------
	CAlphaFilter::~CAlphaFilter()
	{
	}
	//------------------------------------------------------------------------------
	FilterNode* CAlphaFilter::Filter(FilterNode*  fn)
	{
		if(fn->m_RP.m_RS.m_Cull!=CULLT_CCW)
			fn->sortSeed[0]				|=	0x80000000; 
		return CDefaultFilter::Filter(fn);
	}
}