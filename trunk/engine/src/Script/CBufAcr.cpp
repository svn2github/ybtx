#include "stdafx.h"
#include "ScriptCommon.h"
#include "ExpHelper.h"
#include "DataMake.h"
#include "CBuf.h"
#include <string>
#include "CScriptAllocator.h"
#include "TSqrAllocator.inl"
#include "CDataType.h"

namespace sqr
{
	class CLuaBufAcrBase : public CScriptMallocObject
	{
	protected:
		CLuaBufAcrBase( CScript& Script, const char* szFormat )
		{
			CScriptDataMaker Factory( &Script, szFormat );
			size_t nLen = 0;
			do
			{
				CBaseType *p = Factory.MakeNextData( nLen );
				Ast( !Factory.IsErr() );	//字符串格式错误
				Ast( !Factory.IsResult() );
				if( p!=NULL )
				{
					p->SetOffset( 0 );
					m_vecType.push_back( p );
				}
			}
			while( !Factory.IsEnd() );
		}

		~CLuaBufAcrBase()
		{
			VecType_t::iterator iter,end;
			iter = m_vecType.begin();
			end = m_vecType.end();
			for(; iter!=end; ++iter)
				delete (*iter);
		}
	protected:
		typedef vector<CBaseType*, CScriptAllocator<CBaseType*> >	VecType_t;
		VecType_t	m_vecType;
	};

	class CLuaBufReader : public CLuaBufAcrBase
	{
	public:
		typedef vector<CBaseType*, CScriptAllocator<CBaseType*> >	VecType_t;

		CLuaBufReader( CScript& Script, const char* szFormat )
			: CLuaBufAcrBase( Script, szFormat )
		{}

		typedef vector<size_t, CScriptAllocator<CBaseType*> >	VecLen_t;

		VecLen_t	m_vecStrLen;	//对于字符串类型的reader，我们有时候需要指定其长度，防止读取错误

		void GetStrLen(lua_State* pState)
		{
			int32 nTop = lua_gettop(pState);
			int32 nCount = nTop - 2;

			if (nCount == 0)
			{
				return;
			}

			m_vecStrLen.resize(nCount);

			for (int32 i = nTop; i > 2; i--)
			{
				//第一个字符串对应的长度在最右边，方便pop
				m_vecStrLen[i - 3] = (size_t)lua_tonumber(pState, i);
			}
		}

		int Read( lua_State* L, void* pSrc )
		{
			char* p = (char*)pSrc;
			VecType_t::iterator iter, end;
			iter = m_vecType.begin();
			end = m_vecType.end();

			int ExtraSlot = (int)m_vecType.size();
			//防止堆栈溢出，检查
			luaL_checkstack(L, ExtraSlot, "too many arguments in CBuf Read");

			for( ; iter != end; ++iter )
			{
				if( (*iter)->IsStr() )
				{
					CTypeString* pType = dynamic_cast<CTypeString*>(*iter);

					char** pp = &p;

					size_t stLen = strlen(p);

					if (m_vecStrLen.empty())
					{
						//如果已经没有对应的长度了，我们就使用strlen
						pType->Push2LuaStack( L, (char*)pp, stLen );
						p += stLen + 1;
					}
					else
					{
						size_t stSetLen = m_vecStrLen.back();
						m_vecStrLen.pop_back();

						//如果超过了最大长度，我们只读取最大长度数据
						if (stSetLen < stLen)
						{
							stLen = stSetLen;
						}

						pType->Push2LuaStack(L, (char*)pp, stLen);
						
						p += stLen;
					}
				}
				else
				{
					(*iter)->Push2LuaStack( L, p );
					p += (*iter)->GetLen();
				}
				
			}

			int len = int(p - (char*)pSrc);
			return len;
		}
	};

	class CLuaBufWriter : public CLuaBufAcrBase
	{
	public:
		typedef vector<CBaseType*, CScriptAllocator<CBaseType*> >	VecType_t;

		CLuaBufWriter( CScript& Script, const char* szFormat )
			: CLuaBufAcrBase( Script, szFormat )
		{}

		int Write( lua_State* L, void* pDest, int nStkIdx )
		{
			char* p = (char*)pDest;
			VecType_t::iterator iter, end;
			int nIdx = nStkIdx;
			iter = m_vecType.begin();
			end = m_vecType.end();

			for( ; iter != end; ++iter, ++nIdx )
			{
				if( (*iter)->IsStr() )
				{
					char* str = NULL;
					(*iter)->GetFromLuaStack( L, (char*)(&str), nIdx );
					size_t stLen=strlen( str );
					memcpy( p, str, stLen );
					p[stLen]='\0';
					p += stLen + 1 ;
				}
				else
				{
					(*iter)->GetFromLuaStack( L, p, nIdx );
					p += (*iter)->GetLen();
				}
			}

			int len = int(p - (char*)pDest);
			return len;
		}
	};

	//-------------------		common	-------------------
	inline char* __get_CBufAcr_Begin_Pos( lua_State* L, unsigned int uIdx  )
	{
		lua_pushstring( L, "__buf" );
		lua_rawget( L, uIdx );
		char* p = (char*)lua_touserdata( L, -1 );
		lua_pop( L, 1 );

		return p;
	}

	inline unsigned int __get_CBufAcr_Cur( lua_State* L, unsigned int uIdx  )
	{
		lua_pushstring( L, "__cur" );
		lua_rawget( L, uIdx );
		unsigned int l = (uint32)lua_tointeger( L, -1 );
		Ast( l >= 0 );
		lua_pop( L, 1 );

		return l;
	}

	inline char* __get_CBufAcr_Cur_Pos( lua_State* L, unsigned int uIdx )
	{
		lua_pushstring( L, "__buf" );
		lua_rawget( L, uIdx );
		char* p = (char*)lua_touserdata( L, -1 );

		lua_pushstring( L, "__cur" );
		lua_rawget( L, uIdx );
		int cur_pos = (int)lua_tointeger( L, -1 );
		lua_pop( L, 2 );

		return p + cur_pos;
	}

	inline void __set_CBufAcr_Cur( lua_State* L, unsigned int uIdx, unsigned int uPos )
	{
		lua_pushstring( L, "__cur" );
		lua_pushinteger( L, uPos );
		lua_rawset( L, uIdx );
	}

	inline void __inc_CBufAcr_Pos( lua_State* L, unsigned int uIdx, int nPos )
	{
		int pos = __get_CBufAcr_Cur( L, uIdx ) + nPos;
		Ast( pos >= 0 );
		__set_CBufAcr_Cur( L, uIdx, pos );
	}

	//-------------------		write	-------------------
	int __CBufAcr_Write( lua_State* L)
	{
		char* pcur = __get_CBufAcr_Cur_Pos( L, 1 );

		const char* format = lua_tostring( L, 2 );
		CLuaBufWriter writor( _S( L ), format );
		int inc_len = writor.Write( L, pcur, 3 );
		Ast( inc_len >= 0 );

		__inc_CBufAcr_Pos( L, 1, inc_len );
		
		lua_settop( L, 0 );
		return 0;
	}

	//-------------------		read	-------------------
	int __CBufAcr_Read( lua_State* L )
	{
		char* pcur = __get_CBufAcr_Cur_Pos( L, 1 );

		const char* format = lua_tostring( L, 2 );
		int len = (int)strlen( format );

		CLuaBufReader reader( _S( L ), format );

		//如果format里面有字符串类型的，我们可以传入字符串的最大read长度，防止读取错误
		reader.GetStrLen(L);

		int inc_len = reader.Read( L, pcur );
		Ast( inc_len >= 0 );
		
		__inc_CBufAcr_Pos( L, 1, inc_len );

		return len;
	}
	
	//-------------------		tostring	-------------------
	int __CBufAcr_ToString( lua_State* L )
	{
		char* str = __get_CBufAcr_Cur_Pos( L, 1 );
		int len  = (int)strlen( str );
		Ast( len >=0 );
		__inc_CBufAcr_Pos( L, 1, len );

		lua_pushstring( L, str );
		lua_replace( L,1 );
		lua_settop( L, 1 );
		return 1;
	}

	//-------------------		seek	-------------------
	int __CBufAcr_Seek( lua_State* L )
	{
		switch( lua_type( L, 2 ) )
		{
		case LUA_TSTRING:
			Ast( strcmp( "begin", lua_tostring(L, 2) ) == 0 );	//only support "begin"
			__set_CBufAcr_Cur( L, 1, 0 );
			break;
		case LUA_TNUMBER:
			__inc_CBufAcr_Pos( L, 1, (int)lua_tointeger( L, 2) );
			break;
		default:
			GenErr("Seek arg error.");
		}
		
		return 0;
	}

	//-------------------		copy	-------------------
	int __CBufAcr_Copy( lua_State* L )
	{
		char* pd = __get_CBufAcr_Cur_Pos( L, 1 );
		char* ps = __get_CBufAcr_Cur_Pos( L, 2 );
		int len = (int)lua_tointeger( L, 3 );
		Ast( (pd!=NULL) && (ps!=NULL) );
		Ast( len >=0 );

		memcpy( pd, ps, len );
		__inc_CBufAcr_Pos( L, 1, len );
		__inc_CBufAcr_Pos( L, 2, len );

		return 0;
	}

	//-------------------		set	-------------------
	int __CBufAcr_Set( lua_State* L )
	{
		void* p = (void*)__get_CBufAcr_Cur_Pos( L, 1 );
		int val = (int)lua_tointeger( L, 2 );
		int len = (int)lua_tointeger( L, 3 );
		Ast( len>=0 );

		memset( p, val, len );
		__inc_CBufAcr_Pos( L, 1, len );

		return 0;
	}

	void __assignCBufAcrMethodAndPos( lua_State* L, int nStkIdx )
	{
		lua_pushinteger( L, 0 );
		lua_setfield( L, nStkIdx, "__cur" );

		lua_pushcfunction( L, &__CBufAcr_Write );
		lua_setfield( L, nStkIdx, "Write" );
		lua_pushcfunction( L, &__CBufAcr_Read );
		lua_setfield( L, nStkIdx, "Read" );
		lua_pushcfunction( L, &__CBufAcr_ToString );
		lua_setfield( L, nStkIdx, "ToString" );

		lua_pushcfunction( L, &__CBufAcr_Seek );
		lua_setfield( L, nStkIdx, "Seek" );

		lua_pushcfunction( L, &__CBufAcr_Copy );
		lua_setfield( L, nStkIdx, "Copy" );

		lua_pushcfunction( L, &__CBufAcr_Set );
		lua_setfield( L, nStkIdx, "Set" );

		lua_getglobal( L, "__mt_readonly" );
		Ast( !lua_isnil( L, -1 ) );
		lua_setmetatable( L, nStkIdx );
	}

	void __makeTable2CBufAcr( lua_State* L, int nStkIdx, void* p )
	{
		lua_pushlightuserdata( L, p );
		lua_setfield( L, nStkIdx, "__buf" );

		__assignCBufAcrMethodAndPos( L, nStkIdx );
	}

	int __createCBufAcr( lua_State* L )
	{
		lua_newtable( L );
		Ast( lua_istable( L, 1 ) );
		lua_getfield( L, 1, "__buf" );
		lua_setfield( L, 2, "__buf" );

		__assignCBufAcrMethodAndPos( L, 2 );

		lua_settop( L, 2 );
		lua_remove( L, 1 );

		return 1;
	}
}

