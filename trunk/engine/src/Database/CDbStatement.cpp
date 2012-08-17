#include "stdafx.h"
#include "CDbStatement.h"
#include "CDbParamBinder.h"
#include "CDbResultBinder.h"
#include "CLock.inl"
#include "CDbConnection.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "TSqrAllocator.inl"

CDbStatement::CDbStatement(CDbConnection* pConn,const char* szSqlBuffer,size_t uSize)
:m_pConn(pConn)
,m_pStmt(NULL)
,m_pResMetaData(NULL)
,m_pTag(NULL)
,m_stNumFields(0)
,m_stParamCount(0)
,m_pFields(NULL)
,m_sSqlBuffer(szSqlBuffer)
{
	Lock();

	SQR_TRY
	{

		m_pStmt=mysql_stmt_init( m_pConn->m_pMySql );

		if(!m_pStmt)
			GenErr( "mysql_stmt_init failed" , mysql_stmt_error(m_pStmt) );
		
		if(mysql_stmt_prepare(m_pStmt,szSqlBuffer,uSize))
			GenErr( "mysql_stmt_prepare failed" , mysql_stmt_error(m_pStmt) );
		
		m_pResMetaData=mysql_stmt_result_metadata(m_pStmt);

		if(m_pResMetaData)
		{
			m_stNumFields = mysql_num_fields(m_pResMetaData);
			m_pFields = mysql_fetch_fields(m_pResMetaData);
		}

		m_stParamCount = mysql_stmt_param_count(m_pStmt);
	}
	SQR_CATCH(exp)
	{
		Unlock();

		SQR_THROW(exp);
	}
	SQR_TRY_END;

	Unlock();

}


CDbStatement::~CDbStatement(void)
{
	Lock();

	SQR_TRY
	{
		if(m_pResMetaData)
		{
			mysql_free_result(m_pResMetaData);
			m_pResMetaData = NULL;
		}
		mysql_stmt_free_result(m_pStmt);
		//mysql_stmt_reset(m_pStmt);
		mysql_stmt_close(m_pStmt);
	}
	SQR_CATCH(exp)
	{
		Unlock();

		SQR_THROW(exp);
	}
	SQR_TRY_END;

	Unlock();
}


IDbParamBinder* CDbStatement::CreateParamBinder()const
{
	return new CDbParamBinder(this);
}


IDbResultBinder* CDbStatement::CreateResultBinder()const
{
	return new CDbResultBinder(this);
}

void CDbStatement::FetchResultRow(size_t uRowIndex)
{
	Lock();

	SQR_TRY
	{
		if( uRowIndex>=GetResultRowNum() )
			GenErr("uRowIndex >= GetResultRowNum().", m_sSqlBuffer.c_str());

		mysql_stmt_data_seek(m_pStmt,uRowIndex);
		//modify by jiaben:
		//mysql_stmt_fetch will return 4 kinds of value
		//0 (means no err)
		//1 (means err)
		//MYSQL_NO_DATA (means no data)
		//MYSQL_DATA_TRUNCATED (means data truncated)
		//if MYSQL_DATA_TRUNCATED or MYSQL_NO_DATA then we do nothing
		//only when 1 we do sth.
		if( mysql_stmt_fetch(m_pStmt) == 1 )
		{
			ostringstream strm;
			strm << mysql_stmt_error(m_pStmt) << endl;
			strm << "Statement: " << m_sSqlBuffer << endl;

			GenErr( "mysql_stmt_fetch failed" , strm.str().c_str() );
		}

	}
	SQR_CATCH(exp)
	{
		Unlock();

		SQR_THROW(exp);
	}
	SQR_TRY_END;

	Unlock();
}

size_t CDbStatement::GetParamNum()const
{
	return m_stParamCount;
}

size_t CDbStatement::GetResultColNum()const
{
	return m_stNumFields;
}

const char* CDbStatement::GetResultColName(size_t uColIndex) const
{
	char* szName = NULL;

	if (uColIndex >= GetResultColNum())
		GenErr("CDbStatement::GetResultColName 传入的列索引大于总列数", m_sSqlBuffer.c_str());

	szName = m_pFields[uColIndex].name;

	return szName;
}

size_t CDbStatement::GetResultRowNum()const
{
	size_t stResult = 0;

	Lock();

	SQR_TRY
	{
		stResult = (size_t)mysql_stmt_num_rows(m_pStmt);
	}
	SQR_CATCH(exp)
	{
		Unlock();

		exp.AppendMsg(m_sSqlBuffer.c_str());
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	Unlock();

	return stResult;
}

IDbConnection* CDbStatement::GetConnection()const
{
	return m_pConn;
}

void CDbStatement::ClearResult()
{
	Lock();

	SQR_TRY
	{
		mysql_stmt_reset(m_pStmt);
		mysql_stmt_free_result(m_pStmt);
	}
	SQR_CATCH(exp)
	{
		Unlock();
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	Unlock();
}

void CDbStatement::Execute()
{
	Lock();

	SQR_TRY
	{
		if(mysql_stmt_execute(m_pStmt))
		{
			GenErr("mysql_stmt_execute failed:",  mysql_stmt_error(m_pStmt));
		}
		if(mysql_stmt_store_result(m_pStmt))
		{
			GenErr("mysql_stmt_store_result failed:",  mysql_stmt_error(m_pStmt));
		}	
	}
	SQR_CATCH(exp)
	{
		Unlock();
	
		exp.AppendType( m_sSqlBuffer.c_str() );

		SQR_THROW(exp);
	}
	SQR_TRY_END;

	Unlock();
}

void CDbStatement::Release()
{
	delete this;
}

void CDbStatement::SetTag(void* pTag)
{
	m_pTag = pTag;
}

void* CDbStatement::GetTag()const
{
	return m_pTag;
}


void CDbStatement::Lock()const
{
	m_pConn->m_Lock.Lock();
}

void CDbStatement::Unlock()const
{
	m_pConn->m_Lock.Unlock();
}

size_t CDbStatement::GetFieldLength(size_t uColIndex)
{
	if (uColIndex >= GetResultColNum())
		GenErr("CDbStatement::GetDataLength 传入的列索引大于总列数", m_sSqlBuffer.c_str());

	return m_pFields[uColIndex].length;
}

namespace sqr
{
	EDbFieldType GetDbFieldType(MYSQL_FIELD& field);
}

EDbFieldType CDbStatement::GetFieldType(size_t uColIndex)
{
	if (uColIndex >= GetResultColNum())
		GenErr("CDbStatement::GetDataType 传入的列索引大于总列数", m_sSqlBuffer.c_str());

	return GetDbFieldType(m_pFields[uColIndex]);	
}

string CDbStatement::GetSql() const
{
	return m_sSqlBuffer.c_str();
}
