#include "stdafx.h"
#include "CDbParamBinder.h"
#include "CDbStatement.h"
#include "CDbConnection.h"
#include "ExpHelper.h"
#include "CDbBind.h"
#include "CLock.inl"


CDbParamBinder::CDbParamBinder(const CDbStatement* pStmt)
:m_pStmt(pStmt)
{
	m_aryBind = new CDbBind[m_pStmt->GetParamNum()];
}

CDbParamBinder::~CDbParamBinder(void)
{
	delete [] m_aryBind;
}

void CDbParamBinder::ConfirmParamIndexValid( size_t uIndex )
{
	if( uIndex < m_pStmt->GetParamNum() )
		return;

	ostringstream strm;
	strm<< uIndex <<","<< m_pStmt->GetParamNum();
	GenErr( "uIndex >= GetParamNum()" , strm.str() );	
}

void CDbParamBinder::SetParamNull(const bool* pBuffer,size_t uIndex)
{
	ConfirmParamIndexValid( uIndex );
	
	CDbBind* pbind=m_aryBind+uIndex;
	pbind->SetIsNull(const_cast<bool*>(pBuffer));
	pbind->SetBuffer(NULL);
	pbind->SetBuffType(MYSQL_TYPE_NULL);
	pbind->SetIsUnsigned(false);
}

void CDbParamBinder::SetParamInt8(const int8* pBuffer, size_t uIndex)
{
	SetParamIntegerSeries(static_cast<const void*>(pBuffer), uIndex,MYSQL_TYPE_TINY,false);
}

void CDbParamBinder::SetParamUint8(const uint8* pBuffer, size_t uIndex)
{
	SetParamIntegerSeries(static_cast<const void*>(pBuffer), uIndex,MYSQL_TYPE_TINY,true);
}

void CDbParamBinder::SetParamInt16(const int16* pBuffer, size_t uIndex)
{
	SetParamIntegerSeries(static_cast<const void*>(pBuffer), uIndex,MYSQL_TYPE_SHORT,false);
}

void CDbParamBinder::SetParamUint16(const uint16* pBuffer, size_t uIndex)
{
	SetParamIntegerSeries(static_cast<const void*>(pBuffer), uIndex,MYSQL_TYPE_SHORT,true);
}

void CDbParamBinder::SetParamInt32(const int32* pBuffer, size_t uIndex)
{
	SetParamIntegerSeries(static_cast<const void*>(pBuffer), uIndex,MYSQL_TYPE_LONG,false);
}

void CDbParamBinder::SetParamUint32(const uint32* pBuffer, size_t uIndex)
{
	SetParamIntegerSeries(static_cast<const void*>(pBuffer), uIndex,MYSQL_TYPE_LONG,true);
}

void CDbParamBinder::SetParamInt64(const int64* pBuffer, size_t uIndex)
{
	SetParamIntegerSeries(static_cast<const void*>(pBuffer), uIndex,MYSQL_TYPE_LONGLONG,false);
}

void CDbParamBinder::SetParamUint64(const uint64* pBuffer, size_t uIndex)
{
	SetParamIntegerSeries(static_cast<const void*>(pBuffer), uIndex,MYSQL_TYPE_LONGLONG,true);
}

void CDbParamBinder::SetParamFloat(const float* pBuffer, size_t uIndex)
{
	SetParamRealSeries(static_cast<const void*>(pBuffer), uIndex,MYSQL_TYPE_FLOAT);
}

void CDbParamBinder::SetParamDouble(const double* pBuffer, size_t uIndex)
{
	SetParamRealSeries(static_cast<const void*>(pBuffer), uIndex,MYSQL_TYPE_DOUBLE);
}

void CDbParamBinder::SetParamText(const void* pBuffer,size_t uMaxSize,size_t* pActualSize, size_t uIndex)
{
	SetParamBufferSeries(static_cast<const void*>(pBuffer),uMaxSize,pActualSize, uIndex,MYSQL_TYPE_STRING);
}

void CDbParamBinder::SetParamBinary(const void* pBuffer,size_t uMaxSize,size_t* pActualSize, size_t uIndex)
{
	SetParamBufferSeries(static_cast<const void*>(pBuffer),uMaxSize,pActualSize, uIndex,MYSQL_TYPE_BLOB);
}



void CDbParamBinder::SetParamIntegerSeries(const void* pBuffer, size_t uIndex,
										   enum_field_types eBufferType,bool bIsUnsigned)
{
	ConfirmParamIndexValid( uIndex );

	CDbBind* pbind=m_aryBind+uIndex;
	pbind->SetIsNull(NULL);
	pbind->SetBuffer(pBuffer);
	pbind->SetBuffType(eBufferType);
	pbind->SetIsUnsigned(bIsUnsigned);
}


void CDbParamBinder::SetParamRealSeries(const void* pBuffer,
										size_t uIndex,enum_field_types eBufferType)
{
	ConfirmParamIndexValid( uIndex );

	CDbBind* pbind=m_aryBind+uIndex;
	pbind->SetIsNull(NULL);
	pbind->SetBuffer(pBuffer);
	pbind->SetBuffType(eBufferType);
}


void CDbParamBinder::SetParamBufferSeries(const void* pBuffer,size_t uMaxSize,
										  size_t* pActualSize, size_t uIndex,enum_field_types eBufferType)
{
	ConfirmParamIndexValid( uIndex );

	CDbBind* pbind=m_aryBind+uIndex;
	pbind->SetIsNull(NULL);

	pbind->SetBuffer(pBuffer);
	pbind->SetBuffType(eBufferType);
	pbind->SetBuffLength(uMaxSize);
	pbind->SetLength(pActualSize);
}


IDbStatement* CDbParamBinder::GetStatement()const
{
	return const_cast<CDbStatement*>(m_pStmt);
}

void CDbParamBinder::Bind()
{
	CLock& Lock=m_pStmt->m_pConn->m_Lock;

	Lock.Lock();

	SQR_TRY
	{
		if( mysql_stmt_bind_param(m_pStmt->m_pStmt, (MYSQL_BIND*)m_aryBind) )
			GenErr( "mysql_stmt_bind_param failed with error" , mysql_stmt_error(m_pStmt->m_pStmt) );
	}
	SQR_CATCH(exp)
	{
		Lock.Unlock();
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	Lock.Unlock();
}

void CDbParamBinder::Release()
{
	delete this;
}
