

#include "stdafx.h"
#include "ShareStack.h"

using namespace std;


ShareStack::ShareStack()
{
	InitializeCriticalSection(&m_cs);
}

ShareStack::~ShareStack()
{
	DeleteCriticalSection(&m_cs);
}

ShareStack* ShareStack::GetInst()
{
	static ShareStack share;
	return &share;
}


void ShareStack::PushMsg(const wchar_t* msg)
{
	EnterCriticalSection(&m_cs);
	m_MsgQueue.push_back(msg);
	LeaveCriticalSection(&m_cs);
}

wstring ShareStack::PopMsg()
{
	wstring Msg = m_MsgQueue.back();
	EnterCriticalSection(&m_cs);
	m_MsgQueue.pop_back();
	LeaveCriticalSection(&m_cs);

	return Msg;
}

size_t ShareStack::GetMsgSize()
{
	return m_MsgQueue.size();
}

void ShareStack::PushCmd(const wchar_t* cmd)
{
	EnterCriticalSection(&m_cs);
	m_CmdQueue.push_back(cmd);
	LeaveCriticalSection(&m_cs);
}


wstring ShareStack::PopCmd()
{
	wstring Cmd = m_CmdQueue.back();
	EnterCriticalSection(&m_cs);
	m_CmdQueue.pop_back();
	LeaveCriticalSection(&m_cs);

	return Cmd;
}
