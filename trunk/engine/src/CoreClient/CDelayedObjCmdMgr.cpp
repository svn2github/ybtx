#include "stdafx.h"
#include "CDelayedObjCmdMgr.h"
#include "CSlowObjCmd.h"


CDelayedObjCmdMgr::~CDelayedObjCmdMgr(void)
{
	while(!m_lstCmd.empty())
		delete m_lstCmd.front();
}

bool CDelayedObjCmdMgr::IsEmpty()const
{
	return m_lstCmd.empty();
}

void CDelayedObjCmdMgr::FlushAllCommand()
{
	while(!m_lstCmd.empty())
		m_lstCmd.front()->OnTick();
}
