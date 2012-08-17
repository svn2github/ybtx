#include "StdAfx.h"
#include "CPostEffectPass.h"
#include "CScreenAlignedQuad.h"
#include "CRenderTexture.h"
#include "CGpuProgram.h"
#include "CGraphic.h"
#include "CGpuProgramParameters.h"
#include "CRenderPipeline.h"
#include "TSqrAllocator.inl"
#include "CPostProcessManager.h"

namespace sqr
{
const uint MAX_TEXTURE_NUM = 4;

//------------------------------------------------------------------------------
CPostEffectPass::CPostEffectPass()
	: prePass(NULL)
	, nextPass(NULL)
{
	m_invViewSize[0] = 0.0f;
	m_invViewSize[1] = 0.0f;
	// empty
}

//------------------------------------------------------------------------------
CPostEffectPass::~CPostEffectPass()
{
	this->target = NULL;
	this->sources.clear();
}

//------------------------------------------------------------------------------
void CPostEffectPass::Apply()
{
	if (this->IsLast())
	{
		// present to backbuffer
		//CRenderTarget* pTarget = CGraphic::GetInst()->GetActiveRenderTarget();
		//pTarget->Clear();
	}
	else
	{
		this->target->Begin(this->needClear);
	}

	CRenderPipeline::GetInst()->SetVertexShaderF(PPSP_INV_VIEW_SIZE_VS, m_invViewSize, 1);
	CRenderPipeline::GetInst()->SetFragmentShaderF(PPSP_INV_VIEW_SIZE_PS, m_invViewSize, 1);

	CRenderPipeline* pPipeline = CRenderPipeline::GetInst();
	pPipeline->_SetRenderStyle(RS_ALPHATESTENABLE, FALSE);
	pPipeline->_SetRenderStyle(RS_ALPHABLENDENABLE, FALSE);
	pPipeline->_SetRenderStyle(RS_ZENABLE, FALSE);
	pPipeline->_SetRenderStyle(RS_ZWRITEENABLE, FALSE);
	pPipeline->_SetRenderStyle(RS_CULLMODE, CULLT_NONE);
	pPipeline->_SetRenderStyle(RS_CLIPPLANEENABLE, 0);
	float depthbias = 0.0f;
	pPipeline->_SetRenderStyle(RS_DEPTHBIAS, *(DWORD*)(&depthbias));
	pPipeline->_SetSamplerState(0, SAMP_MINFILTER, TEXF_LINEAR);
	pPipeline->_SetSamplerState(0, SAMP_MAGFILTER, TEXF_LINEAR);
	pPipeline->_SetSamplerState(0, SAMP_MIPFILTER, TEXF_LINEAR);
	pPipeline->_SetSamplerState(0, SAMP_ADDRESSU, TADDRESS_CLAMP);
	pPipeline->_SetSamplerState(0, SAMP_ADDRESSV, TADDRESS_CLAMP);
	pPipeline->_SetSamplerState(1, SAMP_MINFILTER, TEXF_LINEAR);
	pPipeline->_SetSamplerState(1, SAMP_MAGFILTER, TEXF_LINEAR);
	pPipeline->_SetSamplerState(1, SAMP_MIPFILTER, TEXF_LINEAR);
	pPipeline->_SetSamplerState(1, SAMP_ADDRESSU, TADDRESS_CLAMP);
	pPipeline->_SetSamplerState(1, SAMP_ADDRESSV, TADDRESS_CLAMP);
	pPipeline->SetShader(m_shader_handle);

	// set source parameters
	for (index_t i = 0; i < MAX_TEXTURE_NUM; ++i)
	{
		ITexture* texture = NULL;
		if (i < this->sources.size())
		{
			texture = this->sources[i].get_unsafe();
			if (NULL == texture)
			{
				// previous target
				texture = this->prePass->GetTarget().get();
			}
		}
		CRenderPipeline::GetInst()->_SetTexture(i, texture);
	}
	// draw screen quad
	CScreenAlignedQuad::GetInst()->Render();

	if (!this->IsLast())
	{
		this->target->End();
	}
}

//------------------------------------------------------------------------------
void CPostEffectPass::UpdateViewSize()
{
	if ( !m_shader_handle.IsValid() )
		return;

	uint width, height;
	if (this->IsLast())
	{
		CRenderTarget* pTarget = CGraphic::GetInst()->GetActiveRenderTarget();
		width = pTarget->GetWidth();
		height = pTarget->GetHeight();
	}
	else
	{
		width = this->target->GetWidth();
		height = this->target->GetHeight();
	}
	m_invViewSize[0] = 1.0f / width;
	m_invViewSize[1] = 1.0f / height;
	
}

}// namespace sqr
