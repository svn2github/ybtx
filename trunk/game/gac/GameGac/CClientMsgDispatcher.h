#pragma once
#include "IRpcCommon.h"
#include "PtrlGas2GacCPPDef.h"
#include "TComMsgDispatcher.h"
#include "CEntityClientManager.h"

namespace sqr
{
	class CConnClient;
	class ICoreObjectFollowerHandler;
}

template < typename ImpClass, typename Traits >
class CClientMsgDispatcher
	: public TComMsgDispatcher<ImpClass, Traits, CConnClient>
{
public:
	inline bool VerifyCharacter(uint32 uObjID)
	{		
		CCharacterFollower* pCharacter= CEntityClientManager::GetInst()->GetCharacter(uObjID);
		if(!pCharacter)
			return false;
		if(!pCharacter->CoreObjisExist())
			return false;
		
		return true;
	}

	inline ICoreObjectFollowerHandler* GetCoreObjHandler(uint32 uObjID) {}

	inline void LogMsgRecvTrafficForCpp(const char* szProtlName, uint16 uMsgID, uint32 uMsgLen)
	{
		CConnClient::Inst()->LogMsgRecvTrafficForCpp(szProtlName, uMsgID, uMsgLen);
	}
};
