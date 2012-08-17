#include "stdafx.h"
#include "CCoreMsg.h"
#include "CCoreMsgMgr.h"

CCoreMsg::CCoreMsg(void)
{
	//CCoreMsgMgr::Inst().PostMsg( this );
}

CCoreMsg::~CCoreMsg(void)
{
}


void* CCoreMsg::operator new( size_t stSize, CCoreMsgMgr& MsgMgr)throw()
{
	//return CCoreMsgMgr::Inst().MemPool().Alloc(stSize);
	return MsgMgr.MemPool().Alloc(stSize);
}
