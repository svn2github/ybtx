#pragma once

class NoneAction
{
public:
	static bool SetHandlerNumber(uint32 uSize, int32 nIdOffset){return true;};
	template <typename ImpClass> static void RegisterCharacterCommand(){};
	template <typename ImpClass> static void RegisterFighterFollowerCommand(){};
	template <typename ImpClass> static void RegisterFighterDirectorCommand(){};
	template <typename ImpClass> static void RegisterMagicCommand(){};
	template <typename ImpClass> static void RegisterActorCommand(){};
	template <typename ImpClass> static void RegisterApexCommand(){};
};


