#include "stdafx.h"
#include "CMemCallStackInfoMgr.h"
#include "CMemCallStackInfo.h"
#include "MemoryHelper.h"
#include "BaseHelper.h"
#include "ThreadHelper.h"
#include "ExpHelper.h"
#include <iostream>
#include "TimeHelper.h"

CMemCallStackInfoMgr& CMemCallStackInfoMgr::Inst()
{
	//这个单件是不打算删除的，一直保存到进程退出
	static CMemCallStackInfoMgr* ls_pInst=new CMemCallStackInfoMgr;
	return *ls_pInst;
}

CMemCallStackInfoMgr::CMemCallStackInfoMgr(void) : nLastElem(0), nHadElem(0)
{
	CreateLock(&m_Lock); 
}

CMemCallStackInfoMgr::~CMemCallStackInfoMgr(void)
{
	GenErr("CMemCallStackInfoMgr should never destruct.");
}

void CMemCallStackInfoMgr::AddStack(CMemCallStackInfo*& pStackInfo, uint32 uMemSize)
{
	pair<CMemCallStackInfoSet::iterator,bool> p=m_setStack.insert(pStackInfo);

	if( p.second )
	{
		pStackInfo->m_it=p.first;
	}
	else
	{
		delete pStackInfo;
		pStackInfo=*p.first;
	}
	pStackInfo->AddRef(uMemSize);
}

void CMemCallStackInfoMgr::DelStack(CMemCallStackInfo*& pStack,uint32 uMemSize)
{
	if( !pStack->DelRef(uMemSize) )
		return;

	m_setStack.erase( pStack->m_it );
	delete pStack;
}

void CMemCallStackInfoMgr::Print()
{
	CMemCallStackInfoSet::const_iterator it=m_setStack.begin();
	CMemCallStackInfoSet::const_iterator itEnd=m_setStack.end();

	char szBuffer[32];

	WatchLeakage_MemLog("\n************DumpStack begin*************\n");
	WatchLeakage_MemLog(szBuffer);

	for(;it!=itEnd;it++)
	{
		sprintf(szBuffer,"\n%d times call,%d bytes",(int)(*it)->GetCount(),(int)(*it)->GetMemUsage());

		WatchLeakage_MemLog(szBuffer);
		WatchLeakage_MemLog("---------------------------------------------------\n");
		(*it)->Print();
	}

	WatchLeakage_MemLog("************DumpStack end***************\n\n");
}

