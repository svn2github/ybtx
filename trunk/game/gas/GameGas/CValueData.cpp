#include "stdafx.h"
#include "CValueData.h"

template <>
const int32 CValueData::GetBy<int32>() const
{
	return m_iVal;
}

template <>
const float CValueData::GetBy<float>() const
{
	return m_fVal;
}

template <>
const uint32 CValueData::GetBy<uint32>() const
{
	return m_uVal;
}


template <>
void CValueData::SetBy(int32 Val)
{
	m_iVal = Val;
}

template <>
void CValueData::SetBy(float Val)
{
	m_fVal = Val;
}

template <>
void CValueData::SetBy(uint32 Val)
{
	m_uVal = Val;
}


