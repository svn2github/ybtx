#pragma once
#include "TObjectBank.h"

template<typename ObjType, template<typename>class TAlloc >
TObjectBank<ObjType, TAlloc>::TObjectBank(uint32 uSize)
{
	m_uSize = uSize;
}


template<typename ObjType, template<typename>class TAlloc >
TObjectBank<ObjType, TAlloc>::TObjectBank(uint32 uSize,bool bPrepare)
{
	m_uSize = uSize;
	if( !bPrepare )
		return;

	for( uint32 i=0;i<uSize; ++ i )
		m_deqObj.push_back( new ObjType );
}

template<typename ObjType, template<typename>class TAlloc >
TObjectBank<ObjType, TAlloc>::~TObjectBank()
{
	while( !m_deqObj.empty() )
		delete Pop();
}

template<typename ObjType, template<typename>class TAlloc >
void TObjectBank<ObjType, TAlloc>::Push(ObjType* pObj)
{
	if( m_deqObj.size() > m_uSize )
	{
		delete pObj;
		return;
	}
	m_deqObj.push_back(pObj);
}

template<typename ObjType, template<typename>class TAlloc >
ObjType* TObjectBank<ObjType, TAlloc>::Pop()
{
	if( m_deqObj.empty() )
		return NULL;		
	
	ObjType* pObj = m_deqObj.back();
	m_deqObj.pop_back();

	return pObj;
}


template<typename ObjType, template<typename>class TAlloc >
ObjType* TObjectBank<ObjType, TAlloc>::PopOrNew()
{
	ObjType* pObj = Pop();
	if( pObj )
		return pObj;
	return new ObjType;
}

