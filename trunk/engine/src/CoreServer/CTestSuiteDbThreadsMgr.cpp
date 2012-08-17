#include "stdafx.h"
#include "CTestSuiteDbThreadsMgr.h"
#ifdef _WIN32
#include <Windows.h>
#endif
#include <iostream>
#include "ThreadTypes.h"
#include "ThreadHelper.h"
#include "DatabaseTest.h"
#include <ctime>
#include "IDbThreadMgr.h"
#include "IDbConnectionMgr.h"
#include "IDatabase.h"
#include "CValueTable.h"
#include "CVariant.h"
#include "CDbContext.h"

namespace sqr
{
#ifndef _WIN32
#define MAX_PATH 260
#endif

	const char* RegTestSuiteDbThreadsMgr()
	{
		static TestSuiteFactory<CTestSuiteDbThreadsMgr> m_factory;
		const char* szSuiteName="CTestSuiteDbThreadsMgr";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}

	class CDbThreadHandler : public IDbThreadMgrHandler
	{
	public:
		CDbThreadHandler(void)
		{
			CreateSemaphore(&m_smHandler,0,INT_MAX);
		}

		~CDbThreadHandler(void)
		{
			DestroySemaphore(&m_smHandler);
		}

		void OnQueryJobDone(void)
		{
			PutSemaphore(&m_smHandler);
		}
		
		HSEMAPHORE	m_smHandler;
	};
	
	class ICallback : public CDbContext
	{
	public:
		virtual void SetQueryResult(void* pResult) =0;
		virtual ~ICallback(){}
	};

	class CStatementCallback : public ICallback
	{
	public:

		CStatementCallback(void){};
		virtual ~CStatementCallback(void){};

		void SetQueryResult(void* pResult)
		{
			m_pResult = static_cast<IDbStatement*>(pResult);
		}


	protected:

		IDbStatement*	m_pResult;
	};

	class CTextCallback : public ICallback
	{
	public:

		CTextCallback(void){};
		virtual ~CTextCallback(void){};

		void SetQueryResult(void* pResult)
		{
			m_pText = static_cast<IDbTextResult*>(pResult);
		}

	protected:
		IDbTextResult*	m_pText;
	};


	//-----------------------
	class CResStatSelect : public CStatementCallback
	{
	public:
		CResStatSelect(){};
		virtual ~CResStatSelect(){};

		void Do(void)
		{
			cout<< m_pResult->GetResultRowNum() << endl;
		}

	};

	class CResStatInsert : public CStatementCallback
	{
	public:
		CResStatInsert(){};
		virtual ~CResStatInsert(){};

		void Do(void)
		{
			cout<< m_pResult->GetResultRowNum() << endl;
		}
	};

	class CResTextInsert : public CTextCallback
	{
	public:
		CResTextInsert(){};
		virtual ~CResTextInsert(){};

		void Do(void)
		{
			//cout<< m_pText->GetRowNum() << endl;
			//m_pText->Release();
		}
	};

	class CResTextSelect : public CTextCallback
	{
	public:
		CResTextSelect(){};
		virtual ~CResTextSelect(){};

		void Do(void)
		{
			//cout<< m_pText->GetRowNum() << endl;
			//m_pText->Release();
		}
	};


	void CTestSuiteDbThreadsMgr::setUp()
	{
		m_pConnMgr=NULL;
		m_pDataBase=NULL;
		this->m_semWait = new CDbThreadHandler;
		this->m_pDataBase = GetDatabase();
		this->m_pMgr = GetDbThreadMgr(2, m_semWait);
		this->m_pConnMgr = GetDbConnectionMgr(m_pDataBase, 2, "127.0.0.1", 3306, "root", "1", "");

		char szDrop[] = "drop database if exists DbThreadsMgrTest;";
		char szCreate[] = "create database DbThreadsMgrTest;";
		char szUse[] = "use DbThreadsMgrTest;";
		char szCreateTable[] = "create table test (test_ID int unsigned not null, test_Content char(20) not null );";

		IDbConnection* pConn = m_pConnMgr->GetConnection(0);
		pConn->Execute(szDrop);
		pConn->Execute(szCreate);
		pConn->Execute(szUse);
		pConn->Execute(szCreateTable);
	}

	void CTestSuiteDbThreadsMgr::tearDown()
	{
		if(m_pConnMgr)
			m_pConnMgr->Release();
		if(m_pDataBase)
			m_pDataBase->Release();
		delete m_semWait;
	}
	
	void CTestSuiteDbThreadsMgr::TestInsertAndSelectCount()
	{
		time_t t;
		srand(static_cast<int>(std::time(&t)));

		CStatementCallback* pCallback = new CStatementCallback;
		CStatementCallback* pCallback2 = new CStatementCallback;

		char szQuery[] = "insert into test(test_ID, test_Content) values(?,?);";

		const uint32 COUNT = rand()%20;
		
		char szContent[] = "testdf";

		IDbConnection* pConn = m_pConnMgr->GetConnection(0);
		IDbStatement* pStat = pConn->CreateStatement(szQuery);
		/*IDbParamBinder* pPb = pStat->CreateParamBinder();
		
		uint32 uIndex = 0;
		uint32 uParam = 1;
		pPb->SetParamUint32(&uParam,  uIndex++);
		pPb->SetParamText(szContent, sizeof(szContent), &size, uIndex++);
		pPb->Bind();
		pPb->Release();*/
		
		//CValueTable *pValueTable = CDbResultParser::CreateDbResultSet("ns[6]");
		vector<CVariant*>*pVarList = new vector<CVariant*>;
		CVariant* pVal = new CVariant;
		pVal->SetNumber((double)1);
		pVarList->push_back(pVal);
		pVal = new CVariant;
		pVal->SetString(szContent);
		pVarList->push_back(pVal);

		for(uint32 n=0; n < COUNT; n++ )
		{
			m_pMgr->AddQueryStatementJob(pStat, pCallback, pVarList);

			switch(GetSemaphore(&m_semWait->m_smHandler))
			{
			case 0:
				break;
			default:
				{
					ostringstream strm;
					strm<<"GetSemaphore failed with error:"<<
#ifndef _WIN32
						strerror(errno);
#else
						GetLastError();
#endif
					GenErr(strm.str());
				}
			}
			m_pMgr->PopQueryResult();
		}

		vector<CVariant*>::iterator iter, end;
		iter = pVarList->begin();
		end = pVarList->end();

		while (iter != end)
		{
			delete (*iter);
			iter++;
		}

		delete pVarList;
		

		char szSelect[] = "select count(*) from test;";
		uint32 uSelCount;
		IDbConnection* pConn2 = m_pConnMgr->GetConnection(0);
		IDbStatement* pStatRes = pConn2->CreateStatement(szSelect);
		/*IDbResultBinder* pRb = pStatRes->CreateResultBinder();
		bool error;
		pRb->SetResultUint32(&uSelCount, NULL, &error, 0);
		pRb->Bind();
		pRb->Release();*/
		
		pVarList = new vector<CVariant*>;
		m_pMgr->AddQueryStatementJob(pStatRes, pCallback2, pVarList);
		delete pVarList;

		switch(GetSemaphore(&m_semWait->m_smHandler))
		{
		case 0:
			break;
		default:
			{
				ostringstream strm;
				strm<<"GetSemaphore failed with error \""<<strerror(errno)<<"\""<<ends;
				GenErr(strm.str());
			}
		}

		CValueTable *pValueTable;

		CDbContext* pDbContext = m_pMgr->GetQueryResult();
		pValueTable = pDbContext->GetValueTable();
		//pStatres->FetchResultRow(0);
		
		uSelCount = (uint32)pValueTable->GetNumber<double>(0, (uint32)0);
		
		CPPUNIT_ASSERT_EQUAL(COUNT, uSelCount);
		pValueTable->Release();

		delete pCallback2;
		delete pCallback;
	}


}
