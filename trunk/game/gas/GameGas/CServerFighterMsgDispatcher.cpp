#include "stdafx.h"
#include "CServerFighterMsgDispatcher.h"
#include "PtrlGac2GasCPPDef.h"
#include "CBaseMsgDispatcher.inl"
#include "CCharacterMediator.h"
#include "CFighterMediator.h"

#include "TMsgDispatcher.inl"
#include "TMsgDispatcher_inl.inl"
#include "TMsgHandler.inl"

void CServerFighterMsgDispatcher::RegisterMsgHandler()
{
	SetHandlerNumber(eGac2GasCPP_Fighter_End, -eGac2GasCPP_Fighter_Begin);

	RegisterFighterCommand<CGac2GasCPP_DoSkill>();
	RegisterFighterCommand<CGac2GasCPP_StopNormalAttack>();
	RegisterFighterCommand<CGac2GasCPP_DoNormalAttack>();
	RegisterFighterCommand<CGac2GasCPP_StartOrRestartNADueToSkillFailure>();
	RegisterFighterCommand<CGac2GasCPP_SetTargetPos>();
	RegisterFighterCommand<CGac2GasCPP_CancelState>();
	RegisterFighterCommand<CGac2GasCPP_CancelCoerceMove>();	
	RegisterFighterCommand<CGac2GasCPP_OpenObjPanel>();
	RegisterFighterCommand<CGac2GasCPP_DestroyServant>();
	RegisterFighterCommand<CGac2GasCPP_SetAutoNormalAttack>();
	RegisterFighterCommand<CGac2GasCPP_CancelCastingProcess>();
	RegisterFighterCommand<CGac2GasCPP_GetTargetCastingProcess>();
	RegisterFighterCommand<CGac2GasCPP_GetTargetOfTarget>();
	RegisterFighterCommand<CGac2GasCPP_NotifyBurstSoulArrived>();
}

template<typename CmdClass>
void CServerFighterMsgDispatcher::RegisterFighterCommand()
{
	SetHandler<CmdClass>(&CServerFighterMsgDispatcher::OnFighterCommand<CmdClass>);
}

template<typename CmdClass>
void CServerFighterMsgDispatcher::OnFighterCommand(const CmdClass* pCmd, void* pParam)
{
	CCharacterMediator* pMediator = CCharacterMediator::GetCharacterMediatorByConn(GetConnection());
	if (pMediator)
	{
		CFighterMediator* pFighterMediator = pMediator->GetFighter()->CastToFighterMediator();
		Ast(pFighterMediator);
		pFighterMediator->OnClientCommand(pCmd);
	}
}
