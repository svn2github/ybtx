#pragma once
#include "TCoreApp.h"
#include "TMetaSceneMgr.h"
#include "TCoreSceneMgr.h"
#include "TExpSafeApp.inl"


template<typename Traits>
TCoreApp<Traits>::TCoreApp(void)
{
	new ImpMetaSceneMgr_t;
	new ImpCoreSceneMgr_t;
}

template<typename Traits>
TCoreApp<Traits>::~TCoreApp(void)
{
	delete ImpCoreSceneMgr_t::Inst();
	delete ImpMetaSceneMgr_t::Inst();
}

