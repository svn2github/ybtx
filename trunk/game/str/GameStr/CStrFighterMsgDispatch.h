#pragma once
#include "PtrlGas2GacCPPDcl.h"
#include "PtrlGas2GacCPPDef.h"
#include "PtrlGac2GasCPPDcl.h"
#include "CStrMsgDispatcher.h"

class CStressFighterMsgDispatcher;
class StrFighterTraits{
public:
	typedef NoneAction Character;
	typedef CStressFighterMsgDispatcher Fighter;
	typedef NoneAction Actor;
	typedef NoneAction Apex;
};
class CStressFighterMsgDispatcher
	: public CStressMsgDispatcher<CStressFighterMsgDispatcher,StrFighterTraits>
{
	friend class TComMsgDispatcher<CStressFighterMsgDispatcher,StrFighterTraits, CConnStress>;
private:
	template<typename CmdClass>
	static void RegisterFighterCommand()
	{
		SetHandler<CmdClass>(&CStressFighterMsgDispatcher::OnFighterCommand);
	}
	template<typename CmdClass>
	void OnFighterCommand(const CmdClass *pCmd, void *pParam){};

	template<typename CmdClass>
	static void RegisterFighterFollowerCommand()
	{
		RegisterFighterCommand <CmdClass>();
	}

	template<typename CmdClass>
	static void RegisterFighterDirectorCommand()
	{
		RegisterFighterCommand <CmdClass>();
	}

	template<typename CmdClass>
	static void RegisterMagicCommand()
	{
		SetHandler<CmdClass>(&CStressFighterMsgDispatcher::OnMagicCommand);
	}
	template<typename CmdClass>
	void OnMagicCommand(const CmdClass *pCmd, void *pParam){};
};
