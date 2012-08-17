#include "stdafx.h"
#include "CColorStage.h"
#include "CRenderPipeline.h"

namespace sqr
{
	const TEXTURESTAGESTATETYPE CColorStage::ms_Arg1Type[ET_COUNT] = { TSS_COLORARG1, TSS_ALPHAARG1 };
	const TEXTURESTAGESTATETYPE CColorStage::ms_Arg2Type[ET_COUNT] = { TSS_COLORARG2, TSS_ALPHAARG2 };
	const TEXTURESTAGESTATETYPE CColorStage::ms_OpType[ET_COUNT]   = { TSS_COLOROP,   TSS_ALPHAOP   };

	CColorStage::CColorStage(StageFlag sf)
		: CFixStage(sf)
		, m_op(0)
		, m_arg1(0)
		, m_arg2(0)
		, m_type(ET_FOURCEDWORD)
	{
	}

	void CColorStage::Execute( CRenderStyleExInst* pRSInst )
	{
		CRenderPipeline* pPipline = CRenderPipeline::GetInst();

		pPipline->_SetTextureStageState( m_eStage, ms_Arg1Type[m_type],	m_arg1 );
		pPipline->_SetTextureStageState( m_eStage, ms_Arg2Type[m_type],	m_arg2 );
		pPipline->_SetTextureStageState( m_eStage, ms_OpType[m_type],	m_op   );
	}

	void CColorStage::DefaultSetting()
	{
		m_arg1 = TA_TEXTURE;
		m_arg1 = TA_DIFFUSE;

		m_op = (m_eStage == SF_Stage1) ? TOP_SELECTARG1 : TOP_DISABLE;
	}

	void CColorStage::BinLoad()
	{
		
	}

	void CColorStage::BinSave()
	{

	}
}

