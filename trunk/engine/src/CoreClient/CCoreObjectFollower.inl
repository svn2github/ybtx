#pragma once
#include "CCoreObjectFollower.h"
#include "CDelayedObjCmdMgr.inl"
#include "CConnClient.h"

template<>
void CCoreObjectFollower::OnSlowServerCommand<const CGas2GacOC_Move_Follower_To>(const CGas2GacOC_Move_Follower_To* pCmd)
{
	//cout << "move to specialized " << CConnClient::Inst()->GetEstimatedServerFrameTime() << endl;
	m_queMoveToBaseTime.push(CConnClient::Inst()->GetEstimatedServerFrameTime());
	m_pDelayedCmdMgr->AddSlowCommand(this,pCmd);
}

template<typename CmdType>
void CCoreObjectFollower::OnSlowServerCommand(const CmdType* pCmd)
{
	m_pDelayedCmdMgr->AddSlowCommand(this,pCmd);
}

template<typename CmdType>
void CCoreObjectFollower::OnFastServerCommand(const CmdType* pCmd)
{
	m_pDelayedCmdMgr->AddFastCommand(this,pCmd);
}

