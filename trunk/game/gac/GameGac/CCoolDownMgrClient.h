#pragma once
#include "TCoolDownMgr.h"
#include "CFighterDirector.h"

namespace sqr
{
	class CAppClient;
}

class CCoolDownMgrClient
	:public TCoolDownMgr<CAppClient,CFighterDirector>
{
public:
	static void Init(CFighterDirector* pFighter);
	static void Unit();
	static CCoolDownMgrClient& Inst();
private:
	CCoolDownMgrClient(CFighterDirector* pFighter);

	static CCoolDownMgrClient*	ms_pInst;
};
