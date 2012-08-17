// DbPerfTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "IDatabase.h"
#include "CQuickRand.h"
#include "CTimeCheckPoint.h"
#include "ExpHelper.h"

using namespace sqr;

//打印使用帮助
static void PrintUsage();


//命令行参数提取
static const TCHAR* GetDatabaseFromArgs(int nArgCount,TCHAR* aryArg[]);
static const TCHAR* GetOperationFromArgs(int nArgCount,TCHAR* aryArg[]);
static const TCHAR* GetOptionFromArgs(int nArgCount,TCHAR* aryArg[],const TCHAR* szOption);


//真正的数据库操作
static void DoCreateOperation(IDbConnection* pConn,const TCHAR* szDatabase);
static void DoInsertOperation(IDbConnection* pConn);
static void DoSelectOperation(IDbConnection* pConn);
static void DoUpdateOperation(IDbConnection* pConn);


int main(int argc, TCHAR* argv[])
{
	if( argc < 2 )
	{
		PrintUsage();
		return 0;
	}

	const TCHAR* szDatabase=GetDatabaseFromArgs(argc,argv);
	if( !szDatabase )
	{
		PrintUsage();
		return 0;
	}

	const TCHAR* szHostName=GetOptionFromArgs(argc,argv,"-h");
	const TCHAR* szUserName=GetOptionFromArgs(argc,argv,"-u");
	const TCHAR* szPassword=GetOptionFromArgs(argc,argv,"-p");
	const TCHAR* szPort=GetOptionFromArgs(argc,argv,"-o");

	if( !szHostName )
		szHostName="127.0.0.1";
	uint16 uPort= !szPort ? 3306:atoi(szPort);

	IDatabase* pDatabase=NULL;
	IDbConnection* pConn=NULL;

	SQR_TRY
	{
		pDatabase=GetDatabase();
		pConn=pDatabase->CreateConnection(szHostName,uPort,szUserName,szPassword,NULL);

		const TCHAR* szOperation=GetOperationFromArgs(argc,argv);

		if(szOperation)
		{
			if( strcmp("create",szOperation)==0 )
			{
				DoCreateOperation(pConn,szDatabase);
				return 0;
			}

			stringstream strm;
			strm<<"use "<<szDatabase;
			pConn->Execute(strm.str().c_str());

			if( strcmp("insert",szOperation)==0 )
			{
				DoInsertOperation(pConn);
				return 0;
			}
			else if( strcmp("select",szOperation)==0 )
			{
				DoSelectOperation(pConn);
				return 0;
			}
			else if( strcmp("update",szOperation)==0 )
			{
				DoUpdateOperation(pConn);
				return 0;
			}
			PrintUsage();
			return 0;
		}
		else
		{
			stringstream strm;
			strm<<"use "<<szDatabase;
			pConn->Execute(strm.str().c_str());

			DoInsertOperation(pConn);
		}
	}
	SQR_CATCH(exp)
	{
		clog<<exp.ErrorMsg()<<endl;
	}
	SQR_TRY_END;

	if(pConn)
		pConn->Release();
	if(pDatabase)
		pDatabase->Release();

	return 0;
}



/////////////////////////////////////////////////////////////////////////////////
//					帮助
/////////////////////////////////////////////////////////////////////////////////
//打印程序的用法
static void PrintUsage()
{
	cerr<<"Usage: TestDbPerf [Option] Database [Operation]"<<endl
		<<"  Options:"<<endl
		<<"    -hHostname      Connect to host."<<endl
		<<"    -uUserName      UserName."<<endl
		<<"    -pPassword      Password."<<endl
		<<"    -oPort          Port."<<endl
		<<"  Operations:"<<endl
		<<"    create          Create test database."<<endl
		<<"    insert          Endless insert.Default operation."<<endl
		<<"    select          Endless select."<<endl
		<<"    update          Endless update."<<endl;		
}


/////////////////////////////////////////////////////////////////////////////////
//					参数处理
/////////////////////////////////////////////////////////////////////////////////

//从命令行参数中提取数据库名称
static const TCHAR* GetDatabaseFromArgs(int nArgCount,TCHAR* aryArg[])
{
	for(int i=1;i<nArgCount;++i)
	{
		const TCHAR* szArg=aryArg[i];
		if( szArg[0] != '-' )
			return szArg;
	}
	return NULL;
}

static const TCHAR* GetOperationFromArgs(int nArgCount,TCHAR* aryArg[])
{
	int i=1;
	for(;i<nArgCount;++i)
	{
		if( aryArg[i][0] != '-' )
			break;
	}

	++i;

	for(;i<nArgCount;++i)
	{
		const TCHAR* szArg=aryArg[i];
		if( szArg[0] != '-' )
			return szArg;
	}
	return NULL;
}

//从命令行参数中提取某个开关的额外参数
static const TCHAR* GetOptionFromArgs(int nArgCount,TCHAR* aryArg[],const TCHAR* szOption)
{
	for(int i=1;i<nArgCount;++i)
	{
		const TCHAR* szArg=aryArg[i];

		if(strncmp(szOption,szArg,strlen(szOption))!=0)
			continue;

		return szArg+2;
	}
	return NULL;
}



/////////////////////////////////////////////////////////////////////////////////
//					数据库操作
/////////////////////////////////////////////////////////////////////////////////
static void DoCreateOperation(IDbConnection* pConn,const TCHAR* szDatabase)
{
	stringstream strm;
	
	strm<<"drop database if exists "<<szDatabase<<ends;
	pConn->Execute(strm.str().c_str());

	strm.seekp(0);
	strm.seekg(0);


	strm<<"create database "<<szDatabase<<ends;
	pConn->Execute(strm.str().c_str());

	strm.seekp(0);
	strm.seekg(0);


	strm<<"use "<<szDatabase<<ends;
	pConn->Execute(strm.str().c_str());

	strm.seekp(0);
	strm.seekg(0);

	
	strm<<
		"create table test"
		"("
		"id bigint unsigned auto_increment,"
		"value0 double,"
		"value1 double,"
		"value2 double,"
		"value3 double,"
		"value4 double,"
		"value5 double,"
		"primary key(id)"
		")engine=innodb "
		//"partition by key(id) "
		//"partitions 10"
		<<ends;
	pConn->Execute(strm.str().c_str());	
}


static int32 GetRowCount(IDbConnection* pConn)
{
	pConn->Execute
		(
		"insert into test set "
		"value0=rand(),"
		"value1=rand(),"
		"value2=rand(),"
		"value3=rand(),"
		"value4=rand(),"
		"value5=rand()"
		);	
	return int32(pConn->LastInsertId());
}

static uint32 g_uInterval=5000;

static void DoInsertOperation(IDbConnection* pConn)
{
	IDbStatement* pStmt=pConn->CreateStatement
		(
		"insert into test set "
		"value0=rand(),"
		"value1=rand(),"
		"value2=rand(),"
		"value3=rand(),"
		"value4=rand(),"
		"value5=rand()"
		);

	SQR_TRY
	{
		for(;;)
		{
			CTimeCheckPoint tcp;
			int i=0;
			for(;tcp.GetElapse()<g_uInterval;++i) 
			{
				pStmt->Execute();
			}
			cout<<i*1000/g_uInterval<<endl;
		}
	}
	SQR_CATCH(exp)
	{
		cout << exp.ErrorMsg() << endl;;
		pStmt->Release();
		throw;
	}
	SQR_TRY_END;
	
	pStmt->Release();
}

static void DoSelectOperation(IDbConnection* pConn)
{
	IDbStatement* pStmt=pConn->CreateStatement("select value0,value1,value2,value3,value4,value5 from test where id=?");
	IDbParamBinder* pPb=pStmt->CreateParamBinder();
	IDbResultBinder* pRb=pStmt->CreateResultBinder();
	
	int32 nId;
	double value;
	
	pPb->SetParamInt32(&nId,0);
	pPb->Bind();

	pRb->SetResultDouble(&value,NULL,NULL,0);
	pRb->SetResultDouble(&value,NULL,NULL,1);
	pRb->SetResultDouble(&value,NULL,NULL,2);
	pRb->SetResultDouble(&value,NULL,NULL,3);
	pRb->SetResultDouble(&value,NULL,NULL,4);
	pRb->SetResultDouble(&value,NULL,NULL,5);
	pRb->Bind();

	SQR_TRY
	{
		int nRowCount=GetRowCount(pConn);
		for(;;)
		{			
			if(nRowCount==0)
			{
				cerr<<"Database is empty."<<endl;
				break;
			}

			int nRandRange = 5000*200;

			if( nRowCount< nRandRange )
				nRandRange = nRowCount;
			
			const int nRandBegin = (nRowCount - nRandRange)/2;
			const int nRandEnd = (nRowCount + nRandRange)/2;

			CTimeCheckPoint tcp;
			int i=0;
			for(;tcp.GetElapse()<g_uInterval;++i) 
			{
				nId=RandRC1(nRandBegin,nRandEnd);
				//nId=RandRC1(1,1453529186)/145;
				pStmt->Execute();
				pStmt->FetchResultRow(0);
			}
			cout<<i*1000/g_uInterval<<endl;

			nRowCount=GetRowCount(pConn);
		}
	}
	SQR_CATCH(exp)
	{
		cout << exp.ErrorMsg() << endl;
		pPb->Release();
		pStmt->Release();
		throw;
	}
	SQR_TRY_END;

	pPb->Release();
	pStmt->Release();
}

static void DoUpdateOperation(IDbConnection* pConn)
{
	IDbStatement* pStmt=pConn->CreateStatement
		(
		"update test set "
		"value0=rand(),"
		"value1=rand(),"
		"value2=rand(),"
		"value3=rand(),"
		"value4=rand(),"
		"value5=rand() "
		"where id=?"
		);
	IDbParamBinder* pPb=pStmt->CreateParamBinder();

	int32 nId;
	
	pPb->SetParamInt32(&nId,0);
	pPb->Bind();

	SQR_TRY
	{
		int nRowCount=GetRowCount(pConn);
		for(;;)
		{			
			if(nRowCount==0)
			{
				cerr<<"Database is empty."<<endl;
				break;
			}
			CTimeCheckPoint tcp;
			int i=0;
			for(;tcp.GetElapse()<g_uInterval;++i) 
			{
				nId=RandRC1(1,nRowCount);
				pStmt->Execute();
			}
			cout<<i*1000/g_uInterval<<endl;
			nRowCount=GetRowCount(pConn);
		}
	}
	SQR_CATCH(exp)
	{
		cout << exp.ErrorMsg() << endl;
		pPb->Release();
		pStmt->Release();
		throw;
	}
	SQR_TRY_END;

	pPb->Release();
	pStmt->Release();
}
