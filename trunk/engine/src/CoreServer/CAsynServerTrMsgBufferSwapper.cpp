#include "stdafx.h"
#include "CAsynServerTrMsgBufferSwapper.h"
#include "CAppServer.h"

CAsynServerTrMsgBufferSwapper::CAsynServerTrMsgBufferSwapper()
{
	Inst() = this;
}

CAsynServerTrMsgBufferSwapper::~CAsynServerTrMsgBufferSwapper()
{
	Inst() = NULL;
}

CAsynServerTrMsgBufferSwapper*& CAsynServerTrMsgBufferSwapper::Inst()
{
	static CAsynServerTrMsgBufferSwapper* ls_pInst = NULL;
	return ls_pInst;
}

void CAsynServerTrMsgBufferSwapper::OnWakeLeft()
{
	CAppServer::Inst()->ReactorCancelBlock();
}
