#include "stdafx.h"
#include "DataMake.h"
#include "CScript.h"
#include "CScriptData.h"
#include "CBaseType.h"
#include "CDataType.h"


//===============================	CScriptDataMaker 		===============================
CScriptDataMaker::CScriptDataMaker(CScript* pScript, const char* szFormat)
:m_eState(eParam),m_szFormat(szFormat),m_pCur(szFormat), m_pScript( pScript )
{}

CBaseType* CScriptDataMaker::MakeNextData( size_t& posCur )
{
	if( m_eState==eEnd || m_eState==eError )
		return NULL;

	if( *m_pCur=='\0' )
	{
		if( m_eState!=eError )
			m_eState = eEnd;

		return NULL;
	}

	CBaseType* pType = NULL;
	switch( *m_pCur++ )
	{
	case ':' :
		m_eState = eResult;
		return NULL;
	case '(' :
		pType = MakeObject( CBaseType::eFlagNull );
		break;	
	case 't':
	case 'T':
		pType = new CTypeSize;
		break;
	case 'z' :
		pType = new CTypeInt64;
		break;
	case 'Z':
		pType = new CTypeUint64;
		break;
	case 'i' :
		pType = new CTypeInt32;
		break;
	case 'I' :
		pType = new CTypeUint32;
		break;
	case 'h' :
		pType = new CTypeInt16;
		break;
	case 'H' :
		pType = new CTypeUint16;
		break;
	case 'c' :
		pType = new CTypeInt8;
		break;
	case 'C' :
		pType = new CTypeUint8;
		break;
	case 'f' :
		pType = new CTypeFloat;
		break;
	case 'd' :
		pType = new CTypeDouble;
		break;
	case 's' :
		pType = new CTypeString( false );
		break;
	case 'S' :
		pType = new CTypeString( true );
		break;
	case 'b' :
		pType = new CTypeBool;
		break;
	case 'm' :
		pType = new CTypeMemory;
		break;
	case 'a':
		pType = new CTypeVariant;
		break;
	case 'l':
		if( *m_pCur == ':' || *m_pCur == '\0')
		{
			pType = new CTypeArgList;
		}
		else{
			GenErr("可变参数只能在函数参数的最后一个");
		}
		
		break;
	default:
		m_eState = eError;
		return NULL;
	}

	pType->SetOffset( posCur );
	posCur += pType->GetLen();

	return pType;
}


CBaseType* CScriptDataMaker::MakeObject( CBaseType::EFlag eFlag)
{
	string sClassName;
	for( char c = *m_pCur++; c != ')' ; c = *m_pCur++ )
	{
		Ast( c );
		sClassName.push_back( c );
	}

	CScriptData* pData = m_pScript->GetData();
	CScriptData::MapSizeOfNum_t::iterator it = pData->m_mapSizeOfEnum.find( sClassName.c_str() );

	if( it == pData->m_mapSizeOfEnum.end() )
	{
		CClass* pClass=m_pScript->GetClassInfo( sClassName.c_str() );
		if( ! pClass )
		{
			ostringstream strm;
			strm<<"\""<<sClassName<<"\" not exists.";
			GenErr(strm.str());
		}
		return new CTypeCppObj( pClass, eFlag );
	}
	if( it->second == 4 )
		return new CTypeUint32();
	if( it->second == 2 )
		return new CTypeUint16();
	return new CTypeUint8();
}

