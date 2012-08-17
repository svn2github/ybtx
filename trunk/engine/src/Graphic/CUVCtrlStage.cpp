#include "stdafx.h"
#include "CUVCtrlStage.h"
#include "CRenderPipeline.h"

namespace sqr
{
	CUVCtrlStage::CUVCtrlStage(StageFlag sf)
		: CFixStage(sf)
	{
		m_eStageEx = SFE_UV;
	}

	void CUVCtrlStage::Execute( CRenderStyleExInst* pRSInst )
	{
		CRenderPipeline* pPipline = CRenderPipeline::GetInst();
		pPipline->_SetTextureStageState( m_eStage, TSS_TEXCOORDINDEX, m_uvSrc|m_uv);
	}
}
