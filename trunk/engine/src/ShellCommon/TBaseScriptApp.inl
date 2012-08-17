#pragma once
#include "TBaseScriptApp.h"
//#include "BaseHelper.h"

template<typename ImpRunner_t>
ImpRunner_t* TBaseScriptApp<ImpRunner_t>::Inst()
{
	return static_cast<ImpRunner_t*>( CBaseScriptApp::Inst() );
}


template<typename ImpRunner_t>
CScript* TBaseScriptApp<ImpRunner_t>::CreateMainVM()

{
	const float fCallbackRatio=ImpRunner_t::Config_t::Inst()->GetGCCallbackRatio();
	const float fNormalUnpackRatio=ImpRunner_t::Config_t::Inst()->GetGCNormalUnpackRatio();
	const float fSerialUnpackRatio=ImpRunner_t::Config_t::Inst()->GetGCSerialUnpackRatio();

	return CBaseScriptApp::CreateMainVM(fCallbackRatio,fNormalUnpackRatio,fSerialUnpackRatio);
}

