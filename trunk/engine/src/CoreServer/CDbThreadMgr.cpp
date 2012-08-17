#include "stdafx.h"
#include "CDbThreadMgr.h"
#include "CDbThread.h"
#include "ThreadHelper.h"
#include "CQueryJob.h"
#include "IDatabase.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "CDbContext.h"
#include "CQueryResultQueue.h"
#include "CQueryJobQueue.h"
#include "TSqrAllocator.inl"
#include "TObjectBank.inl"
#include "CVariant.h"
#include "CDbVariantBank.h"

namespace sqr
{
	IDbThreadMgr* GetDbThreadMgr(uint8 uNumThread, IDbThreadMgrHandler* pHandler)
	{
		return new CDbThreadMgr(uNumThread, pHandler);
	}
}

template class TObjectBank<CVariant,TMtDbAllocator>;

CDbThreadMgr::CDbThreadMgr(uint8 uNumThread, IDbThreadMgrHandler* pHandler)
:m_bEndThreads(false)
,m_pHandler(pHandler)
{
	m_pStrVariantBank=new CDbVariantBank( m_vecThreads.size()*2,true );
	m_pVariantBank=new CDbVariantBank( m_vecThreads.size()*10,true );
	
	m_pBankSingleStmtJob		=new TObjectBank<CQuerySingleStmtJob,TMtDbAllocator>	(m_vecThreads.size(),true);
	m_pBankBatchStmtJob			=new TObjectBank<CQueryBatchStmtJob,TMtDbAllocator>		(m_vecThreads.size(),true);
	m_pBankTextJob				=new TObjectBank<CQueryTextJob,TMtDbAllocator>			(m_vecThreads.size(),true);
	m_pBankCommitTransJob		=new TObjectBank<CCommitTransJob,TMtDbAllocator>		(m_vecThreads.size(),true);
	m_pBankRollbackTransJob		=new TObjectBank<CRollbackTransJob,TMtDbAllocator>		(m_vecThreads.size(),true);
	m_pBankSetSavepointJob		=new TObjectBank<CSetSavepointJob,TMtDbAllocator>		(m_vecThreads.size(),true);
	m_pBankRollbackTransToJob	=new TObjectBank<CRollbackTransToJob,TMtDbAllocator>	(m_vecThreads.size(),true);

	m_pResultQueue	=new CQueryResultQueue;
	m_pJobQueue		=new CQueryJobQueue( uNumThread );
	
	MgrInit();
	CreateDbThreads(uNumThread);
}

CDbThreadMgr::~CDbThreadMgr(void)
{
	ReleaseThreads();

	for( uint8 uIndex=0; uIndex < m_vecThreads.size(); uIndex++ )
		delete m_vecThreads[uIndex];

	DestroySemaphore(&m_smJobQueue);

	DestroyLock(&m_slkJobQueue);

	delete m_pResultQueue;
	m_pResultQueue=NULL;

	delete m_pJobQueue;
	m_pJobQueue=NULL;
	
	delete m_pBankSingleStmtJob;
	delete m_pBankBatchStmtJob;
	delete m_pBankTextJob;
	delete m_pBankCommitTransJob;
	delete m_pBankRollbackTransJob;
	delete m_pBankSetSavepointJob;
	delete m_pBankRollbackTransToJob;
	
	delete m_pVariantBank;
	delete m_pStrVariantBank;
}

void CDbThreadMgr::MgrInit()
{
	CreateSemaphore(&m_smJobQueue,0,INT_MAX);

	CreateLock(&m_slkJobQueue);
}

void CDbThreadMgr::CreateDbThreads(uint8 uNum)
{
	m_vecThreads.resize(uNum);
	for( uint8 uIndex=0; uIndex < uNum; uIndex++ )
	{
		CDbThread* pThread = new CDbThread(this);
		m_vecThreads[uIndex] = pThread;
	}
}

void CDbThreadMgr::AddQueryStatementJob(IDbStatement* pStatement, CDbContext* pContext,
										vector<CVariant*> *pArgList)
{
	CQuerySingleStmtJob* pJob=m_pBankSingleStmtJob->PopOrNew();
	pJob->Init(pStatement, pContext,pArgList,this);
	AddQueryJob(pJob);
}

void CDbThreadMgr::AddQueryBatchStatementJob(IDbStatement* pStatement, CDbContext* pContext,
											 uint32 uExecTime,vector<CVariant*> *pArgList)
{
	CQueryBatchStmtJob* pJob=m_pBankBatchStmtJob->PopOrNew();
	pJob->Init(pStatement, pContext,uExecTime,pArgList,this);
	AddQueryJob(pJob);
}

void CDbThreadMgr::AddQueryTextJob(IDbConnection* pConnection, const char* szSql, 
								   uint32 uSize, CDbContext* pContext)
{
	CQueryTextJob* pJob=m_pBankTextJob->PopOrNew();
	pJob->Init(pConnection, szSql, uSize, pContext);
	AddQueryJob(pJob);
}

void CDbThreadMgr::AddCommitTransJob(IDbConnection* pConnection, CDbContext* pContext)
{
	CCommitTransJob* pJob=m_pBankCommitTransJob->PopOrNew();
	pJob->Init(pConnection, pContext);
	AddQueryJob(pJob);
}

void CDbThreadMgr::AddRollbackTransJob(IDbConnection* pConnection, CDbContext* pContext)
{
	CRollbackTransJob* pJob=m_pBankRollbackTransJob->PopOrNew();
	pJob->Init(pConnection, pContext);
	AddQueryJob(pJob);
}

void CDbThreadMgr::AddSetSavepointJob(IDbConnection* pConnection, CDbContext* pContext, const char* szName)
{
	CSetSavepointJob* pJob=m_pBankSetSavepointJob->PopOrNew();
	pJob->Init(pConnection, pContext,szName);
	AddQueryJob(pJob);
}

void CDbThreadMgr::AddRollbackTransToJob(IDbConnection* pConnection, CDbContext* pContext, const char* szName)
{
	CRollbackTransToJob* pJob=m_pBankRollbackTransToJob->PopOrNew();
	pJob->Init(pConnection, pContext,szName);
	AddQueryJob(pJob);
}

void CDbThreadMgr::ReleaseThreads()
{
	m_bEndThreads = true;

	for(size_t Index=0; Index < m_vecThreads.size(); Index++)
	{
		PutSemaphore(&m_smJobQueue);
	}
}

void CDbThreadMgr::Release()
{
	delete this;
}

IDbThreadMgrHandler* CDbThreadMgr::GetHandler() const
{
	return m_pHandler;
}


void CDbThreadMgr::AddQueryJob(CQueryJob* pJob)
{
	m_pJobQueue->Push(pJob);
	PutSemaphore(&m_smJobQueue);
}


void CDbThreadMgr::HandleJob()
{
	CQueryJob* pJob = m_pJobQueue->Pop();
	
	if( !pJob )
	{
		LogErr("数据库线程在队列中无法获得Job!");
		return;
	}

	pJob->DoJob();
	
	m_pResultQueue->Push(pJob);
	
	m_pHandler->OnQueryJobDone();	
}

CDbContext*	CDbThreadMgr::GetQueryResult()
{
	return m_pResultQueue->Get(this);
}

bool CDbThreadMgr::PopQueryResult()
{
	return m_pResultQueue->Pop(this);
}
