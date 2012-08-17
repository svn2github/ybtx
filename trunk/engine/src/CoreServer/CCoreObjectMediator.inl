#pragma once
#include "CCoreObjectMediator.h"
#include "ExpHelper.h"
#include "CMedObjFollowState.h"


template <typename CmdType>
void CCoreObjectMediator::OnClientCommandWithTimeStamp(const CmdType* pCmd)
{
	if(!m_pFolState)
	{
		ostringstream strm;
		strm<<"Mediator object is not in fol state while receiving cmd "<<CmdType::GetName();
		GenErr(strm.str());
	}

	if(m_pFolState->m_uClientTimeStamp>pCmd->uqbTimeStamp)
	{
		ostringstream strm;
		strm<<"Invalid time stamp while receving cmd "<<CmdType::GetName();
		GenErr(strm.str());
	}

	m_pFolState->m_uClientTimeStamp=pCmd->uqbTimeStamp;

	OnClientCommand(pCmd);
}
