#include "stdafx.h"
#include "CAoi2LogicThreadMsgBuffer.h"
#include "CAsynServerTrMsgBufferSwapper.h"
#include "CAoiResult.h"
#include "CLogicThreadJob.inl"
#include "CLogicThreadResult.inl"

CAoi2LogicThreadMsgBuffer::CAoi2LogicThreadMsgBuffer()
: Parent_t(10240, 1024 * 1024, CAsynServerTrMsgBufferSwapper::Inst())
{
	Inst() = this;
}

CAoi2LogicThreadMsgBuffer::~CAoi2LogicThreadMsgBuffer()
{

}

void CAoi2LogicThreadMsgBuffer::Init()
{
	new CAoi2LogicThreadMsgBuffer();
}

void CAoi2LogicThreadMsgBuffer::Unit()
{
	delete Inst();
	Inst() = NULL;
}

CAoi2LogicThreadMsgBuffer*& CAoi2LogicThreadMsgBuffer::Inst() 
{
	static CAoi2LogicThreadMsgBuffer* s_pInst = NULL;
	return s_pInst;
}
