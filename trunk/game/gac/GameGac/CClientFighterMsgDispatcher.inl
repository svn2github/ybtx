#pragma once
#include "stdafx.h"
#include "CClientFighterMsgDispatcher.h"
#include "CFighterDirector.h"
#include "CEntityClientManager.h"

template<typename CmdClass>
void CClientFighterMsgDispatcher::RegisterFighterCommand()
{
	SetHandler<CmdClass>(&CClientFighterMsgDispatcher::OnFighterCommand);
}

template<typename CmdClass, typename Fighter, void (Fighter::*pFun)(const CmdClass*)>
void CClientFighterMsgDispatcher::RegisterFighterCommand()
{
	SetHandler<CmdClass>(&CClientFighterMsgDispatcher::OnFighterCommand<CmdClass, Fighter, pFun>);
}

template<typename CmdClass>
void CClientFighterMsgDispatcher::RegisterMagicCommand()
{
	SetHandler<CmdClass>(&CClientFighterMsgDispatcher::OnMagicCommand);
}

template<typename CmdClass, typename Fighter, void (Fighter::*pFun)(const CmdClass*)>
void CClientFighterMsgDispatcher::OnFighterCommand(const CmdClass* pCmd, void* pParam)
{
	if( !VerifyCharacter(pCmd->uObjID) )
	{
		return;
	}
	
	Fighter* pFighter=class_cast<Fighter*>(CEntityClientManager::GetInst()->GetCharacter(pCmd->uObjID)->GetFighter());

	if (pFighter) (pFighter->*pFun)(pCmd);
	
}

template<typename CmdClass>
void CClientFighterMsgDispatcher::RegisterFighterFollowerCommand()
{
	RegisterFighterCommand <CmdClass, CFighterFollower, &CFighterFollower::OnFighterCommand>();
}

template<typename CmdClass>
void CClientFighterMsgDispatcher::RegisterFighterDirectorCommand()
{
	RegisterFighterCommand <CmdClass, CFighterDirector, &CFighterDirector::OnFighterCommand>();
}