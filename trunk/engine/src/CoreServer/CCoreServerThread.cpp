#include "stdafx.h"
#include "CCoreServerThread.h"
#include "ThreadHelper.h"
#include "CCoreServerThreadMgr.h"
#include "ExpHelper.h"
#include <time.h>
#include "ErrLogHelper.h"
#include "CSyncAppServer.h"
#include "CSyncServerProxyBank.h"
#include "TraceHelper.h"

CCoreServerThread::CCoreServerThread(CCoreServerThreadMgr* pMgr)
: m_pThreadMgr(pMgr)
{
	CreateThread(&m_hThread,1024,&CCoreServerThread::Execute,this);
}

CCoreServerThread::~CCoreServerThread(void)
{
	JoinThread(&m_hThread);
}



void CCoreServerThread::Execute(void* pParam)
{
	return static_cast<CCoreServerThread*>(pParam)->DoExecute();
}

void CCoreServerThread::DoExecute()
{
	SetAltSigStackEnabled(true);

#ifdef _WIN32
	if(CatchErrorEnabled())
		_set_se_translator(TransESHFun);
#endif
	AddWatchCpuThreadID("CoreServerThread",GetCurTID());

	InitSyncServerProxyBank();

	CSyncAppServer::Inst()->Run();
	CSyncAppServer::Inst()->CleanUp();

	UnitSyncServerProxyBank();


	SetAltSigStackEnabled(false);
}
