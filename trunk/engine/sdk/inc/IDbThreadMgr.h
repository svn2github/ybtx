#pragma once
#include "CDynamicObject.h"
//#include "CVariant.h"

namespace sqr
{
	class IDbStatement;
	class IDbConnection;
	class IDbTextResult;

	class CValueTable;
	class CQueryResult;

	class IDbThreadMgr;

	class IDbThreadMgrHandler
	{
	public:
		virtual void OnQueryJobDone()=0;
	protected:
		virtual ~IDbThreadMgrHandler(){}		
	};

	class CVariant;
	class CDbContext;

	class IDbThreadMgr
		:public virtual CDynamicObject
	{
	public:
		virtual void AddQueryStatementJob(IDbStatement* pStatement, CDbContext* pContext, vector<CVariant*> *pArgList)=0;
		virtual void AddQueryBatchStatementJob(IDbStatement* pStatement, CDbContext* pContext,uint32 uExecTime, vector<CVariant*> *pArgList)=0;
		virtual void AddQueryTextJob(IDbConnection* pConnection, const char* szSql, uint32 uSize, CDbContext* pContext)=0;
		
		virtual void AddCommitTransJob(IDbConnection* pConnection, CDbContext* pContext) = 0;
		virtual void AddRollbackTransJob(IDbConnection* pConnection, CDbContext* pContext) = 0;

		virtual void AddSetSavepointJob(IDbConnection* pConnection, CDbContext* pContext, const char* szName) = 0;
		virtual void AddRollbackTransToJob(IDbConnection* pConnection, CDbContext* pContext, const char* szName) = 0;

		virtual CDbContext* GetQueryResult() = 0;

		virtual bool PopQueryResult(void)=0;
		virtual IDbThreadMgrHandler* GetHandler()const=0;
		virtual void Release()=0;
	};

	IDbThreadMgr* GetDbThreadMgr(uint8 uNumThread, IDbThreadMgrHandler* pHandler);
}

;
