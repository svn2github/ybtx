#include "stdafx.h"
#include "CAsynServerProxyBank.h"
#include "PtRefProxy.inl"
#include "TSqrAllocator.inl"
#include "CCoreObjectServer.h"
#include "ICoreObjectServerHandler.h"
#include "PatternCOR.h"

void InitAsynServerProxyBank()
{
	TPtProxyBankMgr<CPtCORHandler,CCOREvent>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CPtCORSubject,CCOREvent>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<ICoreObjectServerHandler, CCoreObjectServer>::Inst()->CreateProxyBank();
}

void UnitAsynServerProxyBank()
{
	TPtProxyBankMgr<CPtCORHandler,CCOREvent>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CPtCORSubject,CCOREvent>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<ICoreObjectServerHandler, CCoreObjectServer>::Inst()->DestroyProxyBank();
}
