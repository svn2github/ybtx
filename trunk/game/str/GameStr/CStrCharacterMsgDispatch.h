#pragma once
#include "PtrlGas2GacCPPDcl.h"
#include "PtrlGas2GacCPPDef.h"
#include "PtrlGac2GasCPPDcl.h"
#include "CStrMsgDispatcher.h"

class CStressCharacterMsgDispatcher;
class StrCharaterTraits{
public:
	typedef CStressCharacterMsgDispatcher Character;
	typedef NoneAction Fighter;
	typedef NoneAction Actor;
	typedef NoneAction Apex;
};

class CStressCharacterMsgDispatcher
	: public CStressMsgDispatcher<CStressCharacterMsgDispatcher, StrCharaterTraits>
{
	friend class TComMsgDispatcher<CStressCharacterMsgDispatcher,StrCharaterTraits, CConnStress>;
private:
	template<typename CmdClass>
	static void RegisterCharacterCommand()
	{
		SetHandler<CmdClass>(&CStressCharacterMsgDispatcher::OnCharacterCommand);
	}

	template <typename CmdClass>
	void OnCharacterCommand(const CmdClass* pCmd, void *pParam){};
};
