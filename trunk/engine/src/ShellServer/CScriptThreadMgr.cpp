#include "stdafx.h"
#include "CScriptThreadMgr.h"
#include "CScriptThread.h"
#include "ExpHelper.h"
#include "ThreadHelper.h"
#include "CScript.h"
#include "CScriptJob.inl"
#include "CScriptResult.inl"
#include "ErrLogHelper.h"
#include "IDatabase.h"
#include "CAppThreadEvent.h"
#include "CAppServer.h"


CScriptThreadMgr::CScriptThreadMgr()
: m_bQuitSignal(false)
{
	MgrInit();
}

CScriptThreadMgr::~CScriptThreadMgr()
{
	MgrUnit();
}

CScriptThreadMgr* CScriptThreadMgr::ms_pInst = NULL;

void CScriptThreadMgr::Init()
{
	if(ms_pInst)
		GenErr("CScriptThreadMgr::Init can not run twice");

	ms_pInst = new CScriptThreadMgr;
}

void CScriptThreadMgr::Unit()
{
	delete ms_pInst;
	ms_pInst = NULL;
}

CScriptThreadMgr* CScriptThreadMgr::Inst()
{
	return ms_pInst;
}

void CScriptThreadMgr::MgrInit()
{
	TLS_CreateKey(&m_tlsThreadKey);
}

void CScriptThreadMgr::MgrUnit()
{
	TLS_DestroyKey(m_tlsThreadKey);
}

void CScriptThreadMgr::ReleaseThreads()
{
	uint32 uThreadNum = m_vecThread.size();
	
	for(uint32 i = 0; i < uThreadNum; i++)
		delete m_vecThread[i];

	m_vecThread.clear();
}


void CScriptThreadMgr::Start(const char* szFileName)
{
	uint32 uThreadNum = m_vecThread.size();
	for(uint32 i = 0; i < uThreadNum; i++)
	{
		m_vecThread[i]->Spawn(szFileName);
	}
}

void CScriptThreadMgr::Stop()
{
	m_bQuitSignal = true;
	
	uint32 uThreadNum = m_vecThread.size();
	for(uint32 i = 0; i < uThreadNum; i++)
	{
		m_vecThread[i]->Join();
	}
}

void CScriptThreadMgr::PostPackFunJob(uint32 uThreadIndex, 
									  const char* strType, size_t stTypeSize,
									  const char* vecBuf, size_t stBufSize)
{
	CScriptThreadMgr* pThreadMgr = CScriptThreadMgr::Inst();	

	CScriptThread* pThread = pThreadMgr->m_vecThread[uThreadIndex];

	(new (pThread) CScriptPostPackJob(strType, stTypeSize, vecBuf, stBufSize, pThread))->Add(pThread);
}



void CScriptThreadMgr::PostPackFunRes(const char* strType, size_t stTypeSize,
									  const char* vecBuf, size_t stBufSize)
{
	CScriptThreadMgr* pThreadMgr = CScriptThreadMgr::Inst();

	CScriptThread* pThread = (CScriptThread*)TLS_GetValue(pThreadMgr->m_tlsThreadKey);

	(new (pThread) CScriptPostPackRes(strType, stTypeSize, vecBuf, stBufSize, pThread))->Add(pThread);
}

void CScriptThreadMgr::AddChangeFileJob(CFileWatcherData* pData)
{
	size_t size = m_vecThread.size();
	for (size_t i = 0; i < size; ++i)
	{
		CScriptThread* pThread = m_vecThread[i];
		
		(new (pThread) CChangeFileJob(pData, pThread))->Add(pThread);
	}
}


CScript* CScriptThreadMgr::CreateScript()
{
	CScriptThread* pThread = new CScriptThread(this);

	m_vecThread.push_back(pThread);

	pThread->m_uIndex = (int32)(m_vecThread.size() - 1);
	
	return pThread->GetScript();
}

void CScriptThreadMgr::ReleaseAllScript()
{
	ReleaseThreads();	
}

CScript* CScriptThreadMgr::GetScript(uint32 uNum)
{
	return m_vecThread[uNum]->GetScript();
}



