#include "stdafx.h"
#include "CDbResultParser.h"
#include "CValueTable.h"
#include "IDatabase.h"
#include "ExpHelper.h"
#include "IDbThreadMgr.h"
#include "CSqrAllocPool.h"
#include "TSqrAllocator.inl"
#include "CValueTable.h"



//--------------------------------------------------------

typedef std::basic_string<char,std::char_traits<char>,TMtDbAllocator<char> >	ColNameStr_t;


struct DbResultItemInfo
{
	size_t  uBufferSize;
	size_t  uDataSize;
	bool	bNull;
	bool	bError;
	EDbFieldType	eType;
};

typedef vector<DbResultItemInfo, TMtDbAllocator<DbResultItemInfo> >	VecResultItemInfo_t;

const static uint32 uDefaultBlobSizeBuffer = 256;

static void SetColumnInfoOfValueTable(CValueTable* pValueTable, DbResultItemInfo& item, EDbFieldType eType,
							   size_t stLength, uint32 uColIndex)
{
	item.eType = eType;
	EVariantType eVariantType = eVT_Table;
	size_t stBufferSize = 0;

	switch(eType)
	{
	case eDFT_Int8:
		eVariantType = eVT_Int8;
		stBufferSize = sizeof(int8);
		break;
	case eDFT_Uint8:
		eVariantType = eVT_Uint8;
		stBufferSize = sizeof(uint8);
		break;
	case eDFT_Int16:
		eVariantType = eVT_Int16;
		stBufferSize = sizeof(int16);
		break;
	case eDFT_Uint16:
		eVariantType = eVT_Uint16;
		stBufferSize = sizeof(uint16);
		break;
	case eDFT_Int32:
		eVariantType = eVT_Int32;
		stBufferSize = sizeof(int32);
		break;
	case eDFT_Uint32:
		eVariantType = eVT_Uint32;
		stBufferSize = sizeof(uint32);
		break;
	case eDFT_Int64:
		eVariantType = eVT_Int64;
		stBufferSize = sizeof(int64);
		break;
	case eDFT_Uint64:
		eVariantType = eVT_Uint64;
		stBufferSize = sizeof(uint64);
		break;
	case eDFT_Float:
		eVariantType = eVT_Float;
		stBufferSize = sizeof(float);
		break;
	case eDFT_Double:
		eVariantType = eVT_Double;
		stBufferSize = sizeof(double);
		break;
	case eDFT_String:
		eVariantType = eVT_String;	
		stBufferSize = (uint32)stLength + 2;
		break;
	case eDFT_Blob:
		eVariantType = eVT_String;	
		stBufferSize = uDefaultBlobSizeBuffer; 
		break;
	default:
		GenErr("InValid Data Type");
		break;
	}

	pValueTable->SetFieldType(uColIndex, eVariantType);
	item.uBufferSize = stBufferSize;

}

static void PrepareValueTableAndColumnInfo(IDbStatement* pStatement, CValueTable* pValueTable, 
									   VecResultItemInfo_t& vecResultItemInfo)
{
	size_t stColNum = vecResultItemInfo.size();
	size_t stBufferSize = 0;

	for (size_t i = 0; i < stColNum; i++)
	{
		const char* szColName = pStatement->GetResultColName(i);

		EDbFieldType eType = pStatement->GetFieldType(i);
		size_t stLength = pStatement->GetFieldLength(i);

		pValueTable->SetFieldName(i, szColName);

		DbResultItemInfo& iteminfo = vecResultItemInfo[i];		

		SetColumnInfoOfValueTable(pValueTable, iteminfo, eType, stLength, (uint32)i);

		stBufferSize += iteminfo.uBufferSize;
	}

	pValueTable->ReserveTable(stBufferSize);
}


static void BindResultColumn(EDbFieldType eType , IDbResultBinder* pBind, 
							void* buffer,size_t uBufferSize,
							size_t* pDataSize,bool* pIsNull,
							bool* pError,size_t uColIndex)
{
	switch(eType)
	{
	case eDFT_Int8:
		pBind->SetResultInt8((int8*)buffer, pIsNull, pError, uColIndex);
		break;
	case eDFT_Uint8:
		pBind->SetResultUint8((uint8*)buffer, pIsNull, pError, uColIndex);
		break;
	case eDFT_Int16:
		pBind->SetResultInt16((int16*)buffer, pIsNull, pError, uColIndex);
		break;
	case eDFT_Uint16:
		pBind->SetResultUint16((uint16*)buffer, pIsNull, pError, uColIndex);
		break;
	case eDFT_Int32:
		pBind->SetResultInt32((int32*)buffer, pIsNull, pError, uColIndex);
		break;
	case eDFT_Uint32:
		pBind->SetResultUint32((uint32*)buffer, pIsNull, pError, uColIndex);
		break;
	case eDFT_Int64:
		pBind->SetResultInt64((int64*)buffer, pIsNull, pError, uColIndex);
		break;
	case eDFT_Uint64:
		pBind->SetResultUint64((uint64*)buffer, pIsNull, pError, uColIndex);
		break;
	case eDFT_Float:
		pBind->SetResultFloat((float*)buffer, pIsNull, pError, uColIndex);
		break;
	case eDFT_Double:
		pBind->SetResultDouble((double*)buffer, pIsNull, pError, uColIndex);
		break;
	case eDFT_String:
		pBind->SetResultText(buffer, uBufferSize, pDataSize, pIsNull, pError, uColIndex);
		break;
	case eDFT_Blob:
		pBind->SetResultBinary(buffer, uBufferSize, pDataSize, pIsNull, pError, uColIndex);
		break;
	default:
		break;
	}
}

void PrepareBufferInValueTable(IDbStatement* pStatement, IDbResultBinder* pBind, 
						 VecResultItemInfo_t& vecItem, CValueTable* pValueTable,
						 size_t stRowIndex)
{
	//如果有blob字段的，因为这个地段的长度我们只能fetch之后获取，所以是
	//不能够确定预先分配的buffer大小的，为了解决这个问题，我们首先快速
	//获取blob的长度，然后在分配合适的大小重新获取

	size_t stColNum = vecItem.size();
	char buffer[10];
	
	bool bBlobField = false;
	
	for (size_t i = 0; i < stColNum; i++)
	{
		DbResultItemInfo& item = vecItem[i];
		EDbFieldType eType = item.eType;

		if (eType == eDFT_Blob)
		{
			bBlobField = true;
			break;
		}
	}

	if (bBlobField)
	{
		/*
			我们这里必须进行所有参数的绑定，而不是像以前只绑定blob字段的
			因为在windows下面跑debug，如果不全部绑定，mysql那边会报错
		*/
		for (size_t i = 0; i < stColNum; i++)
		{
			DbResultItemInfo& item = vecItem[i];
			EDbFieldType eType = item.eType;
			BindResultColumn(eType, pBind, buffer, 1, &item.uDataSize, 
				&item.bNull, &item.bError, i);
		}
	
		pBind->Bind();
		pStatement->FetchResultRow(stRowIndex);

		pBind->Reset();
	}

	for (size_t i = 0; i < stColNum; i++) 
	{
		DbResultItemInfo& item = vecItem[i];
		EDbFieldType eType = item.eType;

		if (eType == eDFT_Blob)
		{
			item.uBufferSize = item.uDataSize + 2;
		}
		
		//这里我们分配适当的buffer大小，用来存储实际的数据内容
		pValueTable->ExpandBuffer(stRowIndex, i, item.uBufferSize);
	}
}



static void BindResultRow(IDbResultBinder *pBind, VecResultItemInfo_t& vecResultItemInfo, 
									 CValueTable* pValueTable, size_t stRowIndex)
{
	size_t stColNum = vecResultItemInfo.size();

	for (size_t i = 0; i < stColNum; i++)
	{
		DbResultItemInfo& item = vecResultItemInfo[i];
		EDbFieldType eType = item.eType;
		void* buffer = pValueTable->GetBuffer(stRowIndex, i);
		BindResultColumn(eType, pBind, buffer, item.uBufferSize, 
			&item.uDataSize, &item.bNull, &item.bError, i);
	}

	pBind->Bind();
}



CValueTable* CDbResultParser::ParseStatementResult(IDbStatement* pStatement)
{
	uint32 row = pStatement->GetResultRowNum();
	uint32 col = pStatement->GetResultColNum();
	if(col == 0)
		return NULL;

	CValueTable* pValueTable= new CValueTable;
	
	pValueTable->Init(row, col);

	VecResultItemInfo_t	vecResultItemInfo;
	vecResultItemInfo.resize(col);

	PrepareValueTableAndColumnInfo(pStatement, pValueTable, vecResultItemInfo);

	//开始数据绑定以及分析

	//绑定数据
	IDbResultBinder *pBind = pStatement->CreateResultBinder();

	for (size_t i = 0; i < row; i++)
	{
		PrepareBufferInValueTable(pStatement, pBind, vecResultItemInfo, pValueTable, i);

		BindResultRow(pBind, vecResultItemInfo, pValueTable, i);		
		
		//取出数据
		pStatement->FetchResultRow(i);

		//fetch result
		size_t stColNum = vecResultItemInfo.size();

		for (size_t j = 0; j < stColNum; j++)
			pValueTable->SetNull(i, j, vecResultItemInfo[j].bNull);
	}

	pBind->Release();
	
	pStatement->ClearResult();


	return pValueTable;	
}


//----------------------------------------------------------------

void BeforeFetchTextResult(IDbTextResult* pTextResult, CValueTable* pValueTable, VecResultItemInfo_t& vecResultItemInfo)
{
	size_t stColNum = vecResultItemInfo.size();

	size_t stBufferSize = 0;

	for (size_t index = 0; index < stColNum; index++)
	{
		const char* szColName = pTextResult->GetColName(index);
		pValueTable->SetFieldName(index, szColName);

		EDbFieldType eType = pTextResult->GetFieldType(index);
		size_t stLength = pTextResult->GetFieldLength(index);

		DbResultItemInfo& item = vecResultItemInfo[index];

		SetColumnInfoOfValueTable(pValueTable, item, eType, stLength, (uint32)index);

		stBufferSize += item.uBufferSize;
	}

	pValueTable->ReserveTable(stBufferSize);
}

template<typename T>
void FillValueTableNumber(CValueTable* pValueTable, const char* str, 
						  size_t stRowIndex, size_t stColIndex, bool bNull)
{
	T value = (T)atoi(str);
	pValueTable->ExpandBuffer(stRowIndex, stColIndex, sizeof(T));
	pValueTable->SetValue(stRowIndex, stColIndex, &value, sizeof(T), bNull);
	
}

template<typename T>
void FillValueTableNumber64(CValueTable* pValueTable, const char* str, 
						  size_t stRowIndex, size_t stColIndex, bool bNull)
{
#ifdef _WIN32
	T value = (T)_atoi64(str);
#else
	T value = (T)atoll(str);
#endif
	pValueTable->ExpandBuffer(stRowIndex, stColIndex, sizeof(T));
	pValueTable->SetValue(stRowIndex, stColIndex, &value, sizeof(T), bNull);
}

template<typename T>
void FillValueTableFloat(CValueTable* pValueTable, const char* str, 
							size_t stRowIndex, size_t stColIndex, bool bNull)
{
	T value = (T)atof(str);
	pValueTable->ExpandBuffer(stRowIndex, stColIndex, sizeof(T));
	pValueTable->SetValue(stRowIndex, stColIndex, &value, sizeof(T), bNull);
}

void FillValueTableString(IDbTextResult*pTextResult, CValueTable* pValueTable, const char* str, 
						 size_t stRowIndex, size_t stColIndex, bool bNull)
{
	size_t stSize = pTextResult->GetDataLength(stColIndex) + 1;
	pValueTable->ExpandBuffer(stRowIndex, stColIndex, stSize);
	pValueTable->SetValue(stRowIndex, stColIndex, (const void*)str, stSize, bNull);

}

static void LocateTextResult(IDbTextResult* pTextResult, CValueTable* pValueTable, 
					  VecResultItemInfo_t& vecResultItemInfo, size_t stRowIndex)
{
	size_t stColNum = vecResultItemInfo.size();

	pTextResult->Locate(stRowIndex);

	for(size_t stColIndex = 0; stColIndex < stColNum; stColIndex++)
	{
		DbResultItemInfo& item = vecResultItemInfo[stColIndex];

		const char* str = pTextResult->GetData(stColIndex);

		bool bNull = (str == NULL);

		if (bNull)
		{
			pValueTable->SetNull(stRowIndex, stColIndex, true);
		}
		else
		{
			switch(item.eType)
			{
			case eDFT_Int8:
				FillValueTableNumber<int8>(pValueTable, str, stRowIndex, stColIndex, bNull);
				break;
			case eDFT_Uint8:
				FillValueTableNumber<uint8>(pValueTable, str, stRowIndex, stColIndex, bNull);
				break;
			case eDFT_Int16:
				FillValueTableNumber<int16>(pValueTable, str, stRowIndex, stColIndex, bNull);
				break;
			case eDFT_Uint16:
				FillValueTableNumber<uint16>(pValueTable, str, stRowIndex, stColIndex, bNull);
				break;
			case eDFT_Int32:
				FillValueTableNumber<int32>(pValueTable, str, stRowIndex, stColIndex, bNull);
				break;
			case eDFT_Uint32:
				FillValueTableNumber<uint32>(pValueTable, str, stRowIndex, stColIndex, bNull);
				break;
			case eDFT_Int64:
				FillValueTableNumber64<int64>(pValueTable, str, stRowIndex, stColIndex, bNull);
				break;
			case eDFT_Uint64:
				FillValueTableNumber64<uint64>(pValueTable, str, stRowIndex, stColIndex, bNull);
				break;
			case eDFT_Float:
				FillValueTableFloat<float>(pValueTable, str, stRowIndex, stColIndex, bNull);
				break;
			case eDFT_Double:
				FillValueTableFloat<double>(pValueTable, str, stRowIndex, stColIndex, bNull);
				break;
			case eDFT_String:
			case eDFT_Blob:
				FillValueTableString(pTextResult, pValueTable, str, stRowIndex, stColIndex, bNull);
				break;
			default:
				break;
			}
		}


	}
}

CValueTable* CDbResultParser::ParseTextResult(IDbTextResult* pTextResult)
{
	if (!pTextResult)
	{
		return NULL;
	}

	size_t row = pTextResult->GetRowNum();
	size_t col = pTextResult->GetColNum();

	if(col == 0)
		return NULL;

	CValueTable* pValueTable = new CValueTable;
	pValueTable->Init(row, col);

	VecResultItemInfo_t vecResultItemInfo;
	vecResultItemInfo.resize(col);

	BeforeFetchTextResult(pTextResult, pValueTable, vecResultItemInfo);

	//给数据库查询数据分配空间
	for (size_t i = 0; i < row; i++)
	{
		LocateTextResult(pTextResult, pValueTable, vecResultItemInfo, i);
	}


	pTextResult->Release();

	return pValueTable;
}
