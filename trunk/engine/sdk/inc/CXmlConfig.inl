#pragma once
#include "CXmlConfig.h"
#include "ExpHelper.h"
#include "tinyxml/tinyxml.h"
#include <stdarg.h>

namespace sqr
{
	template<typename T>
	struct is_pointer
	{
		static const bool value=false;
	};

	template<typename T>
	struct is_pointer<T*>
	{
		static const bool value=true;
	};
}

template<typename NumberType>
inline NumberType CStrToValue(const char* szText)
{
	if( is_pointer<NumberType>::value )
		GenErr("Don't get pointer value from CXmlConfig except <const char*> !");
	return NumberType( atol(szText) );
}

template<>
int64 CStrToValue<int64>(const char* szText);

template<>
uint64 CStrToValue<uint64>(const char* szText);

template<>
float CStrToValue<float>(const char* szText);

template<>
double CStrToValue<double>(const char* szText);

template<>
bool CStrToValue<bool>(const char* szText);

//由于string不能export dll，所以只能inline在这里了
template<>
inline string CStrToValue<string>(const char* szText)
{
	return string( szText );
}




template<typename ValueType>
ValueType CXmlConfig::GetNodeValue(const ValueType* pDefaultValue,TiXmlNode* pNode,const vector<const char*>& vecElemName)const
{
	const char* szText=NULL;

	if( !pNode || !(szText=pNode->ToElement()->GetText()) )
	{
		if( pDefaultValue )
			return *pDefaultValue;

		GenElemNoContentErr(vecElemName);
	}

	return CStrToValue<ValueType>( szText );
}


template<typename ValueType>
ValueType CXmlConfig::DftGet(ValueType DefaultValue,const char* szElemName)const
{
	vector<const char*> vecElemName;
	vecElemName.push_back( szElemName );

	return GetNodeValue<ValueType>( &DefaultValue,GetNode( vecElemName, false ),vecElemName );	
}


template<typename ValueType>
ValueType CXmlConfig::DftGet(ValueType DefaultValue,const char* szElemName1,const char* szElemName2)const
{
	vector<const char*> vecElemName;
	vecElemName.push_back( szElemName1 );
	vecElemName.push_back( szElemName2 );

	return GetNodeValue<ValueType>( &DefaultValue,GetNode( vecElemName, false ),vecElemName );	
}

template<typename ValueType>
ValueType CXmlConfig::DftGet(ValueType DefaultValue,const char* szElemName1,const char* szElemName2,const char* szElemName3)const
{
	vector<const char*> vecElemName;
	vecElemName.push_back( szElemName1 );
	vecElemName.push_back( szElemName2 );
	vecElemName.push_back( szElemName3 );

	return GetNodeValue<ValueType>( &DefaultValue,GetNode( vecElemName, false ),vecElemName );	
}




template<typename ValueType>
ValueType CXmlConfig::Get(const char* szElemName)const
{
	vector<const char*> vecElemName;
	vecElemName.push_back( szElemName );

	return GetNodeValue<ValueType>( NULL,GetNode( vecElemName, true ),vecElemName );
}

template<typename ValueType>
ValueType CXmlConfig::Get(const char* szElemName1,const char* szElemName2)const
{
	vector<const char*> vecElemName;
	vecElemName.push_back( szElemName1 );
	vecElemName.push_back( szElemName2 );

	return GetNodeValue<ValueType>( NULL,GetNode( vecElemName, true ),vecElemName );
}

template<typename ValueType>
ValueType CXmlConfig::Get(const char* szElemName1,const char* szElemName2,const char* szElemName3)const
{
	vector<const char*> vecElemName;
	vecElemName.push_back( szElemName1 );
	vecElemName.push_back( szElemName2 );
	vecElemName.push_back( szElemName3 );

	return GetNodeValue<ValueType>( NULL,GetNode( vecElemName, true ),vecElemName );
}


template<typename ValueType>
ValueType CXmlConfig::GetNodeAttribValue(const ValueType* pDefaultValue,TiXmlNode* pNode,const char* szAttribName,const vector<const char*>& vecElemName)const
{
	const char* szText=NULL;

	if( !pNode || !(szText=pNode->ToElement()->Attribute(szAttribName)) )
	{
		if( pDefaultValue )
			return *pDefaultValue;

		GenAttribNoContentErr( szAttribName, vecElemName );
	}

	return CStrToValue<ValueType>( szText );
}


template<typename ValueType>
ValueType CXmlConfig::DftAttribGet(ValueType DefaultValue,const char* szAttribName)const
{
	vector<const char*> vecElemName;

	return GetNodeAttribValue<ValueType>( &DefaultValue,GetNode( vecElemName, false ),szAttribName,vecElemName );
}


template<typename ValueType>
ValueType CXmlConfig::DftAttribGet(ValueType DefaultValue,const char* szElemName,const char* szAttribName)const
{
	vector<const char*> vecElemName;
	vecElemName.push_back(szElemName);

	return GetNodeAttribValue<ValueType>( &DefaultValue,GetNode( vecElemName, false ),szAttribName,vecElemName );
}

template<typename ValueType>
ValueType CXmlConfig::DftAttribGet(ValueType DefaultValue,const char* szElemName1,const char* szElemName2,const char* szAttribName)const
{
	vector<const char*> vecElemName;
	vecElemName.push_back(szElemName1);
	vecElemName.push_back(szElemName2);

	return GetNodeAttribValue<ValueType>( &DefaultValue,GetNode( vecElemName, false ),szAttribName,vecElemName );
}



template<typename ValueType>
ValueType CXmlConfig::AttribGet(const char* szAttribName)const
{
	vector<const char*> vecElemName;

	return GetNodeAttribValue<ValueType>( NULL,GetNode( vecElemName, true ),szAttribName,vecElemName );
}

template<typename ValueType>
ValueType CXmlConfig::AttribGet(const char* szElemName,const char* szAttribName)const
{
	vector<const char*> vecElemName;
	vecElemName.push_back(szElemName);

	return GetNodeAttribValue<ValueType>( NULL,GetNode( vecElemName, true ),szAttribName,vecElemName );
}

template<typename ValueType>
ValueType CXmlConfig::AttribGet(const char* szElemName1,const char* szElemName2,const char* szAttribName)const
{
	vector<const char*> vecElemName;
	vecElemName.push_back(szElemName1);
	vecElemName.push_back(szElemName2);

	return GetNodeAttribValue<ValueType>( NULL,GetNode( vecElemName, true ),szAttribName,vecElemName );
}

