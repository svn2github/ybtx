#pragma once
#include "TCoreSceneMgr.h"
#include "ExpHelper.h"

template<typename Traits>
TCoreSceneMgr<Traits>::TCoreSceneMgr(void)
{
	Ast( !IntInst() );
	IntInst() = static_cast<ImpCoreSceneMgr_t*>(this);
}

template<typename Traits>
TCoreSceneMgr<Traits>::~TCoreSceneMgr(void)
{
	IntInst() = NULL;
}

template<typename Traits>
typename Traits::CoreSceneMgr_t*& TCoreSceneMgr<Traits>::IntInst()
{
	static ImpCoreSceneMgr_t* ls_Inst=NULL;
	return ls_Inst;
}

template<typename Traits>
typename Traits::CoreSceneMgr_t* TCoreSceneMgr<Traits>::Inst()
{
	return IntInst();
}
