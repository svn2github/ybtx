#include "stdafx.h"
#include "CRenderFilter.h"

CRenderFilter::CRenderFilter() 
{}

CRenderFilter::~CRenderFilter() 
{}

FilterNode* CRenderFilter::Filter(FilterNode* fn)
{ 
	return NULL; 
}

void CRenderFilter::Render(void)
{}

void CRenderFilter::RenderEx(void)
{ 
	Render(); 
}

void CRenderFilter::Refresh(CRenderFilter* newFilter)
{}

void CRenderFilter::SetLightIndex(uint index)
{}

void CRenderFilter::UpdateRenderStyle(const RenderParam& RS )
{}