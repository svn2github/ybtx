#include "StdAfx.h"
#include "CTexAniWaterEffect.h"
#include "CTexture.h"
#include "CGpuProgram.h"
#include "CGpuProgramParameters.h"
#include "CRenderPipeline.h"
#include "CWaterMgr.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"
namespace sqr
{

CTexAniWaterEffect::CTexAniWaterEffect()
		: currentTextureIndex(InvalidIndex)
{
}

CTexAniWaterEffect::~CTexAniWaterEffect()
{
	// empty
	for (index_t i = 0; i < textures.size(); i++)
		SafeRelease(textures[i]);
}

//------------------------------------------------------------------------------
void
CTexAniWaterEffect::OnFrameUpdate()
{
	CWaterEffect::OnFrameUpdate();

	if (this->textures.size() == 0)
		this->currentTextureIndex = InvalidIndex;
	else
		this->currentTextureIndex = (this->GetTime() / 33) % this->textures.size();
}

//------------------------------------------------------------------------------
void
CTexAniWaterEffect::OnRenderBegin()
{
	CRenderPipeline::GetInst()->SetShader(effectId);
	if (this->currentTextureIndex != InvalidIndex)
		CRenderPipeline::GetInst()->_SetTexture( 0, textures[this->currentTextureIndex] );
	CRenderPipeline::GetInst()->_SetRenderStyle(RS_ALPHATESTENABLE, FALSE);
	CRenderPipeline::GetInst()->_SetTextureStageState(0, TSS_ALPHAOP, TOP_SELECTARG1);
	CRenderPipeline::GetInst()->_SetTextureStageState(0, TSS_ALPHAARG1, TA_DIFFUSE);
	CRenderPipeline::GetInst()->_SetRenderStyle(RS_SRCBLEND, BLEND_INVSRCALPHA);
	CRenderPipeline::GetInst()->_SetRenderStyle(RS_DESTBLEND, BLEND_SRCALPHA);
	CWaterEffect::OnRenderBegin();
}

//------------------------------------------------------------------------------
void
CTexAniWaterEffect::SetAnimationTextures( const SVector<ITexture*>& t )
{
	this->textures.clear();
	this->textures.resize(t.size());
	for (index_t i = 0; i < t.size(); i++)
	{
		this->textures[i] = t[i];
		if ( this->textures[i] != 0 )
			this->textures[i]->AddRef();
	}
}

}// namespace sqr
