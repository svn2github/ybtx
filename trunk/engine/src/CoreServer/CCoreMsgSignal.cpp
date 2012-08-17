#include "stdafx.h"
#include "CCoreMsgSignal.h"
#include "CAppServer.h"
#include "IAppServerHandler.h"
#include "ExpHelper.h"
#include "CThreadWatcher.h"
#include "CSyncAppServer.h"
#include "ThreadHelper.h"
#include "CSqrAllocPoolMgr.h"
#include "BaseHelper.h"
#include "CSqrAllocPoolMgr.h"
#include <csignal>
#include "CCallStackInfo.h"

CCoreMsgSignal::CCoreMsgSignal(int32 nSignal)
:m_nSignal(nSignal)
{
}

CCoreMsgSignal::~CCoreMsgSignal(void)
{
	IAppServerHandler* pHandler=CAppServer::Inst()->GetHandler();
	if(!pHandler)
		return;

	//如果关了容错，我们不进入break了，直接退出
	switch(m_nSignal)
	{
	case SIGINT:
		if( CatchErrorEnabled() )
			pHandler->OnClose();
		else
			pHandler->OnBreak();
		break;
	case SIGTERM:
		pHandler->OnClose();
		break;
	case 40:
		EnableCatchError( !CatchErrorEnabled() );
		break;
	case 41:
		CSyncAppServer::Inst()->LogMem();
		break;
	case 44:
		if (CThreadWatcher::Inst()->Enabled())
		{
			CThreadWatcher::Inst()->End();
		}
		else
		{
			CThreadWatcher::Inst()->Begin();
		}
		break;
	default:
		printf("On signal %d\n",m_nSignal);
	}

}

