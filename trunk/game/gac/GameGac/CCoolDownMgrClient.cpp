#include "stdafx.h"
#include "CCoolDownMgrClient.h"
#include "TCoolDownMgr.inl"
#include "CAppClient.h"

template class TCoolDownMgr<CAppClient,CFighterDirector>;

CCoolDownMgrClient* CCoolDownMgrClient::ms_pInst=NULL;


void CCoolDownMgrClient::Init(CFighterDirector* pFighter)
{
	//Ast(!ms_pInst);
	if (ms_pInst)
	{
		Unit();
	}
	ms_pInst=new CCoolDownMgrClient(pFighter);
}

void CCoolDownMgrClient::Unit()
{
	Ast(ms_pInst);
	delete ms_pInst;
	ms_pInst=NULL;
}

CCoolDownMgrClient& CCoolDownMgrClient::Inst()
{
	return *ms_pInst;
}

CCoolDownMgrClient::CCoolDownMgrClient(CFighterDirector* pFighter)
:TCoolDownMgr(pFighter)
{
}