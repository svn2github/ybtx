#include "stdafx.h"
#include "ScriptX.h"

#include "IDatabase.h"
#include "IDbConnectionMgr.h"
#include "IDbThreadMgr.h"
#include "CDbContext.h"
#include "CScript.h"

#include "CScriptThread.h"
#include "CScriptSerialHelper.h"
#include "IScriptThreadHandler.h"
#include "TimeHelper.h"
#include "IDbThreadScriptHandler.h"
#include "CPos.h"
#include "TSqrAllocator.inl"
#include "CFileWatcherData.h"
#include "CValueTable.h"
#include "CValueTableData.h"
#include "CDbResultSet.h"
#include "TSqrAllocator.inl"
#include "CScriptAppServer.h"
#include "CAppConfigServer.h"
#include "CMegaLog.h"

namespace sqr 
{
	void WriteShellLog(const char* szMsg)
	{
		CMegaLog* pLog = CScriptAppServer::Inst()->GetShellLog();
		if (!pLog)
			return;

		pLog->Write(szMsg);
	}
}

void CScriptThread::RegistThreadCoreServer(CScript& Script)
{
	REGIST_B_CLASS( Script, IScriptThreadHandler );
	REGIST_ABSTRACT_B_CLASS( Script, CScriptThread );

	REGIST_ABSTRACT_B_CLASS( Script, IDbConnectionMgr);
	REGIST_ABSTRACT_B_CLASS( Script, IDbThreadMgr);
	REGIST_B_CLASS( Script, IDbThreadScriptHandler );
	REGIST_ABSTRACT_B_CLASS( Script, IDbConnection );
	REGIST_ABSTRACT_B_CLASS( Script, IDbStatement );

	REGIST_B_CLASS( Script, CDbContext );

	//CScriptThread
	REGIST_CALLBACKFUNCTION( Script, IScriptThreadHandler, OnEndThread, "" );	
	REGIST_CALLBACKFUNCTION( Script, IScriptThreadHandler, OnFileChanged, "(CFileWatcherData)" );

	REGIST_CLASSFUNCTION( Script, CScriptThread, Run, "" );
	REGIST_CLASSFUNCTION( Script, CScriptThread, Quit, "" );
	REGIST_STATICFUNCTION( Script, CScriptThread, Inst, ":(CScriptThread)" );
	REGIST_CLASSFUNCTION( Script, CScriptThread, SetHandler, "(IScriptThreadHandler)" );

	REGIST_CLASSFUNCTION( Script, CScriptThread, GetDbThreadMgr, "C(IDbThreadScriptHandler):(IDbThreadMgr)" );
	REGIST_CLASSFUNCTION( Script, CScriptThread, ReleaseDbThreadsMgr, "" );
	
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CScriptThread,
		IDbConnectionMgr*(CScriptThread::*)(uint8, const char*, uint16, const char*, const char*, const char*),
		GetDbConnectionMgr, GetDbConnectionMgrByHost, "CsHsss:(IDbConnectionMgr)");

	REGIST_CLASSFUNCTION_OVERLOAD( Script, CScriptThread,
		IDbConnectionMgr*(CScriptThread::*)(uint8, const char*, const char*, const char*, const char*),
		GetDbConnectionMgr, GetDbConnectionMgrByPipe, "Cssss:(IDbConnectionMgr)");

	REGIST_CLASSFUNCTION( Script, CScriptThread, ReleaseDbConnectionMgr, "(IDbConnectionMgr)" );
	

	/*******************
	IDbConnectionMgr
	*******************/
	REGIST_CLASSFUNCTION( Script, IDbConnectionMgr, GetConnectionNum, ":C");
	REGIST_CLASSFUNCTION( Script, IDbConnectionMgr, GetConnection, "C:(IDbConnection)");

	
	/*******************
	IDbThreadsMgr
	*******************/
	REGIST_CLASSFUNCTION( Script, IDbThreadMgr, AddQueryStatementJob, "(IDbStatement)(CDbContext)l");
	REGIST_CLASSFUNCTION( Script, IDbThreadMgr, AddQueryBatchStatementJob, "(IDbStatement)(CDbContext)Il");
	REGIST_CLASSFUNCTION( Script, IDbThreadMgr, AddQueryTextJob, "(IDbConnection)sI(CDbContext)");
	REGIST_CLASSFUNCTION( Script, IDbThreadMgr, AddCommitTransJob, "(IDbConnection)(CDbContext)");
	REGIST_CLASSFUNCTION( Script, IDbThreadMgr, AddRollbackTransJob, "(IDbConnection)(CDbContext)" );
	REGIST_CLASSFUNCTION( Script, IDbThreadMgr, AddSetSavepointJob, "(IDbConnection)(CDbContext)s" );
	REGIST_CLASSFUNCTION( Script, IDbThreadMgr, AddRollbackTransToJob, "(IDbConnection)(CDbContext)s" );
	REGIST_CLASSFUNCTION( Script, IDbThreadMgr, GetQueryResult, ":(CDbContext)");
	REGIST_CLASSFUNCTION( Script, IDbThreadMgr, PopQueryResult, ":b");
	
	/*******************
	IDbThreadMgrHandler
	*******************/
	REGIST_CALLBACKFUNCTION( Script, IDbThreadScriptHandler, OnSomeQueryJobDone, "" );
	REGIST_CLASSFUNCTION( Script, IDbThreadScriptHandler, Release, "" );

	/*******************
	IDbConnection
	*******************/
	REGIST_CLASSFUNCTION_OVERLOAD( Script, IDbConnection,
		IDbStatement*(IDbConnection::*)(const char*,size_t),
		CreateStatement, CreateStatementWithSize, "st:(IDbStatement)");

	REGIST_CLASSFUNCTION_OVERLOAD( Script, IDbConnection,
		IDbStatement*(IDbConnection::*)(const char*),
		CreateStatement, CreateStatement, "s:(IDbStatement)");

	REGIST_ENUMTYPE(Script, Isolation_Level);
	REGIST_ENUMERATION(Script, Isolation_Level, READ_UNCOMMITTED);
	REGIST_ENUMERATION(Script, Isolation_Level, READ_COMMITTED);
	REGIST_ENUMERATION(Script, Isolation_Level, REPEATABLE_READ);
	REGIST_ENUMERATION(Script, Isolation_Level, SERIALIZABLE);

	REGIST_CLASSFUNCTION( Script, IDbConnection, EscapeString, "sst:t");
	REGIST_CLASSFUNCTION( Script, IDbConnection, LastInsertId, ":Z");
	REGIST_CLASSFUNCTION( Script, IDbConnection, SetAutoCommit, "b");
	REGIST_CLASSFUNCTION( Script, IDbConnection, SetCharacterSet, "s");
	REGIST_CLASSFUNCTION( Script, IDbConnection, SetIsolationLevel, "(Isolation_Level)");
	REGIST_CLASSFUNCTION( Script, IDbConnection, LastAffectedRowNum, ":Z");
	REGIST_CLASSFUNCTION( Script, IDbConnection, Ping, ":b");
	REGIST_CLASSFUNCTION( Script, IDbConnection, Release, ":");
	REGIST_CLASSFUNCTION( Script, IDbStatement, Release, "" );
	REGIST_CLASSFUNCTION( Script, IDbStatement, GetParamNum, ":t" );

	//CDbContext
	REGIST_CLASSFUNCTION( Script, CDbContext, Clear, "" );
	REGIST_CLASSFUNCTION( Script, CDbContext, GetValueTable, ":(CValueTable)" );
	REGIST_CLASSFUNCTION( Script, CDbContext, SetValueTable, "(CValueTable)" );
	REGIST_CLASSFUNCTION( Script, CDbContext, GetReport, ":m" );

	//CDbResultSet
	REGIST_B_CLASS(Script, CDbResultSet);

	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, EVariantType (CDbResultSet::*)(uint32) , 
		GetType, GetType, "I:I");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, const char*(CDbResultSet::*)(uint32, uint32) ,
		GetString, GetString, "II:s");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, bool(CDbResultSet::*)(uint32, uint32) ,
		IsNull, IsNull, "II:b");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, EVariantType (CDbResultSet::*)(const char*) , 
		GetType, GetTypeByName, "s:I");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, const char*(CDbResultSet::*)(uint32, const char*) ,
		GetString, GetStringByName, "Is:s");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, bool(CDbResultSet::*)(uint32, const char*) ,
		IsNull, IsNullByName, "Is:b");

	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, int8 (CDbResultSet::*)(uint32, uint32), 
		GetNumber, GetInt8, "II:c");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, uint8 (CDbResultSet::*)(uint32, uint32), 
		GetNumber, GetUint8, "II:C");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, int16 (CDbResultSet::*)(uint32, uint32), 
		GetNumber, GetInt16, "II:h");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, uint16 (CDbResultSet::*)(uint32, uint32), 
		GetNumber, GetUint16, "II:H");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, int32 (CDbResultSet::*)(uint32, uint32), 
		GetNumber, GetInt32, "II:i");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, uint32 (CDbResultSet::*)(uint32, uint32), 
		GetNumber, GetUint32, "II:I");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, int64 (CDbResultSet::*)(uint32, uint32), 
		GetNumber, GetInt64, "II:z");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, uint64 (CDbResultSet::*)(uint32, uint32), 
		GetNumber, GetUint64, "II:Z");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, float (CDbResultSet::*)(uint32, uint32), 
		GetNumber, GetFloat, "II:f");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, double (CDbResultSet::*)(uint32, uint32), 
		GetNumber, GetDouble, "II:d");

	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, int8 (CDbResultSet::*)(uint32, const char*), 
		GetNumber, GetInt8ByName, "Is:c");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, uint8 (CDbResultSet::*)(uint32, const char*), 
		GetNumber, GetUint8ByName, "Is:C");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, int16 (CDbResultSet::*)(uint32, const char*), 
		GetNumber, GetInt16ByName, "Is:h");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, uint16 (CDbResultSet::*)(uint32, const char*), 
		GetNumber, GetUint16ByName, "Is:H");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, int32 (CDbResultSet::*)(uint32, const char*), 
		GetNumber, GetInt32ByName, "Is:i");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, uint32 (CDbResultSet::*)(uint32, const char*), 
		GetNumber, GetUint32ByName, "Is:I");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, int64 (CDbResultSet::*)(uint32, const char*), 
		GetNumber, GetInt64ByName, "Is:z");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, uint64 (CDbResultSet::*)(uint32, const char*), 
		GetNumber, GetUint64ByName, "Is:Z");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, float (CDbResultSet::*)(uint32, const char*), 
		GetNumber, GetFloatByName, "Is:f");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, double (CDbResultSet::*)(uint32, const char*), 
		GetNumber, GetDoubleByName, "Is:d");

	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, CVariant* (CDbResultSet::*)(CValueTableData*, uint32, uint32),
		GetValue, GetValue, "(CValueTableData)II:a");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CDbResultSet, CVariant* (CDbResultSet::*)(CValueTableData*, uint32, const char*),
		GetValue, GetValueByName, "(CValueTableData)Is:a");


	REGIST_CLASSFUNCTION(Script, CDbResultSet, SetValueTable, "(CValueTable)");
	REGIST_CLASSFUNCTION(Script, CDbResultSet, GetValueTable, ":(CValueTable)");
	REGIST_CLASSFUNCTION(Script, CDbResultSet, GetRowNum, ":I");
	REGIST_CLASSFUNCTION(Script, CDbResultSet, GetColNum, ":I");
	REGIST_CLASSFUNCTION(Script, CDbResultSet, Release, "");
	

	REGISTER_THREAD_SERIAL( Script, "OnHandleVMCallback");
	REGISTER_THREAD_SERIAL( Script, "OnHandleDbCallback");

	REGIST_GLOBALFUNCTION(Script, WriteShellLog, "s");



	typedef TAppConfig<CSyncTraitsServer> CBaseAppConfigServer;

	REGIST_ABSTRACT_B_CLASS( Script, CBaseAppConfigServer );
	REGIST_ABSTRACT_D_CLASS( Script, CAppConfigServer, CBaseAppConfigServer );

	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigServer, double(CBaseAppConfigServer::*)(const char*)const, Get<double>,GetNumber, "s:d" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigServer, double(CBaseAppConfigServer::*)(const char*,const char*)const, Get<double>,GetNumber2, "ss:d" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigServer, double(CBaseAppConfigServer::*)(const char*,const char*,const char*)const, Get<double>,GetNumber3, "sss:d" );

	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigServer, const char*(CBaseAppConfigServer::*)(const char*)const, Get<const char*>,GetString, "s:s" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigServer, const char*(CBaseAppConfigServer::*)(const char*,const char*)const, Get<const char*>,GetString2, "ss:s" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigServer, const char*(CBaseAppConfigServer::*)(const char*,const char*,const char*)const, Get<const char*>,GetString3, "sss:s" );

	REGIST_STATICFUNCTION( Script, CBaseAppConfigServer,Inst, ":(CAppConfigServer)" );

	REGIST_GLOBALFUNCTION(Script, GetProcessTime, ":Z" );
	REGIST_GLOBALFUNCTION(Script, GetHDProcessTime, ":Z");
}
