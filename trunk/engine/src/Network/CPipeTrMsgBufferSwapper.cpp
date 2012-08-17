#include "stdafx.h"
#include "CPipeTrMsgBufferSwapper.h"
#include "CPipeThreadMgr.h"
#include "CAsynPipeReactor.h"

CPipeTrMsgBufferSwapper::CPipeTrMsgBufferSwapper()
{
	Inst() = this;
}

CPipeTrMsgBufferSwapper::~CPipeTrMsgBufferSwapper()
{
	Inst() = NULL;
}

CPipeTrMsgBufferSwapper*& CPipeTrMsgBufferSwapper::Inst()
{
	static CPipeTrMsgBufferSwapper* ls_pInst = NULL;
	return ls_pInst;
}

void CPipeTrMsgBufferSwapper::OnWakeLeft()
{
	CPipeThreadMgr::Inst()->m_pAsynPipeReactor->PushPipeThreadMsg(0);
}
