#pragma once
#include "CDelayedObjCmdMgr.h"
#include "CSlowObjCmd.inl"
#include "CFastObjCmd.inl"

template<typename CmdType>
void CDelayedObjCmdMgr::AddSlowCommand(CCoreObjectFollower* pObj,const CmdType* pCmd)
{
#ifdef LOG_COREOBJ_MOVE
	pObj->LogMsg("A_S_C");
#endif
	new TSlowObjCmd<CmdType>(pObj,pCmd,sizeof(CmdType)+pCmd->GetPayloadSize());
}

template<typename CmdType>
void CDelayedObjCmdMgr::AddFastCommand(CCoreObjectFollower* pObj,const CmdType* pCmd)
{
	if(m_lstCmd.empty())
	{
#ifdef LOG_COREOBJ_MOVE
		pObj->LogMsg("E_F_C");
#endif
		pObj->OnDelayedServerCommand(pCmd);
		return;
	}

#ifdef LOG_COREOBJ_MOVE
	pObj->LogMsg("A_F_C");
#endif
	m_lstCmd.back()->AddNestedCmd(new TFastObjCmd<CmdType>(pCmd));
}