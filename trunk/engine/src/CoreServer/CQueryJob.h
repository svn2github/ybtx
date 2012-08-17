#pragma once
#include "CMtDbMallocObject.h"
#include "TMtDbAllocator.h"

namespace sqr
{
	class IDbConnection;
	class IDbStatement;
	class IDbTextResult;
	class CDbThreadMgr;

	class CQueryResult;
	class CDbContext;


	class CQueryJob
		: public CMtDbMallocObject
	{
	public:
		void Unit( CDbThreadMgr* pMgr );
		void DoJob(void);
		CDbContext* GetContext(void)const;

		CQueryJob*volatile	m_pNext;
		virtual ~CQueryJob();

	protected:
		CQueryJob();
		void Init( CDbContext* pContext );
		virtual void DoUnit( CDbThreadMgr* pMgr )=0;		

	private:
		virtual void RealDoJob(void)=0;
		virtual IDbConnection* GetConnection()const=0;

	private:
		CDbContext* m_pContext;
	};


	class CQueryDummyJob
		:public CQueryJob
	{
		virtual void DoUnit( CDbThreadMgr* pMgr );
		virtual void RealDoJob(void);
		virtual IDbConnection* GetConnection()const;
	};

	class CValueTable;
	class CVariant;
	class IDbParamBinder;
	class CDbVariantBank;

	class CQueryBaseStatementJob
		: public CQueryJob
	{
	protected:
		void Init(IDbStatement* pStatement, CDbContext* pContext, vector<CVariant*> *pArgList,CDbThreadMgr* pMgr);
		void DoUnit(CDbThreadMgr* pMgr);

		IDbConnection* GetConnection()const;

		IDbStatement* GetStatement()const;

		typedef vector<CVariant*, TMtDbAllocator<CVariant*> > VecArgList_t;

		const VecArgList_t& GetArgList()const;
		VecArgList_t::const_iterator BindAllParam(IDbParamBinder* pPb,VecArgList_t::const_iterator iter,size_t* pLenBuffer);

		size_t GetStrParamNum()const;
	private:
		IDbStatement* m_pStatement;

		VecArgList_t m_ArgList;
		size_t m_stStrParamNum;
	};

	class CQuerySingleStmtJob
		: public CQueryBaseStatementJob
	{
	public:
		void Init(IDbStatement* pStatement, CDbContext* pContext, vector<CVariant*> *pArgList,CDbThreadMgr* pMgr);
	private:
		void DoUnit(CDbThreadMgr* pMgr);
		void RealDoJob();
	};

	class CQueryBatchStmtJob
		: public CQueryBaseStatementJob
	{
	public:
		void Init(IDbStatement* pStatement, CDbContext* pContext, uint32 uExecTime, vector<CVariant*> *pArgList,CDbThreadMgr* pMgr);
	private:
		void DoUnit(CDbThreadMgr* pMgr);
		void RealDoJob();
		uint32 m_uExecTime;
	};


	class CQueryConnectionJob
		:public CQueryJob
	{
	public:
		void Init( CDbContext* pContext , IDbConnection* pConn );
	protected:
		virtual IDbConnection* GetConnection()const;
	private:
		IDbConnection* m_pConn;
	};

	class CQueryTextJob
		: public CQueryConnectionJob
	{
	public:
		void Init(IDbConnection* pConnection, const char* szSql, unsigned uSize, CDbContext* pContext);
	private:
		void DoUnit(CDbThreadMgr* pMgr);
		void RealDoJob();

		typedef std::basic_string<char,std::char_traits<char>,TMtDbAllocator<char> > CmdString_t;

		CmdString_t m_sCmd;
	};

	class CCommitTransJob
		: public CQueryConnectionJob
	{
	public:
		void Init(IDbConnection* pConnection, CDbContext* pContext);
	private:
		void DoUnit(CDbThreadMgr* pMgr);
		void RealDoJob();
	};

	class CRollbackTransJob
		: public CQueryConnectionJob
	{
	public:
		void Init(IDbConnection* pConnection, CDbContext* pContext);
	private:
		void DoUnit(CDbThreadMgr* pMgr);
		void RealDoJob();
	};

	class CSetSavepointJob
		: public CQueryConnectionJob
	{
	public:
		void Init(IDbConnection* pConnection, CDbContext* pContext, const char* szName);
	private:
		void DoUnit(CDbThreadMgr* pMgr);
		void RealDoJob();

		typedef std::basic_string<char,std::char_traits<char>,TMtDbAllocator<char> > CmdString_t;
		CmdString_t m_sCmd;
	};

	class CRollbackTransToJob
		: public CQueryConnectionJob
	{
	public:
		void Init(IDbConnection* pConnection, CDbContext* pContext, const char* szName);
	private:
		void DoUnit(CDbThreadMgr* pMgr);
		void RealDoJob();

		typedef std::basic_string<char,std::char_traits<char>,TMtDbAllocator<char> > CmdString_t;
		CmdString_t m_sCmd;
	};
}
