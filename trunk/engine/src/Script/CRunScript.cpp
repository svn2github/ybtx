#include "stdafx.h"
#include "CRunScript.h"
#include "DataMake.h"
#include "TSqrAllocator.inl"


CRunScript::CRunScript( CScript& Script, const char* szFormat, char* pStack )
: m_pStack( pStack ),m_ParamTypeLen(0),m_uBufSize(0)
{
	if( szFormat == NULL )
		return;

	CScriptDataMaker Factory( &Script, szFormat );

	do
	{
		CBaseType *pData = Factory.MakeNextData( m_uBufSize );
		Ast( !Factory.IsErr() );	//字符串格式错误
		if( pData!=NULL )
		{
			if( Factory.IsParam() )
			{
				m_vecParam.push_back( pData );	//左边的参数，在list的最后面
				m_ParamTypeLen += pData->GetLen();
			}
			else if( Factory.IsResult() )
			{
				m_vecResult.push_back( pData );
			}
		}
	}
	while( !Factory.IsEnd() );
}

CRunScript::~CRunScript()
{
	VecType_t::iterator i,end;
	i = m_vecParam.begin();
	end = m_vecParam.end();
	for(; i!=end; ++i)
		delete (*i);

	i = m_vecResult.begin();
	end = m_vecResult.end();
	for(; i!=end; ++i)
		delete (*i);
}

void CRunScript::GetFromCppStack( char* pDataBuf )
{
	VecType_t::iterator iter,end;
	iter = m_vecParam.begin();
	end = m_vecParam.end();
	char* pArg = m_pStack;

	for( ; iter!= end; ++iter )
	{
		(*iter)->SetValue( pDataBuf, pArg );
		pArg += (*iter)->GetLen();
	}
}

void CRunScript::PushParam2LuaStack( lua_State* pState, char* pDataBuf )
{
	VecType_t::iterator iter,end;
	iter = m_vecParam.begin();
	end = m_vecParam.end();
	for(; iter!= end; ++iter )
	{
		(*iter)->Push2LuaStack( pState, pDataBuf );
	}
}

void CRunScript::ReSetCppStack( lua_State* pState, char* pDataBuf )
{
	VecType_t::iterator iter,end;
	iter = m_vecParam.begin();
	end = m_vecParam.end();
	char* pArg = m_pStack + m_ParamTypeLen;

	VecType_t::reverse_iterator riter,rend;
	riter = m_vecResult.rbegin();
	rend = m_vecResult.rend();
	for( ; riter!= rend; ++riter )
	{
		(*riter)->GetFromLuaStack( pState, pDataBuf, -1 );
		lua_pop( pState, 1 );
	}

	iter = m_vecResult.begin();
	end = m_vecResult.end();
	for( ; iter!= end; ++iter )
	{
		(*iter)->GetValue( pDataBuf, *(void**)pArg );
		pArg += sizeof( ptrdiff_t );
	}
}

bool CRunScript::RunScript( lua_State* pState, char* pDataBuf, int32 nErrFunIndex )
{
	GetFromCppStack( pDataBuf );

	PushParam2LuaStack( pState, pDataBuf );

	bool bResult=true;

	if( lua_pcall( pState, (int)m_vecParam.size(), (int)m_vecResult.size(), nErrFunIndex ) )
		bResult=false;

	ReSetCppStack( pState, pDataBuf );

	return bResult;
}
