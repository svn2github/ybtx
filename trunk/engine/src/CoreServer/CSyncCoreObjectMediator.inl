#pragma once
#include "CSyncCoreObjectMediator.h"
#include "ExpHelper.h"
#include "CMedObjFollowState.h"


template <typename CmdType>
void CSyncCoreObjectMediator::OnClientCommandWithTimeStamp(const CmdType* pCmd)
{
	if(!m_pFolState)
	{
		ostringstream strm;
		strm<<CmdType::GetName();
		GenErr("Mediator object is not in fol state while receiving cmd ", strm.str());
	}

	if(m_pFolState->m_uClientTimeStamp>pCmd->uqbTimeStamp)
	{
		ostringstream strm;
		strm<<CmdType::GetName();
		GenErr("Invalid time stamp while receving cmd ", strm.str());
	}

	m_pFolState->m_uClientTimeStamp=pCmd->uqbTimeStamp;

	OnClientCommand(pCmd);
}

template<typename CmdType>
void CSyncCoreObjectMediator::ViewMulticastSendCmd(const CmdType* pCmd)const
{
	MulticastSendCmd(m_pViewMulticast, reinterpret_cast<const void*>(pCmd), sizeof(CmdType));
}
