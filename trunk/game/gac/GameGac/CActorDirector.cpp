#include "stdafx.h"
#include "CActorDirector.h"
#include "PtrlGac2GasCPPDef.h"
#include "CConnClient.h"
#include "CCharacterFollower.h"

static const uint8 s_uAccumulatedLimit = 8;

CActorDirector::CActorDirector()
	:m_uAccumulatedDir(0)
{
}

CActorDirector::~CActorDirector()
{
}

void CActorDirector::SetMoveDir(uint8 uDir)
{
	CActorFollower::SetMoveDir(uDir);
	m_uAccumulatedDir += abs(int8(uDir - GetActionDir()));
	if (m_uAccumulatedDir < s_uAccumulatedLimit)
		return;
	m_uAccumulatedDir = 0;
	CGac2GasCPP_SyncActionDir Cmd;
	Cmd.uActionDir = uDir;
	Cmd.bSyncToGac = false;
	CConnClient::Inst()->SendCmd(&Cmd);
}

void CActorDirector::TurnAndSyncDir(uint8 uDir)
{
	if (uDir == GetActionDir())
		return;
	GetCharacter()->DoActionDir(uDir);
	CGac2GasCPP_SyncActionDir Cmd;
	Cmd.uActionDir = uDir;
	Cmd.bSyncToGac = true;
	CConnClient::Inst()->SendCmd(&Cmd);
}
