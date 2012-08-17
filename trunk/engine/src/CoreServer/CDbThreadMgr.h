#pragma once
#include "ThreadTypes.h"
#include "IDbThreadMgr.h"
#include "TMtDbAllocator.h"

namespace sqr
{
	class IDbConnection;
	class IDatabase;
	class IDbStatement;

	class CDbThread;
	class CQueryJob;
	class CDbContext;
	class CDbVariantBank;
	class CQueryResultQueue;
	class CQueryJobQueue;

	template<typename,template<typename>class>
	class TObjectBank;
	class CQuerySingleStmtJob;
	class CQueryBatchStmtJob;
	class CQueryTextJob;
	class CCommitTransJob;
	class CRollbackTransJob;
	class CSetSavepointJob;
	class CRollbackTransToJob;
	
	class CDbThreadMgr
		: public IDbThreadMgr
		, public CMtDbMallocObject
	{
		friend class CDbThread;
		friend class CDbEndThreadsJob;

		friend class CQueryBaseStatementJob;
		friend class CQuerySingleStmtJob;
		friend class CQueryBatchStmtJob;
		friend class CQueryTextJob;
		friend class CCommitTransJob;
		friend class CRollbackTransJob;
		friend class CSetSavepointJob;
		friend class CRollbackTransToJob;

	public:

		CDbThreadMgr(uint8 uNumThread, IDbThreadMgrHandler* pHandler);
		~CDbThreadMgr(void);

		void	AddQueryStatementJob(IDbStatement* pStatement, CDbContext* pContext, vector<CVariant*> *pArgList);
		void	AddQueryBatchStatementJob(IDbStatement* pStatement, CDbContext* pContext,uint32 uExecTime, vector<CVariant*> *pArgList);
		void	AddQueryTextJob(IDbConnection* pConnection, const char* szSql, uint32 uSize, CDbContext* pContext);
		void	AddCommitTransJob(IDbConnection* pConnection, CDbContext* pContext);
		void	AddRollbackTransJob(IDbConnection* pConnection, CDbContext* pContext);
		void	AddSetSavepointJob(IDbConnection* pConnection, CDbContext* pContext, const char* szName);
		void	AddRollbackTransToJob(IDbConnection* pConnection, CDbContext* pContext, const char* szName);

		CDbContext*	GetQueryResult();
		
		bool	PopQueryResult(void);
		void	Release(void);
		IDbThreadMgrHandler* GetHandler()const;
	private:
		void	CreateDbThreads(uint8 uNum);
		void	MgrInit(void);
		void	ReleaseThreads(void);

		void	AddQueryJob(CQueryJob *pJob);	
		void	HandleJob();

	private:
		bool	m_bEndThreads;
		IDbThreadMgrHandler*	m_pHandler;

		CDbVariantBank* m_pStrVariantBank;
		CDbVariantBank*	m_pVariantBank;

		HSEMAPHORE	m_smJobQueue;
		
		HLOCK	m_slkJobQueue;
		
		std::vector<CDbThread*,TMtDbAllocator<CDbThread*> >			m_vecThreads;

		typedef list<CQueryJob*, TMtDbAllocator<CQueryJob*> > ListJob_t;
		typedef list<CQueryJob*, TMtDbAllocator<CQueryJob*> > ListResult_t;

		TObjectBank<CQuerySingleStmtJob,TMtDbAllocator>*	m_pBankSingleStmtJob;
		TObjectBank<CQueryBatchStmtJob,TMtDbAllocator>*		m_pBankBatchStmtJob;
		TObjectBank<CQueryTextJob,TMtDbAllocator>*			m_pBankTextJob;
		TObjectBank<CCommitTransJob,TMtDbAllocator>*		m_pBankCommitTransJob;
		TObjectBank<CRollbackTransJob,TMtDbAllocator>*		m_pBankRollbackTransJob;
		TObjectBank<CSetSavepointJob,TMtDbAllocator>*		m_pBankSetSavepointJob;
		TObjectBank<CRollbackTransToJob,TMtDbAllocator>*	m_pBankRollbackTransToJob;

		CQueryJobQueue*			m_pJobQueue;
		CQueryResultQueue*		m_pResultQueue;
	};
}
