#include "stdafx.h"
#include "CDbResultSet.h"
#include "CValueTable.h"
#include "VariantTypes.h"

CDbResultSet::CDbResultSet()
: m_pValueTable(NULL)
{

}

CDbResultSet::~CDbResultSet()
{
	if (m_pValueTable)
	{
		m_pValueTable->Release();
		m_pValueTable = NULL;
	}
}

void CDbResultSet::SetValueTable(CValueTable *pValueTable)
{
	m_pValueTable = pValueTable;
}

CValueTable* CDbResultSet::GetValueTable()
{
	return m_pValueTable;
}


EVariantType CDbResultSet::GetType(uint32 column) 
{
	return m_pValueTable->GetType(column);
}

const char* CDbResultSet::GetString(uint32 row, uint32 column) 
{
	return m_pValueTable->GetString(row, column);
}

template<typename ValueType>
ValueType CDbResultSet::GetNumber(uint32 uRow, uint32 uCol) 
{
	return m_pValueTable->GetNumber<ValueType>(uRow, uCol);
}

bool CDbResultSet::IsNull(uint32 row, uint32 column) 
{
	return m_pValueTable->IsNull(row, column);
}

EVariantType CDbResultSet::GetType(const char* szColumnName) 
{
	return m_pValueTable->GetType(szColumnName);
}

const char* CDbResultSet::GetString(uint32 row, const char* szColumnName) 
{
	return m_pValueTable->GetString(row, szColumnName);
}

template<typename ValueType>
ValueType CDbResultSet::GetNumber(uint32 uRow, const char* szColumnName) 
{
	return m_pValueTable->GetNumber<ValueType>(uRow, szColumnName);
}

bool CDbResultSet::IsNull(uint32 row, const char* szColumnName) 
{
	return m_pValueTable->IsNull(row, szColumnName);
}

CVariant* CDbResultSet::GetValue(CValueTableData* pData, uint32 uRow, uint32 uCol)
{
	return m_pValueTable->GetValue(pData, uRow, uCol);
}

CVariant* CDbResultSet::GetValue(CValueTableData* pData, uint32 uRow, const char* szName)
{
	return m_pValueTable->GetValue(pData, uRow, szName);
}

uint32 CDbResultSet::GetRowNum() 
{
	return m_pValueTable->GetRowNum();
}

uint32 CDbResultSet::GetColNum() 
{
	return m_pValueTable->GetColNum();
}

void CDbResultSet::Release()
{
	if (m_pValueTable)
	{
		m_pValueTable->Release();
		m_pValueTable = NULL;
	}
}


template int8 CDbResultSet::GetNumber<int8>(uint32, uint32);
template uint8 CDbResultSet::GetNumber<uint8>(uint32, uint32);
template int16 CDbResultSet::GetNumber<int16>(uint32, uint32);
template uint16 CDbResultSet::GetNumber<uint16>(uint32, uint32);
template int32 CDbResultSet::GetNumber<int32>(uint32, uint32);
template uint32 CDbResultSet::GetNumber<uint32>(uint32, uint32);
template int64 CDbResultSet::GetNumber<int64>(uint32, uint32);
template uint64 CDbResultSet::GetNumber<uint64>(uint32, uint32);
template float CDbResultSet::GetNumber<float>(uint32, uint32);
template double CDbResultSet::GetNumber<double>(uint32, uint32);

template int8 CDbResultSet::GetNumber<int8>(uint32, const char*);
template uint8 CDbResultSet::GetNumber<uint8>(uint32, const char*);
template int16 CDbResultSet::GetNumber<int16>(uint32, const char*);
template uint16 CDbResultSet::GetNumber<uint16>(uint32, const char*);
template int32 CDbResultSet::GetNumber<int32>(uint32, const char*);
template uint32 CDbResultSet::GetNumber<uint32>(uint32, const char*);
template int64 CDbResultSet::GetNumber<int64>(uint32, const char*);
template uint64 CDbResultSet::GetNumber<uint64>(uint32, const char*);
template float CDbResultSet::GetNumber<float>(uint32, const char*);
template double CDbResultSet::GetNumber<double>(uint32, const char*);


