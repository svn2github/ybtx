#include "StdAfx.h"
#include "CPostEffect.h"
#include "CPostEffectPass.h"
#include "CRenderTexture.h"
#include "TSqrAllocator.inl"

namespace sqr
{

//------------------------------------------------------------------------------
CPostEffect::CPostEffect()
: m_isReset(true)
{
	//empty
}

//------------------------------------------------------------------------------
CPostEffect::~CPostEffect()
{
	this->Clear();
}

//------------------------------------------------------------------------------
void
CPostEffect::Apply()
{
	// apply all the passes
	for_each(this->passes.begin(), this->passes.end(), mem_fun(&CPostEffectPass::Apply));
}

//------------------------------------------------------------------------------
void
CPostEffect::AddPass( const CPostEffectPassPtr& pass )
{
	if (!this->passes.empty())
	{
		this->passes.back()->SetNextPass(pass);
		pass->SetPrePass(this->passes.back());
	}
	this->passes.push_back(pass);
}

}// namespace sqr