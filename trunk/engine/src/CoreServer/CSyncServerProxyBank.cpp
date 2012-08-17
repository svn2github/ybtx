#include "stdafx.h"
#include "CSyncServerProxyBank.h"
#include "CoreObjectServerPosRelation.h"
#include "CSyncCoreObjectServer.h"
#include "CCoreObjTracer.h"
#include "TSqrAllocator.inl"
#include "PtRefProxy.inl"
#include "PatternCOR.h"

void InitSyncServerProxyBank()
{
	TPtProxyBankMgr<CPtCORHandler,CCOREvent>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CPtCORSubject,CCOREvent>::Inst()->CreateProxyBank();
	TPtProxyBankMgr< CSyncCoreObjectServer, TBaseCOEvent<CSyncTraitsServer> >::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CObjPosObservee, CObjPosObserver>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CSyncCoreObjectServer, CCoreObjTracer>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CSyncCoreObjectServer, CAoiViewObjHandler>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CAsyAoiViewObj, CSyncCoreObjectServer>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CAoiViewObjHandler, CAsyAoiViewObj>::Inst()->CreateProxyBank();
}

void UnitSyncServerProxyBank()
{
	TPtProxyBankMgr<CPtCORHandler,CCOREvent>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CPtCORSubject,CCOREvent>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr< CSyncCoreObjectServer, TBaseCOEvent<CSyncTraitsServer> >::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CObjPosObservee, CObjPosObserver>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CSyncCoreObjectServer, CCoreObjTracer>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CSyncCoreObjectServer, CAoiViewObjHandler>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CAsyAoiViewObj, CSyncCoreObjectServer>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CAoiViewObjHandler, CAsyAoiViewObj>::Inst()->DestroyProxyBank();
}
