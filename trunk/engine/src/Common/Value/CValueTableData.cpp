#include "stdafx.h"
#include "CValueTableData.h"
#include "CValueTable.h"
#include "ExpHelper.h"

CValueTableData::CValueTableData()
{

}

CValueTableData::~CValueTableData()
{

}

void CValueTableData::SetData(CValueTable* pValueTable, uint32 uRow, uint32 uCol)
{
	EVariantType eType = pValueTable->GetType(uCol);
	switch(eType)
	{
	case eVT_Bool:
		m_Data.SetNumber(pValueTable->GetNumber<bool>(uRow, uCol));
		break;
	case eVT_Int8:
		m_Data.SetNumber(pValueTable->GetNumber<int8>(uRow, uCol));
		break;
	case eVT_Uint8:
		m_Data.SetNumber(pValueTable->GetNumber<uint8>(uRow, uCol));
		break;
	case eVT_Int16:
		m_Data.SetNumber(pValueTable->GetNumber<int16>(uRow, uCol));
		break;
	case eVT_Uint16:
		m_Data.SetNumber(pValueTable->GetNumber<uint16>(uRow, uCol));
		break;
	case eVT_Int32:
		m_Data.SetNumber(pValueTable->GetNumber<int32>(uRow, uCol));
		break;
	case eVT_Uint32:
		m_Data.SetNumber(pValueTable->GetNumber<uint32>(uRow, uCol));
		break;
	case eVT_Int64:
		m_Data.SetNumber(pValueTable->GetNumber<int64>(uRow, uCol));
		break;
	case eVT_Uint64:
		m_Data.SetNumber(pValueTable->GetNumber<uint64>(uRow, uCol));
		break;
	case eVT_Float:
		m_Data.SetNumber(pValueTable->GetNumber<float>(uRow, uCol));
		break;
	case eVT_Double:
		m_Data.SetNumber(pValueTable->GetNumber<double>(uRow, uCol));
		break;
	case eVT_String:
		m_Data.SetCStr(pValueTable->GetString(uRow, uCol));
		break;
	default:
		GenErr("CValueTableData::SetData Error");
		break;
	}
}

