#pragma once
#include "CClientMsgDispatcher.h"

class CGas2GacCPP_OnBurstSoul;
class CClientFighterMsgDispatcher;

class ClientFighterTraits
{
public:
	typedef NoneAction Character;
	typedef CClientFighterMsgDispatcher Fighter;
	typedef NoneAction Actor;
	typedef NoneAction Apex;
};

class CClientFighterMsgDispatcher
	: public CClientMsgDispatcher< CClientFighterMsgDispatcher, ClientFighterTraits >
{
	friend class TComMsgDispatcher<CClientFighterMsgDispatcher, ClientFighterTraits, CConnClient>;

private:
	template<typename CmdClass>
	static void RegisterFighterCommand();

	template<typename CmdClass>
	static void RegisterFighterFollowerCommand();

	template<typename CmdClass>
	static void RegisterFighterDirectorCommand();

	template<typename CmdClass, typename Fighter, void (Fighter::*pFun)(const CmdClass*)>
	static void RegisterFighterCommand();

	template<typename CmdClass>
	static void RegisterMagicCommand();

	template<typename CmdClass, typename Fighter, void (Fighter::*pFun)(const CmdClass*)>
	void OnFighterCommand(const CmdClass* pCmd, void* pParam);

	void OnMagicCommand(const CGas2GacCPP_OnBurstSoul* pCmd, void *pParam);
};
