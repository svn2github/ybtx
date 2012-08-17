#include "StdAfx.h"
#include "CPostProcessManager.h"
#include "CPostEffectLoader.h"
#include "CRenderTexture.h"
#include "CPostEffect.h"
#include "CPostEffectPass.h"
#include "CScreenAlignedQuad.h"
#include "CRenderPipeline.h"
#include "TSqrAllocator.inl"

//------------------------------------------------------------------------------
namespace sqr
{
const GString CPostProcessManager::ConfigFile = "PostProcess.xml";

//------------------------------------------------------------------------------
CPostProcessManager::CPostProcessManager()
		: defaultRTT(NULL)
		, effectLoader(NULL)
		, nullPass(new CPostEffectPass())
{
	if(!CScreenAlignedQuad::HasInst())
		CScreenAlignedQuad::Create();
}

//------------------------------------------------------------------------------
CPostProcessManager::~CPostProcessManager()
{
	this->nullPass = NULL;
	this->postEffects.clear();
	this->enabledEffects.clear();
	this->defaultRTT = NULL;
	CScreenAlignedQuad::Destroy();
	this->effectLoader = NULL;
}

//------------------------------------------------------------------------------
//void
//CPostProcessManager::Begin(CRenderGroup* pRenderGroup)
//{
//	if (!this->HasPostEffectEnabled())
//		return;
//
//	// set default render target
//	//this->defaultRTT->SetRenderGroup(pRenderGroup);
//	//this->defaultRTT->Begin(true);
//}

//------------------------------------------------------------------------------
void
CPostProcessManager::DoPostProcess()
{
	if (!this->HasPostEffectEnabled())
		return;
	
	//this->defaultRTT->End();
	
	// apply post process
	CRenderPipeline::GetInst()->_SetRenderStyle(RS_ZWRITEENABLE,FALSE);
	CRenderPipeline::GetInst()->_SetRenderStyle(RS_ZENABLE,FALSE);
	GVector<CPostEffectPtr>::iterator it,eit = postEffects.end();
	for( it = postEffects.begin(); it!=eit; ++it )
	{
		if((*it)->IsReset())
			defaultRTT->UpdateDefaultTex();
		(*it)->Apply();
	}
	//for_each(this->postEffects.begin(), this->postEffects.end(), mem_fun(&CPostEffect::Apply));
}

//------------------------------------------------------------------------------
void
CPostProcessManager::Setup()
{
	if (!this->effectLoader.isvalid())
	{
		this->effectLoader = new CPostEffectLoader(ConfigFile.c_str());
	}
	if(postEffects.empty())
	{
		this->effectLoader->PreLoadEffects(this->postEffects);
		// restore enable status
		for (GSet<GString>::iterator it = this->enabledEffects.begin();
			it != this->enabledEffects.end(); ++it)
		{
			//const string& name = *it;
			this->SetEffectEnabled((*it).c_str(), true);
		}
	}
}

//------------------------------------------------------------------------------
void
CPostProcessManager::Discard()
{
	this->nullPass->SetTarget(NULL);
	this->postEffects.clear();
	this->defaultRTT = NULL;
	this->effectLoader->ReleaseUnusedRTT();
}

//------------------------------------------------------------------------------
void
CPostProcessManager::SetEffectEnabled( const string& name, bool enable )
{
	// find effect
	struct _is_name_equal
	{
		GString name;
		bool operator()(const CPostEffectPtr& effect) const
		{
			return (effect->GetName() == name);
		}
	} is_name_equal;
	is_name_equal.name = name.c_str();
	GVector<CPostEffectPtr>::iterator it = std::find_if(this->postEffects.begin(),
		this->postEffects.end(), is_name_equal);
	Ast(this->postEffects.end() != it)

	// update status
	CPostEffectPtr& effect = *it;
	if (enable)
	{
		if (!effect->IsEmpty())
			return;
		else
			this->effectLoader->LoadPostEffect(effect);
		this->enabledEffects.insert(name.c_str());
	}
	else
	{
		effect->Clear();
		// release rtt
		this->enabledEffects.erase(name.c_str());
		if (!this->HasPostEffectEnabled())
		{
			// release default render target
			this->nullPass->SetTarget(NULL);
			this->defaultRTT = NULL;
		}
		this->effectLoader->ReleaseUnusedRTT();
	}
	// create default render target
	if (this->HasPostEffectEnabled())
	{
		this->defaultRTT = this->effectLoader->CreateDefaultRTT();
		this->nullPass->SetTarget(this->defaultRTT);
	}
	
	this->UpdatePassChain();
}

//------------------------------------------------------------------------------
void
CPostProcessManager::UpdatePassChain()
{
	CPostEffectPass* prePass = this->nullPass;
	for (GVector<CPostEffectPtr>::iterator it = this->postEffects.begin();
		it != this->postEffects.end(); ++it)
	{
		CPostEffectPtr& effect = *it;
		if (effect->IsEmpty())
			continue;
		CPostEffectPass* firstPass = effect->GetPass(0);
		prePass->SetNextPass(firstPass);
		firstPass->SetPrePass(prePass);
		prePass = effect->GetPass(effect->GetNumPasses() - 1);
	}
	prePass->SetNextPass(NULL);
}

}// namespace sqr
