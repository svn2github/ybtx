/*
*	Tab File Operation Class
*/

#include "stdafx.h"
#include "CTxtTableFile.h"
#include "CPkgFile.h"
#include "CodeCvs.h"
#include <map>
#include <string>
#include <vector>
#include "ThreadTypes.h"
#include "ThreadHelper.h"
#include "CDataFileAllocator.h"
#include "ErrLogHelper.h"
#include "TSqrAllocator.inl"
#include "TBaseTableFile.inl"
#include "CTableFileValue.h"
#include "CTxtTableFileImp.h"


//--------------------------------------------------------------

template class TBaseTableFile<CTxtTableFileImp>;

CTxtTableFile::CTxtTableFile(void)
{
}

CTxtTableFile::~CTxtTableFile(void)
{
	
}

void CTxtTableFile::Release()
{
	delete this;
}

int32 CTxtTableFile::GetHeight()
{
	Ast(m_pImp);
	return m_pImp->GetHeight();	
}

int32 CTxtTableFile::GetWidth()
{
	Ast(m_pImp);
	return m_pImp->GetWidth();
}

ETableFileFieldType CTxtTableFile::GetType( int32 nColumn )
{
	Ast(m_pImp);
	return m_pImp->GetType(nColumn);
}

ETableFileFieldType CTxtTableFile::GetType( const char* szColumnName )
{
	Ast(m_pImp);
	return m_pImp->GetType(szColumnName);
}

const char* CTxtTableFile::GetString( int32 nRow, int32 nColumn )
{
	Ast(m_pImp);
	return m_pImp->GetString(nRow, nColumn);
}

const char* CTxtTableFile::GetString( int32 nRow, const char* szColumnName )
{
	Ast(m_pImp);
	return m_pImp->GetString(nRow, szColumnName);
}

int32 CTxtTableFile::GetInteger( int32 nRow, int32 nColumn, int32 defaultvalue )
{
	const char* szStr = GetString( nRow, nColumn );
	return szStr[0] ? atoi( szStr ) : defaultvalue;
}

int32 CTxtTableFile::GetInteger( int32 nRow, const char* szColumnName, int32 defaultvalue )
{
	const char* szStr = GetString( nRow, szColumnName );
	return szStr[0] ? atoi( szStr ) : defaultvalue;
}

float CTxtTableFile::GetFloat( int32 nRow, int32 nColumn, float defaultvalue )
{
	const char* szStr = GetString( nRow, nColumn );
	return szStr[0] ? (float)atof( szStr ) : defaultvalue;
}

float CTxtTableFile::GetFloat( int32 nRow, const char* szColumnName, float defaultvalue )
{
	const char* szStr = GetString( nRow, szColumnName );
	return szStr[0] ? (float)atof( szStr ) : defaultvalue;
}

template<typename T>
T CTxtTableFile::GetNumber( int32 nRow, int32 nColumn, T defaultvalue)
{
	const char* szStr = GetString( nRow, nColumn );
	return szStr[0] ? static_cast<T>(atof( szStr )) : defaultvalue;
}

template<typename T>
T CTxtTableFile::GetNumber( int32 nRow, const char* szColumnName, T defaultvalue )
{
	const char* szStr = GetString( nRow, szColumnName );
	return szStr[0] ? static_cast<T>(atof( szStr )) : defaultvalue;
}

template int8 CTxtTableFile::GetNumber<int8>(int32, int32, int8);
template uint8 CTxtTableFile::GetNumber<uint8>(int32, int32, uint8);
template int16 CTxtTableFile::GetNumber<int16>(int32, int32, int16);
template uint16 CTxtTableFile::GetNumber<uint16>(int32, int32, uint16);
template int32 CTxtTableFile::GetNumber<int32>(int32, int32, int32);
template uint32 CTxtTableFile::GetNumber<uint32>(int32, int32, uint32);
template int64 CTxtTableFile::GetNumber<int64>(int32, int32, int64);
template uint64 CTxtTableFile::GetNumber<uint64>(int32, int32, uint64);
template float CTxtTableFile::GetNumber<float>(int32, int32, float);
template double CTxtTableFile::GetNumber<double>(int32, int32, double);

template int8 CTxtTableFile::GetNumber<int8>(int32, const char*, int8);
template uint8 CTxtTableFile::GetNumber<uint8>(int32, const char*, uint8);
template int16 CTxtTableFile::GetNumber<int16>(int32, const char*, int16);
template uint16 CTxtTableFile::GetNumber<uint16>(int32, const char*, uint16);
template int32 CTxtTableFile::GetNumber<int32>(int32, const char*, int32);
template uint32 CTxtTableFile::GetNumber<uint32>(int32, const char*, uint32);
template int64 CTxtTableFile::GetNumber<int64>(int32, const char*, int64);
template uint64 CTxtTableFile::GetNumber<uint64>(int32, const char*, uint64);
template float CTxtTableFile::GetNumber<float>(int32, const char*, float);
template double CTxtTableFile::GetNumber<double>(int32, const char*, double);

CVariant* CTxtTableFile::GetValue(CTableFileValue* pValue, int32 nRow, const char* szColumnName)
{
	ETableFileFieldType eType = GetType(szColumnName);

	switch(eType)
	{
	case eTFFT_String:
		{
			pValue->SetCStr(GetString(nRow, szColumnName));
			return pValue->GetValue();
		}
		break;
	case eTFFT_Number:
		{
			pValue->SetNumber(GetNumber<double>(nRow, szColumnName, 0));
			return pValue->GetValue();
		}
		break;
	default:
		break;
	}
	return NULL;

}

CVariant* CTxtTableFile::GetValue(CTableFileValue* pValue, int32 nRow, int32 nCol)
{
	ETableFileFieldType eType = GetType(nCol);

	switch(eType)
	{
	case eTFFT_String:
		{
			pValue->SetCStr(GetString(nRow, nCol));
			return pValue->GetValue();
		}
		break;
	case eTFFT_Number:
		{
			pValue->SetNumber(GetNumber<double>(nRow, nCol, 0));
			return pValue->GetValue();
		}
		break;
	default:
		break;
	}
	return NULL;

}
