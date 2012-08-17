#include "stdafx.h"
#include "CDbBind.h"

CDbBind::CDbBind(void)
{
	is_null = NULL;
	buffer = NULL;
	length = NULL;
}

CDbBind::~CDbBind(void)
{
	SetBuffer(NULL);
	SetIsNull(NULL);
	SetLength(NULL);
}

void CDbBind::SetBuffer(const void* pBuffer)
{
	buffer = const_cast<void*>(pBuffer);
}

void CDbBind::SetIsNull(bool* pIsNull)
{
	is_null = reinterpret_cast<my_bool*>(pIsNull);
}

void CDbBind::SetLength(size_t* pLength)
{
	length = reinterpret_cast<unsigned long*>(pLength);
}

void CDbBind::SetBuffType(enum_field_types eType)
{
	buffer_type = eType;
}

void CDbBind::SetIsUnsigned(bool bIsUnsigned)
{
	is_unsigned = bIsUnsigned;
}

void CDbBind::SetBuffLength(size_t uSize)
{
	buffer_length = uSize;
}
