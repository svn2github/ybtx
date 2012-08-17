#pragma once
#include "TTlsPtr.h"
#include "ThreadHelper.h"

template<typename ValueType>
TTlsPtr<ValueType>::TTlsPtr(void)
{
	TLS_CreateKey( &m_hKey );
	CreateSpinLock( &m_hLock );
}

template<typename ValueType>
TTlsPtr<ValueType>::~TTlsPtr(void)
{
	SpinLock( &m_hLock );

	while( !m_stkPtr.empty() )
	{
		delete m_stkPtr.top();
		m_stkPtr.pop();
	}
	
	SpinUnlock( &m_hLock );

	DestroySpinLock( &m_hLock );
	TLS_DestroyKey( m_hKey );
}

template<typename ValueType>
TTlsPtr<ValueType>::operator ValueType*()const
{
	ValueType*p=static_cast<ValueType*>( TLS_GetValue( m_hKey ) );

	if( !p )
	{
		p=new ValueType;
		const_cast<TTlsPtr<ValueType>*>(this)->operator=(p);
	}
	return p;
}

template<typename ValueType>
ValueType* TTlsPtr<ValueType>::operator->()const
{
	return (ValueType*)(this);
}


template<typename ValueType>
ValueType* TTlsPtr<ValueType>::operator =(ValueType* p)
{
	TLS_SetValue( m_hKey, p );

	SpinLock( &m_hLock );
	m_stkPtr.push( p );
	SpinUnlock( &m_hLock );

	return p;
}
