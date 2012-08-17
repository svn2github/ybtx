#include "stdafx.h"
#include "CValueTable.h"
#include "ExpHelper.h"
#include "TSqrAllocator.inl"
#include "TVariantTraits.h"
#include "CValueTableData.h"
#include "CMemData.inl"

CValueTable::CValueTable()
: m_uRowNumber(0)
, m_uColNumber(0)
, m_buffNull( CValueMallocObject::GetPool() )
, m_buffOffset( CValueMallocObject::GetPool() )
, m_buffData( CValueMallocObject::GetPool() )
{
}

CValueTable::~CValueTable()
{

}

void CValueTable::Release()
{
	delete this;
}

uint32 CValueTable::GetRowNum()
{
	return m_uRowNumber;
}

uint32 CValueTable::GetColNum()
{
	return m_uColNumber;
}

inline uint32 CValueTable::GetColIndex(const char* szName)
{
	Ast(szName);
	MapName2Col_t::iterator iter = m_mapFieldName2Col.find(szName);
	if (iter != m_mapFieldName2Col.end())
	{
		return iter->second;
	}
	else
	{
		ostringstream strm;
		strm << szName << " 字段不存在" << endl;
		GenErr("SerialTable GetColIndex Error", strm.str().c_str());
		return -1;
	}
}

void CValueTable::GenIndexError(const char* szErrorType, uint32 uRow, uint32 uCol)
{
	ostringstream strm;
	strm << "table的长宽为: (" << m_uRowNumber << ", " << m_uColNumber << "), "
		<< "输入索引为: (" << uRow << ", " << uCol << ")" << endl;
	GenErr(szErrorType, strm.str().c_str());
}

bool CValueTable::IsNull(uint32 uRow, uint32 uCol)
{
	if(uRow >= m_uRowNumber || uCol >= m_uColNumber)
	{
		GenIndexError("SerialTable IsNull 索引不正确", uRow, uCol);
		return true;
	}

	return m_buffNull.Value<char>(uRow * m_uColNumber + uCol) == 1;
}

bool CValueTable::IsNull(uint32 uRow, const char* szName)
{
	return IsNull(uRow, GetColIndex(szName));
}

void CValueTable::GenNullError(uint32 uRow, uint32 uCol)
{
	ostringstream strm;
	strm << "输入的索引: (" << uRow << ", " << uCol << ")" << endl;
	GenErr("输入的索引对应的值为null，不能获取", strm.str().c_str());
}

void CValueTable::GenTypeError(const char* szType, uint32 uCol)
{
	ostringstream strm;
	strm << "第" << uCol << "对应的类型为" << szType << endl;
	GenErr("GetData Type Error", strm.str().c_str());
}

const char* CValueTable::GetString(uint32 uRow, uint32 uCol) 
{
	if(uRow >= m_uRowNumber || uCol >= m_uColNumber)
	{
		GenIndexError("SerialTable GetString 索引不正确", uRow, uCol);
		return NULL;
	}

	size_t stPos = uRow * m_uColNumber + uCol;
	if(m_buffNull.Value<char>(stPos))
	{
		GenNullError(uRow, uCol);
	}
	
	if (m_vecFieldType[uCol] != eVT_String)
	{
		GenTypeError("string", uCol);
	}

	uint32 uOffset = m_buffOffset.Value<uint32>(stPos);
	return m_buffData.Data()+uOffset;
}
	
const char* CValueTable::GetString(uint32 uRow, const char* szName) 
{
	return GetString(uRow, GetColIndex(szName));
}

template<typename ValueType>
ValueType CValueTable::GetNumber(uint32 uRow, uint32 uCol) 
{
	if(uRow >= m_uRowNumber || uCol >= m_uColNumber)
	{
		GenIndexError("SerialTable GetNumber 索引不正确", uRow, uCol);
		return true;
	}

	size_t stPos = uRow * m_uColNumber + uCol;
	if(m_buffNull.Value<char>(stPos))
	{
		GenNullError(uRow, uCol);
	}

	if (m_vecFieldType[uCol] != TVariantTraits<ValueType>::GetId())
	{
		GenTypeError(TVariantTraits<ValueType>::GetName(), uCol);
	}

	uint32 uOffset = m_buffOffset.Value<uint32>(stPos);
	
	ValueType ret = *m_buffData.AsPtr<ValueType>(uOffset);
	
	return ret;
}

template<typename ValueType>
ValueType CValueTable::GetNumber(uint32 uRow, const char* szName) 
{
	return GetNumber<ValueType>(uRow, GetColIndex(szName));
}

EVariantType CValueTable::GetType(uint32 uCol)
{
	if(uCol >= m_uColNumber)
	{
		ostringstream strm;
		strm << "索引列: " << uCol << " 最大列数: " << m_uColNumber << endl;
		GenErr("SerialTable GetType Error, 索引的列数超过了最大列数", strm.str().c_str());
	}

	return m_vecFieldType[uCol];
}

EVariantType CValueTable::GetType(const char* szName)
{
	return GetType(GetColIndex(szName));
}

CVariant* CValueTable::GetValue(CValueTableData* pData, uint32 uRow, const char* szName)
{
	uint32 uCol = GetColIndex(szName);
	return GetValue(pData, uRow, uCol);
}

CVariant* CValueTable::GetValue(CValueTableData* pData, uint32 uRow, uint32 uCol)
{
	if (IsNull(uRow, uCol))
	{
		return NULL;
	}

	pData->SetData(this, uRow, uCol);

	return pData->GetData();
}

//----------------------------------------------------------------------------

void CValueTable::Init(uint32 uHeight, uint32 uWidth)
{
	if (uWidth == 0)
		GenErr("ValueTable的列数不允许为0");
	
	m_uRowNumber = uHeight;
	m_uColNumber = uWidth;

	m_vecFieldType.resize(uWidth);

	size_t stSize = uHeight * uWidth;
	m_buffNull.ReSize(stSize);
	m_buffOffset.ReSize(stSize * sizeof(uint32));
}

void CValueTable::SetFieldType(uint32 uCol, EVariantType eType)
{
	Ast(uCol < m_uColNumber);
	m_vecFieldType[uCol] = eType;
}

void CValueTable::SetFieldName(uint32 uCol, const char* szName)
{
	Ast(uCol < m_uColNumber && szName);
	m_mapFieldName2Col[szName] = uCol;
}

void CValueTable::ReserveTable(size_t stBufferSize)
{
	Ast(stBufferSize);
	m_buffData.Reserve( stBufferSize * m_uRowNumber );
}

void* CValueTable::ExpandBuffer(uint32 uRow, uint32 uCol, size_t stDataSize)
{
	Ast(uRow < m_uRowNumber && uCol < m_uColNumber);

	size_t stPos = m_buffData.Size();

	m_buffData.ReSize( stPos + stDataSize );

	m_buffOffset.Value<uint32>(uRow * m_uColNumber + uCol) = (uint32)stPos;

	return m_buffData.AsPtr<char>(stPos);
}

void* CValueTable::GetBuffer(uint32 uRow, uint32 uCol)const
{
	Ast(uRow < m_uRowNumber && uCol < m_uColNumber);
	
	uint32 uOffset = m_buffOffset.Value<uint32>(uRow * m_uColNumber + uCol); 

	Ast(uOffset < m_buffData.Size());

	return const_cast<void*>( static_cast<const void*>( m_buffData.AsPtr<char>(uOffset) ) );
}

void CValueTable::SetNull(uint32 uRow, uint32 uCol, bool bNull)
{
	Ast(uRow < m_uRowNumber && uCol < m_uColNumber);

	m_buffNull.Value<char>(uRow * m_uColNumber + uCol) = bNull;
}

//将数据填充到table的buffer里面
void CValueTable::SetValue(uint32 uRow, uint32 uCol, const void* pData, size_t stDataSize, bool bNull)
{
	Ast(uRow < m_uRowNumber && uCol < m_uColNumber);
	
	size_t stPos = uRow * m_uColNumber + uCol;

	m_buffNull.Value<char>(stPos) = bNull;

	uint32 uOffset = m_buffOffset.Value<uint32>(stPos); 

	if(!bNull)
	{
		m_buffData.Assign( uOffset, pData , stDataSize );
	}
	else
	{
		m_buffData.Value<char>(uOffset) = '\0';
	}
}	

uint32 CValueTable::GetBufferSize()const
{
	return (uint32)m_buffData.Size();
}

//----------------------------------------------------------------------------

/*
	table serial buffer结构
	
	依次为
	row number，col number, field type, field name, null, offset, data buffer
*/

size_t CValueTable::GetSerializeBufferSize()
{
	size_t stBufferSize = 0;
	stBufferSize += (sizeof(uint32) + sizeof(uint32));
	stBufferSize += m_vecFieldType.size() * sizeof(EVariantType);
	
	//对于 col name，我们需要单独处理
	stBufferSize += sizeof(bool);  //用一个bool记录是否有name
	MapName2Col_t::iterator iter = m_mapFieldName2Col.begin();
	stBufferSize += sizeof(uint32); //name number
	while(iter != m_mapFieldName2Col.end())
	{
		stBufferSize += sizeof(uint32);          //name的长度
		stBufferSize += iter->first.size() + 1;  //存储的name
		stBufferSize += sizeof(uint32);          //对应的col index
		++iter;
	}

	stBufferSize += m_buffNull.Size();
	stBufferSize += m_buffOffset.Size();
	stBufferSize += m_buffData.Size();

	return stBufferSize;
}

void CValueTable::SerializeData(char* szBuffer, const void* pData, size_t stDataSize, size_t& stOffset)
{
	memcpy(szBuffer + stOffset, pData, stDataSize);
	stOffset += stDataSize;
}

void CValueTable::SerializeName(char* szBuffer, size_t& stOffset)
{
	bool bHaveName = !m_mapFieldName2Col.empty();
	
	SerializeData(szBuffer, &bHaveName, sizeof(bool), stOffset);

	if (!bHaveName)
		return;

	uint32 uNameCount = (uint32)m_mapFieldName2Col.size();
	SerializeData(szBuffer, &uNameCount, sizeof(uint32), stOffset);

	MapName2Col_t::iterator iter = m_mapFieldName2Col.begin();
	while(iter != m_mapFieldName2Col.end())
	{
		uint32 uNameSize = (uint32)iter->first.size() + 1;
		SerializeData(szBuffer, &uNameSize, sizeof(uint32), stOffset);
		SerializeData(szBuffer, iter->first.c_str(), uNameSize, stOffset);
		SerializeData(szBuffer, &iter->second, sizeof(uint32), stOffset);
		++iter;
	}
}

void CValueTable::Serialize(char* szBuffer)
{
	size_t stOffset = 0;

	SerializeData(szBuffer, &m_uRowNumber, sizeof(uint32), stOffset);
	SerializeData(szBuffer, &m_uColNumber, sizeof(uint32), stOffset);
	SerializeData(szBuffer, &m_vecFieldType[0], sizeof(EVariantType) * m_vecFieldType.size(), stOffset);
	SerializeName(szBuffer, stOffset);

	//如果rownumber为0，那么我们serialize的是一个空表，下面的内容就不用serial了
	if (m_uRowNumber != 0)
	{
		SerializeData(szBuffer, m_buffNull.Data(), m_buffNull.Size(), stOffset);
		SerializeData(szBuffer, m_buffOffset.Data(), m_buffOffset.Size(), stOffset);
		SerializeData(szBuffer, m_buffData.Data(), m_buffData.Size(), stOffset);
	}
}


void CValueTable::UnSerializeData(void* pData, const char* szBuffer, size_t stDataSize, size_t& stOffset)
{
	memcpy(pData, szBuffer + stOffset, stDataSize);
	stOffset += stDataSize;
}

void CValueTable::UnSerializeName(const char* szBuffer, size_t& stOffset)
{
	bool bHaveName = false;
	UnSerializeData(&bHaveName, szBuffer, sizeof(bool), stOffset);
	if (!bHaveName)
	{
		return ;
	}

	uint32 uNameCount = 0;
	UnSerializeData(&uNameCount, szBuffer, sizeof(uint32), stOffset);

	FieldNameStr_t sName;
	uint32 uNameSize = 0;
	uint32 uColIndex = 0;

	for (uint32 i = 0; i < uNameCount; i++)
	{
		UnSerializeData(&uNameSize, szBuffer, sizeof(uint32), stOffset);
		sName.resize(uNameSize);
		UnSerializeData(&sName[0], szBuffer, uNameSize, stOffset);
		UnSerializeData(&uColIndex, szBuffer, sizeof(uint32), stOffset);

		m_mapFieldName2Col[sName] = uColIndex;
	}
}

void CValueTable::UnSerialize(const char* szBuffer, size_t stBufferSize)
{
	size_t stOffset = 0;
	
	UnSerializeData(&m_uRowNumber, szBuffer, sizeof(uint32), stOffset);
	UnSerializeData(&m_uColNumber, szBuffer, sizeof(uint32), stOffset);

	m_vecFieldType.resize(m_uColNumber);
	size_t stFieldBufferSize = sizeof(EVariantType) * m_vecFieldType.size();
	UnSerializeData(&m_vecFieldType[0], szBuffer, stFieldBufferSize, stOffset);

	UnSerializeName(szBuffer, stOffset);

	if (m_uRowNumber != 0)
	{
		size_t stVecSize = m_uRowNumber * m_uColNumber;

		m_buffNull.ReSize(stVecSize);
		size_t stNullBufferSize = m_buffNull.Size();
		UnSerializeData(m_buffNull.Data(), szBuffer, stNullBufferSize, stOffset);

		m_buffOffset.ReSize(stVecSize*sizeof(uint32));
		size_t stOffsetBufferSize = m_buffOffset.Size();
		UnSerializeData(m_buffOffset.Data(), szBuffer, stOffsetBufferSize, stOffset);

		size_t stBuffer = stBufferSize - stOffset;
		m_buffData.ReSize(stBuffer);

		UnSerializeData(m_buffData.Data(), szBuffer, stBuffer, stOffset);
	}
	
}



template bool CValueTable::GetNumber<bool>(uint32, uint32);
template int8 CValueTable::GetNumber<int8>(uint32, uint32);
template uint8 CValueTable::GetNumber<uint8>(uint32, uint32);
template int16 CValueTable::GetNumber<int16>(uint32, uint32);
template uint16 CValueTable::GetNumber<uint16>(uint32, uint32);
template int32 CValueTable::GetNumber<int32>(uint32, uint32);
template uint32 CValueTable::GetNumber<uint32>(uint32, uint32);
template int64 CValueTable::GetNumber<int64>(uint32, uint32);
template uint64 CValueTable::GetNumber<uint64>(uint32, uint32);
template float CValueTable::GetNumber<float>(uint32, uint32);
template double CValueTable::GetNumber<double>(uint32, uint32);

template bool CValueTable::GetNumber<bool>(uint32, const char*);
template int8 CValueTable::GetNumber<int8>(uint32, const char*);
template uint8 CValueTable::GetNumber<uint8>(uint32, const char*);
template int16 CValueTable::GetNumber<int16>(uint32, const char*);
template uint16 CValueTable::GetNumber<uint16>(uint32, const char*);
template int32 CValueTable::GetNumber<int32>(uint32, const char*);
template uint32 CValueTable::GetNumber<uint32>(uint32, const char*);
template int64 CValueTable::GetNumber<int64>(uint32, const char*);
template uint64 CValueTable::GetNumber<uint64>(uint32, const char*);
template float CValueTable::GetNumber<float>(uint32, const char*);
template double CValueTable::GetNumber<double>(uint32, const char*);

