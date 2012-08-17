#pragma once
#include "CBaseMsgDispatcher.h"
#include "TDispatcherTraits.h"

template <typename ImpClass, typename Traits, typename ConnClass>
class TComMsgDispatcher:
	public CBaseMsgDispatcher<ImpClass,  ConnClass>
{
public:
	static void RegisterMsgHandler();
private:
	template <typename CmdClass> static void RegisterFighterCommand();
	template <typename CmdClass> static void RegisterCharacterCommand();
	template <typename CmdClass> static void RegisterActorCommand();

	template <typename CmdClass> static void RegisterFighterFollowerCommand();
	template <typename CmdClass> static void RegisterFighterDirectorCommand();
	template <typename CmdClass> static void RegisterMagicCommand();

	template <typename CmdClass> static void RegisterApexCommand();
protected:
};
