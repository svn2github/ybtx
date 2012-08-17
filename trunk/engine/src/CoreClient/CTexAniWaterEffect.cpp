#include "StdAfx.h"
#include "CTexAniWaterEffect.h"
#include "ITexture.h"
#include "CGpuProgram.h"
#include "CGpuProgramParameters.h"
#include "CRenderPipeline.h"
#include "CWaterMgr.h"

namespace sqr
{

CTexAniWaterEffect::CTexAniWaterEffect()
		: currentTextureIndex(InvalidIndex)
{
	this->SetEffectID(CWaterMgr::EffectTypeToID(CWaterMgr::TextureAnimation));
}

CTexAniWaterEffect::~CTexAniWaterEffect()
{
	// empty
}

//------------------------------------------------------------------------------
void
CTexAniWaterEffect::OnFrameUpdate()
{
	CWaterEffect::OnFrameUpdate();

	this->currentTextureIndex = (this->GetTime() / 33) % this->textures.size();
}

//------------------------------------------------------------------------------
void
CTexAniWaterEffect::OnRenderBegin()
{
	/*CDx9TexturePtr& texture = this->textures[this->currentTextureIndex];
	this->effectParams->setTexture("texWater", texture->GetD3DTexture());*/
	CRenderPipeline::GetInst()->_SetTexture( 0, textures[this->currentTextureIndex] );
	CWaterEffect::OnRenderBegin();
}

//------------------------------------------------------------------------------
void
CTexAniWaterEffect::SetAnimationTextures( const vector<ITexture*>& t )
{
	this->textures.clear();
	this->textures.resize(t.size());
	for (index_t i = 0; i < t.size(); i++)
		this->textures[i] = t[i];
}

}// namespace sqr
