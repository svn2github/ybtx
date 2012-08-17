#include "stdafx.h"
#include "CSyncCoreMsgMgr.h"
#include "ExpHelper.h"

CSyncCoreMsgMgr* CSyncCoreMsgMgr::ms_pSyncInst = NULL;


void CSyncCoreMsgMgr::Init()
{
	if(ms_pSyncInst)
		GenErr("CCoreMsgMgr is already initial");

	ms_pSyncInst = new CSyncCoreMsgMgr;
}

void CSyncCoreMsgMgr::Unit()
{
	if(!ms_pSyncInst)
		GenErr("CCoreMsgMgr is already uninitial");

	delete ms_pSyncInst;
	ms_pSyncInst = NULL;
}

CCoreMsgMgr& CSyncCoreMsgMgr::Inst()
{
	return *ms_pSyncInst;
}
