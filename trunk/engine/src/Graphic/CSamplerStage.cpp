#include "stdafx.h"
#include "CSamplerStage.h"
#include "CRenderPipeline.h"

CSamplerStage::CSamplerStage(StageFlag sf)
: CFixStage(sf)
{
	m_eStageEx = SFE_TEXTURE;
}

void	CSamplerStage::Execute( CRenderStyleExInst* pRSInst )
{
	CRenderPipeline* pPipline = CRenderPipeline::GetInst();

	SamplerPoolIter it	= m_Samplers.begin();
	SamplerPoolIter eit	= m_Samplers.end();
	for ( ; it != eit; ++it )
	{
		pPipline->_SetSamplerState( m_eStage, it->first, it->second );
	}
}