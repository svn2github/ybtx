#pragma once
#include "CClientMsgDispatcher.h"

class CGas2GacCPP_InitFollower;
class CGas2GacCPP_InitFollowerSyncInfo;
class CGas2GacCPP_InitFollowerEnd;
class CGas2GacCPP_OnDead;
class CGas2GacCPP_OnRemoved;
class CGas2GacCPP_OnReborn;
class CGas2GacCPP_ClearFollowerSyncInfo;
class CGas2GacCPP_InitDirector;
class CGas2GacCPP_InitDirectorEnd;
class CGas2GacCPP_DestroyDirector;
class CGas2GacCPP_OnMoveSpeedChange;
class CGas2GacCPP_DestoryBattleHorse;

class CClientCharacterMsgDispatcher;

class ClientCharacterTraits
{
public:
	typedef CClientCharacterMsgDispatcher Character;
	typedef NoneAction Fighter;
	typedef NoneAction Actor;
	typedef NoneAction Apex;
};

class CClientCharacterMsgDispatcher
	: public CClientMsgDispatcher< CClientCharacterMsgDispatcher, ClientCharacterTraits >
{
	friend class TComMsgDispatcher<CClientCharacterMsgDispatcher, ClientCharacterTraits, CConnClient>;

private:
	template<typename CmdClass>
	static void RegisterCharacterCommand();

	void OnCharacterCommand(const CGas2GacCPP_InitFollower* pCmd, void *pParam);
	void OnCharacterCommand(const CGas2GacCPP_InitFollowerSyncInfo* pCmd, void *pParam);
	void OnCharacterCommand(const CGas2GacCPP_InitFollowerEnd* pCmd, void *pParam);
	void OnCharacterCommand(const CGas2GacCPP_OnDead *pCmd, void *pParam);
	void OnCharacterCommand(const CGas2GacCPP_OnRemoved *pCmd, void *pParam);
	void OnCharacterCommand(const CGas2GacCPP_OnReborn *pCmd, void *pParam);
	void OnCharacterCommand(const CGas2GacCPP_ClearFollowerSyncInfo* pCmd, void *pParam);
	void OnCharacterCommand(const CGas2GacCPP_InitDirector* pCmd, void *pParam);
	void OnCharacterCommand(const CGas2GacCPP_InitDirectorEnd* pCmd, void *pParam);
	void OnCharacterCommand(const CGas2GacCPP_BeginTransfer* pCmd, void *pParam);
	void OnCharacterCommand(const CGas2GacCPP_OnMoveSpeedChange *pCmd, void *pParam);
	void OnCharacterCommand(const CGas2GacCPP_OnCancelAutoTrack* pCmd, void* pParam);
	void OnCharacterCommand(const CGas2GacCPP_DestoryBattleHorse* pCmd, void* pParam);
	void OnCharacterCommand(const CGas2GacCPP_RemoveTeamMember* pCmd, void *pParam);
	void OnCharacterCommand(const CGas2GacCPP_TeamSyncInfo* pCmd, void *pParam);
};
