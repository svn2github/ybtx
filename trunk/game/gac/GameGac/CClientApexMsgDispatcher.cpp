#include "stdafx.h"
#include "CClientApexMsgDispatcher.inl"
#include "TComMsgDispatcher.inl"
#include "CBaseMsgDispatcher.inl"
#include "CApexClient.h"

void CClientApexMsgDispatcher::OnApexCommand(const CGas2GacCpp_ApexProxyMsg *pCmd, void* pParam)
{
	NoticeApec_UserData((char*)(pCmd+1),pCmd->uLen);
}
