#pragma once
#include "CCoreMsgMgr.h"

namespace sqr
{
	
	class CSyncCoreMsgMgr : public CCoreMsgMgr
	{
	public:
		static void Init();
		static void Unit();
		static CCoreMsgMgr& Inst();
	private:
		static CSyncCoreMsgMgr*	ms_pSyncInst;
	};

}
