#include "stdafx.h"
#include "CTextureStage.h"
#include "CRenderStyleEx.h"
#include "CRenderPipeline.h"
#include "CRenderStyleInst.h"
#include "Algorithm.h"

namespace sqr
{
	CTextureStage::CTextureStage(StageFlag sf)
		: CFixStage(sf)
	{
		m_eStageEx = SFE_TEXTURE;
	}

	void CTextureStage::Execute( CRenderStyleExInst* pRSInst )
	{
		CRenderPipeline* pPipline = CRenderPipeline::GetInst();
		pPipline->_SetTexture( m_eStage, pRSInst->GetTexture(m_nTexIdx) );
	}

}

