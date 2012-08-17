#include "stdafx.h"
#include "CAppEvent.h"
#include "CCoreMsgAppEvent.h"
#include "CCoreMsgMgr.h"

CAppEvent::CAppEvent()
{
	CCoreMsgMgr::Inst().PostMsg(new (CCoreMsgMgr::Inst())CCoreMsgAppEvent(this));
}

CAppEvent::~CAppEvent()
{
}

void* CAppEvent::operator new( size_t stSize)throw()
{
	return CCoreMsgMgr::Inst().MemPool().Alloc(stSize);
}
