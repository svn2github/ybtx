#pragma once

#include "ScriptCommon.h"
#include "ScriptCallBaseData.h"
#include "DataAccess.h"
#include "CBaseType.h"

namespace sqr
{
	struct CPackGroup;
	class CPackFunction : public CFunction
	{
		friend class CPackMgr;
	public:
		CPackFunction( CScript& Script, const char* szFormat, bool bFirstArgAsCallContext, const char* szFunName ) 
			: CFunction( Script, szFormat, szFunName ), m_pCallContext(NULL)
		{
			if( bFirstArgAsCallContext )
			{
				m_pCallContext = *m_pData->m_vecParam.rbegin();
				m_pData->m_vecParam.erase( --m_pData->m_vecParam.end() );
				Ast( m_pCallContext->GetLen() <= sizeof( void* ) );
			}
		}
	
		~CPackFunction()
		{
			if(m_pCallContext)
			{
				delete m_pCallContext;
				m_pCallContext = NULL;
			}
		}

		static int32 PackFromLua( lua_State* pState );

		uint32					m_uID;			//函数在整个打包组里的编号（其实就是协议id）
		CPackGroup*				m_pGroup;
		CBaseType*				m_pCallContext;
	};

	class CUnPackCallback : public CBaseCallBack
	{
		friend class CPackMgr;
	public:
		CUnPackCallback( CScript& Script,const char* szFormat, const char* szFunction, bool bFirstArgAsCallContext ) 
			: CBaseCallBack( Script, szFormat, szFunction )
		{
			if( bFirstArgAsCallContext )
			{
				m_pCallContext = *m_pData->m_vecParam.rbegin();
				m_pData->m_vecParam.erase( --m_pData->m_vecParam.end() );
				Ast( m_pCallContext->GetLen() <= sizeof( void* ) );
			}
		}

		~CUnPackCallback()
		{
			if(m_pCallContext)
			{
				delete m_pCallContext;
				m_pCallContext = NULL;
			}
		}

		int32 UnPackToLua();
		int32 UpPack( void* pObj );
		int32 UnPackBuf( char* pDataBuf, char* pBuf, size_t size );
		const char* GetFunctionName(){ return m_sFunName.c_str(); }

		size_t GetFunctionNameSize() { return m_sFunName.size(); }

		CBaseType*	m_pCallContext;
	};

	struct CPackGroup : public CScriptMallocObject
	{
		typedef list<CPackFunction*, CScriptAllocator<CPackFunction*> >	LstPackFunc_t;

		~CPackGroup()
		{
			for( LstPackFunc_t::iterator it = m_listPackInfo.begin(); 
				it != m_listPackInfo.end(); it++ )
				delete *it;
		}
		
		typedef basic_string<char, std::char_traits<char>, CScriptAllocator<char> > String_t;

		String_t					m_szTableName;
		void*						m_pCppContext;
		PACK_FUNCTION				m_pCallBackFun;
		LstPackFunc_t				m_listPackInfo;
	};

	struct CUnPackGroup : public CScriptMallocObject
	{
		typedef map<uint32,CUnPackCallback*, less<uint32>, 
			CScriptAllocator<pair<uint32, CUnPackCallback*> > >	MapUnPackCallBack_t;	

		~CUnPackGroup()
		{
			for( MapUnPackCallBack_t::iterator it = m_mapUnPackInfo.begin(); it != m_mapUnPackInfo.end(); it++ )
				delete it->second;
		}

		typedef basic_string<char, std::char_traits<char>, CScriptAllocator<char> > String_t;

		CScript*					m_pScript;
		String_t					m_sTableName;
		MapUnPackCallBack_t			m_mapUnPackInfo;
	};

	class CPackMgr
	{
		typedef list<CPackGroup, CScriptAllocator<CPackGroup> >	LstPackGroup_t;
		typedef list<CUnPackGroup, CScriptAllocator<CUnPackGroup> >	LstUnPackGroup_t;

		LstPackGroup_t				m_listPackGroup;
		LstUnPackGroup_t			m_listUnPackGroup;

		static size_t UnPack( uint32 uID, CUnPackGroup* pGroup, void* pCallContext, char* pBuf, size_t uBufSize);
		static bool	  OnUnPackError( lua_State* pState, const char* szTableName, int32 nCode, void* pContext, const char* szFunName = "" );

	public:
		CPackMgr(){};
		~CPackMgr(){};

		//注册函数参数打包函数集
		HPACKAGE	RegPackPakage( CScript& Script, PACK_FUNCTION pPackFun, void* pCppContext, const char* szTableName );
		//注册函数参数打包打包函数集的函数
		void		RegPackFunction( CScript& Script, HPACKAGE hPakageHandle, const char* szFunction, uint32 uID, bool bFirstArgAsCallContext, const char* szFormat );
		//注册函数参数解包函数集
		HPACKAGE	RegUnPackPakage( CScript& Script, const char* szTableName, UNPACK_FUNCTION* pUnPackFun );
		//注册函数参数解包函数集的解包函数
		void		RegUnPackFunction( CScript& Script, HPACKAGE hPakageHandle, const char* szFunction, uint32 uID, bool bFirstArgAsCallContext, const char* szFormat );
	};

}

