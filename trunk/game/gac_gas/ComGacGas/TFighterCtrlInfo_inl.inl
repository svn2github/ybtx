#pragma once
#include "TFighterCtrlInfo.h"

template<typename CtrlType>
template<typename FighterInfoType>
bool TFighterCtrlInfo<CtrlType>::IntSetCtrlState(FighterInfoType* pInfo,EFighterCtrlState eState,bool bSet)
{
	if( !CppIsAlive() )
		return false;
	m_pAliveState->SetState(pInfo,eState,bSet);
	return true;
}

