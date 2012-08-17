#include "stdafx.h"
#include <cstring>
#include "CDbConnection.h"
#include "CDbResult.h"
#include "CDbStatement.h"
#include "CDbStatement.h"
#include "ExpHelper.h"
#include "ThreadHelper.h"
#include "CDatabase.h"
#include "ErrLogHelper.h"
#include "CLock.inl"

static void SetTimeoutOption( MYSQL* pSql )
{
	unsigned int uTimeOut=30;
	VerEq( mysql_options( pSql, MYSQL_OPT_READ_TIMEOUT , reinterpret_cast<const char*>( &uTimeOut) ) ,0 );
	VerEq( mysql_options( pSql, MYSQL_OPT_WRITE_TIMEOUT , reinterpret_cast<const char*>( &uTimeOut) ) ,0 );
}

CDbConnection::CDbConnection(CDatabase* pDatabase,const char* szHost,uint16 uPort
							 ,const char* szUser,const char* szPassword,const char* szDatabase)
							 :m_pDatabase(pDatabase),m_pTag(NULL)
{
	m_pDatabase->m_Lock.Lock();

	SQR_TRY
	{
		m_pMySql = mysql_init(NULL);
		if ( !m_pMySql )
			GenErr( "mysql_init failed with error:", mysql_error(m_pMySql) );

		SetTimeoutOption( m_pMySql );
		
		MYSQL* pResult=mysql_real_connect(m_pMySql, szHost, szUser,szPassword,szDatabase, uPort, NULL, 0);
		//mysql 的文档指出，该函数的返回值和他第一个参数相等时调用成功
		if(m_pMySql != pResult)
			GenErr( "mysql_real_connect failed with error:" ,mysql_error(m_pMySql) );
	}
	SQR_CATCH(exp)
	{
		m_pDatabase->m_Lock.Unlock();

		SQR_THROW(exp);
	}
	SQR_TRY_END;

	m_pDatabase->m_Lock.Unlock();
}

CDbConnection::CDbConnection(CDatabase* pDatabase,const char* szPipe
							 ,const char* szUser,const char* szPassword,const char* szDatabase)
							 :m_pDatabase(pDatabase)
{
	m_pDatabase->m_Lock.Lock();

	SQR_TRY
	{
		m_pMySql = mysql_init(NULL);
		if ( !m_pMySql )
			GenErr( "mysql_init failed with error", mysql_error(m_pMySql) );

		SetTimeoutOption( m_pMySql );
		
		MYSQL* pResult=mysql_real_connect(m_pMySql, NULL, szUser,szPassword,szDatabase, 0, szPipe, 0);
		//mysql 的文档指出，该函数的返回值和他第一个参数相等时调用成功
		if(m_pMySql != pResult)
			GenErr( "mysql_real_connect failed with error" , mysql_error(m_pMySql) );
	}
	SQR_CATCH(exp)
	{
		m_pDatabase->m_Lock.Unlock();
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	m_pDatabase->m_Lock.Unlock();
}

CDbConnection:: ~CDbConnection()
{
	Ast(m_pMySql);

	m_pDatabase->m_Lock.Lock();

	SQR_TRY
	{
		mysql_close(m_pMySql);
	}
	SQR_CATCH(exp)
	{
		m_pDatabase->m_Lock.Unlock();
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	m_pDatabase->m_Lock.Unlock();
}

uint64 CDbConnection::LastInsertId()const
{
	uint64 uResult = 0;

	m_Lock.Lock();

	SQR_TRY
	{
		uResult = mysql_insert_id(m_pMySql);
	}
	SQR_CATCH(exp)
	{
		m_Lock.Unlock();
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	m_Lock.Unlock();

	return uResult;
}

uint64 CDbConnection::LastAffectedRowNum()const
{
	uint64 uResult = 0;

	m_Lock.Lock();

	SQR_TRY
	{
		uResult = mysql_affected_rows(m_pMySql);
	}
	SQR_CATCH(exp)
	{
		m_Lock.Unlock();
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	m_Lock.Unlock();

	return uResult;
}

size_t CDbConnection::EscapeString(char* szOut,const char* szIn,size_t uInSize)
{
	size_t stSize=0;

	m_Lock.Lock();

	SQR_TRY
	{
		stSize = mysql_real_escape_string(m_pMySql,szOut,szIn,uInSize);
	}
	SQR_CATCH(exp)
	{
		m_Lock.Unlock();
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	m_Lock.Unlock();
	return stSize;
}

bool CDbConnection::Ping()
{
	bool bResult = true;

	m_Lock.Lock();

	SQR_TRY
	{
		bResult = ( 0==mysql_ping(m_pMySql) );
	}
	SQR_CATCH(exp)
	{
		m_Lock.Unlock();
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	m_Lock.Unlock();

	return bResult;
}


CDbResult* CDbConnection::GetResult(int nQueryReturnCode)
{
	if(nQueryReturnCode)
		GenErr( "mysql_real_query failed with error", mysql_error(m_pMySql) );
	
	if(mysql_field_count(m_pMySql)>0)
	{
		MYSQL_RES* pRes;

		pRes= mysql_store_result(m_pMySql);

		if(!pRes)
			GenErr( "mysql_store_result failed with error",mysql_error(m_pMySql) );
	
		return new CDbResult(pRes,this);
	}
	return NULL;
}

IDbTextResult* CDbConnection::Execute(const char* szSqlBuffer)
{
	IDbTextResult* pResult=NULL;

	m_Lock.Lock();

	SQR_TRY
	{
		pResult = GetResult( mysql_query(m_pMySql,szSqlBuffer) );
	}
	SQR_CATCH(exp)
	{
		m_Lock.Unlock();
		exp.AppendMsg(szSqlBuffer);
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	m_Lock.Unlock();

	return pResult;
}

IDbTextResult* CDbConnection::Execute(const char* szSqlBuffer,size_t uSize)
{
	IDbTextResult* pResult = NULL;

	m_Lock.Lock();

	SQR_TRY
	{
		pResult = GetResult( mysql_real_query(m_pMySql,szSqlBuffer,uSize) );
	}
	SQR_CATCH(exp)
	{
		m_Lock.Unlock();
		exp.AppendMsg(szSqlBuffer);
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	m_Lock.Unlock();

	return pResult;
}

void CDbConnection::SetAutoCommit(bool bEnabled)
{
	if( bEnabled )
	{
		VerEq( Execute("set autocommit=1"),0 );
	}
	else
	{
		VerEq( Execute("set autocommit=0"),0 );
	}
}

void CDbConnection::SetIsolationLevel(Isolation_Level eIsolationLevel)
{
	switch(eIsolationLevel)
	{
	case READ_UNCOMMITTED:
		VerEq( Execute("set session transaction isolation level read uncommitted"),0 );
		break;
	case READ_COMMITTED:
		VerEq( Execute("set session transaction isolation level read committed"),0 );
		break;
	case REPEATABLE_READ:
		VerEq( Execute("set session transaction isolation level repeatable read"),0 );
		break;
	case SERIALIZABLE:
		VerEq( Execute("set session transaction isolation level serializable"),0 );
		break;
	default:
		GenErr( "无效的隔离等级参数" );
	}
}

void CDbConnection::SetCharacterSet(const char* szCharSet)
{
	char szBuffer[32];	// set character_set_client & character_set_result & character_set_connection
	size_t stSize = snprintf( szBuffer, sizeof( szBuffer), "set names %s", szCharSet );
	VerEq( Execute(szBuffer, stSize), 0 );
}

IDbStatement* CDbConnection::CreateStatement(const char* szSqlBuffer,size_t uSize)
{
	return new CDbStatement(this,szSqlBuffer,uSize);
}

IDbStatement* CDbConnection::CreateStatement(const char* szSqlBuffer)
{
	return CreateStatement(szSqlBuffer,size_t(strlen(szSqlBuffer)));
}

void CDbConnection::Release()
{
	delete this;
};

void CDbConnection::SetTag(void* pTag)
{
	m_pTag=pTag;
}

void* CDbConnection::GetTag()const
{
	return m_pTag;
}

void CDbConnection::CommitTrans()
{
	m_Lock.Lock();

	SQR_TRY
	{
		mysql_commit(m_pMySql);
	}
	SQR_CATCH(exp)
	{
		m_Lock.Unlock();
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	m_Lock.Unlock();
}

void CDbConnection::RollbackTrans()
{
	m_Lock.Lock();

	SQR_TRY
	{
		mysql_rollback(m_pMySql);
	}
	SQR_CATCH(exp)
	{
		m_Lock.Unlock();
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	m_Lock.Unlock();
}

void CDbConnection::SetSavepoint(const char* szName)
{
	char szBuffer[64];
	size_t stSize = snprintf( szBuffer, sizeof( szBuffer), "savepoint %s", szName );
	VerEq( Execute(szBuffer, stSize), 0 );
}

void CDbConnection::RollbackTransTo(const char* szName)
{
	char szBuffer[64];
	size_t stSize = snprintf( szBuffer, sizeof( szBuffer), "rollback to %s", szName );
	VerEq( Execute(szBuffer, stSize), 0 );
}
