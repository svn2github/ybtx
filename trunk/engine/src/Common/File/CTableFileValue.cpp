#include "stdafx.h"
#include "CTableFileValue.h"

CTableFileValue::CTableFileValue()
{
	
}

CTableFileValue::~CTableFileValue()
{

}

void CTableFileValue::SetNumber(double value)
{
	m_Value.SetNumber(value);
}

void CTableFileValue::SetCStr(const char* str)
{
	m_Value.SetCStr(str);
}

void CTableFileValue::SetPtr(CBaseObject* pObject)
{
	m_Value.SetPtr(pObject);
}
