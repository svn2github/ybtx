#pragma once
#include "TIDPtrMap.h"
#include "TIDPtrMapIter.inl"
#include "ExpHelper.h"


template<typename ValueType>
inline uint32 TIDPtrMap<ValueType>::GetIndexByID(uint32 uID)
{
	return (uID & 0xfffff) - 1;
}

template<typename ValueType>
TIDPtrMap<ValueType>::TIDPtrMap()
:m_uPtrNum(0)
{
	Init();
}

template<typename ValueType>
TIDPtrMap<ValueType>::~TIDPtrMap()
{
}

template<typename ValueType>
void TIDPtrMap<ValueType>::Init()
{
	m_vecValue.resize(10, make_pair(static_cast<ValueType*>(NULL),0));
	for(uint32 i = 0; i < m_vecValue.size(); ++i)
		m_queGapID.push(i);
}

template<typename ValueType>
inline bool TIDPtrMap<ValueType>::GetValuePair(ValuePair_t*& pPair,uint32 uIndex,uint16 uCount)
{
	if( uIndex >= m_vecValue.size() )
		return false;

	ValuePair_t& p=m_vecValue[uIndex];

	if( p.second != uCount )
		return false;

	pPair=&p;

	return true;
}

template<typename ValueType>
inline bool TIDPtrMap<ValueType>::Get(const ValueType*& pValue,uint32 uID)const
{
	return const_cast< TIDPtrMap<ValueType>* >(this)->Get( const_cast<ValueType*&>(pValue),uID );
}

template<typename ValueType>
inline bool TIDPtrMap<ValueType>::Get(ValueType*& pValue,uint32 uID)
{
	ValuePair_t* pPair;

	bool bFound = GetValuePair( pPair,GetIndexByID(uID), uint16(uID >> 20) );

	if( !bFound )
		return false;

	pValue = pPair->first;

	return true;
}

template<typename ValueType>
bool TIDPtrMap<ValueType>::Del(uint32 uID)
{
	uint32 uIndex = GetIndexByID(uID);

	ValuePair_t* pPair;

	bool bFound = GetValuePair( pPair, uIndex , uint16(uID >> 20) );

	if( !bFound )
		return false;

	--m_uPtrNum;
	pPair->first = NULL;
	pPair->second = ++pPair->second & 0xfff;
	m_queGapID.push( uIndex );
	return true;
}

template<typename ValueType>
uint32 TIDPtrMap<ValueType>::Add(ValueType* pValue)
{
	if(pValue == NULL)
		GenErr("NULL ptr is invalid in IDPtrMap");

	uint32 uID;

	Ast(!m_queGapID.empty());
	const uint32 uIndex = m_queGapID.front();
	m_queGapID.pop();
	ValuePair_t& p = m_vecValue[uIndex];
	p.first = pValue;
	++m_uPtrNum;

	uID = (uIndex+1) | ( uint32(p.second) << 20 );

	if(m_queGapID.size() < 10)
	{
		if(m_vecValue.size() > 0xfffff)
			GenErr("IDPtrMap OverFlow");

		m_vecValue.push_back( make_pair(static_cast<ValueType*>(NULL),0) );
		m_queGapID.push(uint32(m_vecValue.size()) - 1);
	}

	return uID;
}

template<typename ValueType>
typename TIDPtrMap<ValueType>::Iterator TIDPtrMap<ValueType>::Begin()
{
	uint32 uIndexBegin;
	for(uint32 i = 0; i < m_vecValue.size(); ++i)
	{
		if(m_vecValue[i].first != NULL)
		{
			uIndexBegin = i;
			return Iterator(uIndexBegin, this);
		}
	}

	return Iterator(); 
}

template<typename ValueType>
typename TIDPtrMap<ValueType>::Iterator TIDPtrMap<ValueType>::End()
{
	return Iterator();
}

template<typename ValueType>
uint32 TIDPtrMap<ValueType>::Size()const
{
	return m_uPtrNum;
}

template<typename ValueType>
void TIDPtrMap<ValueType>::Clear()
{
	m_vecValue.clear();
	m_queGapID = QueGapID_t();
	Init();
}
