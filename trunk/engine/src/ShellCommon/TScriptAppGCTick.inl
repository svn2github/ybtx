#pragma once 

#include "TScriptAppGCTick.h"
#include "CScript.h"

template<typename Traits>
TScriptAppGCTick<Traits>::TScriptAppGCTick(CScript* pScript)
: m_pScript(pScript)
{
	uint32 uStepInterval = ImpAppConfig_t::Inst()->GetGCStepInterval();
	m_uStep = ImpAppConfig_t::Inst()->GetGCStep();

	if( uStepInterval != 0 )
	{
		ImpApp_t::Inst()->RegisterTick(this, uStepInterval);
		this->SetTickName("ScriptGCStepTick");
	}
}

template<typename Traits>
TScriptAppGCTick<Traits>::~TScriptAppGCTick()
{
	if( Registered() )
		ImpApp_t::Inst()->UnRegisterTick(this);
}


template<typename Traits>
void TScriptAppGCTick<Traits>::OnTick()
{
	m_pScript->GCStep(m_uStep);
}

template<typename Traits>
uint32 TScriptAppGCTick<Traits>::GetStep()const
{
	return m_uStep;
}

