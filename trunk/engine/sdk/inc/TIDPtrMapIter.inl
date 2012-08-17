#pragma once
#include "TIDPtrMapIter.h"
#include "TIDPtrMap.inl"
#include "ExpHelper.h"

template<typename ValueType>
TIDPtrMapIter<ValueType>::TIDPtrMapIter(uint32 uIndex, Container* pContainer)
:m_uIndex(uIndex)
,m_pContainer(pContainer)
{
}

template<typename ValueType>
ValueType& TIDPtrMapIter<ValueType>::operator*()const
{
	if(m_uIndex == uint32(-1) || m_uIndex >= m_pContainer->m_vecValue.size())
	{
		GenErr("³¬³öµü´úÆ÷·¶Î§");
	}
	return *(m_pContainer->m_vecValue[m_uIndex].first);
}

template<typename ValueType>
ValueType* TIDPtrMapIter<ValueType>::operator->()const
{
	if(m_uIndex == uint32(-1) || m_uIndex >= m_pContainer->m_vecValue.size())
	{
		GenErr("³¬³öµü´úÆ÷·¶Î§");
	}
	return m_pContainer->m_vecValue[m_uIndex].first;
}

template<typename ValueType>
TIDPtrMapIter<ValueType>& TIDPtrMapIter<ValueType>::operator++()
{
	if(m_uIndex == uint32(-1) || m_uIndex >= m_pContainer->m_vecValue.size())
	{
		GenErr("³¬³öµü´úÆ÷·¶Î§");
	}

	for(uint32 i = m_uIndex + 1; i < m_pContainer->m_vecValue.size(); ++i)
	{
		if(m_pContainer->m_vecValue[i].first != NULL)
		{
			m_uIndex = i;
			return *this;
		}
	}

	m_uIndex = uint32(-1);
	return *this;
}

template<typename ValueType>
bool TIDPtrMapIter<ValueType>::operator==(const TIDPtrMapIter& i)const
{
	return m_uIndex == i.m_uIndex;
}

template<typename ValueType>
bool TIDPtrMapIter<ValueType>::operator!=(const TIDPtrMapIter& i)const
{
	return m_uIndex != i.m_uIndex;
}
