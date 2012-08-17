#include "stdafx.h"
#include "CGameClientProxyBank.h"
#include "PtRefProxy.inl"
#include "TSqrAllocator.inl"
#include "CCharacterFollower.h"
#include "FXPlayer.h"
#include "CMagicClient.h"

void InitGameClientProxyBank()
{
	TPtProxyBankMgr<CFighterFollower, CFighterFollower>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CMagicFX,CMagicClient>::Inst()->CreateProxyBank();
}

void UnitGameClientProxyBank()
{
	TPtProxyBankMgr<CFighterFollower, CFighterFollower>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CMagicFX,CMagicClient>::Inst()->DestroyProxyBank();
}
