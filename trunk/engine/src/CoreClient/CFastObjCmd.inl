#pragma once
#include "CFastObjCmd.h"
#include "ErrLogHelper.h"
#include "ExpHelper.h"

template<typename CmdType>
TFastObjCmd<CmdType>::TFastObjCmd(const CmdType* pCmd)
:CFastObjCmd(pCmd,sizeof(*pCmd)+pCmd->GetPayloadSize())
{
}

template<typename CmdType>
void TFastObjCmd<CmdType>::Call(CCoreObjectFollower* pObj)
{
#ifdef LOG_COREOBJ_MOVE
	pObj->LogMsg("E_F_C");
#endif
	SQR_TRY
	{
		pObj->OnDelayedServerCommand(reinterpret_cast<const CmdType*>(m_sData.data()));
	}
	SQR_CATCH(exp)
	{
		if(CConnClient::Inst())
		{
			LogExp(exp, CConnClient::Inst());		
		}
		else
		{
			LogExp(exp);
		}
	}
	SQR_TRY_END;
}