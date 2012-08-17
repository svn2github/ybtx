#include "stdafx.h"
#include "CBatchFilter.h"

CBatchFilter::CBatchFilter(  CGeometryParams* pParams, CShadowFilter* pShadowFilter )
:CGeometryBatch(CreateFlag(),NULL,pParams,NULL)
,m_pShadowFilter(pShadowFilter)
{
	
}

CBatchFilter::~CBatchFilter( )
{
}

void	CBatchFilter::Render(void)
{
	if(m_pShadowFilter)
		m_pShadowFilter->UpdateShadowParams();
	CGeometryBatch::Render();
}

void	CBatchFilter::ModifyRS(STATIC_RS&	RS)
{
	if(m_pShadowFilter)
		m_pShadowFilter->SetShadowParams(RS);
}

void	CBatchFilter::ChangeShadowFilter(CShadowFilter* pShadowFilter)
{
	m_pShadowFilter = pShadowFilter;
}