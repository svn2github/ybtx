#include "stdafx.h"
#include "CDbResultBinder.h"
#include "CDbStatement.h"
#include "CDbConnection.h"
#include "ExpHelper.h"
#include "Memory.h"
#include "CLock.inl"

CDbResultBinder::CDbResultBinder(const CDbStatement* pStmt)
:m_pStmt(pStmt)
{
	size_t stColNum = m_pStmt->GetResultColNum();
	m_aryBind= (MYSQL_BIND*)SqrNew(sizeof(MYSQL_BIND) * stColNum, CDbMallocObject::GetPool());
	memset(m_aryBind, 0, sizeof(MYSQL_BIND) * stColNum);
}

CDbResultBinder::~CDbResultBinder(void)
{
	SqrDelete(m_aryBind, CDbMallocObject::GetPool());;
}

void CDbResultBinder::SetResultInt8(int8* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)
{
	SetResultIntegerSeries(static_cast<void*>(pBuffer),pIsNull,pError,uColIndex,MYSQL_TYPE_TINY,false);
}

void CDbResultBinder::SetResultUint8(uint8* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)
{
	SetResultIntegerSeries(static_cast<void*>(pBuffer),pIsNull,pError,uColIndex,MYSQL_TYPE_TINY,true);
}

void CDbResultBinder::SetResultInt16(int16* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)
{
	SetResultIntegerSeries(static_cast<void*>(pBuffer),pIsNull,pError,uColIndex,MYSQL_TYPE_SHORT,false);
}

void CDbResultBinder::SetResultUint16(uint16* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)
{
	SetResultIntegerSeries(static_cast<void*>(pBuffer),pIsNull,pError,uColIndex,MYSQL_TYPE_SHORT,true);
}

void CDbResultBinder::SetResultInt32(int32* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)
{
	SetResultIntegerSeries(static_cast<void*>(pBuffer),pIsNull,pError,uColIndex,MYSQL_TYPE_LONG,false);
}

void CDbResultBinder::SetResultUint32(uint32* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)
{
	SetResultIntegerSeries(static_cast<void*>(pBuffer),pIsNull,pError,uColIndex,MYSQL_TYPE_LONG,true);
}

void CDbResultBinder::SetResultInt64(int64* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)
{
	SetResultIntegerSeries(static_cast<void*>(pBuffer),pIsNull,pError,uColIndex,MYSQL_TYPE_LONGLONG,false);
}

void CDbResultBinder::SetResultUint64(uint64* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)
{
	SetResultIntegerSeries(static_cast<void*>(pBuffer),pIsNull,pError,uColIndex,MYSQL_TYPE_LONGLONG,true);
}

void CDbResultBinder::SetResultFloat(float* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)
{
	SetResultRealSeries(static_cast<void*>(pBuffer),pIsNull,pError,uColIndex,MYSQL_TYPE_FLOAT);
}

void CDbResultBinder::SetResultDouble(double* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)
{
	SetResultRealSeries(static_cast<void*>(pBuffer),pIsNull,pError,uColIndex,MYSQL_TYPE_DOUBLE);
}

void CDbResultBinder::SetResultText(void* pBuffer,size_t uBufferSize,
									size_t* uDataSize,bool* pIsNull,bool* pError,size_t uColIndex)
{
	SetResultBufferSeries(pBuffer,uBufferSize,uDataSize,pIsNull,pError,uColIndex,MYSQL_TYPE_STRING);
}

void CDbResultBinder::SetResultBinary(void* pBuffer,size_t uBufferSize,
									  size_t* uDataSize,bool* pIsNull,bool* pError,size_t uColIndex)
{
	SetResultBufferSeries(pBuffer,uBufferSize,uDataSize,pIsNull,pError,uColIndex,MYSQL_TYPE_BLOB);
}

void CDbResultBinder::ConfirmResultIndexValid( size_t stIndex )
{
	if( stIndex < m_pStmt->GetResultColNum() )
		return;

	ostringstream strm;
	strm<< stIndex <<","<< m_pStmt->GetResultColNum();
	GenErr( "uIndex >= GetResultColNum()" , strm.str() );	
}

void CDbResultBinder::SetResultIntegerSeries(void* pBuffer,bool* pIsNull,bool* pError,
											 size_t uColIndex,enum_field_types eBufferType,bool bIsUnsigned)
{
	ConfirmResultIndexValid( uColIndex );

	MYSQL_BIND* pbind=m_aryBind+uColIndex;
	pbind->buffer=pBuffer;
	pbind->buffer_type=eBufferType;
	pbind->is_unsigned=bIsUnsigned;
	//pbind->length=&pbind->buffer_length;
	pbind->length=NULL;
	pbind->error=reinterpret_cast<my_bool*>(pError);
	pbind->is_null=reinterpret_cast<my_bool*>(pIsNull);
}


void CDbResultBinder::SetResultRealSeries(void* pBuffer,bool* pIsNull,bool* pError,
										  size_t uColIndex,enum_field_types eBufferType)
{
	ConfirmResultIndexValid( uColIndex );

	MYSQL_BIND* pbind=m_aryBind+uColIndex;
	pbind->buffer=pBuffer;
	pbind->buffer_type=eBufferType;
	//pbind->length=&pbind->buffer_length;
	pbind->length=NULL;
	pbind->error=reinterpret_cast<my_bool*>(pError);
	pbind->is_null=reinterpret_cast<my_bool*>(pIsNull);
}


void CDbResultBinder::SetResultBufferSeries(void* pBuffer,size_t uBufferSize,
											size_t* uDataSize,bool* pIsNull,bool* pError,size_t uColIndex,enum_field_types eBufferType)
{
	ConfirmResultIndexValid( uColIndex );

	MYSQL_BIND* pbind=m_aryBind+uColIndex;
	pbind->buffer=pBuffer;
	pbind->buffer_type=eBufferType;
	pbind->buffer_length=uBufferSize;
	pbind->length=reinterpret_cast<unsigned long*>(uDataSize);
	pbind->error=reinterpret_cast<my_bool*>(pError);
	pbind->is_null=reinterpret_cast<my_bool*>(pIsNull);
}


IDbStatement* CDbResultBinder::GetStatement()const
{
	return const_cast<CDbStatement*>(m_pStmt);
}

void CDbResultBinder::Bind()
{
	CLock& Lock=m_pStmt->m_pConn->m_Lock;

	Lock.Lock();

	SQR_TRY
	{
		if( mysql_stmt_bind_result(m_pStmt->m_pStmt,m_aryBind) )
		{
			ostringstream strm;
			strm<<"mysql_stmt_bind_result failed with error:"<<mysql_stmt_error(m_pStmt->m_pStmt)<<ends;
			throw std::runtime_error(strm.str());
		}
	}
	SQR_CATCH(exp)
	{
		Lock.Unlock();
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	Lock.Unlock();
}

void CDbResultBinder::Release()
{
	delete this;
}

void CDbResultBinder::Reset()
{
	size_t stColNum = m_pStmt->GetResultColNum();
	memset(m_aryBind, 0, sizeof(MYSQL_BIND) * stColNum);
}

