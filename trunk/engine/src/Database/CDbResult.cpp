#include "stdafx.h"
#include "CDbResult.h"
#include "CDbConnection.h"
#include "CLock.inl"
#include "ExpHelper.h"


CDbResult::CDbResult(MYSQL_RES* pRes,CDbConnection* pConn)
:m_pRes(pRes),m_Row(NULL),m_pTag(NULL),m_pConn(pConn)
,m_stNumFields(0)
,m_pFields(NULL)
{
	Ast(pRes);
	m_pFields = mysql_fetch_fields(m_pRes);
	m_stNumFields = mysql_num_fields(m_pRes);
}


CDbResult::~CDbResult()
{
	Lock();

	SQR_TRY
	{
		mysql_free_result(m_pRes);
	}
	SQR_CATCH(exp)
	{
		Unlock();
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	Unlock();
}


size_t CDbResult::GetRowNum()
{
	size_t stResult = 0;

	Lock();

	SQR_TRY
	{
		stResult= static_cast<size_t>( mysql_num_rows(m_pRes) );
	}
	SQR_CATCH(exp)
	{
		Unlock();
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	Unlock();

	return stResult;
}

size_t CDbResult::GetColNum()
{
	return m_stNumFields;
}

const char* CDbResult::GetColName(size_t uColIndex) const
{
	const char* szName = NULL;

	if ( uColIndex >= m_stNumFields )
	{
		ostringstream strm;
		strm<< uColIndex << "," << m_stNumFields ;
		GenErr("CDbResult::GetColName 传入的列索引大于总列数" , strm.str());
	}
	
	szName = m_pFields[uColIndex].name;

	return szName;
}

void CDbResult::Locate(size_t uIndex)
{
	Lock();

	SQR_TRY
	{
		if( uIndex >= GetRowNum() )
		{
			ostringstream strm;
			strm<< uIndex << "," << GetRowNum();
			GenErr("CDbResult::Locate 传入行索引>=总行数", strm.str());
		}

		mysql_data_seek(m_pRes,uIndex);
		m_Row=mysql_fetch_row(m_pRes);
		m_Lengths=mysql_fetch_lengths(m_pRes);
	}
	SQR_CATCH(exp)
	{
		Unlock();
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	Unlock();
}

const char* CDbResult::GetData(size_t uColIndex)
{
	Ast(GetRowNum()>0);
	Ast(uColIndex<GetColNum());
	return m_Row[uColIndex];
}


unsigned CDbResult::GetDataLength(size_t uColIndex)
{
	Ast(uColIndex<GetColNum());
	return m_Lengths[uColIndex];
}

void CDbResult::Release()
{
	delete this;
}

void CDbResult::SetTag(void* pTag)
{
	m_pTag = pTag;
}

void* CDbResult::GetTag()const
{
	return m_pTag;
}


void CDbResult::Lock()const
{
	m_pConn->m_Lock.Lock();
}

void CDbResult::Unlock()const
{
	m_pConn->m_Lock.Unlock();
}

namespace sqr
{
	EDbFieldType GetDbFieldType(MYSQL_FIELD& field);
}


size_t CDbResult::GetFieldLength(size_t uColIndex)
{
	Ast(uColIndex<GetColNum());
	
	return m_pFields[uColIndex].length;
}

EDbFieldType CDbResult::GetFieldType(size_t uColIndex)
{
	Ast(uColIndex<GetColNum());

	return GetDbFieldType(m_pFields[uColIndex]);
}
