#include "stdafx.h"
#include "CSyncServerTrMsgBufferMgrSwapper.h"
#include "CSyncAppServer.h"

CSyncServerTrMsgBufferMgrSwapper::CSyncServerTrMsgBufferMgrSwapper()
{
	Inst() = this;
}

CSyncServerTrMsgBufferMgrSwapper::~CSyncServerTrMsgBufferMgrSwapper()
{
	Inst() = NULL;
}

CSyncServerTrMsgBufferMgrSwapper*& CSyncServerTrMsgBufferMgrSwapper::Inst()
{
	static CSyncServerTrMsgBufferMgrSwapper* ls_pInst = NULL;
	return ls_pInst;
}

void CSyncServerTrMsgBufferMgrSwapper::OnWakeLeft()
{
	CSyncAppServer::Inst()->ReactorCancelBlock();
}
