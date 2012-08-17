#include "stdafx.h"
#include "CClientProxyBank.h"
#include "PtRefProxy.inl"
#include "CCoreObjectClient.h"
#include "TSqrAllocator.inl"
#include "PatternCOR.h"

void InitClientProxyBank()
{
	TPtProxyBankMgr<CPtCORHandler,CCOREvent>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CPtCORSubject,CCOREvent>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CCoreObjectClient, TBaseCOEvent<CTraitsClient> >::Inst()->CreateProxyBank();
}

void UnitClientProxyBank()
{
	TPtProxyBankMgr<CPtCORHandler,CCOREvent>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CPtCORSubject,CCOREvent>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CCoreObjectClient, TBaseCOEvent<CTraitsClient> >::Inst()->DestroyProxyBank();
}

