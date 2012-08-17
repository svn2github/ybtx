#include "stdafx.h"
#include "CServerCharacterMsgDispatcher.h"
#include "PtrlGac2GasCPPDef.h"
#include "CBaseMsgDispatcher.inl"
#include "CCharacterMediator.h"

#include "TMsgDispatcher.inl"
#include "TMsgDispatcher_inl.inl"
#include "TMsgHandler.inl"

void CServerCharacterMsgDispatcher::RegisterMsgHandler()
{
	SetHandlerNumber(eGac2GasCPP_Character_End, -eGac2GasCPP_Character_Begin);

	RegisterCharacterCommand<CGac2GasCPP_LockObj>();	
	RegisterCharacterCommand<CGac2GasCPP_MoveByDirection>();
	RegisterCharacterCommand<CGac2GasCPP_SyncActionDir>();
	RegisterCharacterCommand<CGac2GasCPP_ChangeMoveMode>();
	RegisterCharacterCommand<CGac2GasCPP_ChangeDrawWeaponMode>();
	RegisterCharacterCommand<CGac2GasCPP_ChangeNormalAttacking>();
}

template<typename CmdClass>
void CServerCharacterMsgDispatcher::RegisterCharacterCommand()
{
	SetHandler<CmdClass>(&CServerCharacterMsgDispatcher::OnCharacterCommand);
}

template<typename CmdClass>
void CServerCharacterMsgDispatcher::OnCharacterCommand(const CmdClass* pCmd, void* pParam)
{
	CCharacterMediator* pMediator = CCharacterMediator::GetCharacterMediatorByConn(GetConnection());
	if (pMediator)
		pMediator->OnCharacterCommand(pCmd);
}
