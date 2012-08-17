#pragma once
#include "TAppConfig.h"
#include "CXmlConfig.inl"

template<typename Traits>
template<typename ValueType>
ValueType TAppConfig<Traits>::DftGet(ValueType DefaultValue,const char* szElemName)const
{
	return m_pConfig->DftGet( DefaultValue, szElemName );
}

template<typename Traits>
template<typename ValueType>
ValueType TAppConfig<Traits>::DftGet(ValueType DefaultValue,const char* szElemName1,const char* szElemName2)const
{
	return m_pConfig->DftGet( DefaultValue, szElemName1 , szElemName2 );
}

template<typename Traits>
template<typename ValueType>
ValueType TAppConfig<Traits>::DftGet(ValueType DefaultValue,const char* szElemName1,const char* szElemName2,const char* szElemName3)const
{
	return m_pConfig->DftGet( DefaultValue, szElemName1 , szElemName2 , szElemName3 );
}

template<typename Traits>
template<typename ValueType>
ValueType TAppConfig<Traits>::Get(const char* szElemName)const
{
	return m_pConfig->Get<ValueType>( szElemName );
}

template<typename Traits>
template<typename ValueType>
ValueType TAppConfig<Traits>::Get(const char* szElemName1,const char* szElemName2)const
{
	return m_pConfig->Get<ValueType>( szElemName1 , szElemName2 );
}

template<typename Traits>
template<typename ValueType>
ValueType TAppConfig<Traits>::Get(const char* szElemName1,const char* szElemName2,const char* szElemName3)const
{
	return m_pConfig->Get<ValueType>( szElemName1 , szElemName2 , szElemName3 );
}

